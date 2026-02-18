/*
 * txt_file_util.c
 *
 *  Created on: 15 апр. 2024 г.
 *      Author: Rafael Boltachev
 */

#include "txt_file_util.h"
#include <string.h>
#include "fatfs_util.h"
#include "str_util.h"
#include "stm_util.h"


//Сменить строку из файла конфигурации по начальной строке
bool txt_file_change_by_start_of_line(const char * file_name, char * start_of_line, char *new_str)
{
	FIL txt_file, tmp_file;
	UINT bw;
	size_t str_len;
	char buf[MAX_STR_SIZE];


	if (!start_of_line) return false;
	str_len = strlen(start_of_line);
	if (!str_len) return false;


	if (f_open(&txt_file, file_name, FA_READ) != FR_OK)  return false;

	if ( f_open(&tmp_file, "tfcbsol."TMP_FILE_EXT, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
		f_close(&txt_file);
		return false;
	}

	f_readln_init();

	while (f_readln(buf, sizeof(buf), &txt_file) == FR_OK) {
		if (strlen(buf) == 0) continue;
		if (memcmp(buf, start_of_line, str_len) == 0) {
			if ((!new_str) || !strlen(new_str)) continue;
			strcpy_s(buf, sizeof(buf), new_str);
		}
		strcat_s(buf, sizeof(buf), CRLF);
		f_write(&tmp_file, buf, strlen(buf), &bw);
	}

	f_close(&txt_file);
	f_close(&tmp_file);

	f_unlink(file_name);
	f_rename("tfcbsol."TMP_FILE_EXT, file_name);

	return true;
}

//удалить строку из файла конфигурации по начальной строке
bool txt_file_remove_by_start_of_line(const char * file_name, char * start_of_line)
{
	return txt_file_change_by_start_of_line(file_name, start_of_line, NULL);
}

//поиск по началу строки
char * txt_file_find_by_start_of_line(const char * file_name, char * start_of_line)
{
	FIL txt_file;
	size_t len;
	static char __txt_file_find_by_start_of_line_buf[MAX_STR_SIZE];
	size_t str_len;

	if (!start_of_line) return NULL;
	str_len = strlen(start_of_line);
	if (!str_len) return NULL;

	if (f_open(&txt_file, file_name, FA_READ) != FR_OK)  return NULL;

	f_readln_init();

	while (f_readln(__txt_file_find_by_start_of_line_buf, sizeof(__txt_file_find_by_start_of_line_buf), &txt_file) == FR_OK) {
		len = strlen(__txt_file_find_by_start_of_line_buf);
		if (!len) continue;
		if ( memcmp_s(__txt_file_find_by_start_of_line_buf, len, start_of_line, str_len) ) {
			f_close(&txt_file);
			return __txt_file_find_by_start_of_line_buf;
		}
	}

	f_close(&txt_file);
	return NULL;
}


FRESULT txt_file_append_CRLF(const char * file_name, const char * str)
{
		char buf[MAX_STR_SIZE];

		strcpy_s(buf, sizeof(buf), str);
		strcat_s(buf, sizeof(buf), CRLF);
		return f_append(file_name, buf, strlen(buf));
}	


bool txt_file_add_str(const char * file_name, const char * str)
{
	FRESULT fr;

	if (str[strlen(str) - 1] == '\n') {
		fr = f_append(file_name, str, strlen(str));
	} else {
		fr = txt_file_append_CRLF(file_name, str);
	}


	return fr == FR_OK;
}


bool txt_file_print(const char * file_name)
{
	FIL file;
	char buf[MAX_STR_SIZE];


	if ( f_open(&file, file_name, FA_READ) != FR_OK)  return false;

	f_readln_init();

	while (f_readln(buf, sizeof(buf), &file) == FR_OK) {
		if (strlen(buf) == 0) continue;
		ulog(buf);
		ulog(CRLF);
	}

	f_close(&file);

	return true;
}

