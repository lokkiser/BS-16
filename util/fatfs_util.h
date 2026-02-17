#ifndef FATFS_UTIL_H_
#define FATFS_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "def_util.h"
#include "main.h"
#include "ff.h"


//Инициализация чтения из файла построчно
void f_readln_init(void);
//чтение из файла построчно
FRESULT f_readln(TCHAR* buf, int len, FIL* fp);
//разиер файла
int f_file_size(const char * file_name);
FRESULT f_file_size2(const char * file_name, FSIZE_t * fsize);
//добавить данные к файлу
FRESULT f_append(const TCHAR* path, const void* buff, UINT btw);
//добавить данные к файлу в начало файла
FRESULT f_rappend(const TCHAR* path, const void* buff, UINT btw);
//добавить данные к файлу, в начало файла с максимальным размером
FRESULT f_rappend_max_size(const TCHAR* path, const void* buff, UINT btw, UINT max_size);
//контрольная сумма
uint64_t f_crc(const TCHAR* path);
//есть ли файл
uint8_t f_exists(const TCHAR* path);
//копирование файла
FRESULT f_copy(const TCHAR* path_from, const TCHAR* path_to);
//дата fno.fdate
void f_parse_date(WORD value, WORD * year, WORD * month, WORD * mday);
//время fno.ftime
void f_parse_time(WORD value, WORD * hour, WORD * min, WORD * sec);
//открыт ли файл
bool f_is_open(FIL * fil);
//из памяти в файл
bool f_mem2file(void * mem, uint32_t mem_size, const char * file_name);
//кол-во файлов в директории
uint16_t f_files_cnt(const char * directory, const char * extension);
//получить название файла по индексу в директории
bool f_file_name(uint16_t index, const char * directory, const char * extension, char * buf, uint16_t buf_size);

#ifdef HAL_USE_FLASH
//копировать файл во флэш STM32 (без стирания секторов)
bool f_copy_to_flash(const TCHAR* path, uint32_t address);
//копировать файл во флэш STM32 (со стиранием секторов)
bool f_erase_and_copy_to_flash(const char * file_name, uint32_t address, uint8_t stm32flashtype);
#endif
//Путь FATFS по умалчанию
#define F_MKFS_PATH_DEFAULT		""
//Тип форматирования FATFS по умолчанию
#define F_MKFS_FMT_DEFAULT		0

//форматировать FATFS (path="" fmt = FM_FAT, FM_FAT32, FM_EXFAT)
FRESULT f_format(const TCHAR* path, BYTE fmt);
#define F_FORMAT_DEFAULT f_format(F_MKFS_PATH_DEFAULT, F_MKFS_FMT_DEFAULT)
//форматировать FATFS и смонтировать
bool f_format_and_mount(FATFS* fs, BYTE fmt);
bool f_format_and_mount_fatfs(BYTE fmt);
//смонтировать FATFS, cnt - число попыток
bool f_mount_cnt(FATFS* fs, uint8_t cnt);
bool f_mount_cnt_fatfs(uint8_t cnt);
//Корень FATFS
#define F_DEFAULT_PATH			"/"
//напечатать содержимое каталога
uint64_t f_print_dir(const TCHAR* path);
//напечатать файл
bool f_print_file(const char * file_name);
//читать из файла
unsigned int f_stream_read(void * afile, void * data, unsigned int data_len);
//писать в файл
unsigned int f_stream_write(void * afile, void * data, unsigned int data_len);


typedef bool f_enum_files_proc_t(size_t index, char * name, unsigned long size,  void * param);
bool f_enum_files(const char * directory, f_enum_files_proc_t f_enum_files_proc, void * param);



extern FIL f_global_file;

extern FATFS f_global_fatfs;

#ifdef __cplusplus
}
#endif



#endif /* UTIL_FATFS_UTIL_H_ */
