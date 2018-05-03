#include "usart2_app.h"
#include "usart2_app.h"
//#include "usart.h"


//-------------------------------------------------------------------------------------

//static uint8_t TempBuf[UART2_BUFLEN];
//static uint8_t ucBuf[UART2_BUFLEN];
//static uint8_t ucSend[UART2_BUFLEN];
static uint32_t	isp_wr_addr = 0x08000000;
static uint32_t isp_rd_addr = W25Qx_ADDR_IAP;
static uint32_t iap_rd_addr = W25Qx_ADDR_APP;
#define ISP_ONCE_DOWNLOAD_DATA_LEN		(256)
///*---------------------------------------------------------------------------*/

//static comm_state_t SelfBufState;
/*---------------------------------------------------------------------------*/
#define UCBUFSIZE		120
static uint8_t ucbuf[UCBUFSIZE];
/*---------------------------------------------------------------------------*/

//static comm_state_t art1;
static void Reply(comm_state_t * r);

static comm_state_t art2=
{
	.ucBuf = ucbuf,
	.ucSend = NULL,
	.tempbuf = NULL
};

/*---------------------------------------------------------------------------*/




// 设置串口2波特率
//static void USART2_Set_Bande(uint32_t c)
//{
//	if((c < 300) || (c > 115200))
//		return ;

////	__HAL_RCC_USART2_CLK_DISABLE();
//////	LL_USART_Disable(USART2);
////	HAL_NVIC_DisableIRQ(USART2_IRQn);
//	uint32_t iOVER8,usart2_clk,temp1,temp2,temp3;
//	usart2_clk = 42000000;
//	
//	iOVER8 = USART2->CR1 & 0x00008000;

//	if(iOVER8 > 0)//后面小数部分只有后三位有效
//	{
//		temp1 = c * 8 ;

//		temp2 = usart2_clk / temp1 ;//鏁存暟閮ㄥ垎
//		temp3 = usart2_clk % temp1 ;//灏忔暟閮ㄥ垎

//		USART2->BRR |= (uint32_t)((temp3 * 16)/temp1);
//		USART2->BRR |= (uint32_t)(temp2 << 4);
//		
//		USART2->BRR &= (~(0x00000008)); 
//	}else//小数部分4为全部有效
//	{
//		temp1 = c * 8 * 2;
//		
//		temp2 = usart2_clk / temp1 ;
//		temp3 = usart2_clk % temp1 ;

//		USART2->BRR |= (uint32_t)((temp3 * 16)/temp1);
//		USART2->BRR |= (uint32_t)(temp2 << 4);
//	}
////	HAL_NVIC_DisableIRQ(USART2_IRQn);
//////	LL_USART_Enable(USART2);
////	__HAL_RCC_USART2_CLK_ENABLE();
//}//
//
#if 1
//

//==============================================================================
//| 函数功能 | 初始化
//|----------|------------------------------------------------------------------
//| 输入参数 | 无
//|----------|------------------------------------------------------------------
//| 返回参数 | 无
//==============================================================================
static void Init(void )
{
//    memset(&SelfBufState,0,sizeof(SelfBufState));
//    SelfBufState.tempbuf = TempBuf;
//    SelfBufState.ucBuf   = ucBuf;
//    SelfBufState.ucSend  = ucSend;
//	
////    HAL_UART_Receive_DMA(&huart2, SelfBufState.ucBuf, UART2_BUFLEN);  
////    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);  
}//

//==============================================================================
//| 函数功能 | 检查串口1缓冲区是否有数据.
//|----------|------------------------------------------------------------------
//| 输入参数 | 无
//|----------|------------------------------------------------------------------
//| 返回参数 | 无
//==============================================================================
static void check(void)
{
//    //
//    if(SelfBufState.uart2_rec_uart1_flag == 1)
//    {
//        SelfBufState.uart2_rec_uart1_flag = 0;
//        API_USART2.Reply(&SelfBufState);
//    }else////
//        //
//    if(SelfBufState.uart2_rec_flag == 1)//
//    {
//        SelfBufState.uart2_rec_flag = 0;
//        API_USART2.Reply(&SelfBufState);
//    }
//    //
}//


//==============================================================================
//| 函数功能 | 异或校验
//|----------|------------------------------------------------------------------
//| 输入参数 | a(非0检验  0生成校验码) 
//|----------|------------------------------------------------------------------
//| 返回参数 | 成功返回1 失败返回0
//==============================================================================
static uint16_t Check_Compte(bool a,uint8_t  * r,uint16_t len)
{
	if(a)
	{
		if(len<3) return 0;
		
		uint8_t  data_sum = r[0];
		for(uint16_t i=1;i<len;i++)
		{
			data_sum = data_sum ^ r[i];
		}
		
		if(data_sum == 0)
			return len;
	}
	else
	{
		if(len<2) return 0;
		uint8_t  data_sum = r[0];
		for(uint16_t i=1;i<len;i++)
		{
			data_sum = data_sum ^ r[i];
		}
		r[len] = data_sum;
		
		return 1+len;
	}
	return 0;
}


//==============================================================================
//| 函数功能 | 根据协议跳转到相应功能函数.
//|----------|------------------------------------------------------------------
//| 输入参数 | 无
//|----------|------------------------------------------------------------------
//| 返回参数 | 无
//==============================================================================
static void Reply(comm_state_t * r)
{    
//        if(!(API_USART1.jiaZhuanYi(r,false))) return;

//        if(GetCrc16(&(r->tempbuf[1]),(r->ucBufLen - 2)) != 0) return;
        
        uint8_t myCMD      = r->tempbuf[_CTRL_CODE];
        
        switch(myCMD)
        {
        case  0x01://读命令
            break;
        case  _STMISPCODE://0xe0 ISP烧写协议
            r->mode = ISPMODE;
            CMD_7b7d_0xe0(r);
            return;
        case  _STMISPRESE://0xe1 ISP烧写协议,ISP协议相关操作
            r->mode = ISPMODE;
            CMD_7b7d_0xe1(r);
            break;
        case  _STMIAPCODE://IAP
            r->mode = IAPMODE;
//            CMD_IAP_0XD0(r);
            break;
        case  _STMAPPCODE://APP
            r->mode = APPMODE;
            break;
        default:
            r->mode = NONMODE;
            break;
        }
    
}	//


//DMA发送函数  
//static
//void Usart2SendData_DMA(uint8_t *pdata, uint16_t Length)  
//{  
//    while(SelfBufState.dmasend_flag == USART_DMA_SENDING);  
//    SelfBufState.dmasend_flag = USART_DMA_SENDING;  
//    HAL_UART_Transmit_DMA(&huart2, pdata, Length);  
//}
static void putCh(rt_uint8_t c)
{
	while (!(USART2->SR & USART_FLAG_TXE))__NOP();
		USART2->DR = c ;
}
//
static void SendData(uint8_t *pdata, uint16_t Length)
{
	uint16_t i,j=Length-1;
//    delay_ms(5);
	putCh(_FRAME_START);
	for(i=1;i<j;i++)
	{
		if((_FRAME_ESC == pdata[i])||(_FRAME_START == pdata[i])||(_FRAME_END == pdata[i]))
		{
			putCh( 0x40);
			putCh( pdata[i]^0x40);
		}else{
			putCh( pdata[i]);
		}
		
//		fputc(*pdata++,RT_NULL);
	}
	putCh(_FRAME_END);
////    USART2SendData_DMA(pdata,Length);
}
//
static void send(comm_state_t * r)
{
//    if(uart1_xor40_jia(r,true))
	rt_thread_delay(30);
	SendData(r->ucBuf,r->ucBufLen);
}
//
static void sum_send(comm_state_t * r)
{
	for(uint16_t i=0;i<art2.ucBufLen;i++)
	{
		putCh(art2.ucBuf[i]);
	}
}
//
static void isp_write_addr(void)
{
	art2.ucBufLen = 0;
	art2.ucBuf[art2.ucBufLen++] = isp_wr_addr >> 24;
	art2.ucBuf[art2.ucBufLen++] = isp_wr_addr >> 16;
	art2.ucBuf[art2.ucBufLen++] = isp_wr_addr >> 8;
	art2.ucBuf[art2.ucBufLen++] = isp_wr_addr;
	rt_thread_delay(10);
	art2.ucBufLen = Check_Compte(0,art2.ucBuf,art2.ucBufLen);
	sum_send(&art2);
}
//
static void isp_write_data(void)
{
	art2.ucBufLen = 0;
	art2.ucBuf[art2.ucBufLen++] = (ISP_ONCE_DOWNLOAD_DATA_LEN-1);
	API_W25Q128.Read(&art2.ucBuf[art2.ucBufLen],isp_rd_addr,ISP_ONCE_DOWNLOAD_DATA_LEN);
	
	uint8_t da = art2.ucBuf[0];
	for(uint16_t i=1;i<ISP_ONCE_DOWNLOAD_DATA_LEN+1;i++)
	{
		da = da ^ art2.ucBuf[i];
	}
	art2.ucBufLen = ISP_ONCE_DOWNLOAD_DATA_LEN+2;
	art2.ucBuf[art2.ucBufLen-1] = da;
	
	rt_thread_delay(10);
	
	sum_send(&art2);
}
//

  
//DMA发送完成中断回调函数  
//void HAL_UART2_TxCpltCallback(void)  
//{  
//    SelfBufState.dmasend_flag = USART_DMA_SENDOVER;  
//}  //

  
//static void FUNC_IDLE_IRQ(void)
//{
//    uint32_t temp;  

//    temp = huart2.hdmarx->Instance->NDTR;  
//    SelfBufState.ucBufLen =  UART2_BUFLEN - temp;   
//    SelfBufState.uart2_rec_flag = 1;  
//    HAL_UART_Receive_DMA(&huart2,SelfBufState.ucBuf,UART2_BUFLEN);  
//     
//}
//


#endif

//

//
//void * Usart2getucBuf(void)
//{
//    return SelfBufState.ucBuf;
//}
////

//void * Usart2getComStruct(void)
//{
//    return &SelfBufState;
//}
//
//
//需要封装一下发给串口2   ,封装好之后无需增加转义,保存到sendbuf中
static void Usart2ToUsart1(comm_state_t * r)
{
//    comm_state_t * t;
//    
//    t = Usart1getComStruct();
//    
//    t->uart1_rec_uart2_flag = 1;
}//

//=========================================================================================================
/*
 *  xieyi2 implementation for usart2_app xieyi2.
 *
 * COPYRIGHT (C) 2006 - 2013, RT-Thread Development Team
 *
 *  This file is part of RT-Thread (http://www.rt-thread.org)
 *  Maintainer: bernard.xiong <bernard.xiong at gmail.com>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-04-30     Bernard      the first version for usart2_app
 * 2006-05-08     Bernard      change usart2_app thread stack to 2048
 * 2006-06-03     Bernard      add support for skyeye
 * 2006-09-24     Bernard      remove the code related with hardware
 * 2010-01-18     Bernard      fix down then up key bug.
 * 2010-03-19     Bernard      fix backspace issue and fix device read in xieyi2.
 * 2010-04-01     Bernard      add prompt output when start and remove the empty history
 * 2011-02-23     Bernard      fix variable section end issue of usart2_app xieyi2
 *                             initialization when use GNU GCC compiler.
 */

//#include <rthw.h>

#include "usart2_app.h"



/* usart2_app thread */
static struct rt_thread usart2_app_thread;
ALIGN(RT_ALIGN_SIZE)
static char usart2_app_thread_stack[0x110];
struct usart2_app_xieyi *xieyi2;

static struct rt_thread usart2_app_idle_thread;
ALIGN(RT_ALIGN_SIZE)
static char usart2_idle_stack[0x110];



//================================================================================
static rt_err_t usart2_app_rx_ind(rt_device_t dev, rt_size_t size)
{
    RT_ASSERT(xieyi2 != RT_NULL);

    /* release semaphore to let usart2_app thread rx data */
    rt_sem_release(&xieyi2->rx_sem);

    return RT_EOK;
}
//
rt_err_t usart2_idle_ind(rt_device_t dev, rt_size_t size)
{
    RT_ASSERT(xieyi2 != RT_NULL);

    /* release semaphore to let usart2_app thread rx data */
    rt_sem_release(&xieyi2->idle_sem);

    return RT_EOK;
}
//
rt_err_t usart2_mailbox_send(rt_uint32_t value)
{
    RT_ASSERT(xieyi2 != RT_NULL);

    /* release semaphore to let usart2_app thread rx data */
    rt_mb_send(&xieyi2->u2_mb,value);

    return RT_EOK;
}
//
rt_err_t usart2_mailbox_recv(rt_uint32_t *value)
{
    RT_ASSERT(xieyi2 != RT_NULL);

    /* release semaphore to let usart2_app thread rx data */
    rt_mb_recv(&xieyi2->u2_mb,value,RT_WAITING_FOREVER);

    return RT_EOK;
}
//
/**
 * @ingroup usart2_app
 *
 * This function sets the input device of usart2_app xieyi2.
 *
 * @param device_name the name of new input device.
 */
void usart2_app_set_device(const char *device_name)
{
    rt_device_t dev = RT_NULL;

    RT_ASSERT(xieyi2 != RT_NULL);
	
    dev = rt_device_find(device_name);
    if (dev == RT_NULL)
    {
        rt_kprintf("usart2_app: can not find device: %s\n", device_name);
        return;
    }

    /* check whether it's a same device */
    if (dev == xieyi2->device) return;
    /* open this device and set the new device in usart2_app xieyi2 */
	//RT_DEVICE_FLAG_INT_RX
    if (rt_device_open(dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_STREAM) == RT_EOK)
    {
        if (xieyi2->device != RT_NULL)
        {
            /* close old usart2_app device */
            rt_device_close(xieyi2->device);
            rt_device_set_rx_indicate(xieyi2->device, RT_NULL);
        }

        xieyi2->device = dev;
        rt_device_set_rx_indicate(dev, usart2_app_rx_ind);
    }
}

/**
 * @ingroup usart2_app
 *
 * This function returns current usart2_app xieyi2 input device.
 *
 * @return the usart2_app xieyi2 input device name is returned.
 */
const char *usart2_app_get_device()
{
    RT_ASSERT(xieyi2 != RT_NULL);
    return xieyi2->device->parent.name;
}
//
extern struct rt_mailbox mb_key1;
//
static uint8_t getcha(void)
{
	uint32_t a = 50;
//	while((!(art2.receive_flag)) && (a)) 
//	{
//		rt_thread_delay(1);
//		a--;
//	}
	while((!(USART2->SR & USART_FLAG_RXNE)) && (a)) 
	{
		rt_thread_delay(1);
		a--;
	}
	if(a==0)
		return 0;
	else
		return USART2->DR & 0xFF;
//		return art2.ucBuf[0];
}
//
void usart2_isp_entry(void *parameter)//
{
	rt_kprintf("按键ISP创建的线长创建成功了!\n");
	uint8_t ch;
	
	isp_rd_addr = W25Qx_ADDR_IAP;
	
	rt_pin_write(PCB_BOOT0,1);
	rt_pin_write(PCB_RESET,0);
	rt_thread_delay(10);
	rt_pin_write(PCB_RESET,1);
	
	comm_state_t  mb_values;
	
//	PT_INIT(art2.lc);
	art2.ucBufLen = 0;
	putCh(0x7f);//同步
	ch = USART2->DR & 0xFF;
	ch = getcha();//
	if(ch != 0x79) goto maybe_wrong;
	
	art2.ucBufLen = 0;
	putCh(0x43);//擦除
	putCh(0xbc);//
	ch = getcha();//
	if(ch != 0x79) goto maybe_wrong;
	
	art2.ucBufLen = 0;
	putCh(0xff);//
	putCh(0x00);//
	ch = getcha();//
	if(ch != 0x79) goto maybe_wrong;
		
	write_memory_here:
	if(sys_parameter.ispDATA_LEN > (isp_rd_addr - W25Qx_ADDR_IAP))
	{
		art2.ucBufLen = 0;
		putCh(0x31);//
		putCh(0xce);//
		
		ch = getcha();//
		if(ch != 0x79) goto maybe_wrong;
		art2.ucBufLen = 0;
		isp_write_addr();
		
		ch = getcha();//
		if(ch != 0x79) goto maybe_wrong;
		art2.ucBufLen = 0;
		isp_write_data();
		
		ch = getcha();//
		if(ch != 0x79) goto maybe_wrong;
		
		goto write_memory_here;
	}
	
	//到这里就说明写完了
	rt_kprintf("isp已经下载到了待测板子之中");
	
	
	
	maybe_wrong:
	rt_pin_write(PCB_BOOT0,0);
	rt_pin_write(PCB_RESET,0);
	rt_thread_delay(10);
	rt_pin_write(PCB_RESET,1);
	KEY_ONCE = true;

//	while(1){
//	rt_mb_recv(&xieyi2->u2_mb,(rt_uint32_t*)&mb_values,-1);
//	if((mb_values.ucBufLen) <= 0) goto maybe_wrong;
//	
//	PT_BEGIN(art2.lc);
//	if(art2.ucBuf[0] != 0x79) goto maybe_wrong;
//	
//	putCh(0x43);//擦除
//	putCh(0xbc);//
//	
//	PT_YIELD(art2.lc);
//	if(art2.ucBuf[0] != 0x79) goto maybe_wrong;
//	putCh(0xff);//
//	putCh(0x00);//
//	
//	write_memory_here:
//	if(sys_parameter.ispDATA_LEN > (isp_rd_addr - W25Qx_ADDR_IAP))
//	{
//		PT_YIELD(art2.lc);
//		if(art2.ucBuf[0] != 0x79) goto maybe_wrong;
//		putCh(0x31);//
//		putCh(0xce);//
//		
//		PT_YIELD(art2.lc);
//		if(art2.ucBuf[0] != 0x79) goto maybe_wrong;
//		isp_write_addr();
//		
//		PT_YIELD(art2.lc);
//		if(art2.ucBuf[0] != 0x79) goto maybe_wrong;
//		isp_write_data();
//		
//		
//		goto write_memory_here;
//	}
//	
//	PT_YIELD(art2.lc);
//	if(art2.ucBuf[0] != 0x79) goto maybe_wrong;
//	
//	//到这里就说明搞定了
//	rt_kprintf("isp已经下载到了待测板子之中");
//	
//	
//	
//	maybe_wrong:
//	rt_pin_write(PCB_BOOT0,0);
//	rt_pin_write(PCB_RESET,0);
//	rt_thread_delay(10);
//	rt_pin_write(PCB_RESET,1);
//	KEY_ONCE = true;
//	uint8_t as=1;
//	rt_mb_send(&mb_key1,(rt_uint32_t)&as);
//	PT_END(art2.lc);
//}
}
//
void usart2_iap_entry(void *parameter)//
{
	rt_kprintf("按键IAP创建的线长创建成功了!\n");
	iap_rd_addr = W25Qx_ADDR_APP;
	
	
	
	KEY_ONCE = true;
}
//
//
//
void usart2_app_thread_entry(void *parameter)
{
    uint8_t c;

    /* normal is echo mode */
//    xieyi2->echo_mode = 1;
//	xieyi2->buffer = buf_uae1;
//	xieyi2->buflen = 0;

	art2.ucBufLen = 0;
    /* set console device as xieyi2 device */
    if (xieyi2->device == RT_NULL)
    {
//#ifdef RT_USING_CONSOLE
//        xieyi2->device = rt_console_get_device();
//        RT_ASSERT(xieyi2->device);
//        rt_device_set_rx_indicate(xieyi2->device, usart2_app_rx_ind);
//        rt_device_open(xieyi2->device, (RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_STREAM | RT_DEVICE_FLAG_INT_RX));
//#else
        RT_ASSERT(xieyi2->device);
//#endif
    }

    while (1)
    {
        /* wait receive */
        if (rt_sem_take(&xieyi2->rx_sem, RT_WAITING_FOREVER) != RT_EOK) continue;

        /* read one character from device */
        while (rt_device_read(xieyi2->device, 0, &c, 1) == 1)
        {		
//			xieyi2->buffer[xieyi2->buflen] = c;xieyi2->buflen++;
//			if(xieyi2->buflen >= 64) xieyi2->buflen = 0;
			art2.ucBuf[art2.ucBufLen ++] = c;
			if(art2.ucBufLen >= UCBUFSIZE) art2.ucBufLen = 0;
			
        } /* end of device read */
    }
}
//
void usart2_app_idle_thread_entry(void *parameter)
{
//    uint8_t c;

    /* normal is echo mode */
//    xieyi2->echo_mode = 1;


    /* set console device as xieyi2 device */
    if (xieyi2->device == RT_NULL)
    {
//#ifdef RT_USING_CONSOLE
//        xieyi2->device = rt_console_get_device();
//        RT_ASSERT(xieyi2->device);
//        rt_device_set_rx_indicate(xieyi2->device, usart2_app_rx_ind);
//        rt_device_open(xieyi2->device, (RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_STREAM | RT_DEVICE_FLAG_INT_RX));
//#else
        RT_ASSERT(xieyi2->device);
//#endif
    }

    while (1)
    {
        /* wait receive */
        if (rt_sem_take(&xieyi2->idle_sem, RT_WAITING_FOREVER) != RT_EOK) continue;
		
		if(art2.ucBufLen > 0)
		{
			art2.receive_flag = 1;
		}else
			art2.receive_flag = 0;
		
//		SendData(art2.ucBuf,art2.ucBufLen);
//		usart2_mailbox_send((rt_uint32_t)(&art2));
		art2.ucBufLen = 0;
//		xieyi2->buflen = 0;
	}
}
//void usart2_app_system_function_init(const void *begin, const void *end)
//{
//    _syscall_table_begin = (struct usart2_app_syscall *) begin;
//    _syscall_table_end = (struct usart2_app_syscall *) end;
//}

//void usart2_app_system_var_init(const void *begin, const void *end)
//{
//    _sysvar_table_begin = (struct usart2_app_sysvar *) begin;
//    _sysvar_table_end = (struct usart2_app_sysvar *) end;
//}

//#if defined(_MSC_VER)
//#pragma section("FSymTab$a", read)
//const char __fsym_begin_name[] = "__start";
//const char __fsym_begin_desc[] = "begin of usart2_app";
//__declspec(allocate("FSymTab$a")) const struct usart2_app_syscall __fsym_begin =
//{
//    __fsym_begin_name,
//    __fsym_begin_desc,
//    NULL
//};

//#pragma section("FSymTab$z", read)
//const char __fsym_end_name[] = "__end";
//const char __fsym_end_desc[] = "end of usart2_app";
//__declspec(allocate("FSymTab$z")) const struct usart2_app_syscall __fsym_end =
//{
//    __fsym_end_name,
//    __fsym_end_desc,
//    NULL
//};
//#endif

/*
 * @ingroup usart2_app
 *
 * This function will initialize usart2_app xieyi2
 */
 static uint32_t u2MB_mspool[10];
int usart2_app_system_init(void)
{
    rt_err_t result;

    /* create or set xieyi2 structure */
#ifdef RT_USING_HEAP
    xieyi2 = (struct usart2_app_xieyi *)rt_malloc(sizeof(struct usart2_app_xieyi));
    if (xieyi2 == RT_NULL)
    {
        rt_kprintf("no memory for xieyi2\n");
        return -1;
    }
#else
    xieyi2 = &_xieyi2;
#endif

    memset(xieyi2, 0, sizeof(struct usart2_app_xieyi));

    rt_sem_init(&(xieyi2->rx_sem), "uar2x", 0, 0);
	rt_sem_init(&(xieyi2->idle_sem), "u2xle", 0, 0);
    result = rt_thread_init(&usart2_app_thread,
                            "UAxy2",
                            usart2_app_thread_entry, RT_NULL,
                            &usart2_app_thread_stack[0], sizeof(usart2_app_thread_stack),
                            14, 15);

    if (result == RT_EOK)
        rt_thread_startup(&usart2_app_thread);
	
	result = rt_thread_init(&usart2_app_idle_thread,
                            "UAle2",
                            usart2_app_idle_thread_entry, RT_NULL,
                            &usart2_idle_stack[0], sizeof(usart2_idle_stack),
                            15, 15);

    if (result == RT_EOK)
        rt_thread_startup(&usart2_app_idle_thread);
	
	result = rt_mb_init(&xieyi2->u2_mb,"u2_mb",&u2MB_mspool[0],sizeof(u2MB_mspool),RT_IPC_FLAG_FIFO);
	if (result != RT_EOK)
	{
		rt_kprintf("user u2_mb create fault\n");
	}
	
    return 0;
}
INIT_COMPONENT_EXPORT(usart2_app_system_init);
//

//=============================================================================//
const struct  _api_func_uart2 API_USART2=
{
//    USART2_Set_Bande,
    Init,
    check,
    Reply,
    SendData,
//    FUNC_IDLE_IRQ, //空闲中断
//    HAL_UART2_TxCpltCallback,
    Usart2ToUsart1,
	send
};
//




