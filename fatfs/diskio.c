/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#include "flash_io.h"
#include "stm_util.h"



/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

static uint8_t disk_write_forbidden = 0;
static uint8_t disk_write_forbidden_old = 0;

void disk_write_set_forbidden(uint8_t val)
{
	disk_write_forbidden = val;
}

uint8_t disk_write_get_forbidden(void)
{
	return disk_write_forbidden;
}

void disk_write_push_forbidden(void)
{
	disk_write_forbidden_old = disk_write_forbidden;
	disk_write_forbidden = 0;
}

void disk_write_pop_forbidden(void)
{
	disk_write_forbidden = disk_write_forbidden_old;
}


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
/*
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		result = RAM_disk_status();

		// translate the reslut code here

		return stat;

	case DEV_MMC :
		result = MMC_disk_status();

		// translate the reslut code here

		return stat;

	case DEV_USB :
		result = USB_disk_status();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
*/
    //ulog_fmt("disk_status pdrv=%d\r\n", pdrv);
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
/*
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		result = RAM_disk_initialize();

		// translate the reslut code here

		return stat;

	case DEV_MMC :
		result = MMC_disk_initialize();

		// translate the reslut code here

		return stat;

	case DEV_USB :
		result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
*/
	return flash_init() ? RES_OK : RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
/*
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		result = RAM_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

		result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
*/
	return flash_read(buff, sector, count) ? RES_OK : RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
/*
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		result = RAM_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

		result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
*/
    //ulog_fmt("disk_write pdrv=%d sector=%d count=%d\r\n", pdrv, sector, count);

//    if (pdrv != 0) return RES_ERROR;

	if (disk_write_forbidden) return RES_ERROR;

	return flash_write((uint8_t *)buff, sector, count) ? RES_OK : RES_ERROR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
/*
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :

		// Process of the command for the RAM drive

		return res;

	case DEV_MMC :

		// Process of the command for the MMC/SD card

		return res;

	case DEV_USB :

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
*/
    //ulog_fmt("disk_ioctl pdrv=%d cmd=%d\r\n", pdrv, cmd);

    //DRESULT res = RES_ERROR;
    //if(pdrv!=0)   return res;



    switch(cmd)
    {
    	case CTRL_SYNC:
    		return RES_OK;
    	case GET_SECTOR_COUNT:
    		*(uint32_t*)buff = flash_sector_count();
    		return RES_OK;
    	case GET_SECTOR_SIZE:
    		*(uint16_t*)buff = flash_sector_size();
    		return RES_OK;
    	case GET_BLOCK_SIZE:
    		*(uint32_t*)buff= flash_block_size();
    		return RES_OK;
    }

    return RES_PARERR;
}

