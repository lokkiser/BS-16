/*
 * json_util.c
 *
 *  Created on: 22 окт. 2024 г.
 *      Author: Rafael Boltachev
 */


#include "json_util.h"
#include <stdio.h>
#include "str_util.h"


bool json_find_key_value(char * str, char * key, char * buf, uint16_t buf_len)
{
	char * ptr, * ptre;

	ptr = strright(str, strlen(str), key);
	if (!ptr) return FALSE;

	ptr = strright(ptr, strlen(ptr), ":");
	if (!ptr) return FALSE;


	ptre = strstr_s(ptr, strlen(ptr), ",");
	if (!ptre) ptre = strstr_s(ptr, strlen(ptr), "}");
	if (!ptre) ptre = strstr_s(ptr, strlen(ptr), "]");
	if (!ptre) ptre = ptr + strlen(ptr);

	strncpy_s(buf, buf_len, ptr, ptre - ptr);
	strcpy_s(buf, buf_len, strtrim(buf));

	return TRUE;
}

char * json_get_value(char * json, char * key, uint16_t * len)
{
  char buf[64];
  char * ptr, * end_ptr;
  uint16_t i, max_len;
  uint8_t level;

  *len = 0;
  snprintf(buf, sizeof(buf), "\"%s\": ", key);

  ptr = strstr(json, buf);
  if (ptr == NULL) return NULL;
  ptr += strlen(buf);
  ptr = strtrim(ptr);
  if (ptr == NULL) return NULL;

  *len = strlen(ptr);

  if (*ptr == '{') {
     ++ptr;
     max_len = strlen(ptr);
     level = 1;
     for (i = 0; i < max_len; ++i) {
        if (ptr[i] == '{') ++level;
        else if (ptr[i] == '}') {
          --level;
          if (level == 0) {
             *len = i;
             break;
          }
        }
     }
  }
  else if (*ptr == '[') {
     ++ptr;
     end_ptr = strchr(ptr, ']');
     if (!end_ptr) end_ptr = ptr + strlen(ptr);
     *len = end_ptr - ptr;
  }
  else if (*ptr == '"') {
     ++ptr;
     end_ptr = strchr(ptr, '"');
     if (!end_ptr) end_ptr = ptr + strlen(ptr);
     *len = end_ptr - ptr;
  } else {
     end_ptr = strchr(ptr, ',');
     if (!end_ptr) end_ptr = strchr(ptr, '}');
     if (!end_ptr) end_ptr = ptr + strlen(ptr);
     *len = end_ptr - ptr;
  }

  return ptr;
}
