#ifndef __PASSWORD_H__
#define __PASSWORD_H__

#include "main.h"
#include "soft_timer.h"

typedef enum
{
    NO_PASS = 0,
    PASS_IDLE,
    PASS_WAIT_1,
    PASS_WAIT_2,
    PASS_WAIT_3,
    PASS_WAIT_4,
    PASS_CHECK,
    PASS_ERROR,
    PASS_OK,
} password_state_e;

typedef struct
{
    timer_t  timer;
    uint16_t ref;
    uint16_t check;
    password_state_e  state;
} password_t;

void password_init(uint16_t pass);
void password_task(void);
bool password_is_waiting(void);
void password_req(void);
void password_next_step(uint8_t halfbyte);
bool password_is_passed(void);
void password_reset_state(void);


#endif /*__PASSWORD_H__ */
