#include "soft_timer.h"


void initTimer(timer_t* p)
{
    p->isSingleShot = p->isActive = false;
    p->Timestamp = 0;
}

void startTimer(timer_t* p)
{
    //if(!p->isActive)
    {
    	p->Timestamp = HAL_GetTick();
    	p->isActive = true;
    }
}

void startSingleShotTimer(timer_t* p)
{
    p->isSingleShot = true;
    
    startTimer(p);
}

void stopTimer(timer_t* p)
{
    p->isActive = false;
}

bool isTimerExpired(timer_t* p, uint32_t timeout)
{
    if(!p->isActive) return false;

    if(HAL_GetTick() - p->Timestamp >= timeout)
    {
        if(p->isSingleShot)   
          stopTimer(p);    
        else  
          p->Timestamp = HAL_GetTick();  

        return true; 
    }

    return false;
}
