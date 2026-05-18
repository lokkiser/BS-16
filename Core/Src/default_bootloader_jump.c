#include "default_bootloader_jump.h"

extern uint32_t magic_boot;

void JumpToBootloader(void)
{
    // Clear the token so the device doesn't loop into the bootloader forever
    // TAMP->BKP0R = 0x00000000; 

    magic_boot = 0;

    // Lock backup access again
    PWR->CR1 &= ~PWR_CR1_DBP;

    // Turn on SYSCFG to allow memory mapping
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    
    // Remap System Memory to 0x00000000
    SYSCFG->MEMRMP = SYSCFG_MEMRMP_MEM_MODE_0; // Maps System Flash to 0x0

    __DSB();
    __ISB();

    // Reset Vector Table Offset Register
    SCB->VTOR = 0x00000000;

    uint32_t p_stack_pointer = *(volatile uint32_t *)0x1FFF0000;
    void (*p_reset_handler)(void) = (void (*)(void))(*(volatile uint32_t *)(0x1FFF0000 + 4));

    // Set stack pointer and jump
    __set_MSP(p_stack_pointer);
    p_reset_handler();

    while (1); // Code never returns here
}
