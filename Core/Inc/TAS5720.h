
#ifndef __TAS5720_H
#define __TAS5720_H
#endif


#include "main.h"
#include "stm32g4xx_hal_i2c.h"
#define TAS5720_ch0 0xD8 //полный адрес
#define TAS5720_ch1 0xDA
#define TAS5720_ch2 0xDC
#define TAS5720_ch3 0xDE
#define TAS5720_ch4 0xE0
#define TAS5720_ch5 0xE2
#define TAS5720_ch6 0xE4
#define TAS5720_ch7 0xE6

#define RSV_2_1 0b00
#define RSV_3_1 0b100
#define RSV_3_2 0b0
#define RSV_5_1 0b0
#define RSV_5_2 0b01
#define RSV_6_1 0b00
#define RSV_8_1 0b00

// Адреса регистров
#define TAS5720_REG_ID  0x00
#define TAS5720_REG_POWER_CTRL  0x01
#define TAS5720_REG_DIGITAL_CTRL1  0x02
#define TAS5720_REG_DIGITAL_CTRL2 0x03
#define TAS5720_REG_VOLUME_CTRL1  0x04
#define TAS5720_REG_ANALOG_CTRL1  0x06
#define TAS5720_REG_FAULT_STATUS  0x08
#define TAS5720_I2C_ADDR        (0x6C << 1) // 7-bit 0x6C превращаем в 8-bit 0xD8

#define TOTAL_SAMPLES 2048
#define TDM_CHANNELS  8 // Например, стандартный TDM-8 frame
#define AUDIO_BUF_SIZE 4096

extern uint8_t i2c_buffer_index[9]; //массив адресов
extern uint8_t dbg_0, dbg_1;
extern uint8_t i2c_found_dev[16];
extern uint8_t i2c_found_dev_2[16];

extern uint32_t sai_buffer[TOTAL_SAMPLES * TDM_CHANNELS];
extern uint8_t tas_i2c_debug_status;
extern uint8_t refill_arr;
extern uint8_t tdm_active_ch;
extern uint8_t disable_sound;

typedef struct _tas5720 
{
	I2C_HandleTypeDef* hi2c ;
	uint16_t address; //i2cшный адрес прибора
	uint8_t ldrs_data[4]; // ??????
	// первый регистр содержимое, второе адрес в памяти TAS5720
	uint8_t deviceid;
	uint8_t powerctl;
	uint8_t digctl_1;
	uint8_t digctl_2;
	uint8_t volume;
	uint8_t analogctl;
	uint8_t faultcfg;
	uint8_t digclip2;
	uint8_t digclip1;

	uint32_t digclip_full;
	uint8_t sleep;
	uint8_t sdz;
	uint8_t hpf_bypass;
	uint8_t tdm_cfg_src;
	uint8_t ssz_ds;
	uint8_t saif_format;
	uint8_t mute;
	uint8_t tdm_slot_select;
	uint8_t volume_ctl;
	uint8_t pwm_rate;
	uint8_t analogue_gain;
	uint8_t oc_thresh; //25% RW
	uint8_t clke; //R
	uint8_t oce; //R
	uint8_t dce; //R
	uint8_t ote; //R

	uint8_t i2c_buffer[12];
	//uint8_t i2c_buffer_index[9];
} tas5720_t;

extern tas5720_t tas5720_t_0, tas5720_t_1, tas5720_t_2, tas5720_t_3, tas5720_t_4, tas5720_t_5, tas5720_t_6, tas5720_t_7,
tas5720_t_8, tas5720_t_9, tas5720_t_10, tas5720_t_11, tas5720_t_12, tas5720_t_13, tas5720_t_14, tas5720_t_15;
void i2c_scan();
void test_i2c_tas5720();
void tas5720_init(tas5720_t *);
void tas5720_init_2();
void tas5720_init_3();
HAL_StatusTypeDef TAS5720_WriteReg(uint8_t reg, uint8_t value);
uint8_t TAS5720_Init(void);
uint8_t Audio_System_Start(void);
void sin_gen();
