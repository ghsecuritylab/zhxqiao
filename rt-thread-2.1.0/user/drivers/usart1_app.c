#include "usart1_app.h"
#include "usart2_app.h"
#include "usart.h"
static uint8_t tmp_buf[1],ucbuf[1200],ucsendbuf[1];
/*---------------------------------------------------------------------------*/

//static comm_state_t art1;
static void Reply(comm_state_t * r);

static comm_state_t art1=
{
	.ucBuf = ucbuf,
	.ucSend = ucsendbuf,
	.tempbuf = tmp_buf
};
/*---------------------------------------------------------------------------*/
//extern struct rt_mailbox mb_uart1;
//void uart1_thr_entry(void * p)
//{
//	uint8_t * str;
//	uint16_t len;
//	while(1)
//	{
//		if(rt_mb_recv(&mb_uart1,(rt_uint32_t*)&str, RT_WAITING_FOREVER)== RT_EOK)
//		{
//			len = (str[0] << 8) + str[1];
//			memcpy(art1.ucBuf,&str[2],len);
//			art1.ucBufLen = len;
//			Reply(&art1);
//		}
//	}
//}
/*---------------------------------------------------------------------------*/

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
//    SHIFOU_GUANJI = false ;
//    memset(&SelfBufState,0,sizeof(SelfBufState));
//    SelfBufState.tempbuf = TempBuf;
//    SelfBufState.ucBuf   = ucBuf;
//    SelfBufState.ucSend  = ucSend;
	
//    HAL_UART_Receive_DMA(&huart1, SelfBufState.ucBuf, UART1_BUFLEN);  
//    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);  
}

//==============================================================================
//| 函数功能 | 检查串口1缓冲区是否有数据.
//|----------|------------------------------------------------------------------
//| 输入参数 | 无
//|----------|------------------------------------------------------------------
//| 返回参数 | 无
//==============================================================================
static void check(void)
{
//    if(SelfBufState.receive_flag == 1)
//    {
//        SelfBufState.receive_flag = 0;
//        API_USART1.Reply(&SelfBufState);
//    }
//    //
//    if(SelfBufState.uart1_rec_uart2_flag == 1)
//    {//串口2发来的数据就直接发送给PC
//        comm_state_t * r;
//        r       = Usart2getComStruct();
//        
//        SelfBufState.uart1_rec_uart2_flag = 0;
//        memcpy(SelfBufState.tempbuf,r->ucSend,r->ucSendLen);
//       
//        API_USART1.send(&SelfBufState);
//    }
}
//==============================================================================
//| 函数功能 | 解析串口数据.增加或者去除转义,把串口接收到的数据转存到tempbuf,包括
//              头尾
//|----------|------------------------------------------------------------------
//| 输入参数 | y为true表示增加转义,
//|----------|------------------------------------------------------------------
//| 返回参数 | 是否成功
//==============================================================================
static bool uart1_xor40_jia(comm_state_t * r,bool y)
{
    uint16_t i=0,j=0,m;
    uint8_t tda;
    if(y)
    {//从 r->tempbuf 到 ucSend
        i = 1;j=0;
            
		m = r->ucSendLen - 1;
        r->ucSend[j++] = _FRAME_START;
//		r->ucSendLen -= 2;
        for(;i<m;)
		{
           tda =  r->tempbuf[i++];
           if((tda == _FRAME_START) || (tda == _FRAME_END) || (tda == _FRAME_ESC))
           {
               r->ucSend[j++] = _FRAME_ESC;
               r->ucSend[j++] = _FRAME_ESC ^ tda;
           }else
           {
               r->ucSend[j++] = tda;
           }
        }
		
        r->ucSend[j++] = _FRAME_END;
        r->ucSendLen = j;
        return true;
    }else
    {//从 ucBuf 到 r->tempbuf
        if(r->ucBuf[r->ucBufLen-1] != _FRAME_END)
            return false;
        i = 1;j=0;
        r->tempbuf[j++] = _FRAME_START;
        do
        {
            tda = r->ucBuf[i];
            if(tda == _FRAME_ESC)
            {
                r->tempbuf[j++] = tda ^ (r->ucBuf[i+1]);
                i += 2;
            }else
            {
                r->tempbuf[j++] = tda;
                i ++;
            }
            
        }while(r->ucBuf[i] != _FRAME_END);
        r->tempbuf[j++] = _FRAME_END;
        r->ucBufLen = j;
        return true;
    }
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
//    if((r->ucBuf[0] != _FRAME_START)) return;
    
//    if(!(uart1_xor40_jia(r,false))) return;
    
//    if(GetCrc16(&(r->tempbuf[1]),(r->ucBufLen - 2)) != 0) return;
        
    uint8_t myCMD      = r->ucBuf[_CTRL_CODE];
    
    switch(myCMD)
    {
    case  0x01://
        break;
    case  0xB0://下载iap和app程序数据
		CMD_7b7d_0xb0(r);
        break;
    case  _STMISPCODE://0xe0 ISP
        API_USART1.Usart1ToUsart2(r);
        break;
    case  _STMISPRESE://0xe1 ISP
//        API_USART1.SendData(r->ucBuf,r->ucBufLen);
        API_USART1.Usart1ToUsart2(r);
        break;
    case  _STMIAPCODE://0xd0 IAP
        API_USART1.Usart1ToUsart2(r);
        break;
    case  _STMAPPCODE://0xc0 APP
        API_USART1.Usart1ToUsart2(r);
        break;
    default://全部对待测板子进行操作
        break;
    }
}	
	
//==============================================================================
//| 函数功能 | 和校验
//|----------|------------------------------------------------------------------
//| 输入参数 | a(非0检验  0生成校验码) comm_state_t(保存数据的结构体)
//|----------|------------------------------------------------------------------
//| 返回参数 | 成功返回1 失败返回0
//==============================================================================
static uint8_t Check_Compte(bool a,comm_state_t * r)
{
	uint8_t check_sum;
	uint8_t data_sum;
	uint8_t geshu;
	if(a)
	{
		data_sum = r->ucSend[6];
		check_sum = r->ucSend[7];
		geshu = 8;
		
		while(geshu < 16)
		{
			check_sum += (uint8_t)((r->ucSend[geshu]<<8) + (r->ucSend[geshu + 1]));
			geshu += 2;
		}
		
		if(check_sum == data_sum)
			return 1;
	}
	else
	{
		check_sum = r->ucSend[7];
		geshu = 8;
		
		while(geshu < 16)
		{
			check_sum += (uint8_t)((r->ucSend[geshu]<<8) + (r->ucSend[geshu + 1]));
			geshu += 2;
		}
		
		r->ucSend[6] = check_sum;
		
		return 1;
	}
	return 0;
}

//DMA发送函数  
//static
//void Usart1SendData_DMA(uint8_t *pdata, uint16_t Length)  
//{  
//    while(SelfBufState.dmasend_flag == USART_DMA_SENDING);  
//    SelfBufState.dmasend_flag = USART_DMA_SENDING;  
//    HAL_UART_Transmit_DMA(&huart1, pdata, Length);  
//} 
//==============================================================================
//| 函数功能 | 发送数据
//|----------|------------------------------------------------------------------
//| 输入参数 | r(结构体保存着要发送的数据)
//|----------|------------------------------------------------------------------
//| 返回参数 | 无
//==============================================================================
//extern  int fputc(int ch, FILE *f);
static void putCh(rt_uint8_t c)
{
	while (!(USART1->SR & USART_FLAG_TXE))__NOP();
		USART1->DR = c ;
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
////    Usart1SendData_DMA(pdata,Length);
}
//
static void send(comm_state_t * r)
{
//    if(uart1_xor40_jia(r,true))
	rt_thread_delay(30);
	SendData(r->ucBuf,r->ucBufLen);
}
//
static void giveCRCEnd(uint8_t * buf,uint16_t * len)
{
    uint16_t lenc;
    lenc = *len;
    uint16_t crc = GetCrc16(&(buf[1]),lenc - 1);
    buf[lenc++] = (uint8_t)(crc >> 8);
    buf[lenc++] = (uint8_t)(crc >> 0);
    buf[lenc++] = _FRAME_END;
    *len = lenc;
}
//
//static void ispBegin(comm_state_t * r)
//{
//    r->mode = ISPMODE;
//    r->ucSendLen = 0;
//    r->tempbuf[r->ucSendLen++] = _FRAME_START;
//    r->tempbuf[r->ucSendLen++] = _STMISPCODE;
//    r->tempbuf[r->ucSendLen++] = 0x7e;
//    r->tempbuf[r->ucSendLen++] = _READ1_ANSWER;
//    r->tempbuf[r->ucSendLen++] = 0x00;
//    r->tempbuf[r->ucSendLen++] = 0x02;
//    r->tempbuf[r->ucSendLen++] = 0x00;
//    r->tempbuf[r->ucSendLen++] = 0x00;
//    giveCRCEnd(r->tempbuf,&(r->ucSendLen));
//    send(r);
//}
//
void * Usart1getTempbuf(void)
{
    return RT_NULL;//SelfBufState.tempbuf;
}
//

void * Usart1getComStruct(void)
{
    return RT_NULL;//&SelfBufState;
}
//


 
  
//DMA发送完成中断回调函数  
//static void HAL_UART1_TxCpltCallback(void)  
//{  
////     __HAL_DMA_DISABLE(huart->hdmatx);  
//    SelfBufState.dmasend_flag = USART_DMA_SENDOVER;  
//}  
//  
//static void FUNC_IDLE_IRQ(void)
//{
//    uint32_t temp;  
//	
//	temp = huart1.hdmarx->Instance->NDTR;  
//	SelfBufState.ucBufLen =  UART1_BUFLEN - temp;   
//	SelfBufState.receive_flag = 1;  
//	HAL_UART_Receive_DMA(&huart1,SelfBufState.ucBuf,UART1_BUFLEN); 
//	
//}//
//

static void Usart1ToUsart2(comm_state_t * r)
{
//    comm_state_t * t;
//    
//    t = Usart2getComStruct();
//    memcpy(t->ucBuf,r->ucBuf,r->ucBufLen);
//    t->ucBufLen = r->ucBufLen;
//    t->mode     = r->mode;
//    
//    t->uart2_rec_uart1_flag = 1;
}//





/*
 *  xieyi implementation for usart1_app xieyi.
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
 * 2006-04-30     Bernard      the first version for usart1_app
 * 2006-05-08     Bernard      change usart1_app thread stack to 2048
 * 2006-06-03     Bernard      add support for skyeye
 * 2006-09-24     Bernard      remove the code related with hardware
 * 2010-01-18     Bernard      fix down then up key bug.
 * 2010-03-19     Bernard      fix backspace issue and fix device read in xieyi.
 * 2010-04-01     Bernard      add prompt output when start and remove the empty history
 * 2011-02-23     Bernard      fix variable section end issue of usart1_app xieyi
 *                             initialization when use GNU GCC compiler.
 */

//#include <rthw.h>

#include "usart1_app.h"



/* usart1_app thread */
static struct rt_thread usart1_app_thread;
ALIGN(RT_ALIGN_SIZE)
static char usart1_app_thread_stack[0x200];
struct usart1_app_xieyi *xieyi;

//================================================================================
static rt_err_t usart1_app_rx_ind(rt_device_t dev, rt_size_t size)
{
    RT_ASSERT(xieyi != RT_NULL);

    /* release semaphore to let usart1_app thread rx data */
    rt_sem_release(&xieyi->rx_sem);

    return RT_EOK;
}

/**
 * @ingroup usart1_app
 *
 * This function sets the input device of usart1_app xieyi.
 *
 * @param device_name the name of new input device.
 */
void usart1_app_set_device(const char *device_name)
{
    rt_device_t dev = RT_NULL;

    RT_ASSERT(xieyi != RT_NULL);
    dev = rt_device_find(device_name);
    if (dev == RT_NULL)
    {
        rt_kprintf("usart1_app: can not find device: %s\n", device_name);
        return;
    }

    /* check whether it's a same device */
    if (dev == xieyi->device) return;
    /* open this device and set the new device in usart1_app xieyi */
    if (rt_device_open(dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX | \
                       RT_DEVICE_FLAG_STREAM) == RT_EOK)
    {
        if (xieyi->device != RT_NULL)
        {
            /* close old usart1_app device */
            rt_device_close(xieyi->device);
            rt_device_set_rx_indicate(xieyi->device, RT_NULL);
        }

        /* clear line buffer before switch to new device */
//        memset(xieyi->line, 0, sizeof(xieyi->line));
//        xieyi->line_curpos = xieyi->line_position = 0;

        xieyi->device = dev;
        rt_device_set_rx_indicate(dev, usart1_app_rx_ind);
    }
}

/**
 * @ingroup usart1_app
 *
 * This function returns current usart1_app xieyi input device.
 *
 * @return the usart1_app xieyi input device name is returned.
 */
const char *usart1_app_get_device()
{
    RT_ASSERT(xieyi != RT_NULL);
    return xieyi->device->parent.name;
}

/**
 * @ingroup usart1_app
 *
 * This function set the echo mode of usart1_app xieyi.
 *
 * usart1_app_OPTION_ECHO=0x01 is echo mode, other values are none-echo mode.
 *
 * @param echo the echo mode
 */
void usart1_app_set_echo(rt_uint32_t echo)
{
    RT_ASSERT(xieyi != RT_NULL);
    xieyi->echo_mode = (rt_uint8_t)echo;
}

/**
 * @ingroup usart1_app
 *
 * This function gets the echo mode of usart1_app xieyi.
 *
 * @return the echo mode
 */
rt_uint32_t usart1_app_get_echo()
{
    RT_ASSERT(xieyi != RT_NULL);

    return xieyi->echo_mode;
}

//static void xieyi_auto_complete(char *prefix)
//{

//    rt_kprintf("\n");
////    {
////#ifndef usart1_app_USING_MSH_ONLY
////        extern void list_prefix(char * prefix);
////        list_prefix(prefix);
////#endif
////    }

////    rt_kprintf("%s%s", usart1_app_PROMPT, prefix);
//}
//

void usart1_app_run_line( const char *line)
{
	
}
//
//static uint8_t buf_uae1[22],buf_uae1_len=0;

void usart1_app_thread_entry(void *parameter)
{
    uint8_t c;

    /* normal is echo mode */
    xieyi->echo_mode = 1;


    /* set console device as xieyi device */
    if (xieyi->device == RT_NULL)
    {
//#ifdef RT_USING_CONSOLE
//        xieyi->device = rt_console_get_device();
//        RT_ASSERT(xieyi->device);
//        rt_device_set_rx_indicate(xieyi->device, usart1_app_rx_ind);
//        rt_device_open(xieyi->device, (RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_STREAM | RT_DEVICE_FLAG_INT_RX));
//#else
        RT_ASSERT(xieyi->device);
//#endif
    }

    while (1)
    {
        /* wait receive */
        if (rt_sem_take(&xieyi->rx_sem, RT_WAITING_FOREVER) != RT_EOK) continue;

        /* read one character from device */
        while (rt_device_read(xieyi->device, 0, &c, 1) == 1)
        {
//			buf_uae1[buf_uae1_len++] = c;
//			if(buf_uae1_len>=20)
//				buf_uae1_len=0;
//			continue;
			
			static uint16_t wCrcVal;
			static uint16_t wDataLen;
			static uint8_t  cPrev = ~_FRAME_ESC;

			if(c == _FRAME_START) 	// 帧头
			{
				LC_INIT(art1.lc);
				art1.ucBuf[0] = c;
				art1.ucBufLen = 1;
				cPrev = c;
			}
			else
			if(cPrev == _FRAME_ESC)	// 转义
			{
				c = c ^ cPrev;
				cPrev = ~_FRAME_ESC;
			}
			else
			if(c == _FRAME_ESC) 	// 转义
			{
				cPrev = c;
				continue;
			}
			
			PT_BEGIN(art1.lc);
//			if(c != _FRAME_START) break;
			
			//控制码
			PT_YIELD(art1.lc);
			art1.ucBuf[art1.ucBufLen++]	= c;
			
			//控制码子码
			PT_YIELD(art1.lc);
			art1.ucBuf[art1.ucBufLen++]	= c;
			
			//操作码
			PT_YIELD(art1.lc);
			art1.ucBuf[art1.ucBufLen++]	= c;
			
			//数据长度
			PT_YIELD(art1.lc);
			art1.ucBuf[art1.ucBufLen++]	= c;
			wDataLen = c << 8;
			PT_YIELD(art1.lc);
			art1.ucBuf[art1.ucBufLen++]	= c;
			wDataLen += c << 0;
						
						inhere:
			if(wDataLen > 0)
			{
				PT_YIELD(art1.lc);
				art1.ucBuf[art1.ucBufLen++]	= c;
				wDataLen --;
				goto inhere;
			}
			
			//CRC
			PT_YIELD(art1.lc);
			art1.ucBuf[art1.ucBufLen++]	= c;
			wCrcVal  = c << 8;
			PT_YIELD(art1.lc);
			art1.ucBuf[art1.ucBufLen++]	= c;
			wCrcVal += c << 0;
			
			if(GetCrc16(&art1.ucBuf[1],art1.ucBufLen-1) != 0)break;
			
			//尾巴
			PT_YIELD(art1.lc);
			art1.ucBuf[art1.ucBufLen++]	= c;
			if(c != _FRAME_END )break;
			
//			rt_thread_delay(30);
//			art1.ucBuf[1]	=	0xB0;
//			art1.ucBuf[2]	= 	0x00;
//			art1.ucBuf[_OP_TYPE] = 0xaa;
//			art1.ucBuf[_DATA_FLAG] = 0x00;
//			art1.ucBuf[_DATA_FLAG+1] = 0x02;
//			
//			art1.ucBuf[_DATA_LEN] = 0x00;
//			art1.ucBuf[_DATA_LEN+1] = 0x02;
//			
//			art1.ucBufLen = _DATA_FLAG+2;
//			API_USART1.giveCRCEnd(art1.ucBuf,&(art1.ucBufLen));
//			
//			API_USART1.send(&art1);
//				break;
			//协议处理函数
			Reply(&art1);
			
			PT_END(art1.lc);
			
        } /* end of device read */
    }
}

//void usart1_app_system_function_init(const void *begin, const void *end)
//{
//    _syscall_table_begin = (struct usart1_app_syscall *) begin;
//    _syscall_table_end = (struct usart1_app_syscall *) end;
//}

//void usart1_app_system_var_init(const void *begin, const void *end)
//{
//    _sysvar_table_begin = (struct usart1_app_sysvar *) begin;
//    _sysvar_table_end = (struct usart1_app_sysvar *) end;
//}

//#if defined(_MSC_VER)
//#pragma section("FSymTab$a", read)
//const char __fsym_begin_name[] = "__start";
//const char __fsym_begin_desc[] = "begin of usart1_app";
//__declspec(allocate("FSymTab$a")) const struct usart1_app_syscall __fsym_begin =
//{
//    __fsym_begin_name,
//    __fsym_begin_desc,
//    NULL
//};

//#pragma section("FSymTab$z", read)
//const char __fsym_end_name[] = "__end";
//const char __fsym_end_desc[] = "end of usart1_app";
//__declspec(allocate("FSymTab$z")) const struct usart1_app_syscall __fsym_end =
//{
//    __fsym_end_name,
//    __fsym_end_desc,
//    NULL
//};
//#endif

/*
 * @ingroup usart1_app
 *
 * This function will initialize usart1_app xieyi
 */
int usart1_app_system_init(void)
{
    rt_err_t result;

    /* create or set xieyi structure */
#ifdef RT_USING_HEAP
    xieyi = (struct usart1_app_xieyi *)rt_malloc(sizeof(struct usart1_app_xieyi));
    if (xieyi == RT_NULL)
    {
        rt_kprintf("no memory for xieyi\n");
        return -1;
    }
#else
    xieyi = &_xieyi;
#endif

    memset(xieyi, 0, sizeof(struct usart1_app_xieyi));

    rt_sem_init(&(xieyi->rx_sem), "uarx", 0, 0);
    result = rt_thread_init(&usart1_app_thread,
                            "UAxieyi",
                            usart1_app_thread_entry, RT_NULL,
                            &usart1_app_thread_stack[0], sizeof(usart1_app_thread_stack),
                            14, 15);

    if (result == RT_EOK)
        rt_thread_startup(&usart1_app_thread);
    return 0;
}
INIT_COMPONENT_EXPORT(usart1_app_system_init);
//


/*********************************************************************************************/
const struct  _api_func_uart API_USART1 = 
{
	Init,
	check,
	Reply,
	Check_Compte,
	SendData,
//	ispBegin,
	giveCRCEnd,
	send,
//	FUNC_IDLE_IRQ, //空闲中断
//	HAL_UART1_TxCpltCallback,
	Usart1ToUsart2,
	uart1_xor40_jia
};












