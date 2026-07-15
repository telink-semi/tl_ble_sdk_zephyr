/********************************************************************************************************
 * @file    sbc_encode.c
 *
 * @brief   This is the source file for TL521X
 *
 * @author  BLE GROUP
 * @date    06,2020
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd.
 *          All rights reserved.
 *
 *          The information contained herein is confidential property of Telink
 *          Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *          of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *          Co., Ltd. and the licensee or the terms described here-in. This heading
 *          MUST NOT be removed from this file.
 *
 *          Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or
 *          alter) any information contained herein in whole or in part except as expressly authorized
 *          by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible
 *          for any claim to the extent arising out of or relating to such deletion(s), modification(s)
 *          or alteration(s).
 *
 *          Licensees are granted free, non-transferable use of the information in this
 *          file under Mutual Non-Disclosure Agreement. NO WARRANTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#include    "tl_common.h"
#include    "drivers.h"
#include    "audio_config.h"
#include    "sbc.h"



#if ((TL_AUDIO_MODE == TL_AUDIO_RCU_SBC_HID) )//RCU

#define SBC_SYNCWORD            0x9C
#define MSBC_SYNCWORD           0xAD

#define SBC_MODE_MONO           0x00
#define SBC_MODE_DUAL_CHANNEL   0x01
#define SBC_MODE_STEREO         0x02
#define SBC_MODE_JOINT_STEREO   0x03

#define QUALITY_LOWEST  1   //you may notice the quality reduction
#define QUALITY_MEDIUM  2   //pretty good
#define QUALITY_GREAT   3   //as good as it will get without an FPU


#define MSBC_SUBBANDS   8
#define MSBC_BLOCKS     15
#define MSBC_BITPOOL    26
#define MSBC_CHANNELS   1

#define SBC_MODE        SBC_MODE_MONO
#define SBC_SNR         0           // Loudness
#define SBC_SAMPLE_RATE 16000       //16000//44100//46545
#define SBC_SUBBANDS    8


// FRAME_LEN ---> 23    (MIC_MSBC_UNIT_SIZE     80)

/*
#define SBC_BLOCKS      8     //6:1
#define SBC_BITPOOL     15    //6:1

#define SBC_BLOCKS      10    //8:1
#define SBC_BITPOOL     12    //8:1

#define SBC_BLOCKS      15    //12:1
#define SBC_BITPOOL     8     //12:1

*/
#define SBC_BLOCKS      10    //8:1
#define SBC_BITPOOL     12    //8:1


#define SBC_BIGENDIAN   0           // endianness of the source data, Little Endian

#define QUALITY         QUALITY_MEDIUM
#define SPEED_OVER_RAM  0           //30% faster, big impact on speed
#define SBC_CRC_DISABLE 0           //Samsung Must use CRC //5% faster,  and small code size
#define ITER            uint32_t    //iterator up to 180 use fastest type for your platform

//config options end

// calculated Macro defines
#define SBC_CHANNELS    (SBC_MODE == SBC_MODE_MONO ? 1 : 2)
#define SBC_DUAL_CHAN   (SBC_MODE == SBC_MODE_DUAL_CHANNEL)
#define SBC_CODESIZE    (SBC_SUBBANDS * SBC_BLOCKS * SBC_CHANNELS * 2)          // byte-len a frame consume
#define SBC_ALLOCATE    (SBC_SNR ? SBC_AM_SNR : SBC_AM_LOUDNESS)
#define SBC_SR_TYPE     (SBC_SAMPLE_RATE==16000 ? 0 : SBC_SAMPLE_RATE==32000 ? 1 : SBC_SAMPLE_RATE==44100 ? 2 : 3)


#if QUALITY == QUALITY_LOWEST

#define CONST(x)        (x >> 24)
#define SAMPLE_CVT(x)   (x >> 8)
#define INSAMPLE        int8_t
#define OUTSAMPLE       uint8_t //no point producing 16-bit samples using the 8-bit decoder
#define FIXED           int8_t
#define FIXED_S         int16_t
#define OUT_CLIP_MAX    0x7F
#define OUT_CLIP_MIN    -0x80

#define NUM_FRAC_BITS_PROTO 8
#define NUM_FRAC_BITS_COS   6
#define SBC_FIXED_EXTRA_BITS 0

#elif QUALITY == QUALITY_MEDIUM

#define CONST(x)        (x >> 16)
#define SAMPLE_CVT(x)   (x)
#define INSAMPLE        int16_t
#define OUTSAMPLE       uint16_t
#define FIXED           int16_t
#define FIXED_S         int32_t
#define OUT_CLIP_MAX    0x7FFF
#define OUT_CLIP_MIN    -0x8000

#define NUM_FRAC_BITS_PROTO 16
#define NUM_FRAC_BITS_COS   14
#define SBC_FIXED_EXTRA_BITS 0

#elif QUALITY == QUALITY_GREAT

#define CONST(x)        (x)
#define SAMPLE_CVT(x)   (x)
#define INSAMPLE        int16_t
#define OUTSAMPLE       uint16_t
#define FIXED           int32_t
#define FIXED_S         int64_t
#define OUT_CLIP_MAX    0x7FFF
#define OUT_CLIP_MIN    -0x8000

#define NUM_FRAC_BITS_PROTO 32
#define NUM_FRAC_BITS_COS   30
#define SBC_FIXED_EXTRA_BITS 16

#else
#error "You did not define SBC decoder synthesizer quality to use"
#endif


#define FIXED_A         FIXED_S  /* data type for fixed point accumulator */
#define FIXED_T         FIXED    /* data type for fixed point constants */

#define fabs(x)         ((x) < 0 ? -(x) : (x))

/* C does not provide an explicit arithmetic shift right but this will
   always be correct and every compiler *should* generate optimal code */

#define ASR(val, bits)  ((-2 >> 1 == -1) ? ((int32_t)(val)) >> (bits) : ((int32_t) (val)) / (1 << (bits)))

/* A2DP specification: Appendix B, page 69 */
static
#if(!SPEED_OVER_RAM)
const
#endif
int sbc_offset4[4][4] = {
    { -1, 0, 0, 0 },
    { -2, 0, 0, 1 },
    { -2, 0, 0, 1 },
    { -2, 0, 0, 1 }
};

/* A2DP specification: Appendix B, page 69 */
static
#if(!SPEED_OVER_RAM)
const
#endif
int sbc_offset8[4][8] = {
    { -2, 0, 0, 0, 0, 0, 0, 1 },
    { -3, 0, 0, 0, 0, 0, 1, 2 },
    { -4, 0, 0, 0, 0, 0, 1, 2 },
    { -4, 0, 0, 0, 0, 0, 1, 2 }
};

#define SBC_PROTO_FIXED4_SCALE      ((sizeof(FIXED_T) * 8 - 1) - SBC_FIXED_EXTRA_BITS + 1)
#define F_PROTO4(x)                 (FIXED_A)((x * 2) * ((FIXED_A) 1 << (sizeof(FIXED_T) * 8 - 1)) + 0.5)
#define F(x)                        F_PROTO4(x)

static
#if(!SPEED_OVER_RAM)
const
#endif
FIXED_T _sbc_proto_fixed4[40] = {
    F(0.00000000E+00),  F(5.36548976E-04),
    -F(1.49188357E-03),  F(2.73370904E-03),
    F(3.83720193E-03),  F(3.89205149E-03),
    F(1.86581691E-03),  F(3.06012286E-03),

    F(1.09137620E-02),  F(2.04385087E-02),
    -F(2.88757392E-02),  F(3.21939290E-02),
    F(2.58767811E-02),  F(6.13245186E-03),
    -F(2.88217274E-02),  F(7.76463494E-02),

    F(1.35593274E-01),  F(1.94987841E-01),
    -F(2.46636662E-01),  F(2.81828203E-01),
    F(2.94315332E-01),  F(2.81828203E-01),
    F(2.46636662E-01), -F(1.94987841E-01),

    -F(1.35593274E-01), -F(7.76463494E-02),
    F(2.88217274E-02),  F(6.13245186E-03),
    F(2.58767811E-02),  F(3.21939290E-02),
    F(2.88757392E-02), -F(2.04385087E-02),

    -F(1.09137620E-02), -F(3.06012286E-03),
    -F(1.86581691E-03),  F(3.89205149E-03),
    F(3.83720193E-03),  F(2.73370904E-03),
    F(1.49188357E-03), -F(5.36548976E-04),
};
#undef F

#define SBC_COS_TABLE_FIXED4_SCALE      ((sizeof(FIXED_T) * 8 - 1) + SBC_FIXED_EXTRA_BITS)
#define F_COS4(x) (FIXED_A)             ((x) * ((FIXED_A) 1 << (sizeof(FIXED_T) * 8 - 1)) + 0.5)
#define F(x) F_COS4(x)

static
#if(!SPEED_OVER_RAM)
const
#endif
FIXED_T cos_table_fixed_4[32] = {
    F(0.7071067812),  F(0.9238795325), -F(1.0000000000),  F(0.9238795325),
    F(0.7071067812),  F(0.3826834324),  F(0.0000000000),  F(0.3826834324),

    -F(0.7071067812),  F(0.3826834324), -F(1.0000000000),  F(0.3826834324),
    -F(0.7071067812), -F(0.9238795325), -F(0.0000000000), -F(0.9238795325),

    -F(0.7071067812), -F(0.3826834324), -F(1.0000000000), -F(0.3826834324),
    -F(0.7071067812),  F(0.9238795325),  F(0.0000000000),  F(0.9238795325),

    F(0.7071067812), -F(0.9238795325), -F(1.0000000000), -F(0.9238795325),
    F(0.7071067812), -F(0.3826834324), -F(0.0000000000), -F(0.3826834324),
};
#undef F

/* A2DP specification: Section 12.8 Tables
 *
 * Original values are premultiplied by 4 for better precision (that is the
 * maximum which is possible without overflows)
 *
 * Note: in each block of 16 numbers sign was changed for elements 4, 13, 14, 15
 * in order to compensate the same change applied to cos_table_fixed_8
 */
#define SBC_PROTO_FIXED8_SCALE      ((sizeof(FIXED_T) * 8 - 1) - SBC_FIXED_EXTRA_BITS + 1)
#define F_PROTO8(x) (FIXED_A)       ((x * 2) * ((FIXED_A) 1 << (sizeof(FIXED_T) * 8 - 1)) + 0.5)
#define F(x)                        F_PROTO8(x)

static
#if(!SPEED_OVER_RAM)
const
#endif
FIXED_T _sbc_proto_fixed8[80] = {
    F(0.00000000E+00),  F(1.56575398E-04),
    F(3.43256425E-04),  F(5.54620202E-04),
    -F(8.23919506E-04),  F(1.13992507E-03),
    F(1.47640169E-03),  F(1.78371725E-03),
    F(2.01182542E-03),  F(2.10371989E-03),
    F(1.99454554E-03),  F(1.61656283E-03),
    F(9.02154502E-04),  F(1.78805361E-04),
    F(1.64973098E-03),  F(3.49717454E-03),

    F(5.65949473E-03),  F(8.02941163E-03),
    F(1.04584443E-02),  F(1.27472335E-02),
    -F(1.46525263E-02),  F(1.59045603E-02),
    F(1.62208471E-02),  F(1.53184106E-02),
    F(1.29371806E-02),  F(8.85757540E-03),
    F(2.92408442E-03), -F(4.91578024E-03),
    -F(1.46404076E-02),  F(2.61098752E-02),
    F(3.90751381E-02),  F(5.31873032E-02),

    F(6.79989431E-02),  F(8.29847578E-02),
    F(9.75753918E-02),  F(1.11196689E-01),
    -F(1.23264548E-01),  F(1.33264415E-01),
    F(1.40753505E-01),  F(1.45389847E-01),
    F(1.46955068E-01),  F(1.45389847E-01),
    F(1.40753505E-01),  F(1.33264415E-01),
    F(1.23264548E-01), -F(1.11196689E-01),
    -F(9.75753918E-02), -F(8.29847578E-02),

    -F(6.79989431E-02), -F(5.31873032E-02),
    -F(3.90751381E-02), -F(2.61098752E-02),
    F(1.46404076E-02), -F(4.91578024E-03),
    F(2.92408442E-03),  F(8.85757540E-03),
    F(1.29371806E-02),  F(1.53184106E-02),
    F(1.62208471E-02),  F(1.59045603E-02),
    F(1.46525263E-02), -F(1.27472335E-02),
    -F(1.04584443E-02), -F(8.02941163E-03),

    -F(5.65949473E-03), -F(3.49717454E-03),
    -F(1.64973098E-03), -F(1.78805361E-04),
    -F(9.02154502E-04),  F(1.61656283E-03),
    F(1.99454554E-03),  F(2.10371989E-03),
    F(2.01182542E-03),  F(1.78371725E-03),
    F(1.47640169E-03),  F(1.13992507E-03),
    F(8.23919506E-04), -F(5.54620202E-04),
    -F(3.43256425E-04), -F(1.56575398E-04),
};
#undef F

/*
 * To produce this cosine matrix in Octave:
 *
 * b = zeros(8, 16);
 * for i = 0:7
 * for j = 0:15 b(i+1, j+1) = cos((i + 0.5) * (j - 4) * (pi/8))
 * endfor endfor;
 * printf("%.10f, ", b');
 *
 * Note: in each block of 16 numbers sign was changed for elements 4, 13, 14, 15
 *
 * Change of sign for element 4 allows to replace constant 1.0 (not
 * representable in Q15 format) with -1.0 (fine with Q15).
 * Changed signs for elements 13, 14, 15 allow to have more similar constants
 * and simplify subband filter function code.
 */
#define SBC_COS_TABLE_FIXED8_SCALE      ((sizeof(FIXED_T) * 8 - 1) + SBC_FIXED_EXTRA_BITS)
#define F_COS8(x)                       (FIXED_A)((x) *((FIXED_A) 1 << (sizeof(FIXED_T) * 8 - 1)) + 0.5)
#define F(x)                            F_COS8(x)

static
#if(!SPEED_OVER_RAM)
const
#endif
FIXED_T cos_table_fixed_8[128] = {
    F(0.7071067812),  F(0.8314696123),  F(0.9238795325),  F(0.9807852804),
    -F(1.0000000000),  F(0.9807852804),  F(0.9238795325),  F(0.8314696123),
    F(0.7071067812),  F(0.5555702330),  F(0.3826834324),  F(0.1950903220),
    F(0.0000000000),  F(0.1950903220),  F(0.3826834324),  F(0.5555702330),

    -F(0.7071067812), -F(0.1950903220),  F(0.3826834324),  F(0.8314696123),
    -F(1.0000000000),  F(0.8314696123),  F(0.3826834324), -F(0.1950903220),
    -F(0.7071067812), -F(0.9807852804), -F(0.9238795325), -F(0.5555702330),
    -F(0.0000000000), -F(0.5555702330), -F(0.9238795325), -F(0.9807852804),

    -F(0.7071067812), -F(0.9807852804), -F(0.3826834324),  F(0.5555702330),
    -F(1.0000000000),  F(0.5555702330), -F(0.3826834324), -F(0.9807852804),
    -F(0.7071067812),  F(0.1950903220),  F(0.9238795325),  F(0.8314696123),
    F(0.0000000000),  F(0.8314696123),  F(0.9238795325),  F(0.1950903220),

    F(0.7071067812), -F(0.5555702330), -F(0.9238795325),  F(0.1950903220),
    -F(1.0000000000),  F(0.1950903220), -F(0.9238795325), -F(0.5555702330),
    F(0.7071067812),  F(0.8314696123), -F(0.3826834324), -F(0.9807852804),
    -F(0.0000000000), -F(0.9807852804), -F(0.3826834324),  F(0.8314696123),

    F(0.7071067812),  F(0.5555702330), -F(0.9238795325), -F(0.1950903220),
    -F(1.0000000000), -F(0.1950903220), -F(0.9238795325),  F(0.5555702330),
    F(0.7071067812), -F(0.8314696123), -F(0.3826834324),  F(0.9807852804),
    F(0.0000000000),  F(0.9807852804), -F(0.3826834324), -F(0.8314696123),

    -F(0.7071067812),  F(0.9807852804), -F(0.3826834324), -F(0.5555702330),
    -F(1.0000000000), -F(0.5555702330), -F(0.3826834324),  F(0.9807852804),
    -F(0.7071067812), -F(0.1950903220),  F(0.9238795325), -F(0.8314696123),
    -F(0.0000000000), -F(0.8314696123),  F(0.9238795325), -F(0.1950903220),

    -F(0.7071067812),  F(0.1950903220),  F(0.3826834324), -F(0.8314696123),
    -F(1.0000000000), -F(0.8314696123),  F(0.3826834324),  F(0.1950903220),
    -F(0.7071067812),  F(0.9807852804), -F(0.9238795325),  F(0.5555702330),
    -F(0.0000000000),  F(0.5555702330), -F(0.9238795325),  F(0.9807852804),

    F(0.7071067812), -F(0.8314696123),  F(0.9238795325), -F(0.9807852804),
    -F(1.0000000000), -F(0.9807852804),  F(0.9238795325), -F(0.8314696123),
    F(0.7071067812), -F(0.5555702330),  F(0.3826834324), -F(0.1950903220),
    -F(0.0000000000), -F(0.1950903220),  F(0.3826834324), -F(0.5555702330),
};
#undef F

/*
 * Enforce 16 byte alignment for the data, which is supposed to be used
 * with SIMD optimized code.
 */

#define SBC_ALIGN_BITS          4
#define SBC_ALIGN_MASK          ((1 << (SBC_ALIGN_BITS)) - 1)

#ifdef __GNUC__
#define SBC_ALIGNED __attribute__((aligned(1 << (SBC_ALIGN_BITS))))
#else
#define SBC_ALIGNED
#endif

/*
 * Constant tables for the use in SIMD optimized analysis filters
 * Each table consists of two parts:
 * 1. reordered "proto" table
 * 2. reordered "cos" table
 *
 * Due to non-symmetrical reordering, separate tables for "even"
 * and "odd" cases are needed
 */

static
#if(!SPEED_OVER_RAM)
const
#endif
FIXED_T SBC_ALIGNED analysis_consts_fixed4_simd_even[40 + 16] = {
#define C0 1.0932568993
#define C1 1.3056875580
#define C2 1.3056875580
#define C3 1.6772280856

#define F(x) F_PROTO4(x)
     F(0.00000000E+00 * C0),  F(3.83720193E-03 * C0),
     F(5.36548976E-04 * C1),  F(2.73370904E-03 * C1),
     F(3.06012286E-03 * C2),  F(3.89205149E-03 * C2),
     F(0.00000000E+00 * C3), -F(1.49188357E-03 * C3),
     F(1.09137620E-02 * C0),  F(2.58767811E-02 * C0),
     F(2.04385087E-02 * C1),  F(3.21939290E-02 * C1),
     F(7.76463494E-02 * C2),  F(6.13245186E-03 * C2),
     F(0.00000000E+00 * C3), -F(2.88757392E-02 * C3),
     F(1.35593274E-01 * C0),  F(2.94315332E-01 * C0),
     F(1.94987841E-01 * C1),  F(2.81828203E-01 * C1),
    -F(1.94987841E-01 * C2),  F(2.81828203E-01 * C2),
     F(0.00000000E+00 * C3), -F(2.46636662E-01 * C3),
    -F(1.35593274E-01 * C0),  F(2.58767811E-02 * C0),
    -F(7.76463494E-02 * C1),  F(6.13245186E-03 * C1),
    -F(2.04385087E-02 * C2),  F(3.21939290E-02 * C2),
     F(0.00000000E+00 * C3),  F(2.88217274E-02 * C3),
    -F(1.09137620E-02 * C0),  F(3.83720193E-03 * C0),
    -F(3.06012286E-03 * C1),  F(3.89205149E-03 * C1),
    -F(5.36548976E-04 * C2),  F(2.73370904E-03 * C2),
     F(0.00000000E+00 * C3), -F(1.86581691E-03 * C3),
#undef F
#define F(x) F_COS4(x)
     F(0.7071067812 / C0),  F(0.9238795325 / C1),
    -F(0.7071067812 / C0),  F(0.3826834324 / C1),
    -F(0.7071067812 / C0), -F(0.3826834324 / C1),
     F(0.7071067812 / C0), -F(0.9238795325 / C1),
     F(0.3826834324 / C2), -F(1.0000000000 / C3),
    -F(0.9238795325 / C2), -F(1.0000000000 / C3),
     F(0.9238795325 / C2), -F(1.0000000000 / C3),
    -F(0.3826834324 / C2), -F(1.0000000000 / C3),
#undef F

#undef C0
#undef C1
#undef C2
#undef C3
};

static
#if(!SPEED_OVER_RAM)
const
#endif
FIXED_T SBC_ALIGNED analysis_consts_fixed4_simd_odd[40 + 16] = {
#define C0 1.3056875580
#define C1 1.6772280856
#define C2 1.0932568993
#define C3 1.3056875580

#define F(x) F_PROTO4(x)
     F(2.73370904E-03 * C0),  F(5.36548976E-04 * C0),
    -F(1.49188357E-03 * C1),  F(0.00000000E+00 * C1),
     F(3.83720193E-03 * C2),  F(1.09137620E-02 * C2),
     F(3.89205149E-03 * C3),  F(3.06012286E-03 * C3),
     F(3.21939290E-02 * C0),  F(2.04385087E-02 * C0),
    -F(2.88757392E-02 * C1),  F(0.00000000E+00 * C1),
     F(2.58767811E-02 * C2),  F(1.35593274E-01 * C2),
     F(6.13245186E-03 * C3),  F(7.76463494E-02 * C3),
     F(2.81828203E-01 * C0),  F(1.94987841E-01 * C0),
    -F(2.46636662E-01 * C1),  F(0.00000000E+00 * C1),
     F(2.94315332E-01 * C2), -F(1.35593274E-01 * C2),
     F(2.81828203E-01 * C3), -F(1.94987841E-01 * C3),
     F(6.13245186E-03 * C0), -F(7.76463494E-02 * C0),
     F(2.88217274E-02 * C1),  F(0.00000000E+00 * C1),
     F(2.58767811E-02 * C2), -F(1.09137620E-02 * C2),
     F(3.21939290E-02 * C3), -F(2.04385087E-02 * C3),
     F(3.89205149E-03 * C0), -F(3.06012286E-03 * C0),
    -F(1.86581691E-03 * C1),  F(0.00000000E+00 * C1),
     F(3.83720193E-03 * C2),  F(0.00000000E+00 * C2),
     F(2.73370904E-03 * C3), -F(5.36548976E-04 * C3),
#undef F
#define F(x) F_COS4(x)
     F(0.9238795325 / C0), -F(1.0000000000 / C1),
     F(0.3826834324 / C0), -F(1.0000000000 / C1),
    -F(0.3826834324 / C0), -F(1.0000000000 / C1),
    -F(0.9238795325 / C0), -F(1.0000000000 / C1),
     F(0.7071067812 / C2),  F(0.3826834324 / C3),
    -F(0.7071067812 / C2), -F(0.9238795325 / C3),
    -F(0.7071067812 / C2),  F(0.9238795325 / C3),
     F(0.7071067812 / C2), -F(0.3826834324 / C3),
#undef F

#undef C0
#undef C1
#undef C2
#undef C3
};

static
#if(!SPEED_OVER_RAM)
const
#endif
FIXED_T SBC_ALIGNED analysis_consts_fixed8_simd_even[80 + 64] = {
#define C0 2.7906148894
#define C1 2.4270044280
#define C2 2.8015616024
#define C3 3.1710363741
#define C4 2.5377944043
#define C5 2.4270044280
#define C6 2.8015616024
#define C7 3.1710363741

#define F(x) F_PROTO8(x)
     F(0.00000000E+00 * C0),  F(2.01182542E-03 * C0),
     F(1.56575398E-04 * C1),  F(1.78371725E-03 * C1),
     F(3.43256425E-04 * C2),  F(1.47640169E-03 * C2),
     F(5.54620202E-04 * C3),  F(1.13992507E-03 * C3),
    -F(8.23919506E-04 * C4),  F(0.00000000E+00 * C4),
     F(2.10371989E-03 * C5),  F(3.49717454E-03 * C5),
     F(1.99454554E-03 * C6),  F(1.64973098E-03 * C6),
     F(1.61656283E-03 * C7),  F(1.78805361E-04 * C7),
     F(5.65949473E-03 * C0),  F(1.29371806E-02 * C0),
     F(8.02941163E-03 * C1),  F(1.53184106E-02 * C1),
     F(1.04584443E-02 * C2),  F(1.62208471E-02 * C2),
     F(1.27472335E-02 * C3),  F(1.59045603E-02 * C3),
    -F(1.46525263E-02 * C4),  F(0.00000000E+00 * C4),
     F(8.85757540E-03 * C5),  F(5.31873032E-02 * C5),
     F(2.92408442E-03 * C6),  F(3.90751381E-02 * C6),
    -F(4.91578024E-03 * C7),  F(2.61098752E-02 * C7),
     F(6.79989431E-02 * C0),  F(1.46955068E-01 * C0),
     F(8.29847578E-02 * C1),  F(1.45389847E-01 * C1),
     F(9.75753918E-02 * C2),  F(1.40753505E-01 * C2),
     F(1.11196689E-01 * C3),  F(1.33264415E-01 * C3),
    -F(1.23264548E-01 * C4),  F(0.00000000E+00 * C4),
     F(1.45389847E-01 * C5), -F(8.29847578E-02 * C5),
     F(1.40753505E-01 * C6), -F(9.75753918E-02 * C6),
     F(1.33264415E-01 * C7), -F(1.11196689E-01 * C7),
    -F(6.79989431E-02 * C0),  F(1.29371806E-02 * C0),
    -F(5.31873032E-02 * C1),  F(8.85757540E-03 * C1),
    -F(3.90751381E-02 * C2),  F(2.92408442E-03 * C2),
    -F(2.61098752E-02 * C3), -F(4.91578024E-03 * C3),
     F(1.46404076E-02 * C4),  F(0.00000000E+00 * C4),
     F(1.53184106E-02 * C5), -F(8.02941163E-03 * C5),
     F(1.62208471E-02 * C6), -F(1.04584443E-02 * C6),
     F(1.59045603E-02 * C7), -F(1.27472335E-02 * C7),
    -F(5.65949473E-03 * C0),  F(2.01182542E-03 * C0),
    -F(3.49717454E-03 * C1),  F(2.10371989E-03 * C1),
    -F(1.64973098E-03 * C2),  F(1.99454554E-03 * C2),
    -F(1.78805361E-04 * C3),  F(1.61656283E-03 * C3),
    -F(9.02154502E-04 * C4),  F(0.00000000E+00 * C4),
     F(1.78371725E-03 * C5), -F(1.56575398E-04 * C5),
     F(1.47640169E-03 * C6), -F(3.43256425E-04 * C6),
     F(1.13992507E-03 * C7), -F(5.54620202E-04 * C7),
#undef F
#define F(x) F_COS8(x)
     F(0.7071067812 / C0),  F(0.8314696123 / C1),
    -F(0.7071067812 / C0), -F(0.1950903220 / C1),
    -F(0.7071067812 / C0), -F(0.9807852804 / C1),
     F(0.7071067812 / C0), -F(0.5555702330 / C1),
     F(0.7071067812 / C0),  F(0.5555702330 / C1),
    -F(0.7071067812 / C0),  F(0.9807852804 / C1),
    -F(0.7071067812 / C0),  F(0.1950903220 / C1),
     F(0.7071067812 / C0), -F(0.8314696123 / C1),
     F(0.9238795325 / C2),  F(0.9807852804 / C3),
     F(0.3826834324 / C2),  F(0.8314696123 / C3),
    -F(0.3826834324 / C2),  F(0.5555702330 / C3),
    -F(0.9238795325 / C2),  F(0.1950903220 / C3),
    -F(0.9238795325 / C2), -F(0.1950903220 / C3),
    -F(0.3826834324 / C2), -F(0.5555702330 / C3),
     F(0.3826834324 / C2), -F(0.8314696123 / C3),
     F(0.9238795325 / C2), -F(0.9807852804 / C3),
    -F(1.0000000000 / C4),  F(0.5555702330 / C5),
    -F(1.0000000000 / C4), -F(0.9807852804 / C5),
    -F(1.0000000000 / C4),  F(0.1950903220 / C5),
    -F(1.0000000000 / C4),  F(0.8314696123 / C5),
    -F(1.0000000000 / C4), -F(0.8314696123 / C5),
    -F(1.0000000000 / C4), -F(0.1950903220 / C5),
    -F(1.0000000000 / C4),  F(0.9807852804 / C5),
    -F(1.0000000000 / C4), -F(0.5555702330 / C5),
     F(0.3826834324 / C6),  F(0.1950903220 / C7),
    -F(0.9238795325 / C6), -F(0.5555702330 / C7),
     F(0.9238795325 / C6),  F(0.8314696123 / C7),
    -F(0.3826834324 / C6), -F(0.9807852804 / C7),
    -F(0.3826834324 / C6),  F(0.9807852804 / C7),
     F(0.9238795325 / C6), -F(0.8314696123 / C7),
    -F(0.9238795325 / C6),  F(0.5555702330 / C7),
     F(0.3826834324 / C6), -F(0.1950903220 / C7),
#undef F

#undef C0
#undef C1
#undef C2
#undef C3
#undef C4
#undef C5
#undef C6
#undef C7
};

static
#if(!SPEED_OVER_RAM)
const
#endif
FIXED_T SBC_ALIGNED analysis_consts_fixed8_simd_odd[80 + 64] = {
#define C0 2.5377944043
#define C1 2.4270044280
#define C2 2.8015616024
#define C3 3.1710363741
#define C4 2.7906148894
#define C5 2.4270044280
#define C6 2.8015616024
#define C7 3.1710363741

#define F(x) F_PROTO8(x)
     F(0.00000000E+00 * C0), -F(8.23919506E-04 * C0),
     F(1.56575398E-04 * C1),  F(1.78371725E-03 * C1),
     F(3.43256425E-04 * C2),  F(1.47640169E-03 * C2),
     F(5.54620202E-04 * C3),  F(1.13992507E-03 * C3),
     F(2.01182542E-03 * C4),  F(5.65949473E-03 * C4),
     F(2.10371989E-03 * C5),  F(3.49717454E-03 * C5),
     F(1.99454554E-03 * C6),  F(1.64973098E-03 * C6),
     F(1.61656283E-03 * C7),  F(1.78805361E-04 * C7),
     F(0.00000000E+00 * C0), -F(1.46525263E-02 * C0),
     F(8.02941163E-03 * C1),  F(1.53184106E-02 * C1),
     F(1.04584443E-02 * C2),  F(1.62208471E-02 * C2),
     F(1.27472335E-02 * C3),  F(1.59045603E-02 * C3),
     F(1.29371806E-02 * C4),  F(6.79989431E-02 * C4),
     F(8.85757540E-03 * C5),  F(5.31873032E-02 * C5),
     F(2.92408442E-03 * C6),  F(3.90751381E-02 * C6),
    -F(4.91578024E-03 * C7),  F(2.61098752E-02 * C7),
     F(0.00000000E+00 * C0), -F(1.23264548E-01 * C0),
     F(8.29847578E-02 * C1),  F(1.45389847E-01 * C1),
     F(9.75753918E-02 * C2),  F(1.40753505E-01 * C2),
     F(1.11196689E-01 * C3),  F(1.33264415E-01 * C3),
     F(1.46955068E-01 * C4), -F(6.79989431E-02 * C4),
     F(1.45389847E-01 * C5), -F(8.29847578E-02 * C5),
     F(1.40753505E-01 * C6), -F(9.75753918E-02 * C6),
     F(1.33264415E-01 * C7), -F(1.11196689E-01 * C7),
     F(0.00000000E+00 * C0),  F(1.46404076E-02 * C0),
    -F(5.31873032E-02 * C1),  F(8.85757540E-03 * C1),
    -F(3.90751381E-02 * C2),  F(2.92408442E-03 * C2),
    -F(2.61098752E-02 * C3), -F(4.91578024E-03 * C3),
     F(1.29371806E-02 * C4), -F(5.65949473E-03 * C4),
     F(1.53184106E-02 * C5), -F(8.02941163E-03 * C5),
     F(1.62208471E-02 * C6), -F(1.04584443E-02 * C6),
     F(1.59045603E-02 * C7), -F(1.27472335E-02 * C7),
     F(0.00000000E+00 * C0), -F(9.02154502E-04 * C0),
    -F(3.49717454E-03 * C1),  F(2.10371989E-03 * C1),
    -F(1.64973098E-03 * C2),  F(1.99454554E-03 * C2),
    -F(1.78805361E-04 * C3),  F(1.61656283E-03 * C3),
     F(2.01182542E-03 * C4),  F(0.00000000E+00 * C4),
     F(1.78371725E-03 * C5), -F(1.56575398E-04 * C5),
     F(1.47640169E-03 * C6), -F(3.43256425E-04 * C6),
     F(1.13992507E-03 * C7), -F(5.54620202E-04 * C7),
#undef F
#define F(x) F_COS8(x)
    -F(1.0000000000 / C0),  F(0.8314696123 / C1),
    -F(1.0000000000 / C0), -F(0.1950903220 / C1),
    -F(1.0000000000 / C0), -F(0.9807852804 / C1),
    -F(1.0000000000 / C0), -F(0.5555702330 / C1),
    -F(1.0000000000 / C0),  F(0.5555702330 / C1),
    -F(1.0000000000 / C0),  F(0.9807852804 / C1),
    -F(1.0000000000 / C0),  F(0.1950903220 / C1),
    -F(1.0000000000 / C0), -F(0.8314696123 / C1),
     F(0.9238795325 / C2),  F(0.9807852804 / C3),
     F(0.3826834324 / C2),  F(0.8314696123 / C3),
    -F(0.3826834324 / C2),  F(0.5555702330 / C3),
    -F(0.9238795325 / C2),  F(0.1950903220 / C3),
    -F(0.9238795325 / C2), -F(0.1950903220 / C3),
    -F(0.3826834324 / C2), -F(0.5555702330 / C3),
     F(0.3826834324 / C2), -F(0.8314696123 / C3),
     F(0.9238795325 / C2), -F(0.9807852804 / C3),
     F(0.7071067812 / C4),  F(0.5555702330 / C5),
    -F(0.7071067812 / C4), -F(0.9807852804 / C5),
    -F(0.7071067812 / C4),  F(0.1950903220 / C5),
     F(0.7071067812 / C4),  F(0.8314696123 / C5),
     F(0.7071067812 / C4), -F(0.8314696123 / C5),
    -F(0.7071067812 / C4), -F(0.1950903220 / C5),
    -F(0.7071067812 / C4),  F(0.9807852804 / C5),
     F(0.7071067812 / C4), -F(0.5555702330 / C5),
     F(0.3826834324 / C6),  F(0.1950903220 / C7),
    -F(0.9238795325 / C6), -F(0.5555702330 / C7),
     F(0.9238795325 / C6),  F(0.8314696123 / C7),
    -F(0.3826834324 / C6), -F(0.9807852804 / C7),
    -F(0.3826834324 / C6),  F(0.9807852804 / C7),
     F(0.9238795325 / C6), -F(0.8314696123 / C7),
    -F(0.9238795325 / C6),  F(0.5555702330 / C7),
     F(0.3826834324 / C6), -F(0.1950903220 / C7),
#undef F

#undef C0
#undef C1
#undef C2
#undef C3
#undef C4
#undef C5
#undef C6
#undef C7
};

#define SBC_INCREMENT           1       //(MSBC_ENABLE ? 1 : 4)
#define SCALE_OUT_BITS          15
#define SBC_X_BUFFER_SIZE       328

static int16_t position         = (SBC_X_BUFFER_SIZE - SBC_SUBBANDS * 9) & ~7;
static uint8_t sbc_analyze_even = 0;

static int16_t X[SBC_CHANNELS][SBC_X_BUFFER_SIZE];
static uint32_t scale_factor[SBC_CHANNELS][SBC_SUBBANDS];

/* raw integer subband samples in the frame */
static int32_t sb_sample_f[SBC_BLOCKS][SBC_CHANNELS][SBC_SUBBANDS];


/* Calculates the CRC-8 of the first len bits in data*/
static
#if(!SPEED_OVER_RAM)
const
#endif
uint8_t crc_table[256] = {
    0x00, 0x1D, 0x3A, 0x27, 0x74, 0x69, 0x4E, 0x53,
    0xE8, 0xF5, 0xD2, 0xCF, 0x9C, 0x81, 0xA6, 0xBB,
    0xCD, 0xD0, 0xF7, 0xEA, 0xB9, 0xA4, 0x83, 0x9E,
    0x25, 0x38, 0x1F, 0x02, 0x51, 0x4C, 0x6B, 0x76,
    0x87, 0x9A, 0xBD, 0xA0, 0xF3, 0xEE, 0xC9, 0xD4,
    0x6F, 0x72, 0x55, 0x48, 0x1B, 0x06, 0x21, 0x3C,
    0x4A, 0x57, 0x70, 0x6D, 0x3E, 0x23, 0x04, 0x19,
    0xA2, 0xBF, 0x98, 0x85, 0xD6, 0xCB, 0xEC, 0xF1,
    0x13, 0x0E, 0x29, 0x34, 0x67, 0x7A, 0x5D, 0x40,
    0xFB, 0xE6, 0xC1, 0xDC, 0x8F, 0x92, 0xB5, 0xA8,
    0xDE, 0xC3, 0xE4, 0xF9, 0xAA, 0xB7, 0x90, 0x8D,
    0x36, 0x2B, 0x0C, 0x11, 0x42, 0x5F, 0x78, 0x65,
    0x94, 0x89, 0xAE, 0xB3, 0xE0, 0xFD, 0xDA, 0xC7,
    0x7C, 0x61, 0x46, 0x5B, 0x08, 0x15, 0x32, 0x2F,
    0x59, 0x44, 0x63, 0x7E, 0x2D, 0x30, 0x17, 0x0A,
    0xB1, 0xAC, 0x8B, 0x96, 0xC5, 0xD8, 0xFF, 0xE2,
    0x26, 0x3B, 0x1C, 0x01, 0x52, 0x4F, 0x68, 0x75,
    0xCE, 0xD3, 0xF4, 0xE9, 0xBA, 0xA7, 0x80, 0x9D,
    0xEB, 0xF6, 0xD1, 0xCC, 0x9F, 0x82, 0xA5, 0xB8,
    0x03, 0x1E, 0x39, 0x24, 0x77, 0x6A, 0x4D, 0x50,
    0xA1, 0xBC, 0x9B, 0x86, 0xD5, 0xC8, 0xEF, 0xF2,
    0x49, 0x54, 0x73, 0x6E, 0x3D, 0x20, 0x07, 0x1A,
    0x6C, 0x71, 0x56, 0x4B, 0x18, 0x05, 0x22, 0x3F,
    0x84, 0x99, 0xBE, 0xA3, 0xF0, 0xED, 0xCA, 0xD7,
    0x35, 0x28, 0x0F, 0x12, 0x41, 0x5C, 0x7B, 0x66,
    0xDD, 0xC0, 0xE7, 0xFA, 0xA9, 0xB4, 0x93, 0x8E,
    0xF8, 0xE5, 0xC2, 0xDF, 0x8C, 0x91, 0xB6, 0xAB,
    0x10, 0x0D, 0x2A, 0x37, 0x64, 0x79, 0x5E, 0x43,
    0xB2, 0xAF, 0x88, 0x95, 0xC6, 0xDB, 0xFC, 0xE1,
    0x5A, 0x47, 0x60, 0x7D, 0x2E, 0x33, 0x14, 0x09,
    0x7F, 0x62, 0x45, 0x58, 0x0B, 0x16, 0x31, 0x2C,
    0x97, 0x8A, 0xAD, 0xB0, 0xE3, 0xFE, 0xD9, 0xC4
};

static inline uint8_t sbc_crc8(const uint8_t *data, size_t len)
{
    uint8_t crc = 0x0f;
    size_t i;
    uint8_t octet;

    for (i = 0; i < len / 8; i++)
        crc = crc_table[crc ^ data[i]];

    octet = len % 8 ? data[i] : 0;
    for (i = 0; i < len % 8; i++) {
        char bit = ((octet ^ crc) & 0x80) >> 7;

        crc = ((crc & 0x7f) << 1) ^ (bit ? 0x1d : 0);

        octet = octet << 1;
    }

    return crc;
}

/*
 * Code straight from the spec to calculate the bits array
 * Takes a pointer to the frame in question, a pointer to the bits array and
 * the sampling frequency (as 2 bit integer)
 */
#if SPEED_OVER_RAM
_attribute_ram_code_
#endif
static inline void sbc_calculate_bits_internal(int (*bits)[8])
{
    if (SBC_MODE == SBC_MODE_MONO || SBC_MODE == SBC_MODE_DUAL_CHANNEL) {
        int bitneed[2][8], loudness, max_bitneed, bitcount, slicecount, bitslice;
        int ch, sb;

        for (ch = 0; ch < SBC_CHANNELS; ch++) {
            max_bitneed = 0;
            if (SBC_SNR) {
                for (sb = 0; sb < SBC_SUBBANDS; sb++) {
                    bitneed[ch][sb] = scale_factor[ch][sb];
                    if (bitneed[ch][sb] > max_bitneed)
                        max_bitneed = bitneed[ch][sb];
                }
            } else {
                for (sb = 0; sb < SBC_SUBBANDS; sb++) {
                    if (scale_factor[ch][sb] == 0)
                        bitneed[ch][sb] = -5;
                    else {
                        if (SBC_SUBBANDS == 4)
                            loudness = scale_factor[ch][sb] - sbc_offset4[SBC_SR_TYPE][sb];
                        else
                            loudness = scale_factor[ch][sb] - sbc_offset8[SBC_SR_TYPE][sb];
                        if (loudness > 0)
                            bitneed[ch][sb] = loudness / 2;
                        else
                            bitneed[ch][sb] = loudness;
                    }
                    if (bitneed[ch][sb] > max_bitneed)
                        max_bitneed = bitneed[ch][sb];
                }
            }

            bitcount = 0;
            slicecount = 0;
            bitslice = max_bitneed + 1;
            do {
                bitslice--;
                bitcount += slicecount;
                slicecount = 0;
                for (sb = 0; sb < SBC_SUBBANDS; sb++) {
                    if ((bitneed[ch][sb] > bitslice + 1) && (bitneed[ch][sb] < bitslice + 16))
                        slicecount++;
                    else if (bitneed[ch][sb] == bitslice + 1)
                        slicecount += 2;
                }
            } while (bitcount + slicecount < SBC_BITPOOL);

            if (bitcount + slicecount == SBC_BITPOOL) {
                bitcount += slicecount;
                bitslice--;
            }

            for (sb = 0; sb < SBC_SUBBANDS; sb++) {
                if (bitneed[ch][sb] < bitslice + 2)
                    bits[ch][sb] = 0;
                else {
                    bits[ch][sb] = bitneed[ch][sb] - bitslice;
                    if (bits[ch][sb] > 16)
                        bits[ch][sb] = 16;
                }
            }

            for (sb = 0; bitcount < SBC_BITPOOL &&
                            sb < SBC_SUBBANDS; sb++) {
                if ((bits[ch][sb] >= 2) && (bits[ch][sb] < 16)) {
                    bits[ch][sb]++;
                    bitcount++;
                } else if ((bitneed[ch][sb] == bitslice + 1) && (SBC_BITPOOL > bitcount + 1)) {
                    bits[ch][sb] = 2;
                    bitcount += 2;
                }
            }

            for (sb = 0; bitcount < SBC_BITPOOL &&
                            sb < SBC_SUBBANDS; sb++) {
                if (bits[ch][sb] < 16) {
                    bits[ch][sb]++;
                    bitcount++;
                }
            }

        }

    }
    else if (SBC_MODE == SBC_MODE_STEREO || SBC_MODE == SBC_MODE_JOINT_STEREO) {
        int bitneed[2][8], loudness, max_bitneed, bitcount, slicecount, bitslice;
        int ch, sb;

        max_bitneed = 0;
        if (SBC_SNR) {
            for (ch = 0; ch < 2; ch++) {
                for (sb = 0; sb < SBC_SUBBANDS; sb++) {
                    bitneed[ch][sb] = scale_factor[ch][sb];
                    if (bitneed[ch][sb] > max_bitneed)
                        max_bitneed = bitneed[ch][sb];
                }
            }
        } else {
            for (ch = 0; ch < 2; ch++) {
                for (sb = 0; sb < SBC_SUBBANDS; sb++) {
                    if (scale_factor[ch][sb] == 0)
                        bitneed[ch][sb] = -5;
                    else {
                        if (SBC_SUBBANDS == 4)
                            loudness = scale_factor[ch][sb] - sbc_offset4[SBC_SR_TYPE][sb];
                        else
                            loudness = scale_factor[ch][sb] - sbc_offset8[SBC_SR_TYPE][sb];
                        if (loudness > 0)
                            bitneed[ch][sb] = loudness / 2;
                        else
                            bitneed[ch][sb] = loudness;
                    }
                    if (bitneed[ch][sb] > max_bitneed)
                        max_bitneed = bitneed[ch][sb];
                }
            }
        }

        bitcount = 0;
        slicecount = 0;
        bitslice = max_bitneed + 1;
        do {
            bitslice--;
            bitcount += slicecount;
            slicecount = 0;
            for (ch = 0; ch < 2; ch++) {
                for (sb = 0; sb < SBC_SUBBANDS; sb++) {
                    if ((bitneed[ch][sb] > bitslice + 1) && (bitneed[ch][sb] < bitslice + 16))
                        slicecount++;
                    else if (bitneed[ch][sb] == bitslice + 1)
                        slicecount += 2;
                }
            }
        } while (bitcount + slicecount < SBC_BITPOOL);

        if (bitcount + slicecount == SBC_BITPOOL) {
            bitcount += slicecount;
            bitslice--;
        }

        for (ch = 0; ch < 2; ch++) {
            for (sb = 0; sb < SBC_SUBBANDS; sb++) {
                if (bitneed[ch][sb] < bitslice + 2) {
                    bits[ch][sb] = 0;
                } else {
                    bits[ch][sb] = bitneed[ch][sb] - bitslice;
                    if (bits[ch][sb] > 16)
                        bits[ch][sb] = 16;
                }
            }
        }

        ch = 0;
        sb = 0;
        while (bitcount < SBC_BITPOOL) {
            if ((bits[ch][sb] >= 2) && (bits[ch][sb] < 16)) {
                bits[ch][sb]++;
                bitcount++;
            } else if ((bitneed[ch][sb] == bitslice + 1) && (SBC_BITPOOL > bitcount + 1)) {
                bits[ch][sb] = 2;
                bitcount += 2;
            }
            if (ch == 1) {
                ch = 0;
                sb++;
                if (sb >= SBC_SUBBANDS)
                    break;
            } else
                ch = 1;
        }

        ch = 0;
        sb = 0;
        while (bitcount < SBC_BITPOOL) {
            if (bits[ch][sb] < 16) {
                bits[ch][sb]++;
                bitcount++;
            }
            if (ch == 1) {
                ch = 0;
                sb++;
                if (sb >= SBC_SUBBANDS)
                    break;
            } else
                ch = 1;
        }

    }

}

static inline void sbc_calculate_bits(int (*bits)[8]){
    sbc_calculate_bits_internal(bits);
}

static inline void sbc_analyze_four_simd(const int16_t *in, int32_t *out, const FIXED_T *consts)
{
    FIXED_A t1[4];
    FIXED_T t2[4];
    int hop = 0;

    /* rounding coefficient */
    t1[0] = t1[1] = t1[2] = t1[3] =
        (FIXED_A) 1 << (SBC_PROTO_FIXED4_SCALE - 1);

    /* low pass polyphase filter */
    for (hop = 0; hop < 40; hop += 8) {
        t1[0] += (FIXED_A) in[hop] * consts[hop];
        t1[0] += (FIXED_A) in[hop + 1] * consts[hop + 1];
        t1[1] += (FIXED_A) in[hop + 2] * consts[hop + 2];
        t1[1] += (FIXED_A) in[hop + 3] * consts[hop + 3];
        t1[2] += (FIXED_A) in[hop + 4] * consts[hop + 4];
        t1[2] += (FIXED_A) in[hop + 5] * consts[hop + 5];
        t1[3] += (FIXED_A) in[hop + 6] * consts[hop + 6];
        t1[3] += (FIXED_A) in[hop + 7] * consts[hop + 7];
    }

    /* scaling */
    t2[0] = t1[0] >> SBC_PROTO_FIXED4_SCALE;
    t2[1] = t1[1] >> SBC_PROTO_FIXED4_SCALE;
    t2[2] = t1[2] >> SBC_PROTO_FIXED4_SCALE;
    t2[3] = t1[3] >> SBC_PROTO_FIXED4_SCALE;

    /* do the cos transform */
    t1[0]  = (FIXED_A) t2[0] * consts[40 + 0];
    t1[0] += (FIXED_A) t2[1] * consts[40 + 1];
    t1[1]  = (FIXED_A) t2[0] * consts[40 + 2];
    t1[1] += (FIXED_A) t2[1] * consts[40 + 3];
    t1[2]  = (FIXED_A) t2[0] * consts[40 + 4];
    t1[2] += (FIXED_A) t2[1] * consts[40 + 5];
    t1[3]  = (FIXED_A) t2[0] * consts[40 + 6];
    t1[3] += (FIXED_A) t2[1] * consts[40 + 7];

    t1[0] += (FIXED_A) t2[2] * consts[40 + 8];
    t1[0] += (FIXED_A) t2[3] * consts[40 + 9];
    t1[1] += (FIXED_A) t2[2] * consts[40 + 10];
    t1[1] += (FIXED_A) t2[3] * consts[40 + 11];
    t1[2] += (FIXED_A) t2[2] * consts[40 + 12];
    t1[2] += (FIXED_A) t2[3] * consts[40 + 13];
    t1[3] += (FIXED_A) t2[2] * consts[40 + 14];
    t1[3] += (FIXED_A) t2[3] * consts[40 + 15];

    out[0] = t1[0] >>
        (SBC_COS_TABLE_FIXED4_SCALE - SCALE_OUT_BITS);
    out[1] = t1[1] >>
        (SBC_COS_TABLE_FIXED4_SCALE - SCALE_OUT_BITS);
    out[2] = t1[2] >>
        (SBC_COS_TABLE_FIXED4_SCALE - SCALE_OUT_BITS);
    out[3] = t1[3] >>
        (SBC_COS_TABLE_FIXED4_SCALE - SCALE_OUT_BITS);
}

static inline void sbc_analyze_eight_simd(const int16_t *in, int32_t *out,  const FIXED_T *consts)
{
    FIXED_A t1[8];
    FIXED_T t2[8];
    int i, hop;

    /* rounding coefficient */
    t1[0] = t1[1] = t1[2] = t1[3] = t1[4] = t1[5] = t1[6] = t1[7] =
        (FIXED_A) 1 << (SBC_PROTO_FIXED8_SCALE-1);

    /* low pass polyphase filter */
    for (hop = 0; hop < 80; hop += 16) {
        t1[0] += (FIXED_A) in[hop] * consts[hop];
        t1[0] += (FIXED_A) in[hop + 1] * consts[hop + 1];
        t1[1] += (FIXED_A) in[hop + 2] * consts[hop + 2];
        t1[1] += (FIXED_A) in[hop + 3] * consts[hop + 3];
        t1[2] += (FIXED_A) in[hop + 4] * consts[hop + 4];
        t1[2] += (FIXED_A) in[hop + 5] * consts[hop + 5];
        t1[3] += (FIXED_A) in[hop + 6] * consts[hop + 6];
        t1[3] += (FIXED_A) in[hop + 7] * consts[hop + 7];
        t1[4] += (FIXED_A) in[hop + 8] * consts[hop + 8];
        t1[4] += (FIXED_A) in[hop + 9] * consts[hop + 9];
        t1[5] += (FIXED_A) in[hop + 10] * consts[hop + 10];
        t1[5] += (FIXED_A) in[hop + 11] * consts[hop + 11];
        t1[6] += (FIXED_A) in[hop + 12] * consts[hop + 12];
        t1[6] += (FIXED_A) in[hop + 13] * consts[hop + 13];
        t1[7] += (FIXED_A) in[hop + 14] * consts[hop + 14];
        t1[7] += (FIXED_A) in[hop + 15] * consts[hop + 15];
    }

    /* scaling */
    t2[0] = t1[0] >> SBC_PROTO_FIXED8_SCALE;
    t2[1] = t1[1] >> SBC_PROTO_FIXED8_SCALE;
    t2[2] = t1[2] >> SBC_PROTO_FIXED8_SCALE;
    t2[3] = t1[3] >> SBC_PROTO_FIXED8_SCALE;
    t2[4] = t1[4] >> SBC_PROTO_FIXED8_SCALE;
    t2[5] = t1[5] >> SBC_PROTO_FIXED8_SCALE;
    t2[6] = t1[6] >> SBC_PROTO_FIXED8_SCALE;
    t2[7] = t1[7] >> SBC_PROTO_FIXED8_SCALE;


    /* do the cos transform */
    t1[0] = t1[1] = t1[2] = t1[3] = t1[4] = t1[5] = t1[6] = t1[7] = 0;

    for (i = 0; i < 4; i++) {
        t1[0] += (FIXED_A) t2[i * 2 + 0] * consts[80 + i * 16 + 0];
        t1[0] += (FIXED_A) t2[i * 2 + 1] * consts[80 + i * 16 + 1];
        t1[1] += (FIXED_A) t2[i * 2 + 0] * consts[80 + i * 16 + 2];
        t1[1] += (FIXED_A) t2[i * 2 + 1] * consts[80 + i * 16 + 3];
        t1[2] += (FIXED_A) t2[i * 2 + 0] * consts[80 + i * 16 + 4];
        t1[2] += (FIXED_A) t2[i * 2 + 1] * consts[80 + i * 16 + 5];
        t1[3] += (FIXED_A) t2[i * 2 + 0] * consts[80 + i * 16 + 6];
        t1[3] += (FIXED_A) t2[i * 2 + 1] * consts[80 + i * 16 + 7];
        t1[4] += (FIXED_A) t2[i * 2 + 0] * consts[80 + i * 16 + 8];
        t1[4] += (FIXED_A) t2[i * 2 + 1] * consts[80 + i * 16 + 9];
        t1[5] += (FIXED_A) t2[i * 2 + 0] * consts[80 + i * 16 + 10];
        t1[5] += (FIXED_A) t2[i * 2 + 1] * consts[80 + i * 16 + 11];
        t1[6] += (FIXED_A) t2[i * 2 + 0] * consts[80 + i * 16 + 12];
        t1[6] += (FIXED_A) t2[i * 2 + 1] * consts[80 + i * 16 + 13];
        t1[7] += (FIXED_A) t2[i * 2 + 0] * consts[80 + i * 16 + 14];
        t1[7] += (FIXED_A) t2[i * 2 + 1] * consts[80 + i * 16 + 15];
    }

    for (i = 0; i < 8; i++)
        out[i] = t1[i] >>
            (SBC_COS_TABLE_FIXED8_SCALE - SCALE_OUT_BITS);
}

static inline void sbc_analyze_4b_4s_simd(int16_t *x, int32_t *out, int out_stride){
    /* Analyze SBC_BLOCKS */
    sbc_analyze_four_simd(x + 12, out, analysis_consts_fixed4_simd_odd);
    out += out_stride;
    sbc_analyze_four_simd(x + 8, out, analysis_consts_fixed4_simd_even);
    out += out_stride;
    sbc_analyze_four_simd(x + 4, out, analysis_consts_fixed4_simd_odd);
    out += out_stride;
    sbc_analyze_four_simd(x + 0, out, analysis_consts_fixed4_simd_even);
}

static inline void sbc_analyze_4b_8s_simd(int16_t *x, int32_t *out, int out_stride){
    /* Analyze SBC_BLOCKS */
    sbc_analyze_eight_simd(x + 24, out, analysis_consts_fixed8_simd_odd);
    out += out_stride;
    sbc_analyze_eight_simd(x + 16, out, analysis_consts_fixed8_simd_even);
    out += out_stride;
    sbc_analyze_eight_simd(x + 8, out, analysis_consts_fixed8_simd_odd);
    out += out_stride;
    sbc_analyze_eight_simd(x + 0, out, analysis_consts_fixed8_simd_even);
}

static inline int16_t unaligned16_be(const uint8_t *ptr){
    return (int16_t) ((ptr[0] << 8) | ptr[1]);
}

static inline int16_t unaligned16_le(const uint8_t *ptr){
    return (int16_t) (ptr[0] | (ptr[1] << 8));
}

/*
 * Internal helper functions for input data processing. In order to get
 * optimal performance, it is important to have "nsamples", "nchannels"
 * and "big_endian" arguments used with this inline function as compile
 * time constants.
 */

static inline int sbc_encoder_process_input_s4_internal(
    int position,
    const uint8_t *pcm, int16_t X[SBC_CHANNELS][SBC_X_BUFFER_SIZE],
    int nsamples)
{
    /* handle X buffer wraparound */
    if (position < nsamples) {
        if (SBC_CHANNELS > 0)
            memcpy(&X[0][SBC_X_BUFFER_SIZE - 40], &X[0][position],
                            36 * sizeof(int16_t));
        if (SBC_CHANNELS > 1)
            memcpy(&X[1][SBC_X_BUFFER_SIZE - 40], &X[1][position],
                            36 * sizeof(int16_t));
        position = SBC_X_BUFFER_SIZE - 40;
    }

    #define PCM(i) (SBC_BIGENDIAN ? \
        unaligned16_be(pcm + (i) * 2) : unaligned16_le(pcm + (i) * 2))

    /* copy/permutate audio samples */
    while ((nsamples -= 8) >= 0) {
        position -= 8;
        if (SBC_CHANNELS > 0) {
            int16_t *x = &X[0][position];
            x[0]  = PCM(0 + 7 * SBC_CHANNELS);
            x[1]  = PCM(0 + 3 * SBC_CHANNELS);
            x[2]  = PCM(0 + 6 * SBC_CHANNELS);
            x[3]  = PCM(0 + 4 * SBC_CHANNELS);
            x[4]  = PCM(0 + 0 * SBC_CHANNELS);
            x[5]  = PCM(0 + 2 * SBC_CHANNELS);
            x[6]  = PCM(0 + 1 * SBC_CHANNELS);
            x[7]  = PCM(0 + 5 * SBC_CHANNELS);
        }
        if (SBC_CHANNELS > 1) {
            int16_t *x = &X[1][position];
            x[0]  = PCM(1 + 7 * SBC_CHANNELS);
            x[1]  = PCM(1 + 3 * SBC_CHANNELS);
            x[2]  = PCM(1 + 6 * SBC_CHANNELS);
            x[3]  = PCM(1 + 4 * SBC_CHANNELS);
            x[4]  = PCM(1 + 0 * SBC_CHANNELS);
            x[5]  = PCM(1 + 2 * SBC_CHANNELS);
            x[6]  = PCM(1 + 1 * SBC_CHANNELS);
            x[7]  = PCM(1 + 5 * SBC_CHANNELS);
        }
        pcm += 16 * SBC_CHANNELS;
    }
    #undef PCM

    return position;
}

#if SPEED_OVER_RAM
_attribute_ram_code_
#endif
static inline int sbc_encoder_process_input_s8_internal(
    int position,
    const uint8_t *pcm, int16_t X[SBC_CHANNELS][SBC_X_BUFFER_SIZE],
    int nsamples)
{
    /* handle X buffer wraparound */
    if (position < nsamples) {
        if (SBC_CHANNELS > 0)
            memcpy(&X[0][SBC_X_BUFFER_SIZE - 72], &X[0][position],
                            72 * sizeof(int16_t));
        if (SBC_CHANNELS > 1)
            memcpy(&X[1][SBC_X_BUFFER_SIZE - 72], &X[1][position],
                            72 * sizeof(int16_t));
        position = SBC_X_BUFFER_SIZE - 72;
    }

    #define PCM(i) (SBC_BIGENDIAN ? \
        unaligned16_be(pcm + (i) * 2) : unaligned16_le(pcm + (i) * 2))

    if (position % 16 == 8) {
        position -= 8;
        nsamples -= 8;
        if (SBC_CHANNELS > 0) {
            int16_t *x = &X[0][position];
            x[0]  = PCM(0 + (15-8) * SBC_CHANNELS);
            x[2]  = PCM(0 + (14-8) * SBC_CHANNELS);
            x[3]  = PCM(0 + (8-8) *  SBC_CHANNELS);
            x[4]  = PCM(0 + (13-8) * SBC_CHANNELS);
            x[5]  = PCM(0 + (9-8) *  SBC_CHANNELS);
            x[6]  = PCM(0 + (12-8) * SBC_CHANNELS);
            x[7]  = PCM(0 + (10-8) * SBC_CHANNELS);
            x[8]  = PCM(0 + (11-8) * SBC_CHANNELS);
        }
        if (SBC_CHANNELS > 1) {
            int16_t *x = &X[1][position];
            x[0]  = PCM(1 + (15-8) * SBC_CHANNELS);
            x[2]  = PCM(1 + (14-8) * SBC_CHANNELS);
            x[3]  = PCM(1 + (8-8) *  SBC_CHANNELS);
            x[4]  = PCM(1 + (13-8) * SBC_CHANNELS);
            x[5]  = PCM(1 + (9-8) *  SBC_CHANNELS);
            x[6]  = PCM(1 + (12-8) * SBC_CHANNELS);
            x[7]  = PCM(1 + (10-8) * SBC_CHANNELS);
            x[8]  = PCM(1 + (11-8) * SBC_CHANNELS);
        }

        pcm += 16 * SBC_CHANNELS;
    }

    /* copy/permutate audio samples */
    while (nsamples >= 16) {
        position -= 16;
        if (SBC_CHANNELS > 0) {
            int16_t *x = &X[0][position];
            x[0]  = PCM(0 + 15 * SBC_CHANNELS);
            x[1]  = PCM(0 + 7 *  SBC_CHANNELS);
            x[2]  = PCM(0 + 14 * SBC_CHANNELS);
            x[3]  = PCM(0 + 8 *  SBC_CHANNELS);
            x[4]  = PCM(0 + 13 * SBC_CHANNELS);
            x[5]  = PCM(0 + 9 *  SBC_CHANNELS);
            x[6]  = PCM(0 + 12 * SBC_CHANNELS);
            x[7]  = PCM(0 + 10 * SBC_CHANNELS);
            x[8]  = PCM(0 + 11 * SBC_CHANNELS);
            x[9]  = PCM(0 + 3 *  SBC_CHANNELS);
            x[10] = PCM(0 + 6 *  SBC_CHANNELS);
            x[11] = PCM(0 + 0 *  SBC_CHANNELS);
            x[12] = PCM(0 + 5 *  SBC_CHANNELS);
            x[13] = PCM(0 + 1 *  SBC_CHANNELS);
            x[14] = PCM(0 + 4 *  SBC_CHANNELS);
            x[15] = PCM(0 + 2 *  SBC_CHANNELS);
        }
        if (SBC_CHANNELS > 1) {
            int16_t *x = &X[1][position];
            x[0]  = PCM(1 + 15 * SBC_CHANNELS);
            x[1]  = PCM(1 + 7 *  SBC_CHANNELS);
            x[2]  = PCM(1 + 14 * SBC_CHANNELS);
            x[3]  = PCM(1 + 8 *  SBC_CHANNELS);
            x[4]  = PCM(1 + 13 * SBC_CHANNELS);
            x[5]  = PCM(1 + 9 *  SBC_CHANNELS);
            x[6]  = PCM(1 + 12 * SBC_CHANNELS);
            x[7]  = PCM(1 + 10 * SBC_CHANNELS);
            x[8]  = PCM(1 + 11 * SBC_CHANNELS);
            x[9]  = PCM(1 + 3 *  SBC_CHANNELS);
            x[10] = PCM(1 + 6 *  SBC_CHANNELS);
            x[11] = PCM(1 + 0 *  SBC_CHANNELS);
            x[12] = PCM(1 + 5 *  SBC_CHANNELS);
            x[13] = PCM(1 + 1 *  SBC_CHANNELS);
            x[14] = PCM(1 + 4 *  SBC_CHANNELS);
            x[15] = PCM(1 + 2 *  SBC_CHANNELS);
        }
        pcm += 32 * SBC_CHANNELS;
        nsamples -= 16;
    }

    if (nsamples == 8) {
        position -= 8;
        if (SBC_CHANNELS > 0) {
            int16_t *x = &X[0][position];
            x[-7] = PCM(0 + 7 * SBC_CHANNELS);
            x[1]  = PCM(0 + 3 * SBC_CHANNELS);
            x[2]  = PCM(0 + 6 * SBC_CHANNELS);
            x[3]  = PCM(0 + 0 * SBC_CHANNELS);
            x[4]  = PCM(0 + 5 * SBC_CHANNELS);
            x[5]  = PCM(0 + 1 * SBC_CHANNELS);
            x[6]  = PCM(0 + 4 * SBC_CHANNELS);
            x[7]  = PCM(0 + 2 * SBC_CHANNELS);
        }
        if (SBC_CHANNELS > 1) {
            int16_t *x = &X[1][position];
            x[-7] = PCM(1 + 7 * SBC_CHANNELS);
            x[1]  = PCM(1 + 3 * SBC_CHANNELS);
            x[2]  = PCM(1 + 6 * SBC_CHANNELS);
            x[3]  = PCM(1 + 0 * SBC_CHANNELS);
            x[4]  = PCM(1 + 5 * SBC_CHANNELS);
            x[5]  = PCM(1 + 1 * SBC_CHANNELS);
            x[6]  = PCM(1 + 4 * SBC_CHANNELS);
            x[7]  = PCM(1 + 2 * SBC_CHANNELS);
        }
    }
    #undef PCM

    return position;
}


/* Supplementary function to count the number of leading zeros */
static inline int sbc_clz(uint32_t x){
    unsigned int y;
    int n = 32;
    y = x >> 16;
    if (y != 0){
        n = n - 16;
        x = y;
    }
    y = x >> 8;
    if (y != 0){
        n = n - 8;
        x = y;
    }
    y = x >> 4;
    if (y != 0){
        n = n - 4;
        x = y;
    }
    y = x >> 2;
    if (y != 0){
        n = n - 2;
        x = y;
    }
    y = x >> 1;
    if (y != 0)
        return n - 2;
    return n - x;

}

#if SPEED_OVER_RAM
_attribute_ram_code_
#endif
static inline void sbc_calc_scalefactors(int32_t sb_sample_f[SBC_BLOCKS][SBC_CHANNELS][SBC_SUBBANDS], uint32_t scale_factor[SBC_CHANNELS][SBC_SUBBANDS])
{
    int ch, sb, blk;
    for (ch = 0; ch < SBC_CHANNELS; ch++) {
        for (sb = 0; sb < SBC_SUBBANDS; sb++) {
            uint32_t x = 1 << SCALE_OUT_BITS;
            for (blk = 0; blk < SBC_BLOCKS; blk++) {
                int32_t tmp = fabs(sb_sample_f[blk][ch][sb]);
                if (tmp != 0)
                    x |= tmp - 1;
            }
            scale_factor[ch][sb] = (31 - SCALE_OUT_BITS) - sbc_clz(x);
        }
    }
}

static inline int sbc_calc_scalefactors_j(
    int32_t sb_sample_f[SBC_BLOCKS][SBC_CHANNELS][SBC_SUBBANDS],
    uint32_t scale_factor[SBC_CHANNELS][SBC_SUBBANDS])
{
    int blk, joint = 0;
    int32_t tmp0, tmp1;
    uint32_t x, y;

    /* last subband does not use joint stereo */
    int sb = SBC_SUBBANDS - 1;
    x = 1 << SCALE_OUT_BITS;
    y = 1 << SCALE_OUT_BITS;
    for (blk = 0; blk < SBC_BLOCKS; blk++) {
        tmp0 = fabs(sb_sample_f[blk][0][sb]);
        tmp1 = fabs(sb_sample_f[blk][1][sb]);
        if (tmp0 != 0)
            x |= tmp0 - 1;
        if (tmp1 != 0)
            y |= tmp1 - 1;
    }
    scale_factor[0][sb] = (31 - SCALE_OUT_BITS) - sbc_clz(x);
    scale_factor[1][sb] = (31 - SCALE_OUT_BITS) - sbc_clz(y);

    /* the rest of SBC_SUBBANDS can use joint stereo */
    while (--sb >= 0) {
//      int32_t sb_sample_j[16][2];
        int32_t sb_sample_j[SBC_BLOCKS][2];
        x = 1 << SCALE_OUT_BITS;
        y = 1 << SCALE_OUT_BITS;
        for (blk = 0; blk < SBC_BLOCKS; blk++) {
            tmp0 = sb_sample_f[blk][0][sb];
            tmp1 = sb_sample_f[blk][1][sb];
            sb_sample_j[blk][0] = ASR(tmp0, 1) + ASR(tmp1, 1);
            sb_sample_j[blk][1] = ASR(tmp0, 1) - ASR(tmp1, 1);
            tmp0 = fabs(tmp0);
            tmp1 = fabs(tmp1);
            if (tmp0 != 0)
                x |= tmp0 - 1;
            if (tmp1 != 0)
                y |= tmp1 - 1;
        }
        scale_factor[0][sb] = (31 - SCALE_OUT_BITS) -
            sbc_clz(x);
        scale_factor[1][sb] = (31 - SCALE_OUT_BITS) -
            sbc_clz(y);
        x = 1 << SCALE_OUT_BITS;
        y = 1 << SCALE_OUT_BITS;
        for (blk = 0; blk < SBC_BLOCKS; blk++) {
            tmp0 = fabs(sb_sample_j[blk][0]);
            tmp1 = fabs(sb_sample_j[blk][1]);
            if (tmp0 != 0)
                x |= tmp0 - 1;
            if (tmp1 != 0)
                y |= tmp1 - 1;
        }
        x = (31 - SCALE_OUT_BITS) - sbc_clz(x);
        y = (31 - SCALE_OUT_BITS) - sbc_clz(y);

        /* decide whether to use joint stereo for this subband */
        if ((scale_factor[0][sb] + scale_factor[1][sb]) > x + y) {
            joint |= 1 << (SBC_SUBBANDS - 1 - sb);
            scale_factor[0][sb] = x;
            scale_factor[1][sb] = y;
            for (blk = 0; blk < SBC_BLOCKS; blk++) {
                sb_sample_f[blk][0][sb] = sb_sample_j[blk][0];
                sb_sample_f[blk][1][sb] = sb_sample_j[blk][1];
            }
        }
    }

    /* bitmask with the information about SBC_SUBBANDS using joint stereo */
    return joint;
}

static inline void sbc_analyze_8s(int16_t *x, int32_t *out, int out_stride) {
    if (SBC_INCREMENT == 1) {
        if (sbc_analyze_even) {
            sbc_analyze_eight_simd(x, out, analysis_consts_fixed8_simd_even);
        }
        else {
            sbc_analyze_eight_simd(x, out, analysis_consts_fixed8_simd_odd);
        }
        sbc_analyze_even = !sbc_analyze_even;
    }else
        sbc_analyze_4b_8s_simd(x, out, out_stride);
}

#if SPEED_OVER_RAM
_attribute_ram_code_
#endif
static inline int sbc_analyze_audio(){
    int ch, blk;
    int16_t *x;

    switch (SBC_SUBBANDS) {
    case 4:
        for (ch = 0; ch < SBC_CHANNELS; ch++) {
            x = &X[ch][position - 4 * SBC_INCREMENT + SBC_BLOCKS * 4];
            for (blk = 0; blk < SBC_BLOCKS; blk += SBC_INCREMENT) {
                sbc_analyze_4b_4s_simd(x, sb_sample_f[blk][ch], sb_sample_f[blk + 1][ch] - sb_sample_f[blk][ch]);
                x -= 4 * SBC_INCREMENT;
            }
        }
        return SBC_BLOCKS * 4;

    case 8:
        for (ch = 0; ch < SBC_CHANNELS; ch++) {
            x = &X[ch][position - 8 * SBC_INCREMENT + SBC_BLOCKS * 8];
            for (blk = 0; blk < SBC_BLOCKS; blk += SBC_INCREMENT) {
                sbc_analyze_8s(x, sb_sample_f[blk][ch], sb_sample_f[blk + 1][ch] - sb_sample_f[blk][ch]);
                x -= 8 * SBC_INCREMENT;
            }
        }
        return SBC_BLOCKS * 8;

    default:
        return -1;
    }
}

/* Supplementary bitstream writing macros for 'sbc_pack_frame' */
#define PUT_BITS(data_ptr, bits_cache, bits_count, v, n)        \
    do {                                \
        bits_cache = (v) | (bits_cache << (n));         \
        bits_count += (n);                  \
        if (bits_count >= 16) {                 \
            bits_count -= 8;                \
            *data_ptr++ = (uint8_t)             \
                (bits_cache >> bits_count);     \
            bits_count -= 8;                \
            *data_ptr++ = (uint8_t)             \
                (bits_cache >> bits_count);     \
        }                           \
    } while (0)

#define FLUSH_BITS(data_ptr, bits_cache, bits_count)            \
    do {                                \
        while (bits_count >= 8) {               \
            bits_count -= 8;                \
            *data_ptr++ = (uint8_t)             \
                (bits_cache >> bits_count);     \
        }                           \
        if (bits_count > 0)                 \
            *data_ptr++ = (uint8_t)             \
                (bits_cache << (8 - bits_count));   \
    } while (0)

/*
 * Packs the SBC frame from frame into the memory at data. At most len
 * bytes will be used, should more memory be needed an appropriate
 * error code will be returned. Returns the length of the packed frame
 * on success or a negative value on error.
 *
 * The error codes are:
 * -1 Not enough memory reserved
 * -2 Unsupported sampling rate
 * -3 Unsupported number of SBC_BLOCKS
 * -4 Unsupported number of SBC_SUBBANDS
 * -5 Bitpool value out of bounds
 * -99 not implemented
 */

#define CHAR_BIT    8
typedef unsigned long mp_limb_t;
#define GMP_LIMB_BITS (sizeof(mp_limb_t) * CHAR_BIT)
#define GMP_LIMB_MAX (~ (mp_limb_t) 0)
#define GMP_HLIMB_BIT ((mp_limb_t) 1 << (GMP_LIMB_BITS / 2))
#define GMP_LLIMB_MASK (GMP_HLIMB_BIT - 1)

// use  hardware mul32x32_64 to accelerate
#define gmp_umul_ppmm_shift32(w1, u, v)         \
do {                                                \
    mp_limb_t __x0, __x1, __x2, __x3;               \
    unsigned __ul, __vl, __uh, __vh;                \
    mp_limb_t __u = (u), __v = (v);                 \
                                                    \
    __ul = __u & GMP_LLIMB_MASK;                    \
    __uh = __u >> (GMP_LIMB_BITS / 2);              \
    __vl = __v & GMP_LLIMB_MASK;                    \
    __vh = __v >> (GMP_LIMB_BITS / 2);              \
                                                    \
    __x0 = (mp_limb_t) __ul * __vl;                 \
    __x1 = (mp_limb_t) __ul * __vh;                 \
    __x2 = (mp_limb_t) __uh * __vl;                 \
    __x3 = (mp_limb_t) __uh * __vh;                 \
                                                    \
    __x1 += __x0 >> (GMP_LIMB_BITS / 2);            /* this can't give carry */             \
    __x1 += __x2;                                   /* but this indeed can */           \
    if (__x1 < __x2)                                /* did we get it? */                \
    __x3 += GMP_HLIMB_BIT;                          /* yes, add it in the proper pos. */    \
                                                    \
    (w1) = __x3 + (__x1 >> (GMP_LIMB_BITS / 2));    \
                                                    \
} while (0)


#define gmp_add_ssaaaa(sh, sl, ah, al, bh, bl)      \
do {                                                \
    mp_limb_t __x;                                  \
    __x = (al) + (bl);                              \
    (sh) = (ah) + (bh) + (__x < (al));              \
    (sl) = __x;                                     \
} while (0)

#if SPEED_OVER_RAM
_attribute_ram_code_
#endif
static inline ssize_t sbc_pack_frame_internal(uint8_t *data, size_t len, uint8_t joint)
{
    (void)len;
    /* Bitstream writer starts from the fourth byte */
    uint8_t *data_ptr = data + 4;
    uint32_t bits_cache = 0;
    uint32_t bits_count = 0;

    /* Will copy the header parts for CRC-8 calculation here */
    #if(!SBC_CRC_DISABLE)
    uint8_t crc_header[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int crc_pos = 0;
    #endif

    uint32_t audio_sample;

    int ch, sb, blk;    /* channel, subband, block and bit counters */
    int bits[SBC_CHANNELS][SBC_SUBBANDS];       /* bits distribution */
    uint32_t levels[SBC_CHANNELS][SBC_SUBBANDS];    /* levels are derived from that */
    uint32_t sb_sample_delta[SBC_CHANNELS][SBC_SUBBANDS];

    /* Can't fill in crc yet */

    #if(!SBC_CRC_DISABLE)
    crc_header[0] = data[1];
    crc_header[1] = data[2];
    crc_pos = 16;
    #endif

    if (SBC_MODE == SBC_MODE_JOINT_STEREO) {
        PUT_BITS(data_ptr, bits_cache, bits_count, joint, SBC_SUBBANDS);

        #if(!SBC_CRC_DISABLE)
        crc_header[crc_pos >> 3] = joint;
        crc_pos += SBC_SUBBANDS;
        #endif
    }

    for (ch = 0; ch < SBC_CHANNELS; ch++) {
        for (sb = 0; sb < SBC_SUBBANDS; sb++) {
            PUT_BITS(data_ptr, bits_cache, bits_count, scale_factor[ch][sb] & 0x0F, 4);

            #if(!SBC_CRC_DISABLE)
            crc_header[crc_pos >> 3] <<= 4;
            crc_header[crc_pos >> 3] |= scale_factor[ch][sb] & 0x0F;
            crc_pos += 4;
            #endif
        }
    }

    #if(!SBC_CRC_DISABLE)
    /* align the last crc byte */
    if (crc_pos % 8)
        crc_header[crc_pos >> 3] <<= 8 - (crc_pos % 8);

    data[3] = sbc_crc8(crc_header, crc_pos);
    #endif

    sbc_calculate_bits(bits);

    for (ch = 0; ch < SBC_CHANNELS; ch++) {
        for (sb = 0; sb < SBC_SUBBANDS; sb++) {
            levels[ch][sb] = ((1 << bits[ch][sb]) - 1) <<
                (32 - (scale_factor[ch][sb] +
                    SCALE_OUT_BITS + 2));
            sb_sample_delta[ch][sb] = (uint32_t) 1 <<
                (scale_factor[ch][sb] +
                    SCALE_OUT_BITS + 1);
        }
    }

    for (blk = 0; blk < SBC_BLOCKS; blk++) {
        for (ch = 0; ch < SBC_CHANNELS; ch++) {
            for (sb = 0; sb < SBC_SUBBANDS; sb++) {

                if (bits[ch][sb] == 0)
                    continue;
                // we can deduced that (sb_sample_delta[ch][sb] + sb_sample_f[blk][ch][sb]) is greater than 0, and not overflow
                gmp_umul_ppmm_shift32(audio_sample, levels[ch][sb], (sb_sample_delta[ch][sb] + sb_sample_f[blk][ch][sb]));
                PUT_BITS(data_ptr, bits_cache, bits_count,  audio_sample, bits[ch][sb]);
            }
        }
    }

    FLUSH_BITS(data_ptr, bits_cache, bits_count);

    return data_ptr - data;
}

#if SPEED_OVER_RAM
_attribute_ram_code_
#endif
static inline uint32_t sbc_pack_frame(uint8_t *data, size_t len, uint8_t joint){


    data[0] = SBC_SYNCWORD;
    data[1] = ((SBC_SR_TYPE & 0x03) << 6) | (((SBC_BLOCKS/4-1) & 0x03) << 4) | ((SBC_MODE & 0x03) << 2)
        | ((SBC_SNR & 0x01) << 1) | (SBC_SUBBANDS == 4 ? 0 : 1);

//  data[1] = ((SBC_SR_TYPE & 0x03) << 6) | (((SBC_BLOCKS/4) & 0x03) << 4) | ((SBC_MODE & 0x03) << 2)
//      | ((SBC_SNR & 0x01) << 1) | (SBC_SUBBANDS == 4 ? 0 : 1);
    data[2] = SBC_BITPOOL;


    return sbc_pack_frame_internal(data, len, joint);
}


void sbcenc_reset(void){
    position = (SBC_X_BUFFER_SIZE - SBC_SUBBANDS * 9) & ~7;
    sbc_analyze_even = 0;
    memset(X, 0, sizeof(X));
    memset(scale_factor, 0, sizeof(scale_factor));
    memset(sb_sample_f, 0, sizeof(sb_sample_f));
}

extern
#if SPEED_OVER_RAM
_attribute_ram_code_
#else
_attribute_ram_code_
#endif
uint32_t sbc_enc(const uint8_t* buf, uint16_t len, uint8_t *outbuf, uint32_t outbuf_len, uint32_t* out_len){
//    if(len < SBC_CODESIZE){
//      return 0;
//    }
    (void)len;
    (void)out_len;
    /* Select the needed input data processing function and call it */
    if (SBC_SUBBANDS == 8)
        position = sbc_encoder_process_input_s8_internal(position, buf, X, SBC_SUBBANDS * SBC_BLOCKS);
    else
        position = sbc_encoder_process_input_s4_internal(position, buf, X, SBC_SUBBANDS * SBC_BLOCKS);

    sbc_analyze_audio();

    if (SBC_MODE == SBC_MODE_JOINT_STEREO){
        int j = sbc_calc_scalefactors_j(sb_sample_f, scale_factor);
//      *out_len = sbc_pack_frame(outbuf, outbuf_len, j);
        sbc_pack_frame(outbuf, outbuf_len, j);// LLJ Remove out_len, assignment will cause a crash
    }else{
        sbc_calc_scalefactors(sb_sample_f, scale_factor);
//      *out_len = sbc_pack_frame(outbuf, outbuf_len, 0);
        sbc_pack_frame(outbuf, outbuf_len, 0);// LLJ Remove out_len, assignment will cause a crash
    }

    return SBC_CODESIZE;            // code size,  improve later
}




#endif
