#ifndef __USB_TERMINAL_H
#define __USB_TERMINAL_H

#include "main.h"
#include "usbd_cdc_acm_if.h"

typedef enum
{
    TERMINAL_IDLE,
    TERMINAL_INFO,
    TERMINAL_UPDATE,
} terminal_state_e;


// void Terminal_Init(void);
void Terminal_Process(void);

void Terminal_Send(const void* buf, uint16_t len, size_t size, char* tag);
void Terminal_USB_CDC_Callback(uint8_t* buf, uint8_t len);

#endif