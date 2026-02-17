/*
 * logfile_util.c
 *
 *  Created on: 15 апр. 2024 г.
 *      Author: Rafael Boltachev
 */


#include "logfile_util.h"
#include "logfile_cfg.h"
#include "str_util.h"




FRESULT logfile_add_str_and_lf(char * path, char * str)
{
	char buf[MAX_STR_SIZE];

	strcpy_s(buf, sizeof(buf), str);
	strcat_s(buf, sizeof(buf), CRLF);
	return f_append(path, buf, strlen(buf));
}

FRESULT logfile_add(char * str)
{
	FRESULT fr;

	if (f_file_size(LOG_FILE_NAME) > LOG_FILE_MAX_SIZE) { //если размер log файла больше максимального
		f_unlink(LOG_FILE_OLD_NAME);
		f_rename(LOG_FILE_NAME, LOG_FILE_OLD_NAME);
	}


	if (str[strlen(str) - 1] == '\n') {
		fr = f_append(LOG_FILE_NAME, str, strlen(str));
	} else {
		fr = logfile_add_str_and_lf(LOG_FILE_NAME, str);
	}


	return fr;
}
