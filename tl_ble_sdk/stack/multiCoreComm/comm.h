#ifndef STACK_MULTICORECOMM_COMM_H_
#define STACK_MULTICORECOMM_COMM_H_

#include "common/config/user_config.h"
#include "tl_common.h"
#ifndef TLK_MAILBOX_MESSAGE_CONTROLLER_EN
    #define TLK_MAILBOX_MESSAGE_CONTROLLER_EN  0
#endif

#ifndef TLK_MAILBOX_MESSAGE_HOST_EN
    #define TLK_MAILBOX_MESSAGE_HOST_EN 0
#endif


void tlk_multi_core_communication_init(void);


void tlk_multi_core_communication_loop(void);

void tlk_multi_core_send_clock_config(void) ;

void tlk_multi_core_receive_clock_config(void) ;

#endif /* STACK_MULTICORECOMM_COMM_H_ */
