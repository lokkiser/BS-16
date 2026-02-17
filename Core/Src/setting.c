/*
 * setting.c
 *
 *  Created on: 16 мар. 2023 г.
 *      Author: Rafael Boltachev
 */

#include "setting.h"
#include <string.h>
#include <stdio.h>
#include "fatfs_util.h"
#include "str_util.h"

setting_t setting;

bool compare_key(char* s, size_t s_size, const char* key)
{
	if (s_size != strlen(key)) return false;

	return strcmp_s(s, s_size, key);
}

void default_setting(void)
{
	memset(&setting, 0, sizeof(setting));
}

bool load_setting(void)
{
	FIL cfg_file;
	size_t len, key_len;
	char buf[MAX_STR_SIZE], * ptr;

	default_setting();

	if (!fatfs_mount) return false;

	if ( f_open(&cfg_file, BC16_CFG_FILE_NAME, FA_READ) != FR_OK)  return false;

	f_readln_init();

	while (f_readln(buf, sizeof(buf), &cfg_file) == FR_OK) {
		len = strlen(buf);
		if (len == 0) continue;
		ptr = strstr_s(buf, len, "=");
		if (!ptr) continue;
		key_len = ptr - buf;
		if (!key_len) continue;
		ptr++;

		if (compare_key(buf, key_len, SETTING_KEY_MIC_VOLUME)) setting.mic_volume = str2int(ptr);
		else if (compare_key(buf, key_len, SETTING_KEY_SPK_VOLUME)) setting.spk_volume  = str2int(ptr);
		else if (compare_key(buf, key_len, SETTING_KEY_DATE_TIME)) strcpy_s(setting.date_time, sizeof(setting.date_time), ptr);

	}

	f_close(&cfg_file);

	return true;
}

bool save_setting(void)
{
	FIL tmp_file;
	UINT bw;
	char buf[MAX_STR_SIZE];

	if (!fatfs_mount) return false;


	if ( f_open(&tmp_file, TMP_FILE_EXT, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) return false;

	snprintf(buf, sizeof(buf), "%s=%d"CRLF, SETTING_KEY_MIC_VOLUME, setting.mic_volume);
	f_write(&tmp_file, buf, strlen(buf), &bw);

	snprintf(buf, sizeof(buf), "%s=%d"CRLF, SETTING_KEY_SPK_VOLUME, setting.spk_volume);
	f_write(&tmp_file, buf, strlen(buf), &bw);

	snprintf(buf, sizeof(buf), "%s=%s"CRLF, SETTING_KEY_DATE_TIME, setting.date_time);
	f_write(&tmp_file, buf, strlen(buf), &bw);

	f_close(&tmp_file);
	f_unlink(BC16_CFG_FILE_NAME);
	f_rename(TMP_FILE_EXT, BC16_CFG_FILE_NAME);

	return true;
}

/*
void setting_task(void)
{
	if (need_save_setting_tick && HAL_DiffTick(need_save_setting_tick) > 15000) {
		need_save_setting_tick = 0;
		save_setting();
	}
}
*/
