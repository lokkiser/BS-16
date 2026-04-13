#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "main.h"
#include "soft_timer.h"

#define SIN_TABLE_SAMPLES   100

extern const uint16_t sinus_100_points[SIN_TABLE_SAMPLES];

typedef struct
{
    DAC_HandleTypeDef* dac;
    TIM_HandleTypeDef* tim;

    uint32_t duration;
    timer_t  timer;

    uint16_t buf[SIN_TABLE_SAMPLES];

    uint8_t dac_channel; 
    bool on;

} buzzer_t;


void buzzer_init(DAC_HandleTypeDef* hdac, uint8_t channel, TIM_HandleTypeDef* htim);
void buzzer_make_sound(uint32_t duration, uint32_t frequency, uint8_t volume);
void buzzer_add_queue(uint32_t duration, uint32_t frequency, uint8_t volume);
void buzzer_flush(void);
void buzzer_task(void);
bool buzzer_is_active(void);

#endif /* __BUZZER_H__ */
