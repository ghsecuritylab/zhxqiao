#include "W25Q128FVSIGTR.H"

/******************************************************************************
QSPI_flash 空间分配
**最后1M前面16K用于存放一些参数,
**16-48k用于存放boot程序(待测PCB板子的)
**48K-512k用于存放PCB板子的APP程序(升级程序)
**512K 用1K保存各种参数
**513K-结尾 保留
**前面10M用于fat文件系统
**接着4M用于字库
**地址14M-(15M-1) 保留
*******************************************************************************/

/* USER CODE BEGIN PV */

/* Private variables ---------------------------------------------------------*/
uint8_t FLASH_sector_buf[0x1000];//4K    为了适用于flash操作
/* USER CODE END PV */


static _FLASH_MEMORY_STATUS_ W25QXX_Wait_Busy(void);
//static _FLASH_MEMORY_STATUS_ W25QXX_Write_Disable(void);
static _FLASH_MEMORY_STATUS_ W25QXX_Write_Enable(void);


#if 0
//擦除整个芯片		  
//等待时间超长...
static _FLASH_MEMORY_STATUS_ W25QXX_Erase_Chip(void)   
{                            
	uint8_t cmd[4];
	cmd[0] = W25X_ChipErase;
    W25QXX_Write_Enable();                  //SET WEL 
    W25QXX_Wait_Busy();   
  	W25Qx_Enable();                            //使能器件   
    SPI1_Transmit(cmd, 1, W25Qx_TIMEOUT_VALUE);        //发送片擦除命令  
	W25Qx_Disable();                            //取消片选     	      
	W25QXX_Wait_Busy();   				   //等待芯片擦除结束
	return FLASH_OK;
}   //
#endif
#if 0
//进入掉电模式
static _FLASH_MEMORY_STATUS_ W25QXX_PowerDown(void)   
{                             
	uint8_t cmd[4];
	cmd[0] = W25X_PowerDown;
  	W25Qx_Enable();                            //使能器件   
    SPI1_Transmit(cmd, 1, W25Qx_TIMEOUT_VALUE);        //发送掉电命令  
	W25Qx_Disable();                            //取消片选     	      
    delay_us(3);                               //等待TPD  
	return FLASH_OK;
}   
#endif
//唤醒
static _FLASH_MEMORY_STATUS_ W25QXX_WAKEUP(void)   
{                              
	uint8_t cmd[4];
	cmd[0] = W25X_ReleasePowerDown;
  	W25Qx_Enable();                            //使能器件   
    SPI1_Transmit(cmd, 1, W25Qx_TIMEOUT_VALUE);   //  send W25X_PowerDown command 0xAB    
	W25Qx_Disable();                            //取消片选     	      
    delay_us(3);                               //等待TRES1
	return FLASH_OK;
}   

//
static uint8_t BSP_W25Qx_GetStatus(void)
{
    uint8_t byte=W25Qx_IDLE;   
    uint8_t a=W25X_ReadStatusReg;
    W25Qx_Enable();                           //使能器件   
    SPI1_Transmit(&a, 1, W25Qx_TIMEOUT_VALUE);//SPI1_ReadWriteByte(W25X_ReadStatusReg);    //发送读取状态寄存器命令    
    SPI1_Receive(&byte,1,W25Qx_TIMEOUT_VALUE);//SPI1_ReadWriteByte(0Xff);             //读取一个字节  
    W25Qx_Disable();                           //取消片选   
    if(byte & 0x01)
        byte = W25Qx_BUSY;
    return byte; 
}//
//

static _FLASH_MEMORY_STATUS_ W25QXX_Wait_Busy(void)
{
	uint32_t tickstart = rt_tick_get();
	
	while(BSP_W25Qx_GetStatus() == W25Qx_BUSY)
    {
        /* Check for the Timeout */
        if((rt_tick_get() - tickstart) > 500)
        {        
            return FLASH_TIMEOUT;
        }
    }
    return FLASH_OK;
	
}//

/**
  * @brief  Erases the specified block of the QSPI memory. 
  * @param  BlockAddress: Block address to erase  
  * @retval QSPI memory status
  */
static _FLASH_MEMORY_STATUS_ BSP_W25Qx_Erase_Block(uint32_t Address)
{
    uint8_t cmd[4];
    uint32_t tickstart = rt_tick_get();
    cmd[0] = W25X_SectorErase;
    cmd[1] = (uint8_t)(Address >> 16);
    cmd[2] = (uint8_t)(Address >> 8);
    cmd[3] = (uint8_t)(Address);
 
    /* Enable write operations */
//    BSP_W25Qx_WriteEnable();
	W25QXX_Write_Enable();
 
    /*Select the FLASH: Chip Select low */
    W25Qx_Enable();
    /* Send the read ID command */
    SPI1_Transmit(cmd, 4, W25Qx_TIMEOUT_VALUE);    
    /*Deselect the FLASH: Chip Select high */
    W25Qx_Disable();
 
    /* Wait the end of Flash writing */
    return W25QXX_Wait_Busy();
}//
//

static _FLASH_MEMORY_STATUS_ W25QXX_Write_Enable(void)
{
	uint8_t cmd[4];
    cmd[0] = W25X_WriteEnable;
	
	W25Qx_Enable();
	
	if(0 != SPI1_Transmit(cmd, 1, W25Qx_TIMEOUT_VALUE))
        return FLASH_ERROR; 
	
    W25Qx_Disable();
	
	return FLASH_OK;
}//
#if	0
static _FLASH_MEMORY_STATUS_ W25QXX_Write_Disable(void)
{
	uint8_t cmd[4];
    cmd[0] = W25X_WriteDisable;
	
	W25Qx_Enable();
	
	if(0 != SPI1_Transmit(cmd, 1, W25Qx_TIMEOUT_VALUE))
        return FLASH_ERROR; 
	
    W25Qx_Disable();
	
	return FLASH_OK;
}//
#endif
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pData:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
static _FLASH_MEMORY_STATUS_ W25QXX_Write_Page(uint8_t* pData,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
    uint8_t cmd[4];
    uint32_t tickstart = rt_tick_get();
    cmd[0] = W25X_PageProgram;
    cmd[1] = (uint8_t)(WriteAddr >> 16);
    cmd[2] = (uint8_t)(WriteAddr >> 8);
    cmd[3] = (uint8_t)(WriteAddr);
     
	W25QXX_Write_Enable();//使能写
	
    W25Qx_Enable();
    
    if(0 != SPI1_Transmit(cmd, 4, W25Qx_TIMEOUT_VALUE))
        return FLASH_ERROR; 
	
    if(0 != SPI1_Transmit(pData, NumByteToWrite, W25Qx_TIMEOUT_VALUE))
        return FLASH_ERROR; 
	
    W25Qx_Disable();
    
    /* Wait the end of Flash writing */
	return W25QXX_Wait_Busy();
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pData:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
static _FLASH_MEMORY_STATUS_ W25QXX_Write_NoCheck(uint8_t* pData,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
    uint16_t pageremain;	  
    _FLASH_MEMORY_STATUS_ STATUS;
        
    pageremain = 256 - WriteAddr % 256; //单页剩余的字节数		 	    
    if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
    while(1)
    {	   
        STATUS = W25QXX_Write_Page(pData,WriteAddr,pageremain);
        
        if(STATUS != FLASH_OK)
            return STATUS;
        
        if(NumByteToWrite==pageremain)break;//写入结束了
        else //NumByteToWrite>pageremain
        {
                pData+=pageremain;
                WriteAddr+=pageremain;	

                NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
                if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
                else pageremain=NumByteToWrite; 	  //不够256个字节了
        }
    };	
    
    return FLASH_OK;
} 

/**
  * @brief  Read Manufacture/Device ID.
  * @param  return value address
  * @retval None
  */
static void BSP_W25Qx_Read_ID(uint8_t *ID)
{
    uint8_t cmd[4] = {W25X_ManufactDeviceID,0x00,0x00,0x00};

    W25Qx_Enable();

    /* Send the read ID command */

    SPI1_Transmit(cmd, 4, W25Qx_TIMEOUT_VALUE);   

    /* Reception of the data */

    SPI1_Receive(ID, 2, W25Qx_TIMEOUT_VALUE);

    W25Qx_Disable();
}//测试通过
//

/**
  * @brief  Reads an amount of data from the QSPI memory.
  * @param  pData: Pointer to data to be read
  * @param  ReadAddr: Read start address
  * @param  Size: Size of data to read    
  * @retval QSPI memory status
  */
static _FLASH_MEMORY_STATUS_ BSP_W25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint16_t Size)
{
    uint8_t cmd[4];
 
    /* Configure the command */
    cmd[0] = W25X_ReadData;
    cmd[1] = (uint8_t)(ReadAddr >> 16);
    cmd[2] = (uint8_t)(ReadAddr >> 8);
    cmd[3] = (uint8_t)(ReadAddr);
     
    W25Qx_Enable();
    /* Send the read ID command */
    SPI1_Transmit(cmd, 4, W25Qx_TIMEOUT_VALUE);  
    /* Reception of the data */
    if (SPI1_Receive(pData,Size,W25Qx_TIMEOUT_VALUE) != 0)
  {
    return FLASH_ERROR;
  }
    W25Qx_Disable();
    return FLASH_OK;
}//测试好用

/**
  * @brief  Writes an amount of data to the QSPI memory,可以写入任意长度的数据
  * 带自动擦除功能,与其他flash操作共用一个4k的buffer.
  * @param  pData: Pointer to data to be written
  * @param  WriteAddr: Write start address
  * @param  Size: Size of data to write,No more than 0XFFFFFFFFbyte.     
  * @retval QSPI memory status
  */
static _FLASH_MEMORY_STATUS_ BSP_W25Qx_Write(void* pDa, uint32_t WriteAddr, uint16_t Size)
{
    uint8_t *W25Q128_Buffer=&FLASH_sector_buf[0];
    uint32_t current_addr;
    uint32_t tickstart = rt_tick_get(),i;
    uint32_t secpos,secoff,writeSizeInSector,secremain;
	uint8_t * pData = pDa;
    
    if((Size == 0) || (pData == NULL) || ((WriteAddr + Size) >= W25Qx_ALL_ROOM))
        return FLASH_ERROR;
            
    current_addr = WriteAddr;
    writeSizeInSector = Size;
    
    do
    {
        //在当前扇区中的位置,直接作为buf的下标
        secoff = current_addr % W25Qx_SECTOR_SIZE;
        
        //当前要擦除写入的扇区编号
        secpos = current_addr / W25Qx_SECTOR_SIZE;
        
        //扇区剩余空间大小
        secremain = W25Qx_SECTOR_SIZE - secoff;
        
        if(writeSizeInSector<=secremain) 
        {
            secremain=writeSizeInSector;//不大于4096个字节
            writeSizeInSector = 0;
        }else
        writeSizeInSector -= secremain;
            
        _FLASH_MEMORY_STATUS_ w25q_status;
        w25q_status = BSP_W25Qx_Read(W25Q128_Buffer,(secpos * W25Qx_SECTOR_SIZE),W25Qx_SECTOR_SIZE);
        if(w25q_status != FLASH_OK)
            return w25q_status;
    
        for(i=0;i<secremain;i++)//校验数据
        {
            if(W25Q128_Buffer[secoff+i]!=0XFF)break;//需要擦除  	  
        }
        
        if(i<secremain)//需要擦除
        {
            //当前扇区的数据读取出来了,擦除当前扇区
            w25q_status = BSP_W25Qx_Erase_Block(secpos);
            if(w25q_status != FLASH_OK)
                return w25q_status;
            
            for(i=0;i<secremain;i++)	   //复制
            {
                    W25Q128_Buffer[i+secoff]=pData[i];	  
            }
            w25q_status = W25QXX_Write_NoCheck(W25Q128_Buffer,(secpos * W25Qx_SECTOR_SIZE),W25Qx_SECTOR_SIZE);//写入整个扇区  

        }else w25q_status = W25QXX_Write_NoCheck(pData,current_addr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 	
        
        if(w25q_status != FLASH_OK)
            return w25q_status;
           
    } while (writeSizeInSector > 0);
 
    return FLASH_OK;
}//
//

#if TEST == 1
static void test(void)
{
    /* USER CODE BEGIN 2 */
//    API_SPI1.open();
    printf("\r\n SPI-W25Qxxx Example \r\n\r\n");
    
    


//  SPI1->CR1 |=  0x0040;//开启SPI
//  uint8_t a = 0x66,b=0x99;
//   W25Qx_Enable();
//    SPI1_Transmit(&a, 1, W25Qx_TIMEOUT_VALUE); 
//    W25Qx_Disable();
//     W25Qx_Enable();
//    SPI1_Transmit(&b, 1, W25Qx_TIMEOUT_VALUE);
//    W25Qx_Disable();
    
  /*##-1- Read the device ID  ########################*/ 
    uint8_t ID[4]={0x00};
//  BSP_W25Qx_Init();//发送使能重启（66H）和使能重启设备(99H)命令初始化flash
    BSP_W25Qx_Read_ID(ID);
    printf(" W25Qxxx ID is : 0x%02X 0x%02X 0x%02X 0x%02X \r\n\r\n",ID[0],ID[1],ID[2],ID[3]);
   
  
   
  /*##-3- Written to the flash ########################*/ 
  /* fill buffer */
    uint8_t wData[0x100],rData[0x100];
    uint16_t i;
  for(i =0;i<0x100;i ++)
  {
          wData[i] = i;
  }
   
  if(BSP_W25Qx_Write(wData,0x100,0x100)== FLASH_OK)
      printf(" SPI Write ok\r\n");
  else
      printf(" SPI Write error\r\n");
    

  /*##-4- Read the flash     ########################*/ 
  if(BSP_W25Qx_Read(rData,0x100,0x100)== FLASH_OK)
      printf(" SPI Read ok\r\n\r\n");
//  else
//      Error_Handler();
   
  printf("SPI Read Data : \r\n");
  for(i =0;i<0x100;i++)
      printf("0x%02X  ",rData[i]);
  printf("\r\n\r\n");
   
  /*##-5- check date          ########################*/    
//  if(memcmp(wData,rData,0x100) == 0 ) 
//      printf(" W25Q128FV SPI Test OK\r\n");
//  else
//      printf(" W25Q128FV SPI Test False\r\n");
/* USER CODE END 2 */
}//
//
#endif





/******************************************************************************/
const struct  _apiw25qx API_W25Q128 = 
{
#if TEST == 1
    test,
#endif
	BSP_W25Qx_Write,
	BSP_W25Qx_Read,
	BSP_W25Qx_Read_ID,
	W25QXX_WAKEUP
};









