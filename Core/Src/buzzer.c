#include "buzzer.h"
#include "local_spk.h"
#include "stm_util.h"

static buzzer_t buzzer = {0};

const uint16_t sinus_100_points[SIN_TABLE_SAMPLES] =
{ 
    2048, 2177, 2305, 2432, 2557, 2681, 2802, 2920, 3034, 3145, 
    3251, 3353, 3449, 3540, 3625, 3704, 3776, 3842, 3900, 3951, 
    3995, 4031, 4059, 4079, 4091, 4095, 4091, 4079, 4059, 4031, 
    3995, 3951, 3900, 3842, 3776, 3704, 3625, 3540, 3449, 3353, 
    3251, 3145, 3034, 2920, 2802, 2681, 2557, 2432, 2305, 2177, 
    2048, 1919, 1791, 1664, 1539, 1415, 1294, 1176, 1062,  951, 
     845,  743,  647,  556,  471,  392,  320,  254,  196,  145, 
     101,   65,   37,   17,    5,    1,    5,   17,   37,   65, 
     101,  145,  196,  254,  320,  392,  471,  556,  647,  743, 
     845,  951, 1062, 1176, 1294, 1415, 1539, 1664, 1791, 1919,
};

void buzzer_init(DAC_HandleTypeDef* hdac, uint8_t channel, TIM_HandleTypeDef* htim)
{
    buzzer.dac = hdac;
    buzzer.tim = htim;
    buzzer.dac_channel = channel;
}
 
static uint16_t isqrt(uint16_t num) 
{
    uint16_t res = 0;
    uint16_t bit = 1 << 14; // The second-to-top bit is set: 1L<<30 for long

    // "bit" starts at the highest power of four <= the argument.
    while (bit > num)
        bit >>= 2;

    while (bit != 0) 
    {
        if (num >= res + bit) 
        {
            num -= res + bit;
            res = (res >> 1) + bit;
        }
        else
            res >>= 1;
        
        bit >>= 2;
    }

    return res;
}

static uint16_t ilog2(uint32_t num)
{
    uint8_t res = 0;
    
    while (num > 0)
    {
        num>>=1;
        res++;
    }

    return res-1;
}

void buzzer_make_sound(uint32_t duration, uint32_t frequency, uint8_t volume)
{   
    if(local_spk_is_started())
        return;

    buzzer.duration = duration;

    uint8_t log = (volume == 0) ? 0 : ilog2(volume+1);

    set_tim_frequency2(buzzer.tim, 100*frequency, BASE_FREQUENCY);

    for(uint8_t i = 0; i<SIN_TABLE_SAMPLES; i++)
    {
        int32_t val = sinus_100_points[i] - 2048;
        val = (val * 2*log) >> 4;
        val += 2048; 
        buzzer.buf[i] = __USAT((uint16_t)val, 12);
    }

    HAL_DAC_Start_DMA(buzzer.dac, buzzer.dac_channel, (uint32_t*)&buzzer.buf[0], SIN_TABLE_SAMPLES, DAC_ALIGN_12B_R);
    HAL_TIM_Base_Start(buzzer.tim);

    startSingleShotTimer(&buzzer.timer);
    buzzer.on = true;
}

typedef struct 
{
    uint16_t  d;
    uint16_t  f;
    uint8_t vol;
} queue_t;

#define MAX_BUZZER_QUEUE        10
#define MAX_BUZZER_QUEUE_IDX    MAX_BUZZER_QUEUE-1
static queue_t  q[MAX_BUZZER_QUEUE] = {0};
static int8_t idx = MAX_BUZZER_QUEUE_IDX;

static void buzzer_stop(void)
{
    if(buzzer.on)
    {
        HAL_TIM_Base_Stop(buzzer.tim);
        HAL_DAC_Stop_DMA (buzzer.dac, DAC_CHANNEL_2);

        for(uint8_t i = 0; i < SIN_TABLE_SAMPLES; i++)
        {
            buzzer.buf[i] = 0;
        }
        
        set_tim_frequency2(buzzer.tim, BASE_AUDIO_FREQUENCY, BASE_FREQUENCY);

        buzzer.on = false;
        buzzer.duration = 0;
    }
}

static void buzzer_check_queue(void)
{
    if(buzzer.on == false && (idx < MAX_BUZZER_QUEUE_IDX))
    {
        buzzer_make_sound(q[MAX_BUZZER_QUEUE_IDX].d, q[MAX_BUZZER_QUEUE_IDX].f, q[MAX_BUZZER_QUEUE_IDX].vol);

        for(uint8_t j = MAX_BUZZER_QUEUE_IDX; j > idx; j--)
        {
            q[j].d = q[j-1].d;
            q[j].f = q[j-1].f;
            q[j].vol = q[j-1].vol;
            q[j-1].f = q[j-1].d = q[j-1].vol = 0;
        }
        idx++;
    }
}

void buzzer_add_queue(uint32_t duration, uint32_t frequency, uint8_t volume)
{
    if(idx >= 0 && idx < MAX_BUZZER_QUEUE)
    {
        q[idx].d = duration;
        q[idx].f = frequency;
        q[idx].vol = volume; 

        idx--;
    } 
}

void buzzer_flush(void)
{
    buzzer_stop();

    for(uint8_t i = idx+1; i < MAX_BUZZER_QUEUE; i++)
    {
        q[i].f = q[i].d = q[i].vol = 0;
    }
    idx = MAX_BUZZER_QUEUE_IDX;
}

void buzzer_task(void)
{
    if(buzzer.duration && isTimerExpired(&buzzer.timer, buzzer.duration))
        buzzer_stop();

    buzzer_check_queue();   
}

bool buzzer_is_active(void)
{
    return buzzer.on;
}

/*
44100 -> 0:3265

1600  -> 0:8999
*/
