#include "7b7dxieyi.h"

static uint32_t CURRENT_FLASH_ADDR = W25Qx_ADDR_IAP;
static uint32_t addrEnd,WhitchBegin;

void  CMD_7b7d_0xe0(comm_state_t * r)
{
	#if 0
    uint8_t Op = r->tempbuf[_OP_TYPE];
    if(Op != 0x55)
        return;

    uint16_t Len;
    uint8_t *datapt;
    datapt = &(r->tempbuf[_DATA_PTR]);
    Len = (r->tempbuf[_DATA_LEN] << 8) + (r->tempbuf[_DATA_LEN + 1]) - 2;
    
    API_USART2.SendData(datapt,Len);
    #endif
}//
//

void CMD_7b7d_0xe1(comm_state_t * r2)
{
	#if 0
    uint8_t Op = r2->tempbuf[_OP_TYPE];
    if(Op != 0x55)
        return;

    uint8_t CtrlSun = r2->tempbuf[_CTRL_SUN];
    
    switch(CtrlSun)
    {
    case  0x00://
        break;
    case  0x01://开始ISP下载.需要设置波特率.boot0 .一个板子最多开始3次报错
    {
        API_USART2.Set_Bande(1200);
        LL_GPIO_SetOutputPin(GPIOB, PCB_BOOT0_Pin);
        delay_ms(1);
        LL_GPIO_ResetOutputPin(GPIOB, PCB_RESET_Pin);
        delay_ms(10);
        LL_GPIO_SetOutputPin(GPIOB, PCB_RESET_Pin);
        delay_ms(1);
        
        r2->tempbuf[_CTRL_SUN   ] = 0x00;
        r2->tempbuf[_CTRL_SUN +1] = 0xaa;
        r2->tempbuf[_DATA_FLAG  ] = 0x00;
        r2->tempbuf[_DATA_FLAG+1] = 0x00;
        r2->ucSendLen = 8;
        memcpy(r2->ucSend,r2->tempbuf,r2->ucSendLen);
        API_USART1.giveCRCEnd(r2->ucSend,&(r2->ucSendLen));
        API_USART2.Usart2ToUsart1(r2);
    }
        return;
    case  0x02://
        break;
    default:
        break;
    }
	#endif
}//
//

static void CMD_7b7d_0xb0_0x00(comm_state_t * r1)
{
	uint16_t flag = ((r1->ucBuf[_DATA_FLAG]) << 8 ) + (r1->ucBuf[_DATA_FLAG+1]);
	uint8_t *dataptr = &(r1->ucBuf[_DATA_PTR]);
	uint16_t datalen = ((r1->ucBuf[_DATA_LEN]) << 8 ) + (r1->ucBuf[_DATA_LEN+1]);
	
	if(flag == 0x0001) //握手
	{
		CURRENT_FLASH_ADDR = WhitchBegin;
		
		r1->ucBuf[_OP_TYPE] = 0xaa;
		r1->ucBuf[_DATA_FLAG] = 0x00;
		r1->ucBuf[_DATA_FLAG+1] = 0x02;
		
		r1->ucBuf[_DATA_LEN] = 0x00;
		r1->ucBuf[_DATA_LEN+1] = 0x02;
		
		r1->ucBufLen = _DATA_FLAG+2;
		API_USART1.giveCRCEnd(r1->ucBuf,&(r1->ucBufLen));
		
		API_USART1.send(r1);
		
		return;
	}
	
	if(flag == 0x0002) //收到数据
	{
		if((CURRENT_FLASH_ADDR + datalen) < addrEnd) //规定数据只能卸载规定的位置
		{
			if(FLASH_OK == API_W25Q128.Write(dataptr,CURRENT_FLASH_ADDR,datalen))
			{
				CURRENT_FLASH_ADDR += datalen;
				
				r1->ucBuf[_OP_TYPE] = 0xaa;
				r1->ucBuf[_DATA_FLAG] = 0x00;
				r1->ucBuf[_DATA_FLAG+1] = 0x02;
				
				r1->ucBuf[_DATA_LEN] = 0x00;
				r1->ucBuf[_DATA_LEN+1] = 0x02;
				
				r1->ucBufLen = _DATA_FLAG+2;
				API_USART1.giveCRCEnd(r1->ucBuf,&(r1->ucBufLen));
				
				API_USART1.send(r1);
				
				return;
			}			
		}
		
		r1->ucBuf[_OP_TYPE] = 0xaa;
		r1->ucBuf[_DATA_FLAG] = 0x00;
		r1->ucBuf[_DATA_FLAG+1] = 0x04;
		
		r1->ucBuf[_DATA_LEN] = 0x00;
		r1->ucBuf[_DATA_LEN+1] = 0x02;
		
		r1->ucBufLen = _DATA_FLAG+2;
		API_USART1.giveCRCEnd(r1->ucBuf,&(r1->ucBufLen));
		
		API_USART1.send(r1);
		
		return;
		
	}
	
	if(flag == 0x0004) //是最后一包数据
	{
		if((CURRENT_FLASH_ADDR + datalen) < addrEnd) //规定数据只能卸载规定的位置
		{
			if(FLASH_OK == API_W25Q128.Write(dataptr,CURRENT_FLASH_ADDR,datalen))
			{
				CURRENT_FLASH_ADDR += datalen;
								
				if(r1->ucBuf[_CTRL_SUN] == 0)
					sys_parameter.ispDATA_LEN = CURRENT_FLASH_ADDR - W25Qx_ADDR_IAP;
				else if(r1->ucBuf[_CTRL_SUN]  ==  1)
					sys_parameter.ispDATA_LEN = CURRENT_FLASH_ADDR - W25Qx_ADDR_APP;
				parameter_save();
				r1->ucBuf[_OP_TYPE] = 0xaa;
				r1->ucBuf[_DATA_FLAG] = 0xff;
				r1->ucBuf[_DATA_FLAG+1] = 0xf0;
				
				r1->ucBuf[_DATA_LEN] = 0x00;
				r1->ucBuf[_DATA_LEN+1] = 0x02;
				
				r1->ucBufLen = _DATA_FLAG+2;
				API_USART1.giveCRCEnd(r1->ucBuf,&(r1->ucBufLen));
				
				API_USART1.send(r1);
				
				return;
			}			
		}
		
		r1->ucBuf[_OP_TYPE] = 0xaa;
		r1->ucBuf[_DATA_FLAG] = 0xff;
		r1->ucBuf[_DATA_FLAG+1] = 0xf1;
		
		r1->ucBuf[_DATA_LEN] = 0x00;
		r1->ucBuf[_DATA_LEN+1] = 0x02;
		
		r1->ucBufLen = _DATA_FLAG+2;
		API_USART1.giveCRCEnd(r1->ucBuf,&(r1->ucBufLen));
		
		API_USART1.send(r1);
		
		return;
	}
	
}//
//

void CMD_7b7d_0xb0(comm_state_t * r1)
{
    uint8_t Op = r1->ucBuf[_OP_TYPE];
    if(Op != 0x55)
        return;

    uint8_t CtrlSun = r1->ucBuf[_CTRL_SUN];
    
    switch(CtrlSun)
    {
    case  0x00://
	{
		WhitchBegin = W25Qx_ADDR_IAP;
		addrEnd = W25Qx_END_IAP;
		CMD_7b7d_0xb0_0x00(r1);
	}
        break;
    case  0x01://
    {
		WhitchBegin = W25Qx_ADDR_APP;
		addrEnd = W25Qx_END_APP;
		CMD_7b7d_0xb0_0x00(r1);
    }
        return;
    case  0x02://
        break;
    default:
        break;
    }
}//
//

