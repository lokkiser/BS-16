
#ifndef STM_UTIL_H_
#define STM_UTIL_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "def_util.h"
#include "main.h"

typedef struct _portpin 
{
	GPIO_TypeDef* port;
	uint16_t pin;
} portpin_t, * pportpin_t;

extern const portpin_t portpin_NULL;


#define PWRITE(portpin, v)		HAL_GPIO_WritePin(portpin.port, portpin.pin, (v == 0 ?  GPIO_PIN_RESET : GPIO_PIN_SET))
#define PSET(portpin)			HAL_GPIO_WritePin(portpin.port, portpin.pin, GPIO_PIN_SET)
#define PRESET(portpin)			HAL_GPIO_WritePin(portpin.port, portpin.pin, GPIO_PIN_RESET)
#define PGET(portpin)			(HAL_GPIO_ReadPin(portpin.port, portpin.pin) == GPIO_PIN_SET)
#define PCOMPARE(portpin1, portpin2) ((portpin1.port == portpin2.port) && (portpin1.pin == portpin2.pin))

//установлен ли выход
GPIO_PinState OutPinIsSetted(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#define PISSET(portpin)			(OutPinIsSetted(portpin.port, portpin.pin) == GPIO_PIN_SET)
#define PTOGLE(portpin)			HAL_GPIO_TogglePin(portpin.port, portpin.pin)

#define PIN_WRITE(pin, val)		HAL_GPIO_WritePin(pin##_GPIO_Port, pin##_Pin, val)
#define PIN_SET(pin)			HAL_GPIO_WritePin(pin##_GPIO_Port, pin##_Pin, GPIO_PIN_SET)
#define PIN_CLR(pin)			HAL_GPIO_WritePin(pin##_GPIO_Port, pin##_Pin, GPIO_PIN_RESET)
#define PIN_GET(pin)  			HAL_GPIO_ReadPin(pin##_GPIO_Port, pin##_Pin)
#define PIN_TOGGLE(pin)			HAL_GPIO_TogglePin(pin##_GPIO_Port, pin##_Pin)
#define PIN_ISSET(pin)			OutPinIsSetted(pin##_GPIO_Port, pin##_Pin)

#ifdef USE_RCC
uint32_t PCLK1TIM(void);
#endif

#ifdef HAL_TIM_MODULE_ENABLED
void _delay_us_tim(TIM_HandleTypeDef * htim, uint32_t us);
#endif

//микросекунды https://www.carminenoviello.com/2015/09/04/precisely-measure-microseconds-stm32/
//для 86 МгЦ = 7
#ifndef DELAY_US_CYCLE_TIME
#define DELAY_US_CYCLE_TIME		7
#endif
void delay_us_cycle(uint32_t us);

//для 86 МгЦ = 16
#ifndef DELAY_US_ASM_CYCLE_TIME
#define DELAY_US_ASM_CYCLE_TIME		16
#endif
#define DELAY_US_ASM(us) do {\
	asm volatile (	"MOV R0,%[loops]\n\t"\
			"1: \n\t"\
			"SUB R0, #1\n\t"\
			"CMP R0, #0\n\t"\
			"BNE 1b \n\t" : : [loops] "r" (DELAY_US_ASM_CYCLE_TIME * us) : "memory"\
		      );\
} while(0)


//возвращает prescaler и counter_period для таймеров STM, base_freq - частота на входе таймера, required_freq - требуемая частота
void get_stm32_tim_freq_param(uint32_t base_freq, uint32_t required_freq, uint16_t * prescaler, uint16_t * counter_period);

#define DIFF_TICK(told, tnew, tmax)		(tnew < told ? tmax - told + tnew : tnew - told)
#define DIFF_TICKU32(told, tnew)		DIFF_TICK(told, tnew, 0xFFFFFFFF)
#define DIFF_TICKU64(told, tnew)		DIFF_TICK(told, tnew, 0xFFFFFFFFFFFFFFFF)


#ifdef HAL_TIM_MODULE_ENABLED
//установка таймера на частоту (base_frequency - частота процессора)
void set_tim_frequency(TIM_TypeDef * ptim, uint32_t frequency, uint32_t base_frequency);
void set_tim_frequency2(TIM_HandleTypeDef * htim, uint32_t frequency, uint32_t base_frequency);
void set_tim_frequency2_and_start(TIM_HandleTypeDef * htim, uint32_t frequency, uint32_t base_frequency);
//возвращает чвстоту таймера (base_frequency - частота процессора)
uint32_t get_tim_frequency(TIM_TypeDef * ptim, uint32_t base_frequency);
uint32_t get_tim_frequency2(TIM_HandleTypeDef * htim,  uint32_t base_frequency);
//возращает время в мкс. Где htim - таймер на 1 млс (наиболее часто TIM1)
uint64_t HAL_GetTickUs(TIM_HandleTypeDef * htim);
//разница между текущим временем и старым временем в мкс
uint64_t HAL_DiffTickUs(uint64_t told, TIM_HandleTypeDef * htim);
//uint64_t DiffTickUs(uint64_t to, uint64_t tn);
void HAL_DelayUs(uint64_t Delay, TIM_HandleTypeDef * htim);
#endif

//разница между текущим временем и старым временем в мс
uint32_t HAL_DiffTick(uint32_t told);

//возращает время в млс не нулевое
uint32_t HAL_GetTickNoZero(void);

//альтернативная HAL_Delay ф-ция
void HAL_DelayTick(uint32_t Delay);


void tick_to_time(uint32_t tick, uint8_t* hour, uint8_t* minute, uint8_t* sec, uint16_t* milisec);

//установка таймера
//void HAL_SetTick(uint32_t * tick);
//uint32_t DiffTick(uint32_t to, uint32_t tn);
//время прошло ?
//bool HAL_PassTick(uint32_t tick, uint32_t timeout_tick);

#ifdef HAL_ADC_MODULE_ENABLED
//чтение напряжения с ADC
uint32_t get_adc_value(ADC_HandleTypeDef* hadc, uint32_t Timeout);
#ifdef HAL_ADC_INJECTED_ENABLED
//чтение напряжения с ADC с инжектированного канала
uint32_t get_injected_adc_value(ADC_HandleTypeDef* hadc, uint32_t InjectedRank, uint32_t Timeout);
#endif
#endif

//посылка строки через UART Должна быть определена
void ulog(char *str);
//посылка строки с форматом через UART
void ulog_fmt(const char * fmt, ... );

extern char fulog_buf[MAX_STR_SIZE];
void fulog(char * str);
void fulog_fmt(const char * fmt, ... );


#ifndef UID_BASE
#define UID_BASE 0x1FFFF7E8
#endif
char* get_stm_uid(void);
uint8_t* stm_uid(void);




#ifdef HAL_USE_FLASH

#define APP_JUMP_ADDR(app_addr) 	(*( uint32_t *) (app_addr + 4))

//переход к Application (файл bin)
void go_to_application(uint32_t ApplicationAddr);
void go_to_application2(uint32_t ApplicationAddr);
//загружена ли программа по адресу
bool is_programm_download(uint32_t addr, uint32_t size);



HAL_StatusTypeDef hal_write_data_to_flash(uint32_t address, uint8_t* data, uint32_t data_len);


//Low-density devices are STM32F101xx, STM32F102xx and STM32F103xx microcontrollers where the Flash memory density ranges between 16 and 32 Kbytes.
#define STM32F1XX_LOW_FLASH_TYPE					1
//Medium-density devices are STM32F101xx, STM32F102xx and STM32F103xx microcontrollers where the Flash memory density ranges between 64 and 128 Kbytes.
#define STM32F1XX_MEDIUM_FLASH_TYPE					2
//High-density devices are STM32F101xx and STM32F103xx microcontrollers where the Flash memory density ranges between 256 and 512 Kbytes.
#define STM32F1XX_HIGH_FLASH_TYPE					3
//Connectivity line devices are STM32F105xx and STM32F107xx microcontrollers
#define STM32F1XX_CONNECT_FLASH_TYPE				4
//STM32F205xx, STM32F207xx, STM32F215xx and STM32F217xx
#define STM32F2XX_FLASH_TYPE						5
//STM32G473xB STM32G473xC STM32G473xE (dual 2KB single  4 KB )
#define STM32G4XX_SINGLE_BANK_FLASH_TYPE			6
#define STM32G4XX_DUAL_BANK_FLASH_TYPE				7

#define STM32H7XX_FLASH_TYPE						8

int stm32_get_sector_or_page_number(uint32_t address, uint8_t stm32flashtype);



#ifdef	HAL_USE_SECTOR_NUMBER_ERASE
HAL_StatusTypeDef hal_erase_sectors(uint32_t Banks, uint32_t Sector, uint32_t NbSectors);
#endif

#ifdef	HAL_USE_PAGE_NUMBER_ERASE
HAL_StatusTypeDef hal_erase_pages_number(uint32_t Banks, uint32_t PageNumber, uint32_t NbPages);
#endif

#ifdef	HAL_USE_PAGE_ADDR_ERASE
HAL_StatusTypeDef hal_erase_pages_address(uint32_t PageAddress, uint32_t NbPages);
#endif


HAL_StatusTypeDef hal_erase_by_address(uint32_t address_begin, uint32_t address_end, uint8_t stm32flashtype);




#ifdef	HAL_USE_PAGE_NUMBER_ERASE

//запись во флэш
uint32_t  hal_write_to_flash(uint32_t Address, char* data, uint32_t  data_len);
//запись страницы из буфера __flash_buf
bool hal_write_flash_buf_to_flash_page(uint32_t pageAddress);
//сделать перед flash_buf_put
void flash_buf_put_begin(uint32_t pageAddress);
//запись в __flash_buf
void flash_buf_put(char* buf, uint16_t len);
//сделать после flash_buf_put
void flash_buf_put_end(void);

#endif //HAL_USE_PAGE_NUMBER_ERASE


#endif //HAL_USE_FLASH

/*
//запись во флэш
uint32_t  hal_write_to_flash(uint32_t Address, char * data, uint32_t  data_len);
//сделать перед flash_buf_put
void flash_buf_put_begin(uint32_t pageAddress);
//запись в __flash_buf
void flash_buf_put(char * buf, uint16_t len);
//сделать после flash_buf_put
void flash_buf_put_end(void);


void JumpToBootloader(void);
*/

//void HAL_GPIO_ChangeMode(GPIO_TypeDef  *GPIOx, uint16_t GPIO_Pin, uint32_t Mode);


#ifdef __cplusplus
}
#endif

//void UART_flush(UART_HandleTypeDef *huart);

#endif /* STM_UTIL_H_ */
