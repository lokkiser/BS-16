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

#define CTRL_VOLTAGE_BREAK            2.9f //3.1f
#define CTRL_VOLTAGE_DETECTED         1.1f //2.6f
//#define CTRL_VOLTAGE_BUTTON_PRESS        //1.65f
#define CTRL_VOLTAGE_SHORT_CIRCUIT    0.2f

static uint8_t ctrl_index = 0;
float ctrl_adc_mic_old[16]    = {0};
float ctrl_adc_mic_idle[16]   = {0};
float ctrl_adc_spk[16]        = {0};

static uint16_t ctrl_adc1_dma[2] = {0};
static uint16_t ctrl_adc2_dma[2] = {0};

float ctrl_adc_mic_signal[16] = {0};


adc_state_e voltage_out_adc_to_line_state(float volt)
{
    if      (volt > CTRL_VOLTAGE_BREAK)      	return LINE_STATE_BREAK;
    else if (volt > CTRL_VOLTAGE_DETECTED)      return LINE_STATE_DETECTED;
    else if (volt > CTRL_VOLTAGE_SHORT_CIRCUIT) return LINE_STATE_BUTTON_PRESS;
    else                                        return LINE_STATE_SHORT_CIRCUIT;
}

uint8_t line_state_to_color(adc_state_e line_state)
{
    switch (line_state) 
    {
        case LINE_STATE_BREAK:         return MKI_LED_COLOR_BLACK;
        case LINE_STATE_DETECTED:      return MKI_LED_COLOR_GREEN;
        case LINE_STATE_BUTTON_PRESS:  return MKI_LED_COLOR_LIGHTBLUE;
        case LINE_STATE_SHORT_CIRCUIT: return MKI_LED_COLOR_ORANGE;
        default:                       return MKI_LED_COLOR_BLACK;
    }
}

adc_state_e get_ctrl_out_adc_line_state(uint8_t number)
{
    return voltage_out_adc_to_line_state(ctrl_adc_mic_idle[number]);
}

float ctrl_adc_to_voltage(uint16_t value)
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
        ctrl_adc_mic_old[ctrl_index]  = ctrl_adc_mic_idle[ctrl_index];
        ctrl_adc_mic_idle[ctrl_index] = ctrl_adc_to_voltage(ctrl_adc1_dma[0]);

        //adc_sound_buf[0] = (mux_is_active_line()) ? ctrl_adc1_dma[1] : 0;
        ctrl_adc_mic_signal[ctrl_index] = ctrl_adc_to_voltage(ctrl_adc1_dma[1]);

        //fulog_fmt("a1.1:%d a1.3:%d"CRLF, ctrl_adc1_dma[0], ctrl_adc1_dma[1]);
    }
    else if (hadc == &hadc2) 
    {
        ctrl_adc_spk[ctrl_index] = ctrl_adc_to_voltage(ctrl_adc2_dma[0]);
        // ctrl_adc_spk[8 + ctrl_index] = ctrl_adc_to_voltage(ctrl_adc3_dma[1]);
        
        //fulog_fmt("a3.2:%d a3.6:%d"CRLF, ctrl_adc3_dma[0], ctrl_adc3_dma[1]);
    }
}

void ctrl_button_press(uint8_t num)
{
    ulog_fmt("ctrl: VP button press %d"CRLF, num);

	if(get_on_p(num) != P_ON)
    	set_on_p(num, P_AWAIT);
}

/*
void ctrl_button_unpress(uint8_t index)
{
}
*/

static void ctrl_led_front(uint8_t idx, uint8_t color_id)
{
	if (mki_get_led_color(idx) != color_id)
    {
        mki_set_pwm_color(idx+1, mki_get_color_ptr(color_id));
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

    adc_state_e     adc_state = get_ctrl_out_adc_line_state(ctrl_index);
    p_line_state_e line_state = get_on_p(ctrl_index+1);

    switch(adc_state)
    {
    	case LINE_STATE_BREAK:
    	case LINE_STATE_SHORT_CIRCUIT:
    		
    	break;
    
    	case LINE_STATE_DETECTED:
    		if(line_state == P_ERROR)
				set_on_p(ctrl_index+1, P_IDLE);
    	break;

    	case LINE_STATE_BUTTON_PRESS:
        if( (voltage_out_adc_to_line_state(ctrl_adc_mic_old[ctrl_index]) != LINE_STATE_BUTTON_PRESS)
         && (line_state == P_IDLE) )
        {
            ctrl_button_press(ctrl_index+1);
        }
    	break;

    	default: break;
    }

    switch(get_on_p(ctrl_index+1))
    {
    	default:
    	case P_ERROR:
			ctrl_led_front(ctrl_index, MKI_LED_COLOR_RED);
		break;

    	case P_IDLE:
	    	const uint8_t color_id = line_state_to_color(adc_state);
	    	ctrl_led_front(ctrl_index, color_id);
    	break;

    	case P_AWAIT:
    		ctrl_led_front(ctrl_index, MKI_LED_COLOR_LIGHTBLUE);
    	break;

    	case P_ON:
    		ctrl_led_front(ctrl_index, MKI_LED_COLOR_BLUE);
		break;
    }

    //OUT_ADC 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2
    //P_OUT      2.7, 2.7, 2.7, 2.7, 1.6, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.4, 2.7, 2.7, 2.7

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
    else if (ctrl_adc_spk[ctrl_index] > 1.5f) 
        set_fault(FAULT_P_OUT_1_OBRIV + ctrl_index);
    else 
    {
        unset_fault(FAULT_P_OUT_1_KZ + ctrl_index);
        unset_fault(FAULT_P_OUT_1_OBRIV + ctrl_index);
    }

    ctrl_index++;
    ctrl_index %= 16;

    set_adc_line(ctrl_index);
}

void ctrl_init(void)
{
    ctrl_index = 0;
    set_adc_line(ctrl_index);

    //HAL_ADC_Start(&hadc2);
    // HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);
    #warning TODO
}
