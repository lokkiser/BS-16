/*
 * cosole.c
 *
 *  Created on: 17 дек. 2025 г.
 *      Author: Rafael Boltachev
 */

#include "console.h"
#include <string.h>
#include "str_util.h"
#include "stm_util.h"
#include "dynbuf_util.h"
#include "dynbuf_util.h"
#include "dictionary_util.h"
#include "ctrl.h"
#include "fault.h"
#include "local_mic.h"
#include "local_spk.h"
#include "txt_file_util.h"
#include "fatfs_util.h"


static uint8_t console_buf[255] = {0};
static uint16_t console_index = 0;
static uint16_t console_rn_index = 0;

void ulog(char *str)
{
	HAL_UART_Transmit(CONSOLE_UART, (const uint8_t *)str, strlen(str),  100);
}

void console_RxEventCallback(uint16_t Size)
{
	uint16_t i;

	for (i = console_index; i < console_index + Size; i++)
	{
		if ((console_buf[i] == '\r') || (console_buf[i] == '\n'))
		{
			console_buf[i] = '\0';
			console_rn_index = i;
			console_index = 0;
			break;
		}
	}

	if (console_index >= sizeof(console_buf)) console_index = 0;

	HAL_UARTEx_ReceiveToIdle_IT(CONSOLE_UART, &console_buf[console_index], sizeof(console_buf) - console_index);
}

void console_replay_bool(bool value)
{
	if (value) ulog("OK"CRLF);
	else ulog("ERROR"CRLF);
}


void console_rcv(char * msg)
{
	char buf[MAX_STR_SIZE];
	int i;

	if (strequals(msg, STR_REBOOT)) {
		console_replay_bool(true);
		NVIC_SystemReset();
	}
	else if (strequals(msg, "OUT_ADC")) {
		arr_to_str(&ctrl_out_adc[1], 16, VAR_TYPE_FLOAT, buf, sizeof(buf), false);
		ulog(buf);
		ulog(CRLF);
	}
	else if (strequals(msg, "P_OUT")) {
		arr_to_str(&ctrl_p_out[1], 16, VAR_TYPE_FLOAT, buf, sizeof(buf), false);
		ulog(buf);
		ulog(CRLF);
	}
	else if (strequals(msg, STR_FAULTS)) {
		buf[0] = '\0';
		for (i = 0; i < 128; i++) {
			if (is_set_fault(i)) {
				strcat_s(buf, sizeof(buf), int2str(i));
				strcat_s(buf, sizeof(buf), " ");
			}
		}
		ulog(buf);
		ulog(CRLF);
	}
	else if (strequals(msg, "MIC ON")) {
		console_replay_bool( local_mic_start() );
	}
	else if (strequals(msg, "MIC OFF")) {
		local_mic_stop();
		console_replay_bool(true);
	}
	else if (strequals(msg, "SPK ON")) {
		console_replay_bool( local_spk_start() );
	}
	else if (strequals(msg, "SPK OFF")) {
		local_spk_stop();
		console_replay_bool(true);
	}
	else if (strequals(msg, STR_LIST)) {
		console_replay_bool( f_print_dir(F_DEFAULT_PATH) );
	}
	else if (strcmp_s(msg, strlen(msg), STR_PRINT" ")) {
		console_replay_bool( f_print_file(msg + strlen(STR_PRINT" ")) );
	}
	else if (strcmp_s(msg, strlen(msg), "CFGADD ")) {
		console_replay_bool( txt_file_add_str(BC16_CFG_FILE_NAME, msg + strlen("CFGADD ")) );
	}
	else if (strcmp_s(msg, strlen(msg), "CFGDEL ")) {
		console_replay_bool(txt_file_remove_by_start_of_line(BC16_CFG_FILE_NAME, msg + strlen("CFGDEL ")));
	}
	else if (strequals(msg, STR_FORMAT)) {
		console_replay_bool( f_format_and_mount(&f_global_fatfs, F_MKFS_FMT_DEFAULT) );
	}
	else if (strcmp_s(msg, strlen(msg), STR_DELETE" ")) {
		console_replay_bool( f_unlink(msg + strlen(STR_DELETE" ")) == FR_OK );
	}
}


void console_task(void)
{
	if (console_rn_index) 
	{
		console_rcv((char*)console_buf);
		console_rn_index = 0;
	}
}

void console_init(void)
{
	HAL_UARTEx_ReceiveToIdle_IT(CONSOLE_UART, console_buf, sizeof(console_buf));
}
