
#include "password.h"
#include "mki.h"

static password_t password = {0};

void password_init(uint16_t new_pass)
{
    initTimer(&password.timer);

    password.ref = new_pass;

    if(password.ref)
        password.state = PASS_IDLE;
}

void password_task(void)
{
    password_t* p = &password;

    if(isTimerExpired(&p->timer, 30000))
    {
        p->state = PASS_ERROR;   
        p->check = 0;
    }
    
    if(p->state == PASS_CHECK)
    {
        p->state = (p->ref == p->check) ? PASS_OK : PASS_ERROR;
        p->check = 0;

        if(p->state == PASS_OK)
            stopTimer(&p->timer);
    }

    if(p->state == PASS_ERROR)
    {
        mki_wrong_pass_event();
        p->state = PASS_IDLE;
    }
}

void password_req(void)
{
    password_t* p = &password;

    if(p->state == PASS_IDLE /*|| p->state == PASS_ERROR*/ 
    || password_is_waiting() == false)
    {
        p->check = 0;

        p->state = PASS_WAIT_1;
        startSingleShotTimer(&p->timer);
    }
}

void password_next_step(uint8_t halfbyte)
{
    password_t* p = &password;

    if((halfbyte > 0xF) || (password_is_waiting() == false)) 
        return;

    if(p->state == PASS_WAIT_1)
        p->check |= (halfbyte & 0x0F) << 12;
    else if(p->state == PASS_WAIT_2)
        p->check |= (halfbyte & 0x0F) << 8;
    else if(p->state == PASS_WAIT_3)
        p->check |= (halfbyte & 0x0F) << 4;
    else if(p->state == PASS_WAIT_4)
        p->check |= (halfbyte & 0x0F);

    p->state++;
}

bool password_is_waiting(void)
{
    return ((password.state >= PASS_WAIT_1) && (password.state <= PASS_WAIT_4));
}

bool password_is_passed(void)
{
    return password.state == PASS_OK;
}

void password_reset_state(void)
{
    stopTimer(&password.timer);
    password.check = 0;
    password.state = PASS_IDLE;
}
