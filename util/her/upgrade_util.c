/*
 * upgrade_util.c
 *
 *  Created on: 18 апр. 2024 г.
 *      Author: Rafael Boltachev
 */


#include "upgrade_util.h"
#include "stm_util.h"
#include "fatfs_util.h"
#include "math_util.h"
#include "str_util.h"
#include "diskio.h"




bool check_loader(const char* loader_file_name, uint32_t loader_addr, uint8_t stm32flashtype)
{
	bool loader_download, loader_file_exists;
	uint64_t loader_file_crc, mem_crc;
	FSIZE_t loader_file_size;


	loader_download = is_programm_download(loader_addr, 128000); //загрузчик прошит в памяти
	loader_file_exists = f_exists(loader_file_name); //файл загрузчика есть на диске

	if (f_file_size(loader_file_name) < 10 * 1024) loader_file_exists = false;



	if (!loader_download && !loader_file_exists) {
		ulog("check_loader: error, not loader"CRLF);
		return false;
	}

	ulog_fmt("check_loader: loader found in_file:%s in_memory:%s"CRLF, bool2str(loader_file_exists), bool2str(loader_download));


	if (!loader_file_exists) return loader_download; //если нет файла врзвращаем



	if (loader_download) { //если загрузчик прошит в памяти
		f_file_size2(loader_file_name, &loader_file_size); //размер файла загручика
#ifdef HAL_IWDG_MODULE_ENABLED
		HAL_IWDG_Refresh(&hiwdg);
#endif
		loader_file_crc = f_crc(loader_file_name); //кс файла загручика
#ifdef HAL_IWDG_MODULE_ENABLED
		HAL_IWDG_Refresh(&hiwdg);
#endif
		mem_crc = ulCrcSum((unsigned char *)loader_addr, loader_file_size); //кс взагрузчика в памяти
		ulog_fmt("check_loader: loader file_size:%s file_crc:%s  mem_crc:%s"CRLF, ull2str(loader_file_size), ull2str(loader_file_crc), ull2str(mem_crc));
		if (mem_crc == loader_file_crc) return true; //прошит тот же файл
	}


	ulog_fmt("check_loader: loader upgrade start"CRLF);
#ifdef HAL_IWDG_MODULE_ENABLED
	HAL_IWDG_Refresh(&hiwdg);
#endif
	loader_download = f_erase_and_copy_to_flash(loader_file_name, loader_addr, stm32flashtype);
#ifdef HAL_IWDG_MODULE_ENABLED
	HAL_IWDG_Refresh(&hiwdg);
#endif
	ulog_fmt("check_loader: loader upgrade finish, res:%s"CRLF, bool2str(loader_download));

	return loader_download;
}


void go_to_upgrade(const char* upgrade_file_name, const char* loader_file_name, uint32_t loader_addr, uint8_t stm32flashtype)
{

	FSIZE_t upgrade_file_size;
	uint64_t upgrade_file_crc, mem_crc;
	uint8_t usbd_init;


	if (f_exists(UPGRADE_FILE)) f_unlink(UPGRADE_FILE);

	if (!f_exists(upgrade_file_name)) return;

	if (f_file_size2(upgrade_file_name, &upgrade_file_size) != FR_OK) {
		ulog_fmt("go_to_upgrade: file %s size is incorrect"CRLF, upgrade_file_name);
	}

	if (upgrade_file_size < 10 * 1024) {
		ulog_fmt("go_to_upgrade: error file %s size"CRLF, upgrade_file_name);
		return;
	}


	upgrade_file_crc = f_crc(upgrade_file_name);
#ifdef HAL_IWDG_MODULE_ENABLED
	HAL_IWDG_Refresh(&hiwdg);
#endif
	mem_crc = ulCrcSum((unsigned char *)FLASH_BASE, upgrade_file_size);
	if (mem_crc == upgrade_file_crc) { //прошит тот же файл
		ulog_fmt("go_to_upgrade: firmware in stm32 flash and upgrade file %s is equal"CRLF, upgrade_file_name);
		return;
	}

	ulog_fmt("go_to_upgrade: firmware upgrade file %s found"CRLF, upgrade_file_name);



	if (!check_loader(loader_file_name, loader_addr, stm32flashtype)) {
		ulog("go_to_upgrade: loader not in stm32 flash"CRLF);
		return;
	}

	usbd_init = disk_write_get_forbidden();

	disk_write_set_forbidden(0);

	if (f_copy(upgrade_file_name, UPGRADE_FILE) != FR_OK) {
		disk_write_set_forbidden(usbd_init);
		ulog_fmt("go_to_upgrade: copy file %s to %s error"CRLF, upgrade_file_name, UPGRADE_FILE);
		return;
	}


	ulog_fmt("go_to_upgrade: copy file %s to %s ok"CRLF, upgrade_file_name, UPGRADE_FILE);

	ulog_fmt("go_to_upgrade: delete file %s"CRLF, upgrade_file_name);
	f_unlink(upgrade_file_name);

	disk_write_set_forbidden(usbd_init);

	ulog("go_to_upgrade: deinit and go to loader"CRLF);
	deinit();
	go_to_application(loader_addr);
}


bool upgrade(const char * upgrade_file_name, uint8_t stm32flashtype, uint32_t addr)
{
	FSIZE_t upgrade_file_size;
	bool mem2tmp, erase_and_copy;

	if (!f_exists(upgrade_file_name)) {
		ulog_fmt("upgrade: file %s not found"CRLF, upgrade_file_name);
		return false;
	}

	if (f_file_size2(upgrade_file_name, &upgrade_file_size) != FR_OK) {
		f_unlink(upgrade_file_name);
		ulog_fmt("upgrade: file %s size is incorrect"CRLF, upgrade_file_name);
	}

	if (upgrade_file_size <  10 *1024) {
		f_unlink(upgrade_file_name);
		ulog_fmt("upgrade: file size %lu not correct"CRLF, upgrade_file_size);
		return false;
	}



#ifdef HAL_IWDG_MODULE_ENABLED
	HAL_IWDG_Refresh(&hiwdg);
#endif

	mem2tmp = f_mem2file((void *)addr, upgrade_file_size, "upgrade."TMP_FILE_EXT); //копируем прогамму в файл tmp

#ifdef HAL_IWDG_MODULE_ENABLED
	HAL_IWDG_Refresh(&hiwdg);
#endif


	ulog_fmt("upgrade: file %s begin"CRLF, upgrade_file_name);
	erase_and_copy = f_erase_and_copy_to_flash(upgrade_file_name, addr, stm32flashtype);
	ulog_fmt("upgrade: file %s end, res=%s"CRLF, upgrade_file_name, bool2str(erase_and_copy));

#ifdef HAL_IWDG_MODULE_ENABLED
	HAL_IWDG_Refresh(&hiwdg);
#endif

	if (!erase_and_copy) {
		if (mem2tmp) {
			ulog("upgrade: error, recovery"CRLF);
			erase_and_copy = f_erase_and_copy_to_flash("upgrade."TMP_FILE_EXT, addr, stm32flashtype);
#ifdef HAL_IWDG_MODULE_ENABLED
			HAL_IWDG_Refresh(&hiwdg);
#endif
		}
	}

	f_unlink("upgrade."TMP_FILE_EXT);
	f_unlink(upgrade_file_name);

	return erase_and_copy;
}

/*
bool upgrade2(const char * upgrade_file_name, uint8_t stm32flashtype, uint32_t addr)
{
	FSIZE_t upgrade_file_size;
	bool mem2tmp, erase_and_copy;
	uint64_t file_crc, mem_crc;

	if (!f_exists(upgrade_file_name)) {
		ulog_fmt("upgrade: file %s not found"CRLF, upgrade_file_name);
		return FALSE;
	}

	f_file_size2(upgrade_file_name, &upgrade_file_size); //размер файла обновления
	if (upgrade_file_size <  10 *1024) {
		ulog_fmt("upgrade: file %s size %lu not correct"CRLF, upgrade_file_name, upgrade_file_size);
		return FALSE;
	}


#ifdef HAL_IWDG_MODULE_ENABLED
	HAL_IWDG_Refresh(&hiwdg);
#endif

	file_crc = f_crc(upgrade_file_name);
	mem_crc = ulCrcSum((unsigned char *)addr, upgrade_file_size); //кс файла в памяти

	if (file_crc == mem_crc) {
		ulog_fmt("upgrade: file %s and firware in stm32 flash is equals"CRLF, upgrade_file_name);
		return FALSE;
	}

#ifdef HAL_IWDG_MODULE_ENABLED
	HAL_IWDG_Refresh(&hiwdg);
#endif

	mem2tmp = f_mem2file((void *)addr, upgrade_file_size, "tmp");

#ifdef HAL_IWDG_MODULE_ENABLED
	HAL_IWDG_Refresh(&hiwdg);
#endif


	ulog_fmt("upgrade: file %s begin"CRLF, upgrade_file_name);
	erase_and_copy = f_erase_and_copy_to_flash(upgrade_file_name, addr, stm32flashtype);
	ulog_fmt("upgrade: file %s end, res=%s"CRLF, upgrade_file_name, bool2str(erase_and_copy));

#ifdef HAL_IWDG_MODULE_ENABLED
	HAL_IWDG_Refresh(&hiwdg);
#endif

	if (!erase_and_copy) {
		if (mem2tmp) {
			ulog("upgrade: error, recovery"CRLF);
			erase_and_copy = f_erase_and_copy_to_flash("tmp", addr, stm32flashtype);
#ifdef HAL_IWDG_MODULE_ENABLED
			HAL_IWDG_Refresh(&hiwdg);
#endif
		}
	}

	f_unlink("tmp");


	return erase_and_copy;
}
*/
