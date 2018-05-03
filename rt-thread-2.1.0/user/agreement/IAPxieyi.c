
#include "iapxieyi.h"

void CMD_IAP_0XD0(comm_state_t * r2)
{
    uint8_t Op = r2->tempbuf[_OP_TYPE];
    if(Op != 0x55)
        return;

    uint8_t CtrlSun = r2->tempbuf[_CTRL_SUN];
    
    switch(CtrlSun)
    {
    case  0x00://
    {
        uint16_t Len;
        uint8_t *datapt;
        datapt = &(r2->tempbuf[_DATA_PTR]);
        Len = (r2->tempbuf[_DATA_LEN] << 8) + (r2->tempbuf[_DATA_LEN + 1]) - 2;
        
        API_USART2.SendData(datapt,Len);
    }
        break;
    case  0x01://开始IAP下载.需要设置波特率.boot0 .一个板子最多开始3次报错
    {
//        API_USART2.Set_Bande(USART2_115200);
//        LL_GPIO_ResetOutputPin(GPIOB, PCB_BOOT0_Pin);
//        delay_ms(1);
//        LL_GPIO_ResetOutputPin(GPIOB, PCB_RESET_Pin);
//        delay_ms(10);
//        LL_GPIO_SetOutputPin(GPIOB, PCB_RESET_Pin);
//        delay_ms(1);
        
        r2->tempbuf[_CTRL_SUN   ] = 0x01;
        r2->tempbuf[_CTRL_SUN +1] = 0xaa;
        r2->tempbuf[_DATA_FLAG  ] = 0x00;
        r2->tempbuf[_DATA_FLAG+1] = 0x01;
        r2->ucSendLen = 8;
        memcpy(r2->ucSend,r2->tempbuf,r2->ucSendLen);
        API_USART1.giveCRCEnd(r2->ucSend,&(r2->ucSendLen));
        API_USART2.Usart2ToUsart1(r2);
    }
        break;
    case  0x02://
        break;
    default:
        break;
    }
    
}//


//
