/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "sai.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pca9535.h"
#include "stm_util.h"
#include "mki.h"
#include "mux.h"
#include "console.h"
#include "rtc_periph.h"
#include "local_mic.h"
#include "local_spk.h"
#include "ctrl.h"
#include "akb.h"
#include "fault.h"
#include "datetime_util.h"
#include "fatfs_util.h"
#include "flash_io.h"
#include "setting.h"
#include "logfile_util.h"
#include "str_util.h"
#include "mp3play_util.h"
//#include "memplay_util.h"
#include "dacplay_util.h"
#include "mp3files.h"
#include "password.h"
#include "buzzer.h"
#include "usb_device.h"
#include "TAS5720.h"

//#include "usb_terminal.h"
#include "default_bootloader_jump.h"
#include "mcp.h"
#include "remote_mic.h"
#include "local_mic.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//#define TOTAL_SAMPLES 16384

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint8_t testreg[2] = {255, 255};
uint8_t testreg2[2] = {0, 0};
uint8_t configreg[2] = {0, 0};

uint32_t testsound[256] = {};
uint16_t testval = 1;
uint16_t _err_sai = 0;
uint8_t await_debug = 1;
// Итоговый размер буфера: 2048 фреймов * 8 каналов = 16384 элементов

uint32_t test_adc_buffer[TOTAL_SAMPLES];





uint8_t extenders_debug_reg = 0;
uint8_t extenders_debug_reg_read[8] = {};
uint16_t debug_adc[8] = {};
int8_t extenders_error = 0;
uint16_t test_output = 255;//255;
uint8_t reset_timer = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
bool fatfs_mount = false;
bool debug_int_spk = false;

/*typedef struct pca9535_s
{
	I2C_HandleTypeDef* idx;
    unsigned short addr;
    bool busy;
    bool err;
} pca9535_t;*/
//pca9535_t_0, pca9535_t_1, pca9535_t_2, pca9535_t_3;
//DD4 IO0_0

//pca9535_set_reg(pca9535_t * pca9535, pca9535_reg_type_t reg_type, pca9535_port_t port, unsigned char *value, int len)
void test_pin(uint8_t _state)
{
	extenders_debug_reg = _state;
	pca9535_set_reg(&pca9535_t_DD4_MB, PCA9535_REG_TYPE_OUTPUT, PCA9535_PORT_0, &extenders_debug_reg, 1);
	//pca9535_set_reg(&pca9535_t_DD4_MB, PCA9535_REG_OUTPUT_0, PCA9535_PORT_0, &extenders_debug_reg, PCA9535_PORT_0);
	//pca9535_i2c_write(&pca9535_t_DD4_MB, pca9535_t_DD4_MB.addr, PCA9535_REG_OUTPUT_0, extenders_debug_reg);
}


extern uint32_t dd5_change_tick, dd6_change_tick;

uint32_t magic_boot __attribute__ ((section (".noinit")));

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    //fulog_fmt("i%d"CRLF, GPIO_Pin);
    if (GPIO_Pin == INT1_Pin)
    {
        dd5_change_tick = HAL_GetTickNoZero();
    }
    else if (GPIO_Pin == INT2_Pin)
    {
        dd6_change_tick = HAL_GetTickNoZero();
    }
    else if(GPIO_Pin == CLK_IRQ_Pin)
    {
        
    }
}

void UART_flush(UART_HandleTypeDef *huart)
{
  // Flush UART RX buffer if RXNE is set
  if READ_BIT(huart->Instance->ISR, UART_FLAG_RXNE)
  {
    SET_BIT(huart->Instance->RQR, UART_RXDATA_FLUSH_REQUEST);
  }

  // Not available on F030xx devices!
  SET_BIT(huart->Instance->RQR, UART_TXDATA_FLUSH_REQUEST);

  // Clear All Errors (if needed)
  if (READ_BIT(huart->Instance->ISR, UART_FLAG_ORE | UART_FLAG_FE | UART_FLAG_NE))
  {
    __HAL_UART_CLEAR_PEFLAG(huart);
    __HAL_UART_CLEAR_OREFLAG(huart);
    __HAL_UART_CLEAR_FEFLAG(huart);
    __HAL_UART_CLEAR_NEFLAG(huart);
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_UART_CLEAR_TXFECF(huart);
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    UART_flush(huart);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart == CONSOLE_UART)
    {
        console_RxEventCallback(Size);
    }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    /*if (hadc == REMOTE_MIC_ADC_HANDLE)
    {
        local_mic_adc_HalfCpltCallback();
    }
    else */if(hadc == LOCAL_SPK_ADC_HANDLE)
    {
        local_spk_adc_HalfCpltCallback();
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(hadc == LOCAL_MIC_ADC_HANDLE)  //тангента
    {
    	uint32_t adc_val;
    	adc_val = HAL_ADC_GetValue(LOCAL_MIC_ADC_HANDLE);
    	sai_buffer[mic_adc.index] = adc_val << 8;//12bit ->24bit
    	if(mic_adc.index < TOTAL_SAMPLES * TDM_CHANNELS)
    	{
    		mic_adc.index = mic_adc.index + tdm_active_ch + TDM_CHANNELS; //кладём звук в канал выбранного ЦАПА
    	}
    	else
    	{
    		mic_adc.index = 0;
    	}
    }
    else if (hadc == REMOTE_MIC_ADC_HANDLE)  //внешний микрофон, панель
        {
            remote_mic_adc_FullCpltCallback();
        }
    else
    {
        ctrl_FullCpltCallback(hadc);
    }
}

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
    if (hdac == LOCAL_MIC_DAC_HANDLE)
    {
        local_mic_dac_HalfCpltCallback();
    }
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
    if (hdac == LOCAL_MIC_DAC_HANDLE)
    {
        remote_mic_dac_FullCpltCallback();
    }
}

void HAL_DACEx_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef *hdac)
{
    if (hdac == LOCAL_SPK_DAC_HANDLE)
    {
        local_spk_dac_HalfCpltCallback();
    }
}

void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef *hdac)
{
    if (hdac == LOCAL_SPK_DAC_HANDLE)
    {
        local_spk_dac_FullCpltCallback();
    }
}

void HAL_DAC_ErrorCallbackCh1(DAC_HandleTypeDef *hdac)
{
}

void HAL_DACEx_ErrorCallbackCh2(DAC_HandleTypeDef *hdac)
{
}

// Half-transfer callback (First half is ready to read)
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai) {
    //process_audio_data(&sai_buffer[0], BUFFER_SIZE / 2);
}

// Transfer-complete callback (Second half is ready to read)
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai) {
    //process_audio_data(&sai_buffer[BUFFER_SIZE / 2], BUFFER_SIZE / 2);
}

/*void USB_CDC_RxCallback(uint8_t* buf, uint32_t len)
{
    Terminal_USB_CDC_Callback(buf, len);
}*/

void on_timer_25_msec(void)
{
    mki_task();
    ctrl_task();
    password_task();

    /*if(mux_is_active_line())
    {
        if(HAL_GPIO_ReadPin(VIZ_GPIO_Port, VIZ_Pin))
            HAL_GPIO_WritePin(mute_OP_GPIO_Port, mute_OP_Pin, GPIO_PIN_RESET);
        else
            HAL_GPIO_WritePin(mute_OP_GPIO_Port, mute_OP_Pin, GPIO_PIN_SET);
    }
    else
        HAL_GPIO_WritePin(mute_OP_GPIO_Port, mute_OP_Pin, GPIO_PIN_RESET);*/
}

void on_timer_50_msec(void)
{
    buzzer_task();
    //Terminal_Process();
    // memplay_task();
}

void on_timer_250_msec(void)
{
    console_task();
}

void on_timer_1_sec(void)
{
	//TODO
    //HAL_IWDG_Refresh(&hiwdg);

    if (fulog_buf[0] != '\0')
    {
        ulog(fulog_buf);
        fulog_buf[0] = '\0';
    }

    mki_status_LEDs_task();
}

void on_timer_1_min(void)
{
    akb_task();
}

//file log
void filelog(char * str)
{
    char buf[MAX_STR_SIZE];

    strcpy_s(buf, sizeof(buf), rtc_read_datetime_str(true));
    strcat_s(buf, sizeof(buf), " ");
    strcat_s(buf, sizeof(buf), str);
    //strcat_s(buf, sizeof(buf), CRLF);

    logfile_add(buf);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  //if(magic_boot == 0xB00D0473)
  //  JumpToBootloader();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_USB_Device_Init();
  MX_ADC1_Init();
  MX_ADC3_Init();
  MX_ADC2_Init();
  MX_ADC5_Init();
  MX_DAC1_Init();
  MX_I2C3_Init();
  MX_I2C4_Init();
  MX_DAC2_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();
  MX_ADC4_Init();
  MX_SPI1_Init();
  MX_TIM15_Init();
  MX_SAI1_Init();
  MX_TIM4_Init();
  MX_TIM20_Init();
  /* USER CODE BEGIN 2 */

  //MX_USB_DEVICE_Init();

  HAL_DAC_MspInit(&hdac1); //!!!!! ВАЖНО НЕ БУДЕТ РАБОТАТЬ DAC DMA БЕЗ ЭТОЙ СТРОЧК�?
  HAL_DAC_MspInit(&hdac2); //!!!!! ВАЖНО НЕ БУДЕТ РАБОТАТЬ DAC DMA БЕЗ ЭТОЙ СТРОЧК�?

  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc4, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc5, ADC_SINGLE_ENDED);


  HAL_Delay(100);

  ulog(CRLF"Start Trombon BS-16"CRLF);
  //ulog_fmt("Version: %s"CRLF, VERSION);
  ulog_fmt("Version: %s"CRLF, "1.0"); // TODO

  mki_init();
  mux_init();
  //console_init();

  fatfs_mount = false;

  if (flash_init())
  {
      ulog("main: W25Q detected"CRLF);

      fatfs_mount = f_mount_cnt(&f_global_fatfs, 3);
      if (!fatfs_mount)  fatfs_mount = f_format_and_mount(&f_global_fatfs, F_MKFS_FMT_DEFAULT);
      ulog_fmt("main: FatFs f_mount() %s"CRLF, fatfs_mount ? "ok" : "error");
  } 
  else ulog("main: error W25Q not detected"CRLF);

  if (fatfs_mount)
  {
      if (f_exists(BC16_LOADER_FILE_NAME))
      {
          ulog("Copy "BC16_LOADER_FILE_NAME" to stm32 flash base"CRLF);
          f_erase_and_copy_to_flash(BC16_LOADER_FILE_NAME, FLASH_BASE, BC16_FLASH_TYPE);
          ulog("Unlink "BC16_LOADER_FILE_NAME""CRLF);
          f_unlink(BC16_LOADER_FILE_NAME);
      }
  }
  else
  {
      ulog("main: error mount FatFs"CRLF);
      set_fault(FAULT_HARDWARE_ERROR);
  }

  load_setting();
  ulog_fmt("main: cfg "SETTING_KEY_MIC_VOLUME"=%d"CRLF, setting.mic_volume);
  ulog_fmt("main: cfg "SETTING_KEY_SPK_VOLUME"=%d"CRLF, setting.mic_volume);
  ulog_fmt("main: cfg "SETTING_KEY_DATE_TIME" =%s"CRLF, setting.date_time );

  if (HAL_I2C_IsDeviceReady(RTC_I2C_HANDLE, (RTC_I2C_ADDRESS << 1), 3, 1000) != HAL_OK)
  {
      ulog_fmt("main: error bq32000 not ready"CRLF);
      set_fault(FAULT_HARDWARE_ERROR);
  }
  rtc_init();
  //RTC_Time rtcTime;
  //rtcTime.year = 24;
  //rtcTime.hours  = 1;
  //rtcTime.seconds = 23;
  //rtcTime.month = 4;
  //rtcTime.minutes =5;
  //rtcTime.dayOfWeek =2;
  //rtcTime.day =6;
  //RTC_BQ32000_SetTime(&hi2c3, &rtcTime);
  ulog_fmt("BQ32000: datetime now %s"CRLF, rtc_read_datetime_str(true));

  if (strlen(setting.date_time))
  {
      rtc_set_datetime_str(setting.date_time);
      memset(setting.date_time, 0, sizeof(setting.date_time));
      save_setting();
  }
  else if ( ((globalRTC_Time.year < 26) || (globalRTC_Time.year > 40)) ||
          !MONTH_GOD(globalRTC_Time.month)    ||
          !DATE_GOD(globalRTC_Time.day)       ||
          !HOUR_GOD(globalRTC_Time.hours)     ||
          !MINUTE_GOD(globalRTC_Time.minutes) ||
          !SECOND_GOD(globalRTC_Time.seconds) )
  {
      rtc_set_datetime_str("2026-01-19T10:16:01");
  }

  password_init(0x0123);
  #warning hardcoded pass
  akb_init();
  //ctrl_init();

  static timer_t timer_25_msec;
  startTimer(&timer_25_msec);

  static timer_t timer_50_msec;
  startTimer(&timer_50_msec);

  static timer_t timer_250_msec;
  startTimer(&timer_250_msec);

  static timer_t timer_1_sec;
  startTimer(&timer_1_sec);

  static timer_t timer_1_min;
  startTimer(&timer_1_min);

  filelog("Перезагрузка"CRLF);

  // memplay_init(sound_buf, sizeof(sound_buf));
  // memplay(start_sound, sizeof(start_sound), &spk_dacplay_data, setting.spk_volume, &htim15);

  //

  /*buzzer_init(&hdac1, DAC_CHANNEL_2, &htim15);
  buzzer_make_sound(250, 440, 0x10);
  buzzer_add_queue (250, 494, 0x10);
  buzzer_add_queue (250, 554, 0x10);*/
  ulog("BS-16 goto loop"CRLF);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  akb_task();
  //i2c_scan();
  HAL_Delay(100);
  sin_gen();
  if (disable_sound) {
	  HAL_SAI_DMAStop(&hsai_BlockA1);
	  TAS5720_WriteReg(TAS5720_REG_POWER_CTRL, 0xFC);
  }

  //HAL_ADC_Start(LOCAL_MIC_ADC_HANDLE);

  HAL_TIM_Base_Start(&htim4);
  //HAL_TIM_Base_Start(LOCAL_MIC_ADC_TIM_HANDLE);
  //HAL_TIM_Base_Start(REMOTE_MIC_ADC_TIM_HANDLE);

  mcp_init();
  local_mic_start();
  remote_mic_start();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	//HAL_I2C_Init(&hi2c4);
	//HAL_I2C_Mem_Read(&hi2c4, 0x6c, )
	//HAL_I2C_(&hi2c4);
	  //int pca9535_set_reg(pca9535_t * pca9535, pca9535_reg_type_t reg_type, pca9535_port_t port, unsigned char *value, int len) {
	  /*pca9535_set_reg(&pca9535_t_0, PCA9535_REG_TYPE_OUTPUT , PCA9535_PORT_BOTH , testreg, 2);
	  HAL_Delay(500);
	  pca9535_set_reg(&pca9535_t_0, PCA9535_REG_TYPE_OUTPUT , PCA9535_PORT_BOTH , testreg2, 2);*/
	  //HAL_Delay(100);
	  /*_err_sai = HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t *)testsound, 32);
	  if(_err_sai != HAL_OK )
	  {
	    Error_Handler();
	  }*/
	  //test_i2c_tas5720();
    /*if(isTimerExpired(&timer_1_min,  60000))
      on_timer_1_min();

    if(isTimerExpired(&timer_1_sec,   1000))
      on_timer_1_sec();

    if(isTimerExpired(&timer_250_msec, 250))
      on_timer_250_msec();

    if(isTimerExpired(&timer_50_msec,   50))
      on_timer_50_msec();

    if(isTimerExpired(&timer_25_msec,   25))
      on_timer_25_msec();*/
	  //HAL_DAC_Start();

	if (tas_i2c_debug_status != 0x00 && !disable_sound)
	{

		TAS5720_WriteReg(TAS5720_REG_POWER_CTRL, 0xFC);
		if (tas_i2c_debug_status == 0x02){
		}
		HAL_Delay(100); // Даем время усилителю синхронизировать свой внутренний ШИМ-генератор
		HAL_SAI_DMAStop(&hsai_BlockA1);
		HAL_Delay(100);
		HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t*)sai_buffer, TOTAL_SAMPLES * TDM_CHANNELS);//AUDIO_BUF_SIZE);
		//HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t*)silence_buffer, 64);
		HAL_Delay(100);
		TAS5720_WriteReg(TAS5720_REG_POWER_CTRL, 0xFD);
	}
	if (refill_arr)
	{
		sin_gen();
		//refill_arr = 0;
	}
	if (reset_timer)
	{
		reset_timer = 0;
		HAL_TIM_Base_Stop(REMOTE_MIC_ADC_TIM_HANDLE);
		//set_tim_frequency2(&htim15, d_BASE_AUDIO_FREQUENCY, d_BASE_FREQUENCY);
		HAL_TIM_Base_Start(REMOTE_MIC_ADC_TIM_HANDLE);
	}
	set_dd4(test_output);
	set_mux_line(8);
	HAL_Delay(100);
	bat_con();
	akb_task();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 27;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
