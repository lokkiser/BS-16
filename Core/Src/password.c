
#include "password.h"
#include "mki.h"
#include "buzzer.h"

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
        if(p->state != PASS_OK || p->state == PASS_IDLE)
            p->state = PASS_CHECK;
        else
            p->state = PASS_IDLE;     
    }
    
    if(p->state == PASS_CHECK)
    {
        if(p->ref == p->check)
        {
            p->state = PASS_OK;

            buzzer_flush();
            buzzer_make_sound(150, 587, 0xA0);
            buzzer_add_queue( 150, 587, 0);
            buzzer_add_queue( 150, 587, 0xA0);
            buzzer_add_queue( 150, 587, 0);
            buzzer_add_queue( 150, 587, 0xA0);
        }
        else 
        {
            p->state = PASS_ERROR;   
            stopTimer(&password.timer);
            mki_wrong_pass_event();

            buzzer_make_sound(150, 262, 0xFF);
            buzzer_add_queue( 150, 262, 0);
            buzzer_add_queue( 150, 262, 0xFF);
            buzzer_add_queue( 150, 262, 0);
            buzzer_add_queue( 150, 262, 0xFF);
        }
    }
}

void password_req(void)
{
    password_t* p = &password;

    if(p->state == PASS_IDLE 
    || p->state == PASS_ERROR 
    || p->state == PASS_OK 
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
