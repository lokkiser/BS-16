#include "pca9635.h"



void PCA9635_write_reg(pca9635_t* pca9635, uint8_t reg, uint8_t data)
{
	uint8_t tx[2] = { reg, data };

	HAL_I2C_Master_Transmit(pca9635->hi2c, pca9635->address, tx, sizeof(tx), HAL_MAX_DELAY);
}


void pca9635_set_ldr(pca9635_t* pca9635, uint8_t ldr, uint8_t value)
{
	switch (ldr) 
	{
		case  PCA9635_LDR0:
			pca9635->ldrs_data[0] = ( (pca9635->ldrs_data[0] & 0xFC) | value );
		break;
		case  PCA9635_LDR1:
			pca9635->ldrs_data[0] = ( (pca9635->ldrs_data[0] & 0xF3) | (value << 2) );
		break;
		case  PCA9635_LDR2:
			pca9635->ldrs_data[0] = ( (pca9635->ldrs_data[0] & 0xCF) | (value << 4) );
		break;
		case  PCA9635_LDR3:
			pca9635->ldrs_data[0] = ( (pca9635->ldrs_data[0] & 0x3F) | (value << 6) );
		break;
		case  PCA9635_LDR4:
			pca9635->ldrs_data[1] = ( (pca9635->ldrs_data[1] & 0xFC) | value );
		break;
		case  PCA9635_LDR5:
			pca9635->ldrs_data[1] = ( (pca9635->ldrs_data[1] & 0xF3) | (value << 2) );
		break;
		case  PCA9635_LDR6:
			pca9635->ldrs_data[1] = ( (pca9635->ldrs_data[1] & 0xCF) | (value << 4) );
		break;
		case  PCA9635_LDR7:
			pca9635->ldrs_data[1] = ( (pca9635->ldrs_data[1] & 0x3F) | (value << 6) );
		break;
		case  PCA9635_LDR8:
			pca9635->ldrs_data[2] = ( (pca9635->ldrs_data[2] & 0xFC) | value );
		break;
		case  PCA9635_LDR9:
			pca9635->ldrs_data[2] = ( (pca9635->ldrs_data[2] & 0xF3) | (value << 2) );
		break;
		case  PCA9635_LDR10:
			pca9635->ldrs_data[2] = ( (pca9635->ldrs_data[2] & 0xCF) | (value << 4) );
		break;
		case  PCA9635_LDR11:
			pca9635->ldrs_data[2] = ( (pca9635->ldrs_data[2] & 0x3F) | (value << 6) );
		break;
		case  PCA9635_LDR12:
			pca9635->ldrs_data[3] = ( (pca9635->ldrs_data[3] & 0xFC) | value );
		break;
		case  PCA9635_LDR13:
			pca9635->ldrs_data[3] = ( (pca9635->ldrs_data[3] & 0xF3) | (value << 2) );
		break;
		case  PCA9635_LDR14:
			pca9635->ldrs_data[3] = ( (pca9635->ldrs_data[3] & 0xCF) | (value << 4) );
		break;
		case  PCA9635_LDR15:
			pca9635->ldrs_data[3] = ( (pca9635->ldrs_data[3] & 0x3F) | (value << 6) );
		break;
		
		default: break;
	}
}



void pca9635_send_ledout(pca9635_t* pca9635, uint8_t reg_ledout)
{
	PCA9635_write_reg(pca9635, reg_ledout, pca9635->ldrs_data[reg_ledout - PCA9635_REG_LEDOUT0]);
}

//выключить, включить всяко разно светодиод
void pca9635_send_ldr(pca9635_t* pca9635, uint8_t ldr, uint8_t ldr_value)
{
	uint8_t reg_ledout;

	pca9635_set_ldr(pca9635, ldr, ldr_value);
	if (ldr < PCA9635_LDR4) 
	{
		reg_ledout = PCA9635_REG_LEDOUT0;
	}
	else if (ldr < PCA9635_LDR8) 
	{
		reg_ledout = PCA9635_REG_LEDOUT1;
	}
	else if (ldr < PCA9635_LDR12) 
	{
		reg_ledout = PCA9635_REG_LEDOUT2;
	}
	else 
	{
		reg_ledout = PCA9635_REG_LEDOUT3;
	}

	pca9635_send_ledout(pca9635, reg_ledout);
}


void pca9635_init(pca9635_t* pca9635, I2C_HandleTypeDef *hi2c, uint8_t address)
{
	pca9635->hi2c = hi2c;
	pca9635->address = (address << 1);
	pca9635->ldrs_data[0] = 0;
	pca9635->ldrs_data[1] = 0;
	pca9635->ldrs_data[2] = 0;
	pca9635->ldrs_data[3] = 0;
}

bool pca9635_is_ready(pca9635_t* pca9635)
{
	 return HAL_I2C_IsDeviceReady(pca9635->hi2c, pca9635->address, 3, 1000) == HAL_OK;
}


void pca9635_reset(pca9635_t* pca9635)
{
	uint8_t tx[3] = { 0x03, 0xA5, 0x5A };

	HAL_I2C_Master_Transmit(pca9635->hi2c, pca9635->address, tx, sizeof(tx), HAL_MAX_DELAY);

	HAL_Delay(100);
	HAL_I2C_Master_Transmit(pca9635->hi2c, 0x03, &tx[1], 2, HAL_MAX_DELAY);
	HAL_Delay(100);
}


void PCA9635_write_registers (I2C_HandleTypeDef* hi2c, uint16_t address, uint8_t reg, uint8_t data)
{
	uint8_t tx[2] = { reg, data };

	HAL_I2C_Master_Transmit(hi2c, address, tx, sizeof(tx), HAL_MAX_DELAY);
}


void PCA9635_write_all_pwm (I2C_HandleTypeDef* hi2c, uint16_t address, uint8_t* pwm_buffer, uint8_t pwm_buffer_size)
{
	uint8_t tx[18];

	tx[0] = PCA9635_REG_PWM0 | 0x80; //go to register and enable auto incriment

	//copy pwm array to twm buffer
	for(uint8_t i = 0; i < pwm_buffer_size; i++ )
	{
		tx[1 + i] = pwm_buffer[i];
	}

	HAL_I2C_Master_Transmit(hi2c, address, tx, 1 + pwm_buffer_size, HAL_MAX_DELAY);
}

//initialize device and set modes, check datasheet
//(addres, bool sleep mode, bool allcall, bool dmblnk, bool invrt, bool och, bool outdrv, outne can 0-2)
void PCA9635_init(I2C_HandleTypeDef* hi2c, uint16_t address, uint8_t lpower, uint8_t allcall, uint8_t dmblnk, uint8_t invrt, uint8_t och, uint8_t outdrv, uint8_t outne)
{
	uint8_t tx[5];

	tx[0] = PCA9635_REG_MODE1 | 0x80; // go to register and enable auto incriment
	tx[1] = (lpower << 4) | (allcall << 0); //set mode 1
	tx[2] = (dmblnk << 5) | (invrt << 4) | (och << 3) | (outdrv << 2) | (outne << 0); //set mode 2

	HAL_I2C_Master_Transmit(hi2c, address, tx, 3, HAL_MAX_DELAY);

	HAL_Delay(1);

	//now set all leds to PWM and GRP PWM mode
	tx[0] = PCA9635_REG_LEDOUT0 | 0x80; //go to ledout0 and enable auto incriment
	tx[1] = 0xff;
	tx[2] = 0xff;
	tx[3] = 0xff;
	tx[4] = 0xff;

	HAL_I2C_Master_Transmit(hi2c, address, tx, 4, HAL_MAX_DELAY);
}


