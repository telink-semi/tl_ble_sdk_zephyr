/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-23     WangShun     the first version
 * 2024-08-30     WangShun     add addr2line function
 */
#if (__PROJECT_SNIF_MAIN_NODE__ || __PROJECT_SNIF_SUB_NODE__ || __PROJECT_SNIF_FOB_NODE__ || __PROJECT_CS_REFLECTOR_DEMO__ || __PROJECT_CS_INITIATOR_DEMO__ \
        || __PROJECT_CS_REFLECTOR_TEST__ || __PROJECT_CS_INITIATOR_TEST__)

#include "rvbacktrace.h"

unsigned int rvstack_frame[STACK_FRAME_LEN]; // stack frame
unsigned int rvstack_frame_len; // stack frame len

extern unsigned int rvstack_frame_len; // stack frame len

/* get the return address of the current function */
__attribute__((always_inline)) static inline void *backtrace_get_sp(void)
{
    void *sp;
    __asm__ volatile("mv %0, sp\n" : "=r"(sp));
    return sp;
}

/* get the return address of the current function */
__attribute__((always_inline)) static inline void *backtrace_get_pc(void)
{
    void *pc;
    __asm__ volatile("auipc %0, 0\n" : "=r"(pc));
    return pc;
}

void rvbacktrace(void)
{
#ifdef RV_BACKTRACE_USE_FP
    rvbacktrace_fno();
#else
    rvbacktrace_fomit(backtrace_get_sp(), (char *)backtrace_get_pc());
#endif /* RV_BACKTRACE_USE_FP */
}

void rvbacktrace_addr2line(uint32_t *frame)
{
    char buffer[STACK_BUFFER_LEN];
    int offset = 0;

    for (uint32_t i = 0; i < rvstack_frame_len; i++)
    {
#if __riscv_xlen == 64
        uint64_t addr = frame[i] & 0x00000000ffffffff; // bitwise AND to remove sign bit
        offset += snprintf(buffer + offset, STACK_BUFFER_LEN - offset, "%lx ", addr);
#else
        offset += snprintf(buffer + offset, STACK_BUFFER_LEN - offset, "%lx ", frame[i]);
#endif
        if (offset >= STACK_BUFFER_LEN)
            break;
    }
    BACKTRACE_PRINTF("\naddr2line -e %s -a -f %s\n", BACKTRACE_ELF_NAME,buffer);
}
#endif

