/*
 * txt_file_util.h
 *
 *  Created on: 15 апр. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_TXT_FILE_UTIL_H_
#define UTIL_TXT_FILE_UTIL_H_

#include "main.h"

//Сменить строку из файла конфигурации по начальной строке
bool txt_file_change_by_start_of_line(const char * file_name, char * start_of_line, char *new_str);
//удалить строку из файла конфигурации по начальной строке
bool txt_file_remove_by_start_of_line(const char * file_name, char * start_of_line);
//поиск по началу строки
char * txt_file_find_by_start_of_line(const char * file_name, char * start_of_line);
//добавить строку
bool txt_file_add_str(const char * file_name, const char * str);
//печать текстового файла в консоль
bool txt_file_print(const char * file_name);

#endif /* UTIL_TXT_FILE_UTIL_H_ */
