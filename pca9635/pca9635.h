#ifndef PCA9635_DRIVER_H
#define PCA9635_DRIVER_H

#include "main.h"

typedef struct _pca9635 
{
	I2C_HandleTypeDef* hi2c;
	uint8_t address;
	uint8_t ldrs_data[4];
} pca9635_t;


#define PCA9635_REG_MODE1		0x00
#define PCA9635_REG_MODE2		0x01
#define PCA9635_REG_PWM0		0x02
#define PCA9635_REG_PWM1		0x03
#define PCA9635_REG_PWM2		0x04
#define PCA9635_REG_PWM3		0x05
#define PCA9635_REG_PWM4		0x06
#define PCA9635_REG_PWM5		0x07
#define PCA9635_REG_PWM6		0x08
#define PCA9635_REG_PWM7		0x09
#define PCA9635_REG_PWM8		0x0A
#define PCA9635_REG_PWM9		0x0B
#define PCA9635_REG_PWM10		0x0C
#define PCA9635_REG_PWM11		0x0D
#define PCA9635_REG_PWM12		0x0E
#define PCA9635_REG_PWM13		0x0F
#define PCA9635_REG_PWM14		0x10
#define PCA9635_REG_PWM15		0x11
#define PCA9635_REG_GRPPWM		0x12
#define PCA9635_REG_GRPFREQ		0x13
#define PCA9635_REG_LEDOUT0		0x14
#define PCA9635_REG_LEDOUT1		0x15
#define PCA9635_REG_LEDOUT2		0x16
#define PCA9635_REG_LEDOUT3		0x17
#define PCA9635_REG_SUBADR1		0x18
#define PCA9635_REG_SUBADR2		0x19
#define PCA9635_REG_SUBADR3		0x1A
#define PCA9635_REG_ALLCALLADR	0x1B




#define PCA9635_LDR0	0
#define PCA9635_LDR1	1
#define PCA9635_LDR2	2
#define PCA9635_LDR3	3
#define PCA9635_LDR4	4
#define PCA9635_LDR5	5
#define PCA9635_LDR6	6
#define PCA9635_LDR7	7
#define PCA9635_LDR8	8
#define PCA9635_LDR9	9
#define PCA9635_LDR10	10
#define PCA9635_LDR11	11
#define PCA9635_LDR12	12
#define PCA9635_LDR13	13
#define PCA9635_LDR14	14
#define PCA9635_LDR15	15

//LED driver x is off (default power-up state)
#define PCA9635_LDR_VALUE_OFF		0
//LED driver x is fully on (individual brightness and group dimming/blinking not controlled)
#define PCA9635_LDR_VALUE_ON		1
//LED driver x individual brightness can be controlled through its PWMx register
#define PCA9635_LDR_VALUE_PWM		2
//LED driver x individual brightness and group dimming/blinking can be controlled through its PWMx register and the GRPPWM registers
#define PCA9635_LDR_VALUE_GRPPWM	3

void pca9635_set_ldr(pca9635_t *pca9635, uint8_t ldr, uint8_t ldr_value);
void pca9635_send_ledout(pca9635_t *pca9635, uint8_t reg_ledout);

//#define PCA9635_STATIC_INIT(hi2c, address)		{ hi2c,  address, 0, 0, 0, 0 }

void pca9635_init(pca9635_t* pca9635, I2C_HandleTypeDef *hi2c, uint8_t address);
bool pca9635_is_ready(pca9635_t* pca9635);
void pca9635_reset(pca9635_t* pca9635);

void pca9635_send_ldr(pca9635_t* pca9635, uint8_t ldr, uint8_t ldr_value);
void PCA9635_write_reg(pca9635_t* pca9635, uint8_t reg, uint8_t data);

void PCA9635_init(I2C_HandleTypeDef* hi2c, uint16_t address, uint8_t lpower, uint8_t allcall, uint8_t dmblnk, uint8_t invrt, uint8_t och, uint8_t outdrv, uint8_t outne);
//void PCA9635_write_all_pwm (I2C_HandleTypeDef *hi2c, uint16_t address, uint8_t *pwm_buffer, uint8_t pwm_buffer_size);
//void PCA9635_write_registers (I2C_HandleTypeDef *hi2c, uint8_t address, uint8_t reg, uint8_t data);

#endif //PCA9635_DRIVER_H
