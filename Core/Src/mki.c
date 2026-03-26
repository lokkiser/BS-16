/*
 * mki.c
 *
 *  Created on: 15 дек. 2025 г.
 *      Author: Rafael Boltachev
 */

#include "mki.h"
#include "pca9535.h"
#include "pca9635.h"
//#include "stm_util.h"
#include "str_util.h"
#include "fault.h"
#include "mux.h"
#include "local_mic.h"
#include "local_spk.h"

static pca9635_t led_dd1 = {0}, led_dd2 = {0}, led_dd3 = {0}, led_dd4 = {0};
static pca9535_t key_dd5 = {0}, key_dd6 = {0};

//Для прерываний INT
uint32_t dd5_change_tick = 0, dd6_change_tick = 0;

static uint8_t  mki_leds_color[20] = {0};
// static uint8_t  mki_leds_pwm[20]   = {0};
static uint16_t mki_keys[2] = {0xFFFF, 0xFFFF};

const led_color_t  MKI_LED_PWM_BLACK     = {MKI_LED_COLOR_BLACK,     0,    0,    0};
const led_color_t  MKI_LED_PWM_RED       = {MKI_LED_COLOR_RED,    0xFF,    0,    0};
const led_color_t  MKI_LED_PWM_GREEN     = {MKI_LED_COLOR_GREEN,     0, 0xFF,    0};
const led_color_t  MKI_LED_PWM_BLUE      = {MKI_LED_COLOR_BLUE,      0,    0, 0xFF};
const led_color_t  MKI_LED_PWM_ORANGE    = {MKI_LED_COLOR_ORANGE, 0xFF, 0x1F,    0};
const led_color_t  MKI_LED_PWM_LIGHTBLUE = {MKI_LED_COLOR_LIGHTBLUE, 0, 0x3F, 0x3F};
const led_color_t  MKI_LED_PWM_PURPLE    = {MKI_LED_COLOR_PURPLE, 0x3F,    0, 0xFF};
const led_color_t  MKI_LED_PWM_WHITE     = {MKI_LED_COLOR_WHITE,  0xFF, 0xFF, 0xFF};

// const led_color_t  MKI_LED_PWM_YELLOW    = {MKI_LED_COLOR_YELLOW, 0xFF, 0xFF,    0};

uint8_t mki_get_led_color(uint8_t number)
{
    return mki_leds_color[number];
}

const led_color_t* mki_get_color_ptr(uint8_t id)
{
    switch(id)
    {
        default:
        case MKI_LED_COLOR_BLACK:
            return &MKI_LED_PWM_BLACK;
        break;
        case MKI_LED_COLOR_RED:
            return &MKI_LED_PWM_RED;
        break;
        case MKI_LED_COLOR_GREEN:
            return &MKI_LED_PWM_GREEN;
        break;
        case MKI_LED_COLOR_BLUE:
            return &MKI_LED_PWM_BLUE;
        break;
        case MKI_LED_COLOR_ORANGE:
            return &MKI_LED_PWM_ORANGE;
        break;
        case MKI_LED_COLOR_LIGHTBLUE:
            return &MKI_LED_PWM_LIGHTBLUE;
        break;
        case MKI_LED_COLOR_PURPLE:
            return &MKI_LED_PWM_LIGHTBLUE;
        break;
        case MKI_LED_COLOR_WHITE:
            return &MKI_LED_PWM_WHITE;
        break;
    }
}


/*
void mki_set_led_color(uint8_t number, uint8_t color)
{
    uint8_t r, g, b;

    mki_leds_color[number] = color;

    r = color & MKI_LED_COLOR_RED   ? PCA9635_LDR_VALUE_ON : PCA9635_LDR_VALUE_OFF;
    g = color & MKI_LED_COLOR_GREEN ? PCA9635_LDR_VALUE_ON : PCA9635_LDR_VALUE_OFF;
    b = color & MKI_LED_COLOR_BLUE  ? PCA9635_LDR_VALUE_ON : PCA9635_LDR_VALUE_OFF;

    switch (number)
    {
        case MKI_LED_1:
            pca9635_set_ldr(&led_dd1, PCA9635_LDR0, g);
            pca9635_set_ldr(&led_dd1, PCA9635_LDR1, r);
            pca9635_set_ldr(&led_dd1, PCA9635_LDR2, b);
            pca9635_send_ledout(&led_dd1, PCA9635_REG_LEDOUT0);
        break;
        case MKI_LED_2:
            pca9635_set_ldr(&led_dd1, PCA9635_LDR3, g);
            pca9635_set_ldr(&led_dd1, PCA9635_LDR4, r);
            pca9635_set_ldr(&led_dd1, PCA9635_LDR5, b);
            pca9635_send_ledout(&led_dd1, PCA9635_REG_LEDOUT0);
            HAL_Delay(1);
            pca9635_send_ledout(&led_dd1, PCA9635_REG_LEDOUT1);
        break;
        case MKI_LED_3:
            pca9635_set_ldr(&led_dd1, PCA9635_LDR6, g);
            pca9635_set_ldr(&led_dd1, PCA9635_LDR7, r);
            pca9635_set_ldr(&led_dd1, PCA9635_LDR8, b);
            pca9635_send_ledout(&led_dd1, PCA9635_REG_LEDOUT1);
            HAL_Delay(1);
            pca9635_send_ledout(&led_dd1, PCA9635_REG_LEDOUT2);
        break;
        case MKI_LED_4:
            pca9635_set_ldr(&led_dd1, PCA9635_LDR9,  g);
            pca9635_set_ldr(&led_dd1, PCA9635_LDR10, r);
            pca9635_set_ldr(&led_dd1, PCA9635_LDR11, b);
            pca9635_send_ledout(&led_dd1, PCA9635_REG_LEDOUT2);
        break;
        case MKI_LED_5:
            //перевернут по съеме BRG
            pca9635_set_ldr(&led_dd1, PCA9635_LDR12, b);
            pca9635_set_ldr(&led_dd1, PCA9635_LDR13, r);
            pca9635_set_ldr(&led_dd1, PCA9635_LDR14, g);
            pca9635_send_ledout(&led_dd1, PCA9635_REG_LEDOUT3);
        break;
        case MKI_LED_6:
            pca9635_set_ldr(&led_dd1, PCA9635_LDR15, g);
            pca9635_set_ldr(&led_dd2, PCA9635_LDR0,  r);
            pca9635_set_ldr(&led_dd2, PCA9635_LDR1,  b);
            pca9635_send_ledout(&led_dd1, PCA9635_REG_LEDOUT3);
            HAL_Delay(1);
            pca9635_send_ledout(&led_dd2, PCA9635_REG_LEDOUT0);
        break;
        case MKI_LED_7:
            pca9635_set_ldr(&led_dd2, PCA9635_LDR2, g);
            pca9635_set_ldr(&led_dd2, PCA9635_LDR3, r);
            pca9635_set_ldr(&led_dd2, PCA9635_LDR4, b);
            pca9635_send_ledout(&led_dd2, PCA9635_REG_LEDOUT0);
            HAL_Delay(1);
            pca9635_send_ledout(&led_dd2, PCA9635_REG_LEDOUT1);
        break;
        case MKI_LED_8:
            pca9635_set_ldr(&led_dd2, PCA9635_LDR5, g);
            pca9635_set_ldr(&led_dd2, PCA9635_LDR6, r);
            pca9635_set_ldr(&led_dd2, PCA9635_LDR7, b);
            pca9635_send_ledout(&led_dd2, PCA9635_REG_LEDOUT1);
        break;
        case MKI_LED_9:
            pca9635_set_ldr(&led_dd2, PCA9635_LDR8,  g);
            pca9635_set_ldr(&led_dd2, PCA9635_LDR9,  r);
            pca9635_set_ldr(&led_dd2, PCA9635_LDR10, b);
            pca9635_send_ledout(&led_dd2, PCA9635_REG_LEDOUT2);
        break;
        case MKI_LED_10:
            pca9635_set_ldr(&led_dd2, PCA9635_LDR11, g);
            pca9635_set_ldr(&led_dd2, PCA9635_LDR12, r);
            pca9635_set_ldr(&led_dd2, PCA9635_LDR13, b);
            pca9635_send_ledout(&led_dd2, PCA9635_REG_LEDOUT2);
            HAL_Delay(1);
            pca9635_send_ledout(&led_dd2, PCA9635_REG_LEDOUT3);
        break;
        case MKI_LED_11:
            pca9635_set_ldr(&led_dd2, PCA9635_LDR14, g);
            pca9635_set_ldr(&led_dd2, PCA9635_LDR15, r);
            pca9635_set_ldr(&led_dd3, PCA9635_LDR0,  b);
            pca9635_send_ledout(&led_dd2, PCA9635_REG_LEDOUT3);
            HAL_Delay(1);
            pca9635_send_ledout(&led_dd3, PCA9635_REG_LEDOUT0);
        break;
        case MKI_LED_12:
            pca9635_set_ldr(&led_dd3, PCA9635_LDR1, g);
            pca9635_set_ldr(&led_dd3, PCA9635_LDR2, r);
            pca9635_set_ldr(&led_dd3, PCA9635_LDR3, b);
            pca9635_send_ledout(&led_dd3, PCA9635_REG_LEDOUT0);
        break;
        case MKI_LED_13:
            pca9635_set_ldr(&led_dd3, PCA9635_LDR4, g);
            pca9635_set_ldr(&led_dd3, PCA9635_LDR5, r);
            pca9635_set_ldr(&led_dd3, PCA9635_LDR6, b);
            pca9635_send_ledout(&led_dd3, PCA9635_REG_LEDOUT1);
        break;
        case MKI_LED_14:
            pca9635_set_ldr(&led_dd3, PCA9635_LDR7, g);
            pca9635_set_ldr(&led_dd3, PCA9635_LDR8, r);
            pca9635_set_ldr(&led_dd3, PCA9635_LDR9, b);
            pca9635_send_ledout(&led_dd3, PCA9635_REG_LEDOUT1);
            HAL_Delay(1);
            pca9635_send_ledout(&led_dd3, PCA9635_REG_LEDOUT2);
        break;
        case MKI_LED_15:
            pca9635_set_ldr(&led_dd3, PCA9635_LDR10, g);
            pca9635_set_ldr(&led_dd3, PCA9635_LDR11, r);
            pca9635_set_ldr(&led_dd3, PCA9635_LDR12, b);
            pca9635_send_ledout(&led_dd3, PCA9635_REG_LEDOUT2);
            HAL_Delay(1);
            pca9635_send_ledout(&led_dd3, PCA9635_REG_LEDOUT3);
        break;
        case MKI_LED_16:
            pca9635_set_ldr(&led_dd3, PCA9635_LDR13, g);
            pca9635_set_ldr(&led_dd3, PCA9635_LDR14, r);
            pca9635_set_ldr(&led_dd3, PCA9635_LDR15, b);
            pca9635_send_ledout(&led_dd3, PCA9635_REG_LEDOUT3);
        break;
        case MKI_LED_220:
            pca9635_set_ldr(&led_dd4, PCA9635_LDR0, g);
            pca9635_set_ldr(&led_dd4, PCA9635_LDR1, r);
            pca9635_set_ldr(&led_dd4, PCA9635_LDR2, b);
            pca9635_send_ledout(&led_dd4, PCA9635_REG_LEDOUT0);
        break;
        case MKI_LED_BAT:
            pca9635_set_ldr(&led_dd4, PCA9635_LDR3, g);
            pca9635_set_ldr(&led_dd4, PCA9635_LDR4, r);
            pca9635_set_ldr(&led_dd4, PCA9635_LDR5, b);
            pca9635_send_ledout(&led_dd4, PCA9635_REG_LEDOUT0);
            HAL_Delay(1);
            pca9635_send_ledout(&led_dd4, PCA9635_REG_LEDOUT1);
        break;
        case MKI_LED_AVAR:
            pca9635_set_ldr(&led_dd4, PCA9635_LDR6, g);
            pca9635_set_ldr(&led_dd4, PCA9635_LDR7, r);
            pca9635_set_ldr(&led_dd4, PCA9635_LDR8, b);
            pca9635_send_ledout(&led_dd4, PCA9635_REG_LEDOUT1);
            HAL_Delay(1);
            pca9635_send_ledout(&led_dd4, PCA9635_REG_LEDOUT2);
        break;
        case MKI_LED_START:
            pca9635_set_ldr(&led_dd4, PCA9635_LDR9,  g);
            pca9635_set_ldr(&led_dd4, PCA9635_LDR10, r);
            pca9635_set_ldr(&led_dd4, PCA9635_LDR11, b);
            pca9635_send_ledout(&led_dd4, PCA9635_REG_LEDOUT2);
        break;
        default: break;
    }
}
*/
void mki_set_pwm_color(uint8_t number, const led_color_t* color)
{    
    mki_leds_color[number-1] = color->id;

    switch (number)
    {
        case MKI_LED_1:
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM0,  color->g);
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM1,  color->r);
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM2,  color->b);
        break; 
        case MKI_LED_2:
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM3,  color->g);
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM4,  color->r);
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM5,  color->b);
        break; 
        case MKI_LED_3:
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM6,  color->g);
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM7,  color->r);
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM8,  color->b);
        break;
        case MKI_LED_4:
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM9,  color->g);
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM10, color->r);
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM11, color->b);
        break;
        case MKI_LED_5:
            //перевернут по съеме BRG
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM12, color->b);
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM13, color->r);
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM14, color->g);
        break;
        case MKI_LED_6:
            PCA9635_write_reg(&led_dd1, PCA9635_REG_PWM15, color->g);
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM0,  color->r);
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM1,  color->b);
        break;
        case MKI_LED_7:
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM2,  color->g);
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM3,  color->r);
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM4,  color->b);
        break; 
        case MKI_LED_8:
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM5,  color->g);
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM6,  color->r);
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM7,  color->b);
        break;
        case MKI_LED_9:
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM8,  color->g);
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM9,  color->r);
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM10, color->b);
        break;
        case MKI_LED_10:
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM11, color->g);
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM12, color->r);
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM13, color->b);
        break;
        case MKI_LED_11:
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM14, color->g);
            PCA9635_write_reg(&led_dd2, PCA9635_REG_PWM15, color->r);
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM0,  color->b);
        break;
        case MKI_LED_12:
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM1,  color->g);
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM2,  color->r);
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM3,  color->b);
        break; 
        case MKI_LED_13: 
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM4,  color->g);
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM5,  color->r);
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM6,  color->b);
        break; 
        case MKI_LED_14: 
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM7,  color->g);
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM8,  color->r);
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM9,  color->b);
        break;
        case MKI_LED_15:
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM10, color->g);
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM11, color->r);
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM12, color->b);
        break;
        case MKI_LED_16:
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM13, color->g);
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM14, color->r);
            PCA9635_write_reg(&led_dd3, PCA9635_REG_PWM15, color->b);
        break;
        case MKI_LED_220:
            PCA9635_write_reg(&led_dd4, PCA9635_REG_PWM0,  color->g);
            PCA9635_write_reg(&led_dd4, PCA9635_REG_PWM1,  color->r);
            PCA9635_write_reg(&led_dd4, PCA9635_REG_PWM2,  color->b);
        break; 
        case MKI_LED_BAT: 
            PCA9635_write_reg(&led_dd4, PCA9635_REG_PWM3,  color->g);
            PCA9635_write_reg(&led_dd4, PCA9635_REG_PWM4,  color->r);
            PCA9635_write_reg(&led_dd4, PCA9635_REG_PWM5,  color->b);
        break; 
        case MKI_LED_AVAR: 
            PCA9635_write_reg(&led_dd4, PCA9635_REG_PWM6,  color->g);
            PCA9635_write_reg(&led_dd4, PCA9635_REG_PWM7,  color->r);
            PCA9635_write_reg(&led_dd4, PCA9635_REG_PWM8,  color->b);
        break;
        case MKI_LED_START:
            PCA9635_write_reg(&led_dd4, PCA9635_REG_PWM9,  color->g);
            PCA9635_write_reg(&led_dd4, PCA9635_REG_PWM10, color->r);
            PCA9635_write_reg(&led_dd4, PCA9635_REG_PWM11, color->b);
        break;
        default: break;
    }
}

uint16_t mki_get_key_from_dd(pca9535_t * pca9535)
{
    uint16_t res = 0;
    if (pca9535_get_reg(pca9535, PCA9535_REG_TYPE_INPUT, PCA9535_PORT_BOTH, (unsigned char *)&res, 2) < 0) return 0;
    return res;
}


void mki_unpress(uint8_t key)
{
    ulog_fmt("mki_unpress %d"CRLF, key);
    if (key <= MKI_KEY_16)         {}
    else if (key == MKI_KEY_ZVUKOVOI_VIZOV) {}
    else if (key == MKI_KEY_KONEC_SVAZI)     {}
}

void mki_press(uint8_t key)
{
    ulog_fmt("mki_press %d"CRLF, key);
    if (key <= MKI_KEY_16) 
    {
        i2c_mux_line_state_e state = get_on_p(key);
        if(state == P_AWAIT && !mux_is_active_line())
            set_on_p(key, P_ON);
        else if(state == ON || state == P_ERROR)
            set_on_p(key, P_IDLE);
    }
    else if (key == MKI_KEY_ZVUKOVOI_VIZOV) 
    {
        local_mic_start();
        local_spk_start();
    }
    else if (key == MKI_KEY_KONEC_SVAZI) 
    {
        local_mic_stop();
        local_spk_stop();

        // set_dd3(0);
        // set_dd8(0);
    }
}

void mki_test(void)
{
    uint16_t key;

    key = mki_get_key_from_dd(&key_dd5);
    ulog_fmt("mki: DD5 %d"CRLF, key);

    key = mki_get_key_from_dd(&key_dd6);
    ulog_fmt("mki: DD6 %d"CRLF, key);
}


void mki_task(void)
{
    uint16_t key;

    if (dd5_change_tick && (HAL_DiffTick(dd5_change_tick) > 25)) 
    {
        dd5_change_tick = 0;

        key = mki_get_key_from_dd(&key_dd5);

        for (uint8_t i = 0; i < 16; i++) 
        {
            if (!GETBIT(mki_keys[0], i) && GETBIT(key, i)) 
            {
                if (i < 8) 
                    mki_unpress(i + 1);
                else 
                    mki_unpress(24 - i);
            }
            else if (GETBIT(mki_keys[0], i) && !GETBIT(key, i)) 
            {
                if (i < 8) 
                    mki_press(i + 1);
                else 
                    mki_press(24 - i);
            }
        }
        mki_keys[0] = key;
    }
    
    if (dd6_change_tick && (HAL_DiffTick(dd6_change_tick) > 25)) 
    {
        dd6_change_tick = 0;

        key = mki_get_key_from_dd(&key_dd6);

        for (uint8_t i = 0; i < 2; i++) 
        {
            if (!GETBIT(mki_keys[1], i) && GETBIT(key, i)) 
                mki_unpress(i + 17);
            else if (GETBIT(mki_keys[1], i) && !GETBIT(key, i)) 
                mki_press(i + 17);
        }
        mki_keys[1] = key;
    }
}

void mki_init(void)
{
    pca9635_init(&led_dd1, MKI_I2C_HANDLE, MKI_DD1_ADDERESS);
    if (!pca9635_is_ready(&led_dd1)) 
    {
        ulog("mki: error DD1 not ready"CRLF);
        set_fault(FAULT_HARDWARE_ERROR);
    }
    
    pca9635_init(&led_dd2, MKI_I2C_HANDLE, MKI_DD2_ADDERESS);
    if (!pca9635_is_ready(&led_dd2)) 
    {
        ulog("mki: error DD2 not ready"CRLF);
        set_fault(FAULT_HARDWARE_ERROR);
    }
    
    pca9635_init(&led_dd3, MKI_I2C_HANDLE, MKI_DD3_ADDERESS);
    if (!pca9635_is_ready(&led_dd3)) 
    {
        ulog("mki: error DD3 not ready"CRLF);
        set_fault(FAULT_HARDWARE_ERROR);
    }
    
    pca9635_init(&led_dd4, MKI_I2C_HANDLE, MKI_DD4_ADDERESS);
    if (!pca9635_is_ready(&led_dd4)) 
    {
        ulog("mki: error DD4 not ready"CRLF);
        set_fault(FAULT_HARDWARE_ERROR);
    }

    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd1,  PCA9635_REG_MODE1, 0);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd1,  PCA9635_REG_MODE2, 0);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd1, PCA9635_REG_LEDOUT0, 0xAA);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd1, PCA9635_REG_LEDOUT1, 0xAA);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd1, PCA9635_REG_LEDOUT2, 0xAA);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd1, PCA9635_REG_LEDOUT3, 0xAA);


    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd2,  PCA9635_REG_MODE1, 0);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd2,  PCA9635_REG_MODE2, 0);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd2, PCA9635_REG_LEDOUT0, 0xAA);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd2, PCA9635_REG_LEDOUT1, 0xAA);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd2, PCA9635_REG_LEDOUT2, 0xAA);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd2, PCA9635_REG_LEDOUT3, 0xAA);


    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd3,  PCA9635_REG_MODE1, 0);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd3,  PCA9635_REG_MODE2, 0);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd3, PCA9635_REG_LEDOUT0, 0xAA);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd3, PCA9635_REG_LEDOUT1, 0xAA);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd3, PCA9635_REG_LEDOUT2, 0xAA);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd3, PCA9635_REG_LEDOUT3, 0xAA);

    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd4,  PCA9635_REG_MODE1, 0);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd4,  PCA9635_REG_MODE2, 0);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd4, PCA9635_REG_LEDOUT0, 0xAA);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd4, PCA9635_REG_LEDOUT1, 0xAA);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd4, PCA9635_REG_LEDOUT2, 0xAA);
    // HAL_Delay(1);
    PCA9635_write_reg(&led_dd4, PCA9635_REG_LEDOUT3, 0xAA);

    for(uint8_t i = MKI_LED_1; i<=MKI_LED_START; i++)
    {
        mki_set_pwm_color(i, &MKI_LED_PWM_BLACK);
    }

    // Init of LED colors
    mki_set_pwm_color(MKI_LED_220,   &MKI_LED_PWM_GREEN);

    pca9535_init(&key_dd5, MKI_I2C_HANDLE, MKI_DD5_ADDERESS);
    if (pca9535_is_ready(&key_dd5) != 0) 
    {
        ulog("mki: error DD5 not ready"CRLF);
        set_fault(FAULT_HARDWARE_ERROR);
    }
    
    pca9535_init(&key_dd6, MKI_I2C_HANDLE, MKI_DD6_ADDERESS);
    if (pca9535_is_ready(&key_dd6) != 0) 
    {
        ulog("mki: error DD6 not ready"CRLF);
        set_fault(FAULT_HARDWARE_ERROR);
    }
}
