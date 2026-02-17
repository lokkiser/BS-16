/*
 * cfg_util.c
 *
 *  Created on: 10 мар. 2023 г.
 *      Author: Rafael Boltachev
 */

#include "cfg_util.h"
#include "str_util.h"
#include "fatfs_util.h"




bool cfg_read(const char * cfg_file_name)
{
	FIL cfg_file;
	UINT br;
	char * ptr;
	char buf[MAX_STR_SIZE];
	size_t len;



	if ( f_open(&cfg_file, cfg_file_name, FA_READ) != FR_OK)  return false;

	f_readln_init();


	while (f_readln(buf, sizeof(buf), &cfg_file) == FR_OK) {
		len = strlen(buf);
		if (len == 0) continue;
		ptr = strstr_s(buf, len, "=");
		if (!ptr) continue;
		*ptr = '\0';
		cfg_on_read_key_value(buf, ptr + 1);
	}

	f_close(&cfg_file);
	return true;
}

bool cfg_write_key_value(FIL * cfg_file, char * key, char * value)
{
	UINT bw;
	FRESULT fr;
	char buf[MAX_STR_SIZE];


	if (!strlen(key)) return false;

	snprintf(buf, sizeof(buf), "%s=%s"CRLF, key, value);

	fr = f_write(cfg_file, buf, strlen(buf), &bw);

	return (fr == FR_OK) && (strlen(buf) == bw);
}



char * cfg_read_string(const char * cfg_file_name, const char * key)
{
	FIL cfg_file;
	UINT br;
	size_t key_len, len;
	char * ptr, * res;
	char buf[MAX_STR_SIZE];

	key_len = strlen(key);

	if (!key_len) return NULL;

	if ( f_open(&cfg_file, cfg_file_name, FA_READ) != FR_OK)  return NULL;

	f_readln_init();

	res = NULL;

	while (f_readln(buf, sizeof(buf), &cfg_file) == FR_OK) {
		len = strlen(buf);
		if (len == 0) continue;
		if (len < key_len) continue;
		ptr = strstr_s(buf, len, "=");
		if (!ptr) continue;
		len = ptr - buf;
		if (len != key_len) continue;

		if (memcmp(buf, key, key_len) == 0) {
			res = ptr + 1;
			break;
		}
	}

	f_close(&cfg_file);
	return res;
}

bool cfg_write_string(const char * cfg_file_name, char * key, char * value)
{
	char buf[MAX_STR_SIZE];

	if (!strlen(key)) return false;

	snprintf(buf, sizeof(buf), "%s=%s"CRLF, key, value);

	return (f_append(cfg_file_name, buf, strlen(buf)) == FR_OK);

}


/*
bool cfg_read(map_t * map, const char * file_name)
{
	FIL file;
	char buf[MAX_STR_SIZE];
	size_t len;
	char * ptr;

	map_claer(map);

	if ( f_open(&file, file_name, FA_READ) != FR_OK)  return false;

	f_readln_init();


	while (f_readln(buf, sizeof(buf), &file) == FR_OK) {
		len = strlen(buf);
		if (!len) continue;
		ptr = strstr_s(buf, len, "=");
		if (!ptr) continue;
		map_put(&map, buf, ptr - buf, ptr + 1, strlen(ptr + 1));
	}


	f_close(&file);
	return true;
}

bool cfg_write( map_t * map, const char * file_name)
{
	FIL file;
	map_item_t * item;
	UINT bw;
	FRESULT fr;
	bool res;
	char buf[MAX_STR_SIZE];


	if ( f_open(&file, file_name, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)  return false;

	res = true;
	item = map->first;
	while (item) {

		strncpy_s(buf, sizeof(buf), item->data, item->key_len);
		strcat_s(buf, sizeof(buf), "=");
		strncat_s(buf, sizeof(buf), item->data + item->key_len, item->value_len);
		strcat_s(buf, sizeof(buf), CRLF);
		fr = f_write(&file, buf,  strlen(buf), &bw);
		if ((fr != FR_OK) || (bw != strlen(buf))) {
			res = false;
			break;
		}
		item = item->next;
	}



	f_close(&file);
	return res;
}

const char * cfg_get(map_t * map, const char * key)
{
	size_t value_len;

	return map_get(map, key, strlen(key), &value_len);
}

bool cfg_set(map_t * map, const char * key, const char * value)
{
	if ( map_change(map, key, streln(key), value, strlen(value)) ) return true;
	return map_put(map, key, streln(key), value, strlen(value));
}
*/
