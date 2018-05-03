#include "W25Q128FVSIGTR.H"

/******************************************************************************
QSPI_flash �ռ����
**���1Mǰ��16K���ڴ��һЩ����,
**16-48k���ڴ��boot����(����PCB���ӵ�)
**48K-512k���ڴ��PCB���ӵ�APP����(��������)
**512K ��1K������ֲ���
**513K-��β ����
**ǰ��10M����fat�ļ�ϵͳ
**����4M�����ֿ�
**��ַ14M-(15M-1) ����
*******************************************************************************/

/* USER CODE BEGIN PV */

/* Private variables ---------------------------------------------------------*/
uint8_t FLASH_sector_buf[0x1000];//4K    Ϊ��������flash����
/* USER CODE END PV */


static _FLASH_MEMORY_STATUS_ W25QXX_Wait_Busy(void);
//static _FLASH_MEMORY_STATUS_ W25QXX_Write_Disable(void);
static _FLASH_MEMORY_STATUS_ W25QXX_Write_Enable(void);


#if 0
//��������оƬ		  
//�ȴ�ʱ�䳬��...
static _FLASH_MEMORY_STATUS_ W25QXX_Erase_Chip(void)   
{                            
	uint8_t cmd[4];
	cmd[0] = W25X_ChipErase;
    W25QXX_Write_Enable();                  //SET WEL 
    W25QXX_Wait_Busy();   
  	W25Qx_Enable();                            //ʹ������   
    SPI1_Transmit(cmd, 1, W25Qx_TIMEOUT_VALUE);        //����Ƭ��������  
	W25Qx_Disable();                            //ȡ��Ƭѡ     	      
	W25QXX_Wait_Busy();   				   //�ȴ�оƬ��������
	return FLASH_OK;
}   //
#endif
#if 0
//�������ģʽ
static _FLASH_MEMORY_STATUS_ W25QXX_PowerDown(void)   
{                             
	uint8_t cmd[4];
	cmd[0] = W25X_PowerDown;
  	W25Qx_Enable();                            //ʹ������   
    SPI1_Transmit(cmd, 1, W25Qx_TIMEOUT_VALUE);        //���͵�������  
	W25Qx_Disable();                            //ȡ��Ƭѡ     	      
    delay_us(3);                               //�ȴ�TPD  
	return FLASH_OK;
}   
#endif
//����
static _FLASH_MEMORY_STATUS_ W25QXX_WAKEUP(void)   
{                              
	uint8_t cmd[4];
	cmd[0] = W25X_ReleasePowerDown;
  	W25Qx_Enable();                            //ʹ������   
    SPI1_Transmit(cmd, 1, W25Qx_TIMEOUT_VALUE);   //  send W25X_PowerDown command 0xAB    
	W25Qx_Disable();                            //ȡ��Ƭѡ     	      
    delay_us(3);                               //�ȴ�TRES1
	return FLASH_OK;
}   

//
static uint8_t BSP_W25Qx_GetStatus(void)
{
    uint8_t byte=W25Qx_IDLE;   
    uint8_t a=W25X_ReadStatusReg;
    W25Qx_Enable();                           //ʹ������   
    SPI1_Transmit(&a, 1, W25Qx_TIMEOUT_VALUE);//SPI1_ReadWriteByte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
    SPI1_Receive(&byte,1,W25Qx_TIMEOUT_VALUE);//SPI1_ReadWriteByte(0Xff);             //��ȡһ���ֽ�  
    W25Qx_Disable();                           //ȡ��Ƭѡ   
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
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pData:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
static _FLASH_MEMORY_STATUS_ W25QXX_Write_Page(uint8_t* pData,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
    uint8_t cmd[4];
    uint32_t tickstart = rt_tick_get();
    cmd[0] = W25X_PageProgram;
    cmd[1] = (uint8_t)(WriteAddr >> 16);
    cmd[2] = (uint8_t)(WriteAddr >> 8);
    cmd[3] = (uint8_t)(WriteAddr);
     
	W25QXX_Write_Enable();//ʹ��д
	
    W25Qx_Enable();
    
    if(0 != SPI1_Transmit(cmd, 4, W25Qx_TIMEOUT_VALUE))
        return FLASH_ERROR; 
	
    if(0 != SPI1_Transmit(pData, NumByteToWrite, W25Qx_TIMEOUT_VALUE))
        return FLASH_ERROR; 
	
    W25Qx_Disable();
    
    /* Wait the end of Flash writing */
	return W25QXX_Wait_Busy();
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pData:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
static _FLASH_MEMORY_STATUS_ W25QXX_Write_NoCheck(uint8_t* pData,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
    uint16_t pageremain;	  
    _FLASH_MEMORY_STATUS_ STATUS;
        
    pageremain = 256 - WriteAddr % 256; //��ҳʣ����ֽ���		 	    
    if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
    while(1)
    {	   
        STATUS = W25QXX_Write_Page(pData,WriteAddr,pageremain);
        
        if(STATUS != FLASH_OK)
            return STATUS;
        
        if(NumByteToWrite==pageremain)break;//д�������
        else //NumByteToWrite>pageremain
        {
                pData+=pageremain;
                WriteAddr+=pageremain;	

                NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
                if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
                else pageremain=NumByteToWrite; 	  //����256���ֽ���
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
}//����ͨ��
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
}//���Ժ���

/**
  * @brief  Writes an amount of data to the QSPI memory,����д�����ⳤ�ȵ�����
  * ���Զ���������,������flash��������һ��4k��buffer.
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
        //�ڵ�ǰ�����е�λ��,ֱ����Ϊbuf���±�
        secoff = current_addr % W25Qx_SECTOR_SIZE;
        
        //��ǰҪ����д����������
        secpos = current_addr / W25Qx_SECTOR_SIZE;
        
        //����ʣ��ռ��С
        secremain = W25Qx_SECTOR_SIZE - secoff;
        
        if(writeSizeInSector<=secremain) 
        {
            secremain=writeSizeInSector;//������4096���ֽ�
            writeSizeInSector = 0;
        }else
        writeSizeInSector -= secremain;
            
        _FLASH_MEMORY_STATUS_ w25q_status;
        w25q_status = BSP_W25Qx_Read(W25Q128_Buffer,(secpos * W25Qx_SECTOR_SIZE),W25Qx_SECTOR_SIZE);
        if(w25q_status != FLASH_OK)
            return w25q_status;
    
        for(i=0;i<secremain;i++)//У������
        {
            if(W25Q128_Buffer[secoff+i]!=0XFF)break;//��Ҫ����  	  
        }
        
        if(i<secremain)//��Ҫ����
        {
            //��ǰ���������ݶ�ȡ������,������ǰ����
            w25q_status = BSP_W25Qx_Erase_Block(secpos);
            if(w25q_status != FLASH_OK)
                return w25q_status;
            
            for(i=0;i<secremain;i++)	   //����
            {
                    W25Q128_Buffer[i+secoff]=pData[i];	  
            }
            w25q_status = W25QXX_Write_NoCheck(W25Q128_Buffer,(secpos * W25Qx_SECTOR_SIZE),W25Qx_SECTOR_SIZE);//д����������  

        }else w25q_status = W25QXX_Write_NoCheck(pData,current_addr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 	
        
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
    
    


//  SPI1->CR1 |=  0x0040;//����SPI
//  uint8_t a = 0x66,b=0x99;
//   W25Qx_Enable();
//    SPI1_Transmit(&a, 1, W25Qx_TIMEOUT_VALUE); 
//    W25Qx_Disable();
//     W25Qx_Enable();
//    SPI1_Transmit(&b, 1, W25Qx_TIMEOUT_VALUE);
//    W25Qx_Disable();
    
  /*##-1- Read the device ID  ########################*/ 
    uint8_t ID[4]={0x00};
//  BSP_W25Qx_Init();//����ʹ��������66H����ʹ�������豸(99H)�����ʼ��flash
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









