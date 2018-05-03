#ifndef _W25Q128FVSIGTR_H_
#define _W25Q128FVSIGTR_H_

#include "users.h"
#include "sys.h"

#define W25Qx_TIMEOUT_VALUE             1000

#define W25Qx_ALL_ROOM                  0x1000000               // w25q128 容量是128/8 = 16M字节
#define W25Qx_SECTOR_SIZE               0x1000                  // 单个扇区的大小是4K字节,擦除最小单位

#define W25Qx_ADDR_15M					0xf00000				// 第15M的首地址	

#define W25Qx_ADDR_IAP					0xf04000				// IAP的首地址	
#define W25Qx_SIZE_IAP					0x008000				// 分配给IAP的数据长度 32K
#define W25Qx_END_IAP					(W25Qx_ADDR_IAP+W25Qx_SIZE_IAP)					// 分配给IAP的数据长度 32K

#define W25Qx_ADDR_APP					0xf0C000				// APP的首地址	
#define W25Qx_SIZE_APP					0x074000				// 分配给APP的数据长度 464K
#define W25Qx_END_APP					(W25Qx_ADDR_APP+W25Qx_SIZE_APP)				// 分配给APP的数据长度 464K

#define W25Qx_SYS_PARA					0xf80000

//extern SPI_HandleTypeDef hspi1;
////////////////////////////////////////////////////////////////////////////////// 
//指令表
#define W25X_WriteEnable		0x06    //
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

#define W25Qx_Enable()  PAout(15) = 0
//
#define W25Qx_Disable() PAout(15) = 1
//

#define W25Qx_ERROR                     0x08
#define W25Qx_OK                        0x00
#define W25Qx_TIMEOUT                   0x08


#define W25Q128FV_PAGE_SIZE             0x100


#define W25Qx_BUSY                      0x08
#define W25Qx_IDLE                      0x03

#define W25Qx_ERROR                     0x08
#define W25Qx_ERROR                     0x08
#define W25Qx_ERROR                     0x08
#define W25Qx_ERROR                     0x08
#define W25Qx_ERROR                     0x08
#define W25Qx_ERROR                     0x08
#define W25Qx_ERROR                     0x08

typedef enum
{
    FLASH_IDLE,
    FLASH_ERROR,
    FLASH_TIMEOUT,
    FLASH_OK
}_FLASH_MEMORY_STATUS_;
//

struct _apiw25qx
{
#if TEST == 1
    void (*test)(void);
#endif
    _FLASH_MEMORY_STATUS_ (*Write)(uint8_t* pData, uint32_t WriteAddr, uint16_t Size);
    _FLASH_MEMORY_STATUS_ (*Read)(uint8_t* pData, uint32_t ReadAddr, uint16_t Size);
    void (*Read_ID)(uint8_t *ID);
	_FLASH_MEMORY_STATUS_ (*Wakeup)(void)   ;
};
extern const struct  _apiw25qx API_W25Q128;



#endif

