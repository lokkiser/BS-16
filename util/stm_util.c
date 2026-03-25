#include "stm_util.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
//#include <stm32f2xx_hal_flash_ex.h>
//#include <stm32f205xx.h>
#include "str_util.h"

const portpin_t portpin_NULL = {NULL, 0};

#pragma GCC push_options
#pragma GCC optimize ("O0")
void delay_us_cycle(uint32_t us) {
	volatile uint32_t counter = DELAY_US_CYCLE_TIME * us;
	while(counter--);
}
#pragma GCC pop_options

#ifdef USE_RCC

uint32_t PCLK1TIM(void)                // F4 Example
{
    /* Get PCLK1 frequency */
    uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();

    /* Get PCLK1 prescaler */
    if ((RCC->CFGR & RCC_CFGR_PPRE1) == 0) {
        /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
        return (pclk1);
    } else {
        /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
        return (2 * pclk1);
    }
}
#endif

#ifdef HAL_TIM_MODULE_ENABLED
void _delay_us_tim(TIM_HandleTypeDef * htim, uint32_t us)
{
    __HAL_TIM_SET_COUNTER(htim, 0); // set the counter value a 0
    while (__HAL_TIM_GET_COUNTER(htim) < us);        // wait for the counter to reach the us input in the parameter
}
#endif

//установлен ли выход
GPIO_PinState OutPinIsSetted(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	GPIO_PinState bitstatus;

	/* Check the parameters */
	assert_param(IS_GPIO_PIN(GPIO_Pin));

	if((GPIOx->ODR & GPIO_Pin) != (uint32_t)GPIO_PIN_RESET)
	{
		bitstatus = GPIO_PIN_SET;
	}
	else
	{
		bitstatus = GPIO_PIN_RESET;
	}

	return bitstatus;
}

void get_stm32_tim_freq_param(uint32_t base_freq, uint32_t required_freq, uint16_t * prescaler, uint16_t * counter_period)
{
	uint16_t p, c;
	uint32_t freq;
	uint32_t min_diff_freq = 0xFFFFFFFF;
	uint32_t diff_freq;
	uint16_t min_diff_p_c = 0xFFFF;
	uint16_t diff_p_c;

	for (c = 2; c < 0xFFFF; c++) 
	{
		p = base_freq / (c * required_freq);
		if (p < 2) continue;
		freq = (base_freq  / p) / c;

		if (freq > required_freq) diff_freq = freq - required_freq;
		else diff_freq = required_freq - freq;

		if (diff_freq <= min_diff_freq) 
		{
		    min_diff_freq = diff_freq;

		    if (p > c) diff_p_c = p - c;
		    else diff_p_c = c - p;

		    if (diff_p_c <= min_diff_p_c) 
		    {
		       min_diff_p_c = diff_p_c;
		       *prescaler = p;
		       *counter_period = c;
		    }
		}
	}
}

#ifdef HAL_TIM_MODULE_ENABLED
void set_tim_frequency(TIM_TypeDef * ptim, uint32_t frequency, uint32_t base_frequency)
{
	uint16_t prescaler;
	uint16_t counter_period;

	get_stm32_tim_freq_param(base_frequency, frequency, &prescaler, &counter_period);

	ptim->PSC = prescaler - 1;
	ptim->ARR = counter_period - 1;
	ptim->EGR |= TIM_EGR_UG;
}

void set_tim_frequency2(TIM_HandleTypeDef * htim, uint32_t frequency, uint32_t base_frequency)
{
	set_tim_frequency(htim->Instance, frequency, base_frequency);
}


uint32_t get_tim_frequency(TIM_TypeDef * ptim, uint32_t base_frequency)
{
  return (base_frequency / (ptim->PSC + 1)) / (ptim->ARR + 1);
}

uint32_t get_tim_frequency2(TIM_HandleTypeDef * htim,  uint32_t base_frequency)
{
	return get_tim_frequency(htim->Instance, base_frequency);
}

void set_tim_frequency2_and_start(TIM_HandleTypeDef * htim, uint32_t frequency, uint32_t base_frequency)
{
	if (get_tim_frequency2(htim, base_frequency) != frequency) {
		//HAL_TIM_Base_Stop(htim);
		set_tim_frequency2(htim, frequency, base_frequency);
	}
	if (htim->State == HAL_TIM_STATE_READY) HAL_TIM_Base_Start(htim);
}

uint64_t HAL_GetTickUs(TIM_HandleTypeDef * htim)
{
	return 1000 * HAL_GetTick() + __HAL_TIM_GET_COUNTER(htim);
}

uint64_t HAL_DiffTickUs(uint64_t told, TIM_HandleTypeDef * htim)
{
	uint64_t tnew = HAL_GetTickUs(htim);

	return DIFF_TICKU64(told, tnew);
}

uint32_t HAL_DiffTick(uint32_t told)
{
	uint32_t tnew = HAL_GetTick();

	return DIFF_TICKU32(told, tnew);
}

void HAL_DelayUs(uint64_t Delay, TIM_HandleTypeDef * htim)
{
	uint64_t tnew, told;

	told = HAL_GetTickUs(htim);

	do 
	{
		tnew = HAL_GetTickUs(htim);
	}
	while (DIFF_TICKU64(told, tnew) < Delay);

}
#endif

uint32_t HAL_GetTickNoZero(void)
{
	uint32_t res;

	res = HAL_GetTick();
	if (!res) res = 1;
	return res;
}

void HAL_DelayTick(uint32_t Delay)
{
	uint32_t tnew, told;

	told = HAL_GetTick();

	do 
	{
		tnew = HAL_GetTick();
	}
	while (DIFF_TICKU32(told, tnew) < Delay);
}


void tick_to_time(uint32_t tick, uint8_t* hour, uint8_t* minute, uint8_t* sec, uint16_t* milisec)
{
	uint32_t s = (tick / 1000);
	*milisec = tick - s * 1000;
	uint32_t m = s / 60;
	*sec = s - m * 60;
	*hour = m / 60;
	*minute = m - *hour * 60;
}

#ifdef HAL_ADC_MODULE_ENABLED
uint32_t get_adc_value(ADC_HandleTypeDef* hadc, uint32_t Timeout)
{
	HAL_ADC_Start(hadc); // запускаем преобразование сигнала АЦП
    HAL_ADC_PollForConversion(hadc, Timeout); // ожидаем окончания преобразования
    uint32_t res = HAL_ADC_GetValue(hadc); // читаем полученное значение в переменную adc
    HAL_ADC_Stop(hadc); // останавливаем АЦП
	return res;
}

#ifdef HAL_ADC_INJECTED_ENABLED
uint32_t get_injected_adc_value(ADC_HandleTypeDef* hadc, uint32_t InjectedRank, uint32_t Timeout)
{
	HAL_ADCEx_InjectedStart(hadc); // запускаем опрос инжект. каналов
	HAL_ADC_PollForConversion(hadc, Timeout); // ждём окончания
	uint32_t res = HAL_ADCEx_InjectedGetValue(hadc, InjectedRank);
	HAL_ADCEx_InjectedStop(hadc);
	return res;
}
#endif

#endif

void ulog_fmt(const char * fmt, ... )
{
	char buf[MAX_STR_SIZE];

	va_list arg;

	va_start(arg, fmt);

	vsnprintf(buf, sizeof(buf), fmt, arg);

	va_end(arg);

	buf[sizeof(buf) - 1] = 0;
	ulog(buf);
}


char fulog_buf[MAX_STR_SIZE] = {0};


void fulog(char * str)
{
	strcat_s(fulog_buf, sizeof(fulog_buf), str);
}

void fulog_fmt(const char * fmt, ... )
{
	char buf[MAX_STR_SIZE];

	va_list arg;

	va_start(arg, fmt);

	vsnprintf(buf, sizeof(buf), fmt, arg);

	va_end(arg);

	buf[sizeof(buf) - 1] = 0;
	fulog(buf);
}


#ifdef HAL_USE_FLASH


//переход к Application (файл bin)
void go_to_application(uint32_t ApplicationAddr)
{
    uint32_t app_jump_address;

    typedef void(*pFunction)(void);//объявляем пользовательский тип
    pFunction Jump_To_Application;//и создаём переменную этого типа


    app_jump_address = *( uint32_t *) (ApplicationAddr + 4);    //извлекаем адрес перехода из вектора Reset
	Jump_To_Application = (pFunction)app_jump_address;            //приводим его к пользовательскому типу

	//__disable_interrupt();
	__disable_irq();

    SysTick->CTRL =0;
	SysTick->LOAD=0;
	SysTick->VAL=0;

	//Set the clock to the default state
	HAL_RCC_DeInit();


	// отключаем все прерывания в NVIC, очищаем все pending bit
	for (int i=0;i<5;i++)  
	{
		NVIC->ICER[i]=0xFFFFFFFF;
		NVIC->ICPR[i]=0xFFFFFFFF;
	}

	//Peripheral  risit disable
	RCC->AHB1RSTR = 0;
	RCC->AHB2RSTR = 0;
	RCC->AHB3RSTR = 0;
	//RCC->APB1RSTR = 0;
	RCC->APB2RSTR = 0;
	//Peripheral  disable clock
	//RCC->AHB1ENR = 0x00100000;
	RCC->AHB2ENR = 0;
	RCC->AHB3ENR = 0;
	//RCC->APB1ENR = 0;
	RCC->APB2ENR = 0;

	NVIC_SetVector(ApplicationAddr, 0);
	SCB->VTOR = ApplicationAddr;
    __set_MSP(*(__IO uint32_t*)ApplicationAddr);          //устанавливаем SP приложения
    __set_CONTROL(0x00000000);
    __enable_irq();

    Jump_To_Application();		                        //запускаем приложение
}

void go_to_application2(uint32_t ApplicationAddr)
{
	SCB->VTOR = ApplicationAddr;
	__set_MSP(*(__IO uint32_t*)ApplicationAddr);
	NVIC_SystemReset();
}

//загружена ли программа по адресу
bool is_programm_download(uint32_t addr, uint32_t size)
{
	uint32_t addr_jump = *( uint32_t *)(addr + 4);    //извлекаем адрес перехода из вектора Reset
	//ulog_fmt("is_programm_download addr=%x addr_jump=%x\n", (unsigned int)addr, (unsigned int)addr_jump);
	return (addr_jump > addr) && (addr_jump < addr + size);
}

#endif

uint8_t* stm_uid(void)
{
	static uint8_t _stm_uid_buf[12];
	memcpy(_stm_uid_buf, (uint8_t*)(UID_BASE), sizeof(_stm_uid_buf));
	return _stm_uid_buf;
}


char* get_stm_uid(void)
{
	static char get_stm_uid_buf[64] = {0,};
	uint16_t *idBase0 = (uint16_t*)(UID_BASE);
	uint16_t *idBase1 = (uint16_t*)(UID_BASE + 0x02);
	uint32_t *idBase2 = (uint32_t*)(UID_BASE + 0x04);
	uint32_t *idBase3 = (uint32_t*)(UID_BASE + 0x08);

	sprintf(get_stm_uid_buf, "%x-%x-%lx-%lx\n", *idBase0, *idBase1, *idBase2, *idBase3);
	return get_stm_uid_buf;
}

#ifdef HAL_USE_FLASH

#ifndef FLASH_TIMEOUT_VALUE
#define FLASH_TIMEOUT_VALUE       50000U   /* 50 s */
#endif


void hal_flash_wait_for_last_operation(void)
{
#ifdef FLASH_WAIT_IS_USE_BANK
	FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE, FLASH_BANK_1);
#else
	FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
#endif
}

void hal_flash_unlocked(void)
{
	HAL_FLASH_Unlock(); // Разблокируем флеш память
	hal_flash_wait_for_last_operation();

}

void hal_flash_locked(void)
{
	HAL_FLASH_Lock();  // Блокируем флеш память
	hal_flash_wait_for_last_operation();
}

HAL_StatusTypeDef hal_write_data_to_flash(uint32_t address, uint8_t* data, uint32_t data_len)
{
	uint32_t i;
	HAL_StatusTypeDef res;
	uint32_t flash_address;

	flash_address = address;
	res = HAL_OK;


	hal_flash_unlocked();

#ifdef FLASH_TYPEPROGRAM_BYTE
	for (i = 0; i < data_len; ++i) 
	{
		res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, flash_address, data[i]);
		hal_flash_wait_for_last_operation();
		if (res != HAL_OK) break;
		flash_address++;
	}
#else

#ifdef FLASH_TYPEPROGRAM_HALFWORD

	uint16_t * pval, val;

	i = 0;
	pval = (uint32_t *)data;
	while (data_len > 1) 
	{
		res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_address, pval[i]);
		hal_flash_wait_for_last_operation();
		if (res != HAL_OK) 	break;
		data_len -= 2;
		flash_address += 2;
		i++;
	}

	if (data_len) 
	{
		val = 0;
		memcpy(&val, &pval[i], data_len);
		res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_address, val);
		hal_flash_wait_for_last_operation();
	}

#else

#ifdef FLASH_TYPEPROGRAM_WORD

	uint32_t * pval, val;

	i = 0;
	pval = (uint32_t*)data;
	while (data_len > 3) 
	{
		res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_address, pval[i]);
		hal_flash_wait_for_last_operation();
		if (res != HAL_OK) 	break;
		data_len -= 4;
		flash_address += 4;
		i++;
	}

	if (data_len) 
	{
		val = 0;
		memcpy(&val, &pval[i], data_len);
		res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_address, val);
		hal_flash_wait_for_last_operation();
	}
#else

#ifdef FLASH_TYPEPROGRAM_DOUBLEWORD
	uint64_t * pval, val;

	i = 0;
	pval = (uint64_t*)data;
	while (data_len > 7) 
	{
		res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_address, pval[i]);
		hal_flash_wait_for_last_operation();
		if (res != HAL_OK) 	break;
		data_len -= 8;
		flash_address += 8;
		i++;
	}

	if (data_len) 
	{
		val = 0;
		memcpy(&val, &pval[i], data_len);
		res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_address, val);
		hal_flash_wait_for_last_operation();
	}
#else
#error Not supported other type program
#endif //FLASH_TYPEPROGRAM_DOUBLEWORD

#endif //FLASH_TYPEPROGRAM_WORD

#endif //FLASH_TYPEPROGRAM_HALFWORD

#endif

	if (res != HAL_OK) 	ulog_fmt("hal_write_data_to_flash: error write to address:0x%s"CRLF, ul2hexstr(flash_address));

	hal_flash_locked();

	return res;
}

typedef struct __stm32_type_flash_part 
{
	uint32_t number;
	uint32_t count;
	uint32_t size;
} stm32_type_flash_part_t;

void stm32_get_bank_sector_page(uint8_t stm32flashtype, uint32_t address, stm32_type_flash_part_t * bank, stm32_type_flash_part_t * sector, stm32_type_flash_part_t * page)
{

	 uint32_t addr_diff_base;

	 memset(bank, 0xFF, sizeof(stm32_type_flash_part_t));
	 memset(sector, 0xFF, sizeof(stm32_type_flash_part_t));
	 memset(page, 0xFF, sizeof(stm32_type_flash_part_t));

	 addr_diff_base = address - FLASH_BASE;

	switch (stm32flashtype) {
		case STM32F1XX_LOW_FLASH_TYPE:
			bank->number = 1;
			page->size = 1024;
			if (addr_diff_base < 32 * 1024) page->number = addr_diff_base / page->size;
			page->count = 32;
			break;
		case STM32F1XX_MEDIUM_FLASH_TYPE:
			bank->number = 1;
			page->size = 1024;
			if (addr_diff_base < 128 * 1024) page->number = addr_diff_base / page->size;
			page->count = 128;
			break;
		case STM32F1XX_HIGH_FLASH_TYPE:
			bank->number = 1;
			page->size = 2 * 1024;
			if (addr_diff_base < 512 * 1024) page->number = addr_diff_base / page->size;
			page->count = 256;
			break;
		case STM32F1XX_CONNECT_FLASH_TYPE:
			bank->number = 1;
			page->size = 2 * 1024;
			if (addr_diff_base < 256 * 1024) page->number = addr_diff_base / page->size;
			page->count = 128;
			break;
		case STM32F2XX_FLASH_TYPE:
			bank->number = 1;
			//sector->size dinamic
			sector->count = 12;
			if (address < 0x08010000) {
				//sector->size = 16 * 1024;
				sector->number = addr_diff_base / (16 * 1024);
			}
			else if (address < 0x08020000) {
				//sector->size = 64 * 1024;
				sector->number = 4;
			}
			else if (address < 0x08100000) {
				//sector->size = 128 * 1024;
				sector->number = (address - 0x08020000) / (128 * 1024);
			}
			break;
		case STM32G4XX_SINGLE_BANK_FLASH_TYPE:
			bank->number = 1;
			page->size = 4 * 1024;
			if (addr_diff_base < 512 * 1024) page->number = addr_diff_base / page->size;
			page->count = 128;
			break;
		case STM32G4XX_DUAL_BANK_FLASH_TYPE:
			page->size = 2 * 1024;
			if (address < 0x08040000) {
				bank->number = 1;
				page->number = addr_diff_base / page->size;
			}
			else if (address < 0x08080000) {
				bank->number = 2;
				page->number = (address - 0x08040000) / page->size;
			}
			page->count = 128;
			break;
		case STM32H7XX_FLASH_TYPE: //не точно
			page->size = 4 * 1024;
			if (address < 0x08100000) {
				bank->number = 1;
				page->number = addr_diff_base / page->size;
			}
			else if (address < 0x08200000) {
				bank->number = 2;
				page->number = (address - 0x08040000) / page->size;
			}
			page->count = 256;
			break;
	}

}


uint32_t stm32_get_bank(uint8_t stm32flashtype, uint32_t address)
{
	switch (stm32flashtype) {
		case STM32F1XX_LOW_FLASH_TYPE: return 1;
		case STM32F1XX_MEDIUM_FLASH_TYPE: return 1;
		case STM32F1XX_HIGH_FLASH_TYPE: return 1;
		case STM32F1XX_CONNECT_FLASH_TYPE: return 1;
		case STM32F2XX_FLASH_TYPE: return 1;
		case STM32G4XX_SINGLE_BANK_FLASH_TYPE: return 1;
		case STM32G4XX_DUAL_BANK_FLASH_TYPE: return address < 0x08040000 ? 1 : 2;
		case STM32H7XX_FLASH_TYPE: return address < 0x08100000 ? 1 : 2; //не точно
	}
	return 0;
}

uint32_t stm32_get_sector_or_page_count(uint8_t stm32flashtype)
{
	switch (stm32flashtype) {
		case STM32F1XX_LOW_FLASH_TYPE: return 32;
		case STM32F1XX_MEDIUM_FLASH_TYPE: return 128;
		case STM32F1XX_HIGH_FLASH_TYPE: return 256;
		case STM32F1XX_CONNECT_FLASH_TYPE: return 128;
		case STM32F2XX_FLASH_TYPE: return 12;
		case STM32G4XX_SINGLE_BANK_FLASH_TYPE: return 128;
		case STM32G4XX_DUAL_BANK_FLASH_TYPE: return 256;
		case STM32H7XX_FLASH_TYPE: return 256; //не точно
	}
	return 0;
}


uint32_t stm32_get_sector_or_page_size(uint8_t stm32flashtype, uint32_t number)
{

	if (number >= stm32_get_sector_or_page_count(stm32flashtype)) return 0;

	switch (stm32flashtype) {
		case STM32F1XX_LOW_FLASH_TYPE: return 1024;
		case STM32F1XX_MEDIUM_FLASH_TYPE: return 1024;
		case STM32F1XX_HIGH_FLASH_TYPE: return 2 * 1024;
		case STM32F1XX_CONNECT_FLASH_TYPE: return 2 * 1024;
		case STM32F2XX_FLASH_TYPE:
			switch(number) {
				case 0:
				case 1:
				case 2:
				case 3:
					return 16 * 1024;
				case 4:
					return 64 * 1024;
				default:
					return 128 * 1024;
			}
		case STM32G4XX_SINGLE_BANK_FLASH_TYPE:	return 4 * 1024;
		case STM32G4XX_DUAL_BANK_FLASH_TYPE:	return 2 * 1024;
		case STM32H7XX_FLASH_TYPE: return 4 * 1024; //не точно
	}
	return 0;
}



int stm32_get_sector_or_page_number(uint32_t address, uint8_t stm32flashtype)
{
  uint32_t addr_begin, addr_end, i;
  uint32_t sector_or_page_count, sector_or_page_size;

  sector_or_page_count = stm32_get_sector_or_page_count(stm32flashtype);
  if (!sector_or_page_count) return -1;


  addr_begin = FLASH_BASE;

  for (i = 0; i < sector_or_page_count; ++i) {
	  sector_or_page_size = stm32_get_sector_or_page_size(stm32flashtype, i);
	  addr_end = addr_begin + sector_or_page_size;
	  if ((address >= addr_begin) && (address < addr_end)) return i;
	  addr_begin += sector_or_page_size;
  }


  return -1;
}

#ifdef	HAL_USE_SECTOR_NUMBER_ERASE

HAL_StatusTypeDef hal_erase_sectors(uint32_t Banks, uint32_t Sector, uint32_t NbSectors)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SectorError;
	HAL_StatusTypeDef res;

	SectorError = 0;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.Banks = Banks;
	EraseInitStruct.Sector = Sector;
	EraseInitStruct.NbSectors = NbSectors;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_1;

	hal_flash_unlocked();
	res = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
	hal_flash_wait_for_last_operation();
	hal_flash_locked();


	return res;
}

#endif



#ifdef	HAL_USE_PAGE_NUMBER_ERASE

HAL_StatusTypeDef hal_erase_pages_number(uint32_t Banks, uint32_t PageNumber, uint32_t NbPages)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError = 0;
	HAL_StatusTypeDef res;

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks = Banks;
	EraseInitStruct.Page = PageNumber;
	EraseInitStruct.NbPages = NbPages;

	hal_flash_unlocked();
	res = HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);
	FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
	hal_flash_locked();

	return res;
}


#endif


#ifdef	HAL_USE_PAGE_ADDR_ERASE

HAL_StatusTypeDef hal_erase_pages_address(uint32_t PageAddress, uint32_t NbPages)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError = 0;
	HAL_StatusTypeDef res;

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = PageAddress;
	EraseInitStruct.NbPages     = NbPages;

	hal_flash_unlocked();
	res = HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);
	FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);  // Ожидаем окончания операции
	hal_flash_locked();

	return res;
}

#endif


HAL_StatusTypeDef hal_erase_by_address(uint32_t address_begin, uint32_t address_end, uint8_t stm32flashtype)
{
	int erase_begin, erase_end;
	HAL_StatusTypeDef res;
	uint32_t bank;

	erase_begin = stm32_get_sector_or_page_number(address_begin, stm32flashtype);
	erase_end = stm32_get_sector_or_page_number(address_end, stm32flashtype);

	if ((erase_begin < 0) || (erase_end < 0)) {
	    ulog("hal_erase_by_address: error invalid page or sector to erase"CRLF);
	    return HAL_ERROR;
	}

	bank = stm32_get_bank(stm32flashtype, address_begin);
	if (!bank) {
	    ulog("hal_erase_by_address: error invalid bank"CRLF);
	    return HAL_ERROR;
	}

#ifdef	HAL_USE_SECTOR_NUMBER_ERASE
    ulog_fmt("hal_erase_by_address: erase bank %lu, from %d to %d sector"CRLF, bank, erase_begin, erase_end);
	res = hal_erase_sectors(bank, erase_begin, erase_end - erase_begin + 1);
#else

#ifdef	HAL_USE_PAGE_NUMBER_ERASE
	if (stm32flashtype == STM32G4XX_DUAL_BANK_FLASH_TYPE) {
		if (erase_begin > 127) erase_begin -= 128;
		if (erase_end > 127) erase_end -= 128;
	}
    ulog_fmt("hal_erase_by_address: erase bank %lu, from %d to %d page"CRLF, bank, erase_begin, erase_end);
	res = hal_erase_pages_number(bank, erase_begin,  erase_end - erase_begin + 1);
#else

#ifdef	HAL_USE_PAGE_ADDR_ERASE
	ulog_fmt("hal_erase_by_address: erase from %lu to %lu"CRLF, address_begin, address_end);
	int page_size = stm32_get_sector_or_page_size(stm32flashtype, 0);
    res = hal_erase_pages_address(FLASH_BASE + erase_begin * page_size, erase_end - erase_begin + 1);
#else
#error Not supported other type erase
#endif //HAL_USE_PAGE_ADDR_ERASE

#endif //HAL_USE_PAGE_NUMBER_ERASE

#endif  //HAL_USE_SECTOR_NUMBER_ERASE
    return res;
}




#ifdef	HAL_USE_PAGE_NUMBER_ERASE


#define GET_FLASH_PAGE_NUMBER(addr) (uint32_t)((addr - FLASH_BASE) / FLASH_PAGE_SIZE)
#define GET_FLASH_PAGE_ADDRESS(addr) (FLASH_BASE + GET_FLASH_PAGE_NUMBER(addr) * FLASH_PAGE_SIZE)

static char  __flash_buf[FLASH_PAGE_SIZE]; //буфер под страницу


HAL_StatusTypeDef hal_erase_pages(uint32_t PageAddress, uint32_t NbPages)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError = 0;
	HAL_StatusTypeDef res;

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks = FLASH_BANK_1;
	EraseInitStruct.Page = GET_FLASH_PAGE_NUMBER(PageAddress);
	EraseInitStruct.NbPages = NbPages;

	res = HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);
	FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

	return res;

}

//запись во флэш
uint32_t  hal_write_to_flash(uint32_t Address, char * data, uint32_t  data_len)
{
	if (data_len == 0) return 0;

	char * ptr = data;
	uint32_t ptr_Address = Address;
	uint32_t offset;
	uint32_t byte_to_write;
	uint32_t page_address;
	HAL_StatusTypeDef erase_error, prog;
	uint64_t Data;

	hal_flash_unlocked();

	while (data_len > 0) {

		page_address = GET_FLASH_PAGE_ADDRESS(ptr_Address);

		//копируем страницу в буфер
		memcpy((void *)__flash_buf, (void *)page_address, FLASH_PAGE_SIZE);

		//очищаем страницу
		erase_error = hal_erase_pages(page_address, 1);
		if (erase_error == HAL_ERROR) break;

		offset = ptr_Address - page_address; //смещение в странице
		byte_to_write = FLASH_PAGE_SIZE - offset; //кол-во байтов для записи
		if (data_len < byte_to_write) byte_to_write = data_len;

		//копируем данные в буфер
		memcpy((void *)(__flash_buf + offset), (void *)ptr, byte_to_write);

		prog = HAL_OK;
		//записываем страницу во флэш
		for (uint16_t j = 0; j < FLASH_PAGE_SIZE / 8; j++) {
			Data = *(uint64_t *)(__flash_buf + j * 8);
			prog = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, page_address  + j * 8, Data);
			FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);  // Ожидаем окончания операции
			if (prog != HAL_OK) break;
		}
		if (prog != HAL_OK) break;

		data_len -= byte_to_write;
		ptr += byte_to_write;
		ptr_Address += byte_to_write;
	}

	hal_flash_locked();

	return ptr_Address - Address;
}


//запись страницы из буфера __flash_buf
bool hal_write_flash_buf_to_flash_page(uint32_t pageAddress)
{
	uint64_t Data;
	bool res;
	HAL_StatusTypeDef erase_error;


	res = false;

	hal_flash_unlocked();

	erase_error = hal_erase_pages(pageAddress, 1);


	if (erase_error != HAL_ERROR) {

		//записываем страницу во флэш
		for (uint16_t j = 0; j < FLASH_PAGE_SIZE / 8; j++) {
			Data = *(uint64_t *)(__flash_buf + j * 8);
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, pageAddress  + j * 8, Data);
			FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);  // Ожидаем окончания операции
		}
		res = true;
	}

	hal_flash_locked();

	return res;
}

static uint16_t __flash_buf_pos = 0;
static uint32_t __flash_buf_address = 0;

//сделать перед flash_buf_put
void flash_buf_put_begin(uint32_t pageAddress)
{
  __flash_buf_address = pageAddress;
  __flash_buf_pos = 0;
}

//запись в __flash_buf
void flash_buf_put(char * buf, uint16_t len)
{
  uint16_t ost, byte_write;
  char * ptr = buf;

  while (len > 0)
  {
    ost = FLASH_PAGE_SIZE - __flash_buf_pos; //остаток байт в буфере
    byte_write = ( len > ost ? ost : len ); //кол-во байт для записм
    memcpy(__flash_buf + __flash_buf_pos, ptr, byte_write); //копирум данные в буфер
    len -= byte_write;
    __flash_buf_pos += byte_write;
    ptr += byte_write;
    if (__flash_buf_pos >= FLASH_PAGE_SIZE) //вышли за пределы страницы
    {
      //записать во флэш
 	  //ulog_fmt("hal_write_flash_buf_to_flash_page (addr %x, len %d)\r\n", (unsigned int)__flash_buf_address, __flash_buf_pos);
      hal_write_flash_buf_to_flash_page(__flash_buf_address); //запись буфера во флэш
      __flash_buf_address += FLASH_PAGE_SIZE; //к следуюей странице
      __flash_buf_pos -= FLASH_PAGE_SIZE; //позиция для записи
    }
  }
}

//сделать после flash_buf_put
void flash_buf_put_end(void)
{
  if (__flash_buf_pos > 0) {
  	  //ulog_fmt("flash_buf_put_end (addr %x, len %d)\r\n", (unsigned int)__flash_buf_address, __flash_buf_pos);
      hal_write_flash_buf_to_flash_page(__flash_buf_address); //запись буфера во флэш
	  __flash_buf_pos = 0;
  }
}

#endif //HAL_USE_PAGE_NUMBER_ERASE




void HAL_GPIO_ChangeMode(GPIO_TypeDef  * GPIOx, uint16_t GPIO_Pin, uint32_t Mode)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	//HAL_GPIO_DeInit(GPIOx, GPIO_Pin);

	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = Mode;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

	//GPIOx->MODER |= GPIO_MODER
	//GPIOx->MODER &= ~(GPIO_MODER_MODER);
}

/*

//https://community.st.com/t5/stm32-mcus/how-to-jump-to-system-bootloader-from-application-code-on-stm32/ta-p/49424
//STM32CubeProg
 Set the enumeration of the STM32 Microcontrollers Series/
enum{C0, F030x8, F030xC, F03xx, F05, F07, F09, F10xx, F105, F107, F10XL, F2, F3, F4, F7, G0, G4, H503, H563, H573, H7x, H7A, H7B, L0, L1, L4, L5, WBA, WBX, WL, U5};

#define MCU			H7x		//Define here the MCU being used

void JumpToBootloader (void)
{
uint32_t i=0;
void (*SysMemBootJump)(void);


//Set a vector addressed with STM32 Microcontrollers names
//Each vector position contains an address to the boot loader entry point

	volatile uint32_t BootAddr[33];

	BootAddr[C0]     = 0x1FFF0000;
	BootAddr[F030x8] = 0x1FFFEC00;
	BootAddr[F030xC] = 0x1FFFD800;
	BootAddr[F03xx]  = 0x1FFFEC00;
	BootAddr[F05]    = 0x1FFFEC00;
	BootAddr[F07]    = 0x1FFFC800;
	BootAddr[F09]    = 0x1FFFD800;
	BootAddr[F10xx]  = 0x1FFFF000;
	BootAddr[F105]   = 0x1FFFB000;
	BootAddr[F107]   = 0x1FFFB000;
	BootAddr[F10XL]  = 0x1FFFE000;
	BootAddr[F2]     = 0x1FFF0000;
	BootAddr[F3]     = 0x1FFFD800;
	BootAddr[F4]     = 0x1FFF0000;
	BootAddr[F7]     = 0x1FF00000;
	BootAddr[G0]     = 0x1FFF0000;
	BootAddr[G4]     = 0x1FFF0000;
	BootAddr[H503]   = 0x0BF87000;
	BootAddr[H563]   = 0x0BF97000;
	BootAddr[H573]   = 0x0BF97000;
	BootAddr[H7x]    = 0x1FF09800;
	BootAddr[H7A]    = 0x1FF0A800;
	BootAddr[H7B]    = 0x1FF0A000;
	BootAddr[L0]     = 0x1FF00000;
	BootAddr[L1]     = 0x1FF00000;
	BootAddr[L4]     = 0x1FFF0000;
	BootAddr[L5]     = 0x0BF90000;
	BootAddr[WBA]    = 0x0BF88000;
	BootAddr[WBX]    = 0x1FFF0000;
	BootAddr[WL]     = 0x1FFF0000;
	BootAddr[U5]     = 0x0BF90000;

	// Disable all interrupts
	__disable_irq();

	// Disable Systick timer
	SysTick->CTRL = 0;

	//Set the clock to the default state
	HAL_RCC_DeInit();

	//Clear Interrupt Enable Register & Interrupt Pending Register
	for (i=0;i<5;i++)
	{
		NVIC->ICER[i]=0xFFFFFFFF;
		NVIC->ICPR[i]=0xFFFFFFFF;
	}

	//Re-enable all interrupts
	__enable_irq();

	//Set up the jump to boot loader address + 4
	SysMemBootJump = (void (*)(void)) (*((uint32_t *) ((BootAddr[MCU] + 4))));

	//Set the main stack pointer to the boot loader stack
	__set_MSP(*(uint32_t *)BootAddr[MCU]);

	//Call the function to jump to boot loader location/
	SysMemBootJump();

	//Jump is done successfully/
	while (1)
	{
		// Code should never reach this loop
	}
}
*/

#endif //HAL_USE_FLASH

/*

#define SYSTEM_MEMORY_ADDR                                      0x1FFFC400

void JumpToBootloader(void)
{
  HAL_DeInit();
  HAL_RCC_DeInit();
  __disable_irq();

  hal_flash_unlocked();
  hal_erase_pages(FLASH_BASE, 1);
  hal_flash_locked();


  const uint32_t ptr = (*((uint32_t *) SYSTEM_MEMORY_ADDR));
  void (*JumpToSystemMemory)(void);

  JumpToSystemMemory = (void (*)(void)) (*((uint32_t *) SYSTEM_MEMORY_ADDR + 4));
  __set_MSP(ptr);
  JumpToSystemMemory();

  while(1);
}
/

void UART_flush(UART_HandleTypeDef *huart) {
  // Flush UART RX buffer if RXNE is set
  if READ_BIT(huart->Instance->ISR, UART_FLAG_RXNE) {
    SET_BIT(huart->Instance->RQR, UART_RXDATA_FLUSH_REQUEST);
  }

  // Not available on F030xx devices!
  // SET_BIT(huart1.Instance->RQR, UART_TXDATA_FLUSH_REQUEST);

  // Clear All Errors (if needed)
  if (READ_BIT(huart->Instance->ISR, UART_FLAG_ORE | UART_FLAG_FE | UART_FLAG_NE)) {

	__HAL_UART_CLEAR_PEFLAG(huart);
	__HAL_UART_CLEAR_OREFLAG(huart);
	__HAL_UART_CLEAR_FEFLAG(huart);
	__HAL_UART_CLEAR_NEFLAG(huart);
	__HAL_UART_CLEAR_IDLEFLAG(huart);
	__HAL_UART_CLEAR_TXFECF(huart);

  }
}
*/
