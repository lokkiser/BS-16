#include "TAS5720.h"

uint8_t test_buf[16] = {};
uint8_t test_buf_2[16] = {};
uint8_t i2c_buffer_index[9] = {0, 1, 2, 3, 4, 6, 8, 16, 17};
uint8_t i2c_found_dev[16] = {};
uint8_t i2c_found_dev_2[16] = {};
uint8_t i2c_found_index = 0;
uint8_t i2c_found_index_2 = 0;
uint8_t dbg_0, dbg_1 = 0;

tas5720_t tas5720_t_0, tas5720_t_1, tas5720_t_2, tas5720_t_3, tas5720_t_4, tas5720_t_5, tas5720_t_6, tas5720_t_7,
tas5720_t_8, tas5720_t_9, tas5720_t_10, tas5720_t_11, tas5720_t_12, tas5720_t_13, tas5720_t_14, tas5720_t_15 = {
		//.hi2c,
		.address = 0,
		.ldrs_data = {},

		.deviceid  = 0x01,
		.powerctl  = 0xFD,
		.digctl_1  = 0x04,
		.digctl_2  = 0x80,
		.volume    = 0xCF,
		.analogctl = 0x55,
		.faultcfg  = 0x00,
		.digclip2  = 0xFF,
		.digclip1  = 0xFC,

		.digclip_full = 0x0FFFFF,
		.sleep = 1,
		.sdz = 1,
		.hpf_bypass = 0,
		.tdm_cfg_src = 0,
		.ssz_ds = 0,
		.saif_format = 0b011, //= 0b100,
		.mute = 0,
		.tdm_slot_select = 0,
		.volume_ctl = 0b11001111,
		.pwm_rate = 0,
		.analogue_gain = 0,
		.oc_thresh = 3, //25% RW
		.clke = 0, //R
		.oce = 0, //R
		.dce = 0, //R
		.ote = 0, //R

		.i2c_buffer = {},
};
/*tas5720_t tas5720_t_1;
tas5720_t tas5720_t_2;
tas5720_t tas5720_t_3;
tas5720_t tas5720_t_4;
tas5720_t tas5720_t_5;
tas5720_t tas5720_t_6;
tas5720_t tas5720_t_7;*/

#define TAS5720_I2C_ADDR         (0x6C << 1) // Замените на ваш адрес (0x6C, 0x6D, 0x6E или 0x6F)




/**
  * @brief  Настройка TAS5720 в режим TDM и запуск звука
  * @param  hi2c: указатель на структуру I2C в HAL (например, &hi2c1)
  * @param  tdm_slot: номер целевого TDM слота (от 0 до 7)
  * @retval HAL_StatusTypeDef
  */

HAL_StatusTypeDef TAS5720_WriteReg(uint8_t reg, uint8_t value)
{
    return HAL_I2C_Mem_Write(&hi2c3, TAS5720_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
}
uint8_t TAS5720_Init(void)
{
    uint8_t dev_id = 0;
    // Читаем Device ID для верификации связи
    if (HAL_I2C_Mem_Read(&hi2c3, TAS5720_I2C_ADDR, TAS5720_REG_ID, I2C_MEMADD_SIZE_8BIT, &dev_id, 1, 100) != HAL_OK)
    {
        return 0;
    }
    // РЕГИСТР 0x03: Выбор формата аудио
    // Значение 0x00 устанавливает формат I2S, 24-bit (полное соответствие вашему SAI1-A)
    // Обязательно настраиваем ДО снятия Shutdown, чтобы избежать щелчков.
    TAS5720_WriteReg(TAS5720_REG_DIGITAL_CTRL1, 0b10000000);
    // РЕГИСТР 0x04: Цифровая громкость
    // 0xCF — уровень 0 дБ (номинал без усиления и ослабления)
    TAS5720_WriteReg(TAS5720_REG_VOLUME_CTRL1, 0xCF);
    // РЕГИСТР 0x06: Аналоговое усиление
    // 0x10 — стандартное значение аналогового гейна 19.2 dBV
    TAS5720_WriteReg(TAS5720_REG_ANALOG_CTRL1, 0x0A);
    // РЕГИСТР 0x02: Программный вывод из сна (Делаем в самую последнюю очередь)
    // Записываем 0x01: Бит SPK_SD = 1 (включить ШИМ), Бит SPK_MUTE = 0 (включить звук)
    TAS5720_WriteReg(TAS5720_REG_POWER_CTRL, 0xFD);
    return 1;
}

void i2c_scan()
{
	for (uint8_t i = 1; i<128; i++)
	{
		if(HAL_I2C_IsDeviceReady(&hi2c3, (uint16_t)(i<<1), 3, 5) == HAL_OK){ // есть ответ
			i2c_found_dev[i2c_found_index] = i;
			i2c_found_index++;
		}
		if(HAL_I2C_IsDeviceReady(&hi2c4, (uint16_t)(i<<1), 3, 5) == HAL_OK){ // есть ответ
			i2c_found_dev_2[i2c_found_index_2] = i;
			i2c_found_index_2++;
		}
	}
}

void test_i2c_tas5720()
{
	//HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
    //uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
	volatile static HAL_StatusTypeDef i2c_res = 0;
	for (uint8_t _r = 0; _r < 9; _r++)
	{
		i2c_res = HAL_I2C_Mem_Read(tas5720_t_0.hi2c, tas5720_t_0.address, i2c_buffer_index[_r], 1, &test_buf[_r], 1, 10);
		HAL_Delay(10);
		i2c_res = HAL_I2C_Mem_Read(tas5720_t_1.hi2c, tas5720_t_1.address, i2c_buffer_index[_r], 1, &test_buf_2[_r], 1, 10);
		HAL_Delay(10);
	}

	//HAL_I2C_Master_Receive(tas5720_t_0.hi2c, tas5720_t_0.address, test_buf, 2, 10);
}



void tas5720_init(tas5720_t * tas5720_ex)
{
	/*tas5720_ex->address = 0;
	tas5720_ex->ldrs_data = {};

	tas5720_ex->deviceid[2]  = {0x01, 0x00};
	tas5720_ex->powerctl[2]  = {0xFF, 0x01};
	tas5720_ex->digctl_1[2]  = {0x04, 0x02};
	tas5720_ex->digctl_2[2]  = {0x80, 0x03};
	tas5720_ex->volume[2]    = {0xCF, 0x04};
	tas5720_ex->analogctl[2] = {0x55, 0x06};
	tas5720_ex->faultcfg[2]  = {0x00, 0x08};
	tas5720_ex->digclip2[2]  = {0xFF, 0x10};
	tas5720_ex->digclip1[2]  = {0xFC, 0x11};

	uint32_t digclip_full = 0x0FFFFF;
	tas5720_ex->sleep = 1;
	tas5720_ex->sdz = 1;
	tas5720_ex->hpf_bypass = 0;
	tas5720_ex->tdm_cfg_src = 0;
	tas5720_ex->ssz_ds = 0;
	tas5720_ex->saif_format = 0b100;
	tas5720_ex->mute = 0;
	tas5720_ex->tdm_slot_select = 0;
	tas5720_ex->volume_ctl = 0b11001111;
	tas5720_ex->pwm_rate = 0;
	tas5720_ex->analogue_gain = 0;
	tas5720_ex->oc_thresh = 3; //25% RW
	tas5720_ex->clke = 0; //R
	tas5720_ex->oce = 0; //R
	tas5720_ex->dce = 0; //R
	tas5720_ex->ote = 0; //R

	tas5720_ex->i2c_buffer[12] = {};*/\
	tas5720_t_0.hi2c = &hi2c4;
	tas5720_t_0.address = TAS5720_ch0;
	tas5720_t_1.hi2c = &hi2c4;
	tas5720_t_1.address = TAS5720_ch5;

	//HAL_I2C_Mem_Write(tas5720_t_0.hi2c, tas5720_t_0.address, 0x3, 1, &tas5720_t_0.i2c_buffer[3], 1, 10);
	//uint8_t _num = 0;
	uint8_t _adr = 0;
	/*for (_adr = 0; _adr < 9; _adr++){
		HAL_I2C_Master_Transmit(tas5720_t_0.hi2c, tas5720_t_0.address, &tas5720_t_0.i2c_buffer[_adr], 2, 10);
		//HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,uint16_t Size, uint32_t Timeout)

	}*/
    // 1. Read the current value of Serial Control Register 1
	uint8_t reg_val = 0;
    if (HAL_I2C_Mem_Read(&hi2c4, TAS5720_ch0, 0x01, I2C_MEMADD_SIZE_8BIT, &reg_val, 0, HAL_MAX_DELAY) == HAL_OK)
    {
        // 2. Clear Bit 0 (RSET = 0) to enter sleep mode
        reg_val &= ~(0x01);

        // 3. Write the updated value back to the register
        HAL_I2C_Mem_Write(&hi2c4, TAS5720_ch0, 0x01, I2C_MEMADD_SIZE_8BIT, &reg_val, 0, HAL_MAX_DELAY);
    }

    if (HAL_I2C_Mem_Read(&hi2c4, TAS5720_ch5, 0x01, I2C_MEMADD_SIZE_8BIT, &reg_val, 0, HAL_MAX_DELAY) == HAL_OK)
    {
        // 2. Clear Bit 0 (RSET = 0) to enter sleep mode
        reg_val &= ~(0x01);

        // 3. Write the updated value back to the register
        HAL_I2C_Mem_Write(&hi2c4, TAS5720_ch5, 0x01, I2C_MEMADD_SIZE_8BIT, &reg_val, 0, HAL_MAX_DELAY);
    }

}

void tas5720_init_2()
{
	tas5720_t_0.saif_format = 0b011;
	tas5720_t_0.i2c_buffer[2] = (uint8_t) (  ((tas5720_t_0.hpf_bypass & 0b1) << 8) + ((tas5720_t_0.tdm_cfg_src & 0b1) << 7)
	+ ((tas5720_t_0.ssz_ds & 0b1)<<4) + (tas5720_t_0.saif_format & 0b111)); //2
	HAL_I2C_Mem_Write(tas5720_t_0.hi2c, tas5720_t_0.address, 2, 1, &tas5720_t_0.i2c_buffer[2], 1, 10);

	tas5720_t_1.saif_format = 0b011;
	tas5720_t_1.i2c_buffer[2] = (uint8_t) (  ((tas5720_t_1.hpf_bypass & 0b1) << 8) + ((tas5720_t_1.tdm_cfg_src & 0b1) << 7)
	+ ((tas5720_t_1.ssz_ds & 0b1)<<4) + (tas5720_t_1.saif_format & 0b111)); //2
	HAL_I2C_Mem_Write(tas5720_t_1.hi2c, tas5720_t_1.address, 2, 1, &tas5720_t_1.i2c_buffer[2], 1, 10);

}

/*void TAS5720_EnterSleep(I2C_HandleTypeDef *hi2c)
{
    uint8_t reg_val = 0;

    // 1. Read the current value of Serial Control Register 1
    if (HAL_I2C_Mem_Read(hi2c, TAS5720_I2C_ADDR, TAS5720_REG_SERIAL_CTRL1, I2C_MEMADD_SIZE_8BIT, &reg_val, 1, HAL_MAX_DELAY) == HAL_OK)
    {
        // 2. Clear Bit 0 (RSET = 0) to enter sleep mode
        reg_val &= ~(0x01);

        // 3. Write the updated value back to the register
        HAL_I2C_Mem_Write(hi2c, TAS5720_I2C_ADDR, TAS5720_REG_SERIAL_CTRL1, I2C_MEMADD_SIZE_8BIT, &reg_val, 1, HAL_MAX_DELAY);
    }
}

void TAS5720_ExitSleep(I2C_HandleTypeDef *hi2c)
{
    uint8_t reg_val = 0;

    // 1. Read current register value
    if (HAL_I2C_Mem_Read(hi2c, TAS5720_I2C_ADDR, TAS5720_REG_SERIAL_CTRL1, I2C_MEMADD_SIZE_8BIT, &reg_val, 1, HAL_MAX_DELAY) == HAL_OK)
    {
        // 2. Set Bit 0 (RSET = 1) to enter active operational mode
        reg_val |= 0x01;

        // 3. Write back to register
        HAL_I2C_Mem_Write(hi2c, TAS5720_I2C_ADDR, TAS5720_REG_SERIAL_CTRL1, I2C_MEMADD_SIZE_8BIT, &reg_val, 1, HAL_MAX_DELAY);
    }
}*/

void tas5720_init_3()
{
	/*test_i2c_tas5720();
	tas5720_t_0.sdz = 0;
	tas5720_t_0.i2c_buffer[1] = (uint8_t) ((tas5720_t_0.digclip_full >> 12) & 0b11111100) + (tas5720_t_0.sleep & 0b1) + (tas5720_t_0.sdz & 0b1);//1
	HAL_I2C_Mem_Write(tas5720_t_0.hi2c, tas5720_t_0.address, 1, 1, &tas5720_t_0.i2c_buffer[1], 1, 10);
	tas5720_t_0.i2c_buffer[6] = tas5720_t_0.faultcfg;
	HAL_I2C_Mem_Write(tas5720_t_0.hi2c, tas5720_t_0.address, 6, 1, &tas5720_t_0.i2c_buffer[6], 1, 10);
	HAL_Delay(1000);
	test_i2c_tas5720();
	tas5720_t_0.sdz = 1;
	tas5720_t_0.i2c_buffer[1] = (uint8_t) ((tas5720_t_0.digclip_full >> 12) & 0b11111100) + (tas5720_t_0.sleep & 0b1) + (tas5720_t_0.sdz & 0b1);//1
	HAL_I2C_Mem_Write(tas5720_t_0.hi2c, tas5720_t_0.address, 1, 1, &tas5720_t_0.i2c_buffer[1], 1, 10);
	test_i2c_tas5720();

	test_i2c_tas5720();
	tas5720_t_1.sdz = 0;
	tas5720_t_1.i2c_buffer[1] = (uint8_t) ((tas5720_t_1.digclip_full >> 12) & 0b11111100) + (tas5720_t_1.sleep & 0b1) + (tas5720_t_1.sdz & 0b1);//1
	HAL_I2C_Mem_Write(tas5720_t_1.hi2c, tas5720_t_1.address, 1, 1, &tas5720_t_1.i2c_buffer[1], 1, 10);
	tas5720_t_1.i2c_buffer[6] = tas5720_t_1.faultcfg;
	HAL_I2C_Mem_Write(tas5720_t_1.hi2c, tas5720_t_1.address, 6, 1, &tas5720_t_1.i2c_buffer[6], 1, 10);
	HAL_Delay(1000);
	test_i2c_tas5720();
	tas5720_t_1.sdz = 1;
	tas5720_t_1.i2c_buffer[1] = (uint8_t) ((tas5720_t_1.digclip_full >> 12) & 0b11111100) + (tas5720_t_1.sleep & 0b1) + (tas5720_t_1.sdz & 0b1);//1
	HAL_I2C_Mem_Write(tas5720_t_1.hi2c, tas5720_t_1.address, 1, 1, &tas5720_t_1.i2c_buffer[1], 1, 10);
	test_i2c_tas5720();*/

    uint8_t reg_val = 0;

    if (HAL_I2C_Mem_Read(&hi2c4, TAS5720_ch0, 0x01, I2C_MEMADD_SIZE_8BIT, &reg_val, 0, HAL_MAX_DELAY) == HAL_OK)
    {
        // 2. Clear Bit 0 (RSET = 0) to enter sleep mode
    	reg_val |= 0x01;

        // 3. Write the updated value back to the register
        HAL_I2C_Mem_Write(&hi2c4, TAS5720_ch0, 0x01, I2C_MEMADD_SIZE_8BIT, &reg_val, 0, HAL_MAX_DELAY);
    }

    if (HAL_I2C_Mem_Read(&hi2c4, TAS5720_ch5, 0x01, I2C_MEMADD_SIZE_8BIT, &reg_val, 0, HAL_MAX_DELAY) == HAL_OK)
    {
        // 2. Clear Bit 0 (RSET = 0) to enter sleep mode
    	reg_val |= 0x01;

        // 3. Write the updated value back to the register
        HAL_I2C_Mem_Write(&hi2c4, TAS5720_ch5, 0x01, I2C_MEMADD_SIZE_8BIT, &reg_val, 0, HAL_MAX_DELAY);
    }


    test_i2c_tas5720();
}


void tas5720_form(tas5720_t * tas5720_ex)
{
	tas5720_ex->i2c_buffer[0] = tas5720_ex->deviceid; //0 адрес памяти усилителя
	tas5720_ex->i2c_buffer[1] = (uint8_t) ((tas5720_ex->digclip_full >> 12) & 0b11111100) + (tas5720_ex->sleep & 0b1) + (tas5720_ex->sdz & 0b1);//1
	tas5720_ex->i2c_buffer[2] = (uint8_t) (  (tas5720_ex->hpf_bypass & 0b1 << 8) + ((tas5720_ex->tdm_cfg_src & 0b1) << 7)
	+ ((tas5720_ex->ssz_ds & 0b1)<<4) + (tas5720_ex->saif_format & 0b111)); //2
	tas5720_ex->i2c_buffer[3] = (uint8_t) ((RSV_3_1 << 5) + (RSV_3_2 << 3) + ((tas5720_ex->mute & 0b1) << 4) + (tas5720_ex->tdm_slot_select & 0b111));//3
	tas5720_ex->i2c_buffer[4] = tas5720_ex->volume_ctl; //4
	tas5720_ex->i2c_buffer[5] = (uint8_t) ((RSV_5_1 << 7) + (RSV_5_2) + ((tas5720_ex->pwm_rate & 0b111) << 4) + (tas5720_ex->analogue_gain & 0b11)); //6
	tas5720_ex->i2c_buffer[6] = (uint8_t) ((RSV_6_1 << 6) + ((tas5720_ex->oc_thresh & 0b11) << 4) + ((tas5720_ex->clke & 0b1) << 3)
			+ ((tas5720_ex->oce & 0b1) << 2) + ((tas5720_ex->dce & 0b1) << 1) + (tas5720_ex->ote & 0b1)); //8
	tas5720_ex->i2c_buffer[7] = (uint8_t) ((tas5720_ex->digclip_full >> 6)); //16
	tas5720_ex->i2c_buffer[8] = (uint8_t) ((tas5720_ex->digclip_full & 0b111111 << 5) + (tas5720_ex->tdm_slot_select & 0b111)); //17
}

