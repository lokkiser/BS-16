/*
 * mux.c
 *
 *  Created on: 17 дек. 2025 г.
 *      Author: Rafael Boltachev
 */


#include "mux.h"
#include "pca9535.h"
#include "fault.h"
#include "math_util.h"

static pca9535_t dd3, dd8;

static line_p_outs_u p_outs = {0};
// static uint16_t  dd3_value = 0, dd8_value = 0;

void set_mux_line(uint8_t num)
{
    PIN_WRITE(MCU_MUX_CTRL_1, (num & 0x01) ? 1 : 0);
    PIN_WRITE(MCU_MUX_CTRL_2, (num & 0x02) ? 1 : 0);
    PIN_WRITE(MCU_MUX_CTRL_3, (num & 0x04) ? 1 : 0);

    if(num < 8)
    {
        PIN_CLR(MCU_MUX_CTRL_4);  // INH Pin - инверсированный
        PIN_SET(MCU_MUX_CTRL_5);
    }
    else if(num < 16)
    {
        PIN_SET(MCU_MUX_CTRL_4);
        PIN_CLR(MCU_MUX_CTRL_5);
    }
    else
    {
        PIN_SET(MCU_MUX_CTRL_4);
        PIN_SET(MCU_MUX_CTRL_5);
    }
} 

// -------------------------- ON_P_MIC ON_P_SPK --------------------------
//установить на мс DD3 код value
void set_dd3(uint16_t value)
{
    pca9535_set_reg(&dd3, PCA9535_REG_TYPE_OUTPUT, PCA9535_PORT_BOTH, (uint8_t*)&value, 2);
    p_outs.data.dd3 = value;
}

//установить на мс DD8 код value
void set_dd8(uint16_t value)
{
    pca9535_set_reg(&dd8, PCA9535_REG_TYPE_OUTPUT, PCA9535_PORT_BOTH, (uint8_t*)&value, 2);
    p_outs.data.dd8 = value;
}

//включить/выключить линию on_p = 1...16
void set_on_p(uint8_t num, p_line_state_e state)
{
    if(state == P_ON)
        mux_cancel_all();

    switch (num) 
    {
        case 1:
            p_outs.lines.p_1 = state;
            set_dd3(p_outs.data.dd3);
        break;
        case 2:
            p_outs.lines.p_2 = state;
            set_dd3(p_outs.data.dd3);
        break;
        case 3:
            p_outs.lines.p_3 = state;
            set_dd3(p_outs.data.dd3);
        break;
        case 4:
            p_outs.lines.p_4 = state;
            set_dd3(p_outs.data.dd3);
        break;
        case 5:
            p_outs.lines.p_5 = state;
            set_dd3(p_outs.data.dd3);
        break;
        case 6:
            p_outs.lines.p_6 = state;
            set_dd3(p_outs.data.dd3);
        break;
        case 7:
            p_outs.lines.p_7 = state;
            set_dd3(p_outs.data.dd3);
        break;
        case 8:
            p_outs.lines.p_8 = state;
            set_dd3(p_outs.data.dd3);
        break;
        case 9:
            p_outs.lines.p_9 = state;
            set_dd8(p_outs.data.dd8);
        break;
        case 10:
            p_outs.lines.p_10 = state;
            set_dd8(p_outs.data.dd8);
        break;
        case 11:
            p_outs.lines.p_11 = state;
            set_dd8(p_outs.data.dd8);
        break;
        case 12:
            p_outs.lines.p_12 = state;
            set_dd8(p_outs.data.dd8);
        break;
        case 13:
            p_outs.lines.p_13 = state;
            set_dd8(p_outs.data.dd8);
        break;
        case 14:
            p_outs.lines.p_14 = state;
            set_dd8(p_outs.data.dd8);
        break;
        case 15:
            p_outs.lines.p_15 = state;
            set_dd8(p_outs.data.dd8);
        break;
        case 16:
            p_outs.lines.p_16 = state;
            set_dd8(p_outs.data.dd8);
        break;

        default: break;
    }
}

p_line_state_e get_on_p(uint8_t on_p)
{
    uint32_t res = (p_outs.data.dd8 << 16) | p_outs.data.dd3;
    res >>= (on_p-1)*2; 

    return res&0x3;
}

void mux_init(void)
{
    uint16_t init = 0;

    pca9535_init(&dd3, MUX_I2C_HANDLE, MUX_DD3_ADDERESS);
    if (pca9535_is_ready(&dd3) != 0) 
    {
        ulog("mux: error DD3 not ready"CRLF);
        set_fault(FAULT_HARDWARE_ERROR);
    }

    HAL_Delay(1);
    pca9535_set_reg(&dd3, PCA9535_REG_TYPE_CONFIG, PCA9535_PORT_BOTH, (uint8_t*)&init, 2); //output
    HAL_Delay(1);
    set_dd3(0);
    
    HAL_Delay(1);

    pca9535_init(&dd8, MUX_I2C_HANDLE, MUX_DD8_ADDERESS);
    if (pca9535_is_ready(&dd8) != 0) 
    {
        ulog("mux: error DD4 not ready"CRLF);
        set_fault(FAULT_HARDWARE_ERROR);
    }
    HAL_Delay(1);
    pca9535_set_reg(&dd8, PCA9535_REG_TYPE_CONFIG, PCA9535_PORT_BOTH, (uint8_t*)&init, 2);
    HAL_Delay(1);
    set_dd8(0);
}

bool mux_is_active_line(void)
{
    return !!((p_outs.data.dd3 & 0x5555) || (p_outs.data.dd8 & 0x5555));
}

void mux_cancel_all(void)
{
    set_dd3(p_outs.data.dd3 & ~0x5555);
    set_dd8(p_outs.data.dd8 & ~0x5555);
}

