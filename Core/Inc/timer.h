#ifndef __GPIO_H__
#define __GPIO_H__

#include "main.h"

typedef struct 
{
    uint32_t Timestamp;

    bool isSingleShot;
    bool isActive;
}timer_t;


void initTimer(timer_t* p);
void startSingleShotTimer(timer_t* p);
void startTimer(timer_t* p);
void stopTimer(timer_t* p);
bool isTimerExpired(timer_t* p, uint32_t timeout);


#endif /*__ GPIO_H__ */
