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

//Контроль выходов АУ, их микрофона и динамика
//Мы переключаем через мультиплексор каналы и через делитель смотрим,
//какое напряжение мы получаем на делителе.
//Если там 3.3, то обрыв, 0 кз и в таком духе.

// #define CTRL_VOLTAGE_BREAK            2.9f  // 3.1f    
// #define CTRL_VOLTAGE_DETECTED         1.1f  // 2.6f	 
// #define CTRL_VOLTAGE_BUTTON_PRESS	 1.65f
// #define CTRL_VOLTAGE_SHORT_CIRCUIT    0.2f

#define CTRL_VOLTAGE_BREAK            3600 
#define CTRL_VOLTAGE_DETECTED         1365 
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

static uint8_t line_state_to_color(adc_state_e line_state)
{
    switch (line_state) 
    {
        case LINE_STATE_BREAK:         return MKI_LED_COLOR_ORANGE;
        case LINE_STATE_DETECTED:      return MKI_LED_COLOR_GREEN;
        case LINE_STATE_BUTTON_PRESS:  return MKI_LED_COLOR_GREEN; // MKI_LED_COLOR_LIGHTBLUE;
        case LINE_STATE_SHORT_CIRCUIT: return MKI_LED_COLOR_ORANGE;
        default:                       return MKI_LED_COLOR_BLACK;
    }
}

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

static void ctrl_button_press(uint8_t num)
{
    ulog_fmt("ctrl: VP button press %d"CRLF, num);

	if(get_on_p(num) != P_ON)
    {
		set_line_p_state(num, P_AWAIT);
        startSingleShotTimer(&get_line_obj_ptr(num)->timer);
    }
}

static void set_line_led_color(uint8_t num, uint8_t color_id)
{
	if (mki_get_led_color(num-1) != color_id)
	{
	   mki_set_pwm_color(num, mki_get_color_ptr(color_id));
	}
	get_line_obj_ptr(num)->color_id = color_id;
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
        
        cur_line->adc_state 	 = adc_to_state(ctrl_adc_mic_idle[ctrl_index]);
        cur_line->adc_state_last = adc_to_state(ctrl_adc_mic_last[ctrl_index]);
        cur_line->adc_state_old  = adc_to_state(ctrl_adc_mic_old [ctrl_index]);
        cur_line->p_state        = get_on_p(cur_line->num);
        cur_line->color_id       = mki_get_led_color(ctrl_index);

        if(isTimerExpired(&cur_line->timer, 10000))
        {
            cur_line->isMissed = true;
            set_line_p_state(cur_line->num, P_IDLE);
        }

        switch(cur_line->adc_state)
        {
        	case LINE_STATE_BREAK:
        	case LINE_STATE_SHORT_CIRCUIT:
        		
        	break;
        
        	case LINE_STATE_DETECTED:
    		if(cur_line->p_state == P_ERROR)
    			set_line_p_state(cur_line->num, P_IDLE);
        	break;

        	case LINE_STATE_BUTTON_PRESS:
            if( (cur_line->adc_state_old  != LINE_STATE_BUTTON_PRESS)
             && (cur_line->adc_state_last == LINE_STATE_BUTTON_PRESS)
             && (cur_line->p_state == P_IDLE) 
              )
            {
                ctrl_button_press(cur_line->num);
            }
        	break;

        	default: break;
        }

        switch(cur_line->p_state)
        {
        	default:
        	case P_ERROR:
    			set_line_led_color(cur_line->num, MKI_LED_COLOR_WHITE);
    		break;

        	case P_IDLE:
                if(cur_line->isMissed)
                {
                    set_line_led_color(cur_line->num, MKI_LED_COLOR_RED);
                }
                else
                {
        	    	const uint8_t new_color_id = line_state_to_color(cur_line->adc_state);
        	    	set_line_led_color(cur_line->num, new_color_id);
                }
        	break;

        	case P_AWAIT:
        		set_line_led_color(cur_line->num, MKI_LED_COLOR_LIGHTBLUE);
        	break;

        	case P_ON:
        		set_line_led_color(cur_line->num, MKI_LED_COLOR_BLUE);
    		break;
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
        else if (ctrl_adc_spk[ctrl_index] > 1862) 
            set_fault(FAULT_P_OUT_1_OBRIV + ctrl_index);
        else 
        {
            unset_fault(FAULT_P_OUT_1_KZ + ctrl_index);
            unset_fault(FAULT_P_OUT_1_OBRIV + ctrl_index);
        }
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

        // #warning hardcoded
        // lines[i].isConfigured = true;
	}

    #warning hardcoded
    lines[0].isConfigured = true;

    ctrl_index = 0;
    set_mux_line(ctrl_index);
}

void set_line_p_state(uint8_t num, p_line_state_e state)
{
	Obj_line_t* line = get_line_obj_ptr(num);

    line->p_state = state;
	set_on_p(num, state);

    if(state != P_IDLE)
    {
        stopTimer(&line->timer);
        line->isMissed = false;
    } 
}

p_line_state_e get_line_p_state(uint8_t num)
{
	return get_line_obj_ptr(num)->p_state;
}

adc_state_e get_line_adc_state(uint8_t num)
{
	return get_line_obj_ptr(num)->adc_state;
}
