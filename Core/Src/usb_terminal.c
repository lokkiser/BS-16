#include "usb_terminal.h"
#include "default_bootloader_jump.h"

static terminal_state_e state;
static char terminal_tx_buf[256] = "";

static timer_t connection_timer;


void Terminal_Process(void)
{
    if(state == TERMINAL_INFO && isTimerExpired(&connection_timer, 2000))
        state = TERMINAL_IDLE;
}

static void Terminal_MsgInvalid(void)
{
    sprintf(terminal_tx_buf, "BS-16: Invalid command.\r\n");
    CDC_Transmit(0, (uint8_t*)terminal_tx_buf, strlen(terminal_tx_buf));  // hardcoded CDC 0
}

static void Terminal_MsgStart(void)
{
    state = TERMINAL_INFO;
    startSingleShotTimer(&connection_timer);
}

static void Terminal_MsgUpdate(void)
{
    state = TERMINAL_UPDATE;

    sprintf(terminal_tx_buf, "BS-16: Starting DFU Bootloader...\r\n");
    CDC_Transmit(0, (uint8_t*)terminal_tx_buf, strlen(terminal_tx_buf));  // hardcoded CDC 0

    // Device_ReqBootloaderJump();   
    HAL_Delay(5);
    JumpToBootloader();  
    //дальше программа не исполняется
}

static void Terminal_InfoReq(void)
{
    if(state != TERMINAL_INFO) return;

     uint8_t info[4] = {0};

    // // RS485 Addr
    // info[0]  = Device_GetAddr();

    // //Device info
    // info[1] |= Device_RS485_GetConnection() << 4;

    // info[1] |= Device_wasSound(CH_1) << 3;
    // info[1] |= Device_wasSound(CH_2) << 2;

    // info[1] |= Device_GetMode(CH_1)  << 1;   
    // info[1] |= Device_GetMode(CH_2)  << 0;

    // //Errors
    // info[2]  = Device_GetErrors();

    // // LEDs
    // info[3] |= IS_LED_READY()        << 4;
    // info[3] |= IS_LED_START()        << 3;
    // info[3] |= IS_LED_MALFUNCTION()  << 2; 
    // info[3] |= IS_LED_PROTECTION_1() << 1;
    // info[3] |= IS_LED_PROTECTION_2() << 0;

    Terminal_Send(info, sizeof(info), sizeof(uint8_t), "INFO:");
}

void Terminal_Send(const void* buf, uint16_t len, size_t size, char* tag)
{
    if(state != TERMINAL_INFO) return;

    char locStr[192] = "";
    char locChr[16]  = "";

    if(size == sizeof(uint8_t))
    {
        for(uint16_t i=0; i<len; i++)
        {
            snprintf(locChr, sizeof(locChr), "0x%02X ", *(uint8_t*)&buf[i]);
            strncat(locStr, locChr, strlen(locChr));
        }
    }
    else if(size == sizeof(uint16_t))
    {
        for(uint16_t i=0; i<len; i++)
        {
            snprintf(locChr, sizeof(locChr), "0x%02X ", *(uint16_t*)&buf[i*size]);
            strncat(locStr, locChr, strlen(locChr));
        }
    }
    else if(size == sizeof(uint32_t))
    {
        for(uint16_t i=0; i<len; i++)
        {
            snprintf(locChr, sizeof(locChr), "0x%02X ", *(uint32_t*)&buf[i*size]);
            strncat(locStr, locChr, strlen(locChr));
        }
    }
            
    snprintf(terminal_tx_buf, sizeof(terminal_tx_buf), "%s %s\r\n", tag, locStr);
    CDC_Transmit(0, (uint8_t*)terminal_tx_buf, strlen(terminal_tx_buf));          // hardcoded CDC 0
}

void Terminal_USB_CDC_Callback(uint8_t* buf, uint8_t len)
{
    startSingleShotTimer(&connection_timer);

    if(len>=2 && strstr((char*)buf, "BS-16:"))
    {
        if(strstr((char*)buf, "Info Req") != 0) /*"BS-16: Info Req"*/
        {
            Terminal_InfoReq();
        }
        else if(strstr((char*)buf, "Update") != 0)  /*"BS-16: Update"*/ 
        {
            Terminal_MsgUpdate();   
        }
        else if(strstr((char*)buf, "Start GUI Terminal") != 0) /*"BS-16: Start GUI Terminal"*/
        {
            Terminal_MsgStart();
        }
        else
        {
            Terminal_MsgInvalid();
        }
    }
    else
    {
        Terminal_MsgInvalid();
    }   
}
