#ifndef INI_UTIL_H_
#define INI_UTIL_H_

#include "def_util.h"
#include "fatfs_util.h"


#ifdef __cplusplus
extern "C" {
#endif

 //Must be defined
void ini_on_read_section(char * section);
 //Must be defined
void ini_on_read_key_value(char * key, char * value);

FRESULT ini_read(const char * file_name);
FRESULT ini_write_section(FIL * file, char * section);
FRESULT ini_write_key_value(FIL * file, char * key, char * value);

/*

#ifndef INI_KEY_SIZE
#define INI_KEY_SIZE                64
#endif

#ifndef INI_VALUE_SIZE
#define INI_VALUE_SIZE              128
#endif

#ifndef INI_SECTION_NAME_SIZE
#define INI_SECTION_NAME_SIZE       64
#endif

#ifndef INI_DATA_SIZE
#define INI_DATA_SIZE               20
#endif

#ifndef INI_SECTION_SIZE
#define INI_SECTION_SIZE            5
#endif

typedef struct _ini_data {
  char key[INI_KEY_SIZE];
  char value[INI_VALUE_SIZE];
} ini_data_t;


typedef struct _ini_section {
  char name[INI_SECTION_NAME_SIZE];
  ini_data_t datas[INI_DATA_SIZE];
  int datas_count;
} ini_section_t;


typedef struct _ini {
  ini_section_t sections[INI_SECTION_SIZE];
  int sections_count;
}  ini_t;





bool ini_load_from_file(char * path, ini_t * pini);
ini_section_t * ini_get_section(const char * name, ini_t * pini);
int ini_get_section_index(const char * name, ini_t * pini);
char * ini_section_get_value(const char * key, ini_section_t * section);
char * ini_get_value(const char * section, const char * key, ini_t * pini);

*/

#ifdef __cplusplus
}
#endif

#endif /* INI_UTIL_H_ */
