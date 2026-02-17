#include "ini_util.h"
#include <string.h>
#include <stdio.h>
#include "str_util.h"



FRESULT ini_read(const char * file_name)
{
   FIL file;
   char buf[MAX_STR_SIZE], * ptrb, * ptre;
   FRESULT fr;

   fr = f_open(&file, file_name, FA_READ);
   if (fr != FR_OK) return fr;

   f_readln_init();

   while (f_readln(buf, sizeof(buf), &file) == FR_OK) {
        if (!strlen(buf)) continue;
        ptrb = strstr_s(buf, strlen(buf), "[");
        if (ptrb) {
          ptre = strstr_s(buf, strlen(buf), "]");
          if (ptre) {
             *ptre = '\0';
             ini_on_read_section(ptrb + 1);
             continue;
          }
        }

        ptrb = strstr_s(buf, strlen(buf), "=");
        if (ptrb) {
           *ptrb = '\0';
           ini_on_read_key_value(buf, ptrb + 1);
         }
   }
   f_close(&file);
   return FR_OK;
}

FRESULT ini_write_section(FIL * file, char * section)
{
  char buf[MAX_STR_SIZE];
  UINT bw;
  FRESULT fr;

  snprintf(buf, sizeof(buf), "[%s]"CRLF,  section);

  fr = f_write (file, buf, strlen(buf), &bw);

  if ((fr == FR_OK) && (strlen(buf) == bw)) return FR_OK;

  return fr;
}

FRESULT ini_write_key_value(FIL * file, char * key, char * value)
{
  char buf[MAX_STR_SIZE];
  UINT bw;
  FRESULT fr;

  snprintf(buf, sizeof(buf), "%s=%s"CRLF, key, value);

  fr = f_write(file, buf, strlen(buf), &bw);

  if ((fr == FR_OK) && (strlen(buf) == bw)) return FR_OK;

  return fr;
}



/*
bool ini_load_from_file(char * path, ini_t * pini)
{
  FILE * fil;
  char str[INI_KEY_SIZE + INI_VALUE_SIZE + 2], * ptr, *ptre;
  ini_section_t * section = NULL;

  memset(pini, 0, sizeof(ini_t));
  if ( (fil = fopen(path, "r")) == NULL) return FALSE;

  while (fgets(str, sizeof(str), fil) != NULL)
  {
    if ((ptr = strtrim(str)) == NULL) continue;

    if ( (ptr[0] == '[') && (ptr[strlen(ptr) - 1] == ']') ) {
      section = &pini->sections[pini->sections_count];
      memcpy(section->name, ptr + 1, MIN(strlen(ptr) - 2, INI_SECTION_NAME_SIZE));
      pini->sections_count++;
      if (pini->sections_count >= INI_SECTION_SIZE) break;
    } else if ( (ptre = strchr(ptr, '=')) != NULL ) {
      if (section) {
        if (section->datas_count < INI_DATA_SIZE) {
          memcpy(section->datas[section->datas_count].key, ptr, MIN(ptre - ptr, INI_KEY_SIZE) );
          memcpy(section->datas[section->datas_count].value, ptre + 1, MIN(strlen(ptre + 1), INI_VALUE_SIZE) );
          section->datas_count++;
        }
      }
    }

  }

  fclose(fil);

  return TRUE;
}



bool ini_load_from_str(char * str, ini_t * pini)
{
  char * ptr, * ptre, *line, tmp[INI_KEY_SIZE + INI_VALUE_SIZE + 2];
  int len;
  ini_section_t * section = NULL;


  if (!strlen(str)) return FALSE;

  memset(pini, 0, sizeof(ini_t));

  ptr = str;
  do {
	  ptre = strstr_s(ptr, strlen(ptr), "\r\n");
	  if (ptre) len = ptre - ptr;
	  else  len = strlen(ptr);
	  strncpy_s(tmp, sizeof(tmp), ptr, len);
	  if ((line = strtrim(tmp)) != NULL) {
		  if ( (line[0] == '[') && (line[strlen(line) - 1] == ']') ) {
    	    section = &pini->sections[pini->sections_count];
        	strncpy_s(section->name, INI_SECTION_NAME_SIZE, line + 1, strlen(line) - 2);
        	pini->sections_count++;
        	if (pini->sections_count >= INI_SECTION_SIZE) break;
      	  } else if ( (ptre = strchr_s(line, strlen(line), '=')) != NULL ) {
      		  if (section) {
      			  if (section->datas_count < INI_DATA_SIZE) {
      				  strncpy_s(section->datas[section->datas_count].key, INI_KEY_SIZE, line, ptre - line);
      				  strncpy_s(section->datas[section->datas_count].value, INI_VALUE_SIZE, ptre + 1, strlen(ptre + 1));
      				  section->datas_count++;
      			  }
      		  }
      	  }
	  }

  } while ((ptr = strright(ptr, strlen(ptr), "\r\n")) != NULL);


  return TRUE;
}

int ini_get_section_index(const char * name, ini_t * pini)
{
    int i;
    
    for (i = 0; i < pini->sections_count; i++) {
      if (strcmp(pini->sections[i].name, name) == 0) return i;
    }
    return -1;
}

ini_section_t * ini_get_section(const char * name, ini_t * pini)
{
  int index = ini_get_section_index(name, pini);
  return index == -1 ? NULL : &pini->sections[index];
}


char * ini_section_get_value(const char * key, ini_section_t * section)
{
  int i;

  for (i = 0; i < section->datas_count; ++i) {
     if (strcmp(section->datas[i].key, key) == 0) return section->datas[i].value;
  }
  return NULL;
}


char * ini_get_value(const char * section, const char * key, ini_t * pini)
{
  ini_section_t * s = ini_get_section(section, pini);
  if (s == NULL) return NULL;

  return ini_section_get_value(key, s);
}
*/
