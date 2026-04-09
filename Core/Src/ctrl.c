/*
 * ctrl.c
 *
 *  Created on: 29 дек. 2025 г.
 *      Author: Rafael Boltachev
 */


#include "ctrl.h"
#include "mux.h"
#include "stm_util.h"
#include "fault.h"
#include "mki.h"
#include "local_mic.h"
#include "local_spk.h"


//Контроль выходов АУ, их микрофона и динамика
//Мы переключаем через мультиплексор каналы и через делитель смотрим,
//какое напряжение мы получаем на делителе.

// 12-bit ADC (4095 max)
#define CTRL_VOLTAGE_BREAK            3600 
#define CTRL_VOLTAGE_DETECTED         1365 
// #define CTRL_VOLTAGE_SHORT_CIRCUIT
#define CTRL_VOLTAGE_SHORT_CIRCUIT     128 //248

static uint8_t  ctrl_index = 0;
static uint16_t ctrl_adc1_dma[2] = {0};
static uint16_t ctrl_adc2_dma[2] = {0};

static Obj_line_t lines[16];

static uint16_t ctrl_adc_mic_idle[16]   = {0};
static uint16_t ctrl_adc_mic_last[16]   = {0};
static uint16_t ctrl_adc_mic_old[16]    = {0};

static uint16_t ctrl_adc_spk[16]        = {0};
static uint16_t ctrl_adc_mic_signal[16] = {0};

static adc_state_e adc_to_state(uint16_t value) 
{
    if      (value > CTRL_VOLTAGE_BREAK)      	 return LINE_STATE_BREAK;
    else if (value > CTRL_VOLTAGE_DETECTED)      return LINE_STATE_DETECTED;
    else if (value > CTRL_VOLTAGE_SHORT_CIRCUIT) return LINE_STATE_BUTTON_PRESS;
    else                                         return LINE_STATE_SHORT_CIRCUIT;
}

static float ctrl_adc_to_voltage(uint16_t value)
{
    return 3.3f * (float)value / 4095.0f;
}

//EXT_SPK_I
//Контролируем ток (средняя точка и от неё выше-нише) через усилитель (амплитуда) можем вычитывать через АЦП.
float get_ext_spk_i(void)
{
    return ctrl_adc_to_voltage(get_injected_adc_value(&hadc5, 1, 100)); 
}

//EXT_SPK_OFFSET
//задаёт ток сдвига. Это надо чтобы на выход подавать какое-то напряжение, достаточное для питания светодиода.
//Т.е. мы подаём звук (переменная составляющая) + постоянная составляющая.
void set_ext_spk_offset(float volt)
{
    uint32_t value = 4095.0f * volt / 3.3f;

    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
}

void ctrl_FullCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc == &hadc1) 
    {
        ctrl_adc_mic_old[ctrl_index]  = ctrl_adc_mic_last[ctrl_index];
        ctrl_adc_mic_last[ctrl_index] = ctrl_adc_mic_idle[ctrl_index];
        ctrl_adc_mic_idle[ctrl_index] = ctrl_adc1_dma[0];

        ctrl_adc_mic_signal[ctrl_index] = ctrl_adc1_dma[1];
    }
    else if (hadc == &hadc2) 
    {
        ctrl_adc_spk[ctrl_index] = ctrl_adc2_dma[0];
    }
}

static Obj_line_t* get_line_obj_ptr(uint8_t num)
{
	return (lines[num-1].num == num) ? &lines[num-1] : NULL;
}

static void set_line_p_state(uint8_t num, p_line_state_e state)
{
    Obj_line_t* line = get_line_obj_ptr(num);

    line->p_state = state;
    set_on_p(num, state);
}

static void ctrl_button_press(uint8_t num)
{
    ulog_fmt("ctrl: VP button press %d"CRLF, num);

	if(get_on_p(num) != P_ON)
    {
		set_line_p_state(num, P_AWAIT);
        set_line_event(num, EVENT_PANEL_CALL);

        startSingleShotTimer(&get_line_obj_ptr(num)->timer);
    }
}

static void set_line_led_color(uint8_t num, uint8_t color_id)
{
    if(mki_is_global_event() == false)
    {
    	if (mki_get_led_color(num-1) != color_id)
    	{
    	   mki_set_pwm_color(num, color_id);
    	}
    }
	get_line_obj_ptr(num)->color_id = color_id;
}

static void update_line_leds(void)
{
    for(uint8_t i = 0; i < 16; i++)
    {
        set_line_led_color(i+1, lines[i].color_id);
    }
}

void ctrl_task(void)
{
    HAL_StatusTypeDef status;
    status = HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ctrl_adc1_dma, 2);
    if (status != HAL_OK) 
    {
        ulog_fmt("ctrl: error invalid start dma adc1 status = %d"CRLF, status);
    }

    status = HAL_ADC_Start_DMA(&hadc2, (uint32_t*)ctrl_adc2_dma, 1);
    if (status != HAL_OK) 
    {
        ulog_fmt("ctrl: error invalid start dma adc2 status = %d"CRLF, status);
    }

    HAL_Delay(1); // Чтобы точно завершить обработку прерываний

    Obj_line_t* cur_line = &lines[ctrl_index];

    #warning TODO
    if(cur_line->isConfigured)
    {
        cur_line->adc_state        = adc_to_state(ctrl_adc_mic_idle[ctrl_index]);
        adc_state_e adc_state_last = adc_to_state(ctrl_adc_mic_last[ctrl_index]);
        adc_state_e adc_state_old  = adc_to_state(ctrl_adc_mic_old [ctrl_index]);
        cur_line->p_state  = get_on_p(cur_line->num);
        cur_line->color_id = mki_get_led_color(ctrl_index);

        if(isTimerExpired(&cur_line->timer, 10000))
        {
            set_line_p_state(cur_line->num, P_IDLE);
            set_line_event(cur_line->num, EVENT_MISSED_CALL);
        }

        switch(cur_line->adc_state)
        {
        	case LINE_STATE_DETECTED:
    		if(cur_line->p_state == P_ERROR)
    			set_line_p_state(cur_line->num, P_IDLE);
        	break;

        	case LINE_STATE_BUTTON_PRESS:
            if( (adc_state_old  != LINE_STATE_BUTTON_PRESS)
             && (adc_state_last == LINE_STATE_BUTTON_PRESS)
             && (cur_line->p_state == P_IDLE) )
            {
                ctrl_button_press(cur_line->num);
            }
        	break;

        	case LINE_STATE_BREAK:
        	case LINE_STATE_SHORT_CIRCUIT:        		
        	default: break;
        }

        switch(cur_line->last_event)
        {
            case NO_EVENT:
            if(cur_line->adc_state == LINE_STATE_BREAK || cur_line->adc_state == LINE_STATE_SHORT_CIRCUIT)
                set_line_led_color(cur_line->num, MKI_LED_COLOR_ORANGE);
            else
                set_line_led_color(cur_line->num, MKI_LED_COLOR_GREEN);
            break;
            
            case EVENT_PANEL_CALL:
                set_line_led_color(cur_line->num, MKI_LED_COLOR_PURPLE);
            break;

            case EVENT_AWAIT_LINE:            
                set_line_led_color(cur_line->num, MKI_LED_COLOR_LIGHTBLUE);
            break;
            
            case EVENT_LINE_ON:
                set_line_led_color(cur_line->num, MKI_LED_COLOR_BLUE);
            break;
            
            case EVENT_MISSED_CALL:
                set_line_led_color(cur_line->num, MKI_LED_COLOR_RED);
            break;

            default: break;
        }

        if (ctrl_adc_mic_idle[ctrl_index] < CTRL_VOLTAGE_SHORT_CIRCUIT) 
            set_fault(FAULT_OUT_ADC1_KZ + ctrl_index);
        else if (ctrl_adc_mic_idle[ctrl_index] > CTRL_VOLTAGE_BREAK) 
            set_fault(FAULT_OUT_ADC1_OBRIV + ctrl_index);
        else 
        {
            unset_fault(FAULT_OUT_ADC1_KZ + ctrl_index);
            unset_fault(FAULT_OUT_ADC1_OBRIV + ctrl_index);
        }

        if (ctrl_adc_spk[ctrl_index] < CTRL_VOLTAGE_SHORT_CIRCUIT) 
            set_fault(FAULT_P_OUT_1_KZ + ctrl_index);
        else if (ctrl_adc_spk[ctrl_index] > 1862) // TODO
            set_fault(FAULT_P_OUT_1_OBRIV + ctrl_index);
        else 
        {
            unset_fault(FAULT_P_OUT_1_KZ + ctrl_index);
            unset_fault(FAULT_P_OUT_1_OBRIV + ctrl_index);
        }
    }
    else 
    {
        set_line_led_color(cur_line->num ,MKI_LED_COLOR_BLACK);
    }

    ctrl_index++;
    ctrl_index %= 16;

    set_mux_line(ctrl_index);
}

void ctrl_init(void)
{
	for(uint8_t i = 0; i < 16; i++)
	{
		lines[i].num = i+1;
		lines[i].color_id = MKI_LED_COLOR_BLACK;
		lines[i].p_state  = P_IDLE;
	}

    for(uint8_t i = 0; i < 4;  i++)
    {
        #warning hardcoded
        lines[i].isConfigured = true;
    }

    ctrl_index = 0;
    set_mux_line(ctrl_index);
}

p_line_state_e get_line_p_state(uint8_t num)
{
	return get_line_obj_ptr(num)->p_state;
}

adc_state_e get_line_adc_state(uint8_t num)
{
	return get_line_obj_ptr(num)->adc_state;
}

void set_line_event(uint8_t num, line_event_e event)
{
    Obj_line_t* line = get_line_obj_ptr(num);

    if(event == EVENT_PANEL_CALL)
        startSingleShotTimer(&line->timer);
    else 
        stopTimer(&line->timer);

    switch(event)
    {
        case NO_EVENT:
        local_spk_stop();
        local_mic_stop();
        //no break
        case EVENT_MISSED_CALL:
        set_line_p_state(num, P_IDLE);
        //no break
        default: break;

        case EVENT_AWAIT_LINE:
        local_spk_start();
        //no break            
        case EVENT_PANEL_CALL:
        set_line_p_state(num, P_AWAIT);
        break;

        case EVENT_LINE_ON:
        set_line_p_state(num, P_ON);

        local_spk_start();
        local_mic_start();
        break;
    }

    line->last_event = event;
}

line_event_e get_last_line_event(uint8_t num)
{
    return get_line_obj_ptr(num)->last_event;
}

bool is_line_configurated(uint8_t num)
{
    return get_line_obj_ptr(num)->isConfigured;
}
