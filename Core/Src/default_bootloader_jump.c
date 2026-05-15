#include "default_bootloader_jump.h"


void JumpToBootloader(void)
{
  // Reset USB
  MX_USB_DEVICE_DeInit();

  HAL_DeInit();

  HAL_Delay(5);

  /* Disable all interrupts */
  __disable_irq();

  /* Disable Systick timer */
  HAL_RCC_DeInit();

  for (IRQn_Type i = 0; i < FMAC_IRQn; i++) 
  { 
    NVIC_DisableIRQ(i);
    NVIC_ClearPendingIRQ(i);
  }

  // IMPORTANT! Disable Systick AFTER HAL_RCC_Deinit
  // Some devices were locking up inside of HAL_RCC_DeInit

  // Disable Systick
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL  = 0;
  SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

  __enable_irq();

  /* ARM Cortex-M Programming Guide to Memory Barrier Instructions.*/
  __DSB();

  __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();

  typedef struct 
  {
    void  *SP;
    void (*PC)(void);
  } SP_PC_t;
  
  #define SYSBOOT ((SP_PC_t*)0x1FFF0000)
  
  SCB->VTOR = (uint32_t)SYSBOOT;

  /* Remap is bot visible at once. Execute some unrelated command! */
  __DSB();
  __ISB();

  __set_MSP((uint32_t)SYSBOOT->SP);
  SYSBOOT->PC();
  
  while (1)
  {}
}