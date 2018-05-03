/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "users.h"


#define SD_CA			0
#define W25Q128_FLASH	1

/* Definitions of physical drive number for each drive */
#define DEV_RAM		SD_CA	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		W25Q128_FLASH	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	uint16_t id;

	switch (pdrv) {
	case DEV_RAM :
//		result = RAM_disk_status();

		// translate the reslut code here

		stat = STA_NOINIT;
	break;
	case DEV_MMC :
//		result = MMC_disk_status();

		// translate the reslut code here
		API_W25Q128.Read_ID((uint8_t*)&id);
	if(id == 0x17ef)
		stat = 0;
	else
		stat = STA_NOINIT;
	break;

	case DEV_USB :
//		result = USB_disk_status();

		// translate the reslut code here

		stat = STA_NOINIT;
	break;
	default :
		stat = STA_NOINIT;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	uint16_t id;

	switch (pdrv) {
	case DEV_RAM :

		stat = STA_NOINIT;
	break;
	case DEV_MMC :
		SPI_Config();
		API_W25Q128.Wakeup();
		API_W25Q128.Read_ID((uint8_t*)&id);
	if(id == 0x17ef)
		stat = 0;
	else
		stat = STA_NOINIT;
	break;

	case DEV_USB :

		stat = STA_NOINIT;
	break;
	default :
		stat = STA_NOINIT;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT stat;
	_FLASH_MEMORY_STATUS_ a=FLASH_ERROR;
	
	if(!count) return RES_ERROR;

	switch (pdrv) {
	case DEV_RAM :
		stat = RES_ERROR;
	break;
	case DEV_MMC :
		for(UINT i=0;i<count;i++)
		{
			a = API_W25Q128.Read(buff,sector<<12,4096);
			buff += 4096;
			sector ++;
		}
	if(a == FLASH_OK)
		stat = RES_OK;
	else
		stat = RES_ERROR;
	break;

	case DEV_USB :

		stat = RES_ERROR;
	break;
	default :
		stat = RES_ERROR;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT stat;
	_FLASH_MEMORY_STATUS_ a = FLASH_ERROR;

	
	if(!count) return RES_ERROR;
	
	switch (pdrv) {
	case DEV_RAM :

		stat = RES_ERROR;
	break;
	case DEV_MMC :
		for(UINT i=0;i<count;i++)
		{
			a = API_W25Q128.Write((uint8_t*)buff,sector<<12,4096);
			buff += 4096;
			sector ++;
		}
	if(a == FLASH_OK)
		stat = RES_OK;
	else
		stat = RES_ERROR;
	break;

	case DEV_USB :

		stat = RES_ERROR;
	break;
	default :
		stat = RES_ERROR;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_ERROR;

    switch(pdrv)
    {
    case      DEV_RAM:
        res =  RES_NOTRDY; 
      break;
    case      DEV_MMC:
        switch(cmd)
        {
        case        CTRL_SYNC:
			
            break;
        case        GET_SECTOR_COUNT:
            *(DWORD*)buff = 2560;
            res = RES_OK;
            break;
        case        GET_SECTOR_SIZE:
            *(DWORD*)buff = 4096;
            res = RES_OK;
            break;
        case        GET_BLOCK_SIZE:
            *(DWORD*)buff = 1;
            res = RES_OK;
            break;
        default     :
            res =  RES_ERROR; 
        }
      break;
      
	case DEV_USB :
//		result = USB_disk_status();

		// translate the reslut code here

		res = RES_ERROR;
	break;
    default       :
    res =  RES_ERROR; 
    }
    
    return res;
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
__weak DWORD get_fattime (void)
{
  return 0;
}

