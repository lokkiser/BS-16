#include "fatfs_util.h"
#include "str_util.h"
#include "stm_util.h"
#include "math_util.h"




static char f_readln_buf[MAX_STR_SIZE + 2];
static UINT f_readln_load = 0;

void f_readln_init(void)
{
	f_readln_load = 0;
}

//чтение из файла построчно
FRESULT f_readln(TCHAR* buf, int len, FIL* fp)
{
  UINT br;
  FRESULT fr;
  char * ptr;
  int l;

  if (f_readln_load == 0) //данных в буфере нет
  {
    fr = f_read(fp, f_readln_buf, sizeof(f_readln_buf),  &f_readln_load);
    if ( (fr != FR_OK) || (f_readln_load == 0) ) {
    	return FR_INVALID_PARAMETER;
    }
  }

  if ( (ptr = strchr_s(f_readln_buf, f_readln_load, '\n')) == NULL ) { //не нашли \n в строке
    l = sizeof(f_readln_buf) - f_readln_load;  //остаток байтов в буфере
    if (l <= 0) {  //строка не влазит в буфер
      l = MIN(f_readln_load, len - 1);
      memcpy(buf, f_readln_buf,  l);
      buf[l] = '\0';
      if (l > 0) {
          if (buf[l - 1] == '\r') buf[l - 1] = '\0';
      }
      f_readln_load = 0;
      return FR_OK;
    }
    ptr = f_readln_buf + f_readln_load;
    fr = f_read(fp, ptr, l,  &br); //подгрузку делаеи со смещением f_readln_load
    if ( (fr != FR_OK) || (br == 0) ) { //прочитали все
      l = MIN(f_readln_load, len - 1);
      memcpy(buf, f_readln_buf,  l);
      buf[l] = '\0';
      if (l > 0) {
          if (buf[l - 1] == '\r') buf[l - 1] = '\0';
      }
      f_readln_load = 0;
      return FR_OK;
    }
    f_readln_load += br; //всего загруженных байт
    if ( (ptr = strchr_s(f_readln_buf, f_readln_load, '\n')) == NULL ) { //строка не влазит в буфер
      l = MIN(f_readln_load, len - 1);
      memcpy(buf, f_readln_buf,  l);
      buf[l] = '\0';
      if (l > 0) {
          if (buf[l - 1] == '\r') buf[l - 1] = '\0';
      }
      f_readln_load = 0;
      return  FR_OK;
    }
  }


  l = MIN((ptr - f_readln_buf), len - 1);  //байтов до \n
  memcpy(buf, f_readln_buf, l);
  buf[l] = '\0';
  if (l > 0) {
    if (buf[l - 1] == '\r') buf[l - 1] = '\0';
  }

  ptr++; //за \n
  l = f_readln_load - (ptr - f_readln_buf); //длина остатка
  if (l <= 0) {
    f_readln_load = 0;
    return FR_OK;
  }
  memcpy(f_readln_buf, ptr, l); //копируем остаток в начало
  f_readln_load = l;
  return  FR_OK;
}

//размер файла
int f_file_size(const char * file_name)
{
	FILINFO FileInfo;

	if (f_stat(file_name, &FileInfo) != FR_OK) return -1;
	return (int)FileInfo.fsize;
}

//размер файла 2
FRESULT f_file_size2(const char * file_name, FSIZE_t * fsize)
{
	FILINFO FileInfo;
	FRESULT res;

	res = f_stat(file_name, &FileInfo);
	*fsize = FileInfo.fsize;

	return res;
}


//добавить данные к файлу
FRESULT f_append(const TCHAR* path, const void* buff, UINT btw)
{

	FIL file;
	FRESULT fr;
	UINT bw;

	if ( f_open(&file, path, FA_OPEN_ALWAYS | FA_WRITE | FA_OPEN_APPEND) != FR_OK)  return FR_NO_FILE;

	bw = 0;

	fr = f_write(&file, buff, btw, &bw);
	f_close(&file);

	if (bw != btw) return FR_INVALID_PARAMETER;

	return fr;
}


//добавить данные к файлу, в начало файла
FRESULT f_rappend_max_size(const TCHAR* path, const void* buff, UINT btw, UINT max_size)
{
	FIL file, tmp_file;
	UINT bw, br, sz;
	FRESULT fr;
	char buf[1024];


	fr = f_open(&file, path, FA_OPEN_ALWAYS | FA_READ);
	if (fr != FR_OK)  return fr;

	fr = f_open(&tmp_file, "frams."TMP_FILE_EXT, FA_CREATE_ALWAYS | FA_WRITE);
	if (fr != FR_OK) {
		f_close(&file);
		return fr;
	}

	fr = f_write(&tmp_file, buff,  btw, &bw);
	if ( (fr != FR_OK) || (btw != bw) ) {
		f_close(&file);
		f_close(&tmp_file);
		f_unlink("frams."TMP_FILE_EXT);
		return FR_INVALID_PARAMETER;
	}

	sz = bw;


	while (f_read(&file, buf, sizeof(buf), &br) == FR_OK) {
		if (br == 0) break;
		fr = f_write(&tmp_file, buf, br, &bw);
		if ( (fr != FR_OK) || (br != bw) ) {
			f_close(&file);
			f_close(&tmp_file);
			f_unlink("frams."TMP_FILE_EXT);
			return FR_INVALID_PARAMETER;
		}
		sz += bw;
		if (max_size && (sz >= max_size)) break;
	}

	f_close(&file);
	f_close(&tmp_file);

	f_unlink(path);
	f_rename("frams."TMP_FILE_EXT, path);

	return FR_OK;
}

//добавить данные к файлу, в начало файла
FRESULT f_rappend(const TCHAR* path, const void* buff, UINT btw)
{
	return f_rappend_max_size(path, buff, btw, 0);
}

//контрольная сумма
uint64_t f_crc(const TCHAR* path)
{
	FIL file;
	UINT br, i;
	FRESULT fr;
	uint64_t res;
	uint8_t buf[1024];

	fr = f_open(&file, path, FA_READ);
	if (fr != FR_OK)  return 0;

	res = 0;

	while (f_read(&file, buf, sizeof(buf), &br) == FR_OK) {
		if (br == 0) break;
		for (i = 0; i < br; ++i) res += buf[i];
	}

	f_close(&file);

	return res;
}


FRESULT f_copy(const TCHAR* path_from, const TCHAR* path_to)
{
	FIL file, tmp_file;
	UINT br, bw;
	FRESULT fr;
	uint8_t buf[1024];

	fr = f_open(&file, path_from, FA_READ);
	if (fr != FR_OK)  return fr;


	fr = f_open(&tmp_file, "fc."TMP_FILE_EXT, FA_CREATE_ALWAYS | FA_WRITE);
	if (fr != FR_OK) {
		f_close(&file);
		return fr;
	}



	while (f_read(&file, buf, sizeof(buf), &br) == FR_OK) {
		if (br == 0) break;
		fr = f_write(&tmp_file, buf,  br, &bw);
		if ( (fr != FR_OK) || (br != bw) ) {
			f_close(&file);
			f_close(&tmp_file);
			f_unlink("fc."TMP_FILE_EXT);
			return FR_INVALID_PARAMETER;
		}
	}

	f_close(&file);
	f_close(&tmp_file);

	f_unlink(path_to);
	f_rename("fc."TMP_FILE_EXT, path_to);

	return FR_OK;
}

//существует ли файл
uint8_t f_exists(const TCHAR* path)
{
	FIL file;

	if (f_open(&file, path, FA_READ) != FR_OK)  return 0;
	f_close(&file);
	return 1;
}


//fno.fdate = (WORD)(((year - 1980) * 512U) | month * 32U | mday);
void f_parse_date(WORD value, WORD * year, WORD * month, WORD * mday)
{
	WORD v;

	*year = value / 512U;
	v = value - (*year) * 512U;
	*year += 1980;

	*month = v / 32U;
	v = v - (*month) * 32U;
	*mday = v;
}

//fno.ftime = (WORD)(hour * 2048U | min * 32U | sec / 2U);
void f_parse_time(WORD value, WORD * hour, WORD * min, WORD * sec)
{
	WORD v;

	*hour = value / 2048U;
	v = value - (*hour) * 2048U;
	*min = v / 32U;
	v = v - (*min) * 32U;
	*sec = v * 2U;
}

//файл открыт
bool f_is_open(FIL * fil)
{
	return fil->obj.fs != NULL;
}


bool f_mem2file(void * mem, uint32_t mem_size, const char * file_name)
{
	FIL fil;
	unsigned char * ptr;
	uint32_t len;
	FRESULT fr;
	UINT bw;

	if ( f_open(&fil, file_name, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) return false;


	len = mem_size;
	ptr = (unsigned char *)mem;

	while (len > 0) {
		bw = 0;
		fr = f_write(&fil, ptr, MIN(512, len), &bw);
		if (fr != FR_OK) break;
		if (!bw) break;
		if (len < bw) {
			len = 0;
			break;
		}
		len -= bw;
		ptr += bw;
	}

	f_close(&fil);

	return len == 0;
}

#ifndef FF_LFN_BUF
#define FF_LFN_BUF	256
#endif

uint16_t f_files_cnt(const char * directory, const char * extension)
{
	DIR dir;
	FILINFO fno;
	uint16_t i;
	TCHAR extlower[FF_LFN_BUF + 1];
	TCHAR extupper[FF_LFN_BUF + 1];


	if ( f_opendir(&dir, directory) != FR_OK ) return 0;

	if (extension) {
		str2lower(extlower, sizeof(extlower), extension);
		str2upper(extupper, sizeof(extupper), extension);
	}


	i = 0;
	while (f_readdir(&dir, &fno) == FR_OK) {
		if (fno.fname[0] == 0) break;  // Break on error or end of dir
		if (fno.fname[0] == '.')  continue;
		if (fno.fattrib & AM_DIR) continue;     //Это директория
		if (extension) {
			if (!strstr_s(fno.fname, strlen(fno.fname), extlower) && !strstr_s(fno.fname, strlen(fno.fname), extupper) ) continue;
		}
		i++;
	}
	f_closedir(&dir);

	return i;
}

bool f_file_name(uint16_t index, const char * directory, const char * extension, char * buf, uint16_t buf_size)
{
	DIR dir;
	FILINFO fno;
	uint16_t i;
	TCHAR extlower[FF_LFN_BUF + 1];
	TCHAR extupper[FF_LFN_BUF + 1];

	if ( f_opendir(&dir, directory) != FR_OK ) return false;

	if (extension) {
		str2lower(extlower, sizeof(extlower), extension);
		str2upper(extupper, sizeof(extupper), extension);
	}

	i = 0;
	while (f_readdir(&dir, &fno) == FR_OK) {
		if (fno.fname[0] == 0) break;  // Break on error or end of dir
		if (fno.fname[0] == '.')  continue;
		if (fno.fattrib & AM_DIR) continue;     //Это директория
		if (extension) {
			if (!strstr_s(fno.fname, strlen(fno.fname), extlower) && !strstr_s(fno.fname, strlen(fno.fname), extupper) ) continue;
		}

		if (i == index) {
			strcpy_s(buf, buf_size, fno.fname);
			f_closedir(&dir);
			return true;
		}
		i++;
	}
	f_closedir(&dir);
	return false;
}

#ifdef HAL_USE_FLASH


bool f_copy_to_flash(const TCHAR* path, uint32_t address)
{
	FIL file;
	UINT br;
	uint8_t buf[1024];
	uint32_t flash_address, write_bytes;
	FSIZE_t fsz;

	if (f_file_size2(path, &fsz) != FR_OK) return false;

	if (!fsz) return false;

	if (f_open(&file, path, FA_READ) != FR_OK)  return false;


    ulog_fmt("f_copy_to_flash: start write file %s to flash, address:0x%s file_size:%lu"CRLF, path, ul2hexstr(address), fsz);


	ulog(CRLF"[");
    flash_address = address;
	while (f_read(&file, buf, sizeof(buf), &br) == FR_OK) {
		if (!br) break;

	    //ulog_fmt("write address:0x%s br:%lu"CRLF, ul2hexstr(flash_address), br);

		if (hal_write_data_to_flash(flash_address, buf, br) != HAL_OK) {
			ulog("f_copy_to_flash: error write to flash"CRLF);
			break;
		}


		flash_address += br;
		ulog("*");
	}
	f_close(&file);
	ulog("]"CRLF);


	write_bytes = flash_address - address;

    ulog_fmt("f_copy_to_flash: finish write file %s to flash, write_bytes:%lu, res:%s"CRLF, path, write_bytes, bool2str((write_bytes >= fsz)));

	return (write_bytes >= fsz);

}





bool f_erase_and_copy_to_flash(const char * file_name, uint32_t address, uint8_t stm32flashtype)
{
	FSIZE_t fsz;
	uint64_t file_crc, mem_crc;
	bool res;


	if (!f_exists(file_name)) return false;

	if (f_file_size2(file_name, &fsz) != FR_OK) return false;

	if (!fsz) return false;



    ulog_fmt("hal_erase_by_address:0x%s, size:%lu"CRLF, ul2hexstr(address), fsz);
	//очищаем флэш
	if (hal_erase_by_address(address, address + fsz, stm32flashtype) != HAL_OK) {
	    ulog("f_erase_and_copy_to_flash: invalid erase by address"CRLF);
	    return false;
	}

	//копируем данные во флэщ
	res = f_copy_to_flash(file_name, address);
	if (!res) {
	    ulog("f_erase_and_copy_to_flash: error f_copy_to_flash"CRLF);
		return false;
	}

	file_crc = f_crc(file_name);
	mem_crc = ulCrcSum((unsigned char *)address, fsz); //кс файла в памяти
	res = (mem_crc == file_crc);

	ulog_fmt("f_erase_and_copy_to_flash: file_size:%lu file_crc:%s mem_crc:%s res:%s"CRLF, fsz, ull2str(file_crc), ull2str(mem_crc), bool2str(res));

	if (!res) hal_erase_by_address(address, address + fsz, stm32flashtype);


	return res;
}

#endif

//форматировать FATFS (path="" fmt = FM_FAT, FM_FAT32, FM_EXFAT)
FRESULT f_format(const TCHAR* path, BYTE fmt)
{
	MKFS_PARM mkfs_param;
	FRESULT fr;
	char work[FF_MAX_SS];

	mkfs_param.fmt = fmt;
	mkfs_param.n_fat = 0;
	mkfs_param.align = 0;
	mkfs_param.n_root = 0;
	mkfs_param.au_size = 0;


	fr = f_mkfs(path, (fmt == 0 ? NULL : &mkfs_param), work, FF_MAX_SS);
	return fr;
}


bool f_format_and_mount(FATFS* fs, BYTE fmt)
{
	FRESULT fr;

#ifdef HAL_IWDG_MODULE_ENABLED
		HAL_IWDG_Refresh(&hiwdg);
#endif

	fr = f_format(F_MKFS_PATH_DEFAULT, fmt);

#ifdef HAL_IWDG_MODULE_ENABLED
		HAL_IWDG_Refresh(&hiwdg);
#endif

	if (fr != FR_OK) {
		ulog_fmt("FatFs: format failed, res=%d"CRLF, fr);
		return false;
	}
	ulog("FatFs: format succes"CRLF);


	fr = f_mount(fs, F_MKFS_PATH_DEFAULT, 1);

#ifdef HAL_IWDG_MODULE_ENABLED
		HAL_IWDG_Refresh(&hiwdg);
#endif

	if (fr != FR_OK) {
		ulog_fmt("FatFs: f_mount error, res=%d"CRLF, fr);
		return false;
	}
	ulog("FatFs: f_mount succes"CRLF);


	return true;
}

bool f_format_and_mount_fatfs(BYTE fmt)
{
	FATFS fs;

	return f_format_and_mount(&fs, fmt);

}

bool f_mount_cnt(FATFS* fs, uint8_t cnt)
{
	FRESULT fr;

	do {
		fr = f_mount(fs, F_MKFS_PATH_DEFAULT, 1);
		cnt--;
		if (fr != FR_OK) HAL_Delay(250);

#ifdef HAL_IWDG_MODULE_ENABLED
		HAL_IWDG_Refresh(&hiwdg);
#endif

	}
	while (cnt && (fr != FR_OK));
	return (fr == FR_OK);
}

bool f_mount_cnt_fatfs(uint8_t cnt)
{
	FATFS fs;

	return f_mount_cnt(&fs, cnt);
}


uint64_t f_print_dir(const TCHAR* path)
{

	 FRESULT res;
	 DIR dir;
	 FILINFO fno;
	 uint8_t cnt;
	 char *fn;
	 uint64_t sz;
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif


	 res = f_opendir(&dir, path);                       //Open the directory
	 if (res != FR_OK) return 0;

	 cnt = 0;
	 sz = 0;
	 while (cnt < 0xFF) {
		 res = f_readdir(&dir, &fno);                   // Read a directory item
		 //ulog_fmt("FatFs: f_readdir res:%d\n", res);
		 if ((res != FR_OK) || (fno.fname[0] == '\0')) break;  // Break on error or end of dir
		 if (fno.fname[0] == '.')  continue;
		 if (fno.fattrib & AM_DIR) continue;     //Это директория

#if _USE_LFN
         fn = *fno.lfname ? fno.lfname : fno.fname;
#else
         fn = fno.fname;
#endif
		 ulog_fmt("FatFs: file:%s size:%lu"CRLF, fn, fno.fsize);
		 cnt++;
		 sz += fno.fsize;
     }
     f_closedir(&dir);

     return sz;
}


bool f_print_file(const char * file_name)
{
	FIL file;
	char buf[512];

	if ( f_open(&file, file_name, FA_READ) != FR_OK)  return false;

	f_readln_init();

	while (f_readln(buf, sizeof(buf), &file) == FR_OK) {
		if (strlen(buf) == 0) continue;
		ulog_fmt("%s"CRLF, buf);
	}

	f_close(&file);

	return true;
}

unsigned int f_stream_read(void * afile, void * data, unsigned int data_len)
{
	unsigned int br;

	if (f_read((FIL *)afile, data, data_len,  &br) != FR_OK) return 0;

	return br;
}

unsigned int f_stream_write(void * afile, void * data, unsigned int data_len)
{

	unsigned int bw;

	if (f_write((FIL *)afile, data, data_len,  &bw) != FR_OK) return 0;

	return bw;
}


FIL f_global_file = {0};
FATFS f_global_fatfs = {0};


bool f_enum_files(const char * directory, f_enum_files_proc_t f_enum_files_proc, void * param)
{
	DIR dir;
	FILINFO fno;
	size_t i;

	if (!f_enum_files_proc) return false;

	if ( f_opendir(&dir, directory) != FR_OK ) return false;

	i = 0;
	while (f_readdir(&dir, &fno) == FR_OK) {
		if (fno.fname[0] == 0) break;  // Break on error or end of dir
		if (fno.fname[0] == '.')  continue;
		if (fno.fattrib & AM_DIR) continue;     //Это директория
		if (f_enum_files_proc(i, fno.fname, fno.fsize, param)) break;
		i++;
	}
	f_closedir(&dir);

	return true;
}

/*
//добавить данные к файлу, в начало файла
FRESULT f_rappend(const TCHAR* path, const void* buff, UINT btw)
{
	FIL file, tmp_file;
	UINT bw, br;
	FRESULT fr;
	char buf[1024];

	fr = f_open(&file, path, FA_OPEN_ALWAYS | FA_READ);
	if (fr != FR_OK)  return fr;

	fr = f_open(&tmp_file, "tmp", FA_CREATE_ALWAYS | FA_WRITE);
	if (fr != FR_OK) {
		f_close(&file);
		return fr;
	}

	fr = f_write(&tmp_file, buff,  btw, &bw);
	if ( (fr != FR_OK) || (btw != bw) ) {
		f_close(&file);
		f_close(&tmp_file);
		f_unlink("tmp");
		return FR_INVALID_PARAMETER;
	}


	while (f_read(&file, buf, sizeof(buf), &br) == FR_OK) {
		if (br == 0) break;
		fr = f_write(&tmp_file, buf, br, &bw);
		if ( (fr != FR_OK) || (br != bw) ) {
			f_close(&file);
			f_close(&tmp_file);
			f_unlink("tmp");
			return FR_INVALID_PARAMETER;
		}
	}

	f_close(&file);
	f_close(&tmp_file);

	f_unlink(path);
	f_rename("tmp", path);

	return FR_OK;
}


typedef bool enum_files_from_ext_proc_t(size_t index, char * name, void * param);

bool enum_files_from_ext(const char * ext, enum_files_from_ext_proc_t enum_files_from_ext_proc, void * param)
{
	DIR dir;
	FILINFO fno;
	char buf_l[32], buf_u[32];
	size_t i;


	if (!enum_files_from_ext_proc) return false;

	memset(buf_l, 0, sizeof(buf_l));
	memset(buf_u, 0, sizeof(buf_u));

	for (i = 0; i < MIN(strlen(ext), MIN(sizeof(buf_l) - 1, sizeof(buf_u) - 1) ); i++) {
		buf_l[i] = tolower(ext[i]);
		buf_u[i] = toupper(ext[i]);
	}


	if (!fatfs_mount) return false;
	if ( f_opendir(&dir, "/") != FR_OK ) return false;

	i = 0;
	while (f_readdir(&dir, &fno) == FR_OK) {
		if (fno.fname[0] == 0) break;  // Break on error or end of dir
		if (fno.fname[0] == '.')  continue;
		if (fno.fattrib & AM_DIR) continue;     //Это директория
		if (!strstr_s(fno.fname, strlen(fno.fname), buf_l) && !strstr_s(fno.fname, strlen(fno.fname), buf_u) ) continue;
		if (enum_files_from_ext_proc(i, fno.fname, param)) break;
		i++;
	}
	f_closedir(&dir);

	return false;
}
*/

