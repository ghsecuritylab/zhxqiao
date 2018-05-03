/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2014-04-27     Bernard      make code cleanup. 
 */

#include <board.h>
#include <rtthread.h>
#include "users.h"



#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#include "stm32f4xx_eth.h"
#endif

#ifdef RT_USING_GDB
#include <gdb_stub.h>
#endif

void rt_init_thread_entry(void* parameter)
{
    /* GDB STUB */
#ifdef RT_USING_GDB
    gdb_set_device("uart6");
    gdb_start();
#endif

    /* LwIP Initialization */
#ifdef RT_USING_LWIP
    {
        extern void lwip_sys_init(void);

        /* register ethernetif device */
        eth_system_device_init();

        rt_hw_stm32_eth_init();

        /* init lwip system */
        lwip_sys_init();
        rt_kprintf("TCP/IP initialized!\n");
    }
#endif
}
//
#if	w25q_thr_test
static struct rt_thread w25q_thr;
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t w25q_thr_stack[0x100];
#endif
//

//=================================================================================
#if	w25q_thr_test
static void w25q_thr_entry(void *parameter)
{
	while(1)
	{
		uint8_t id[2];
		rt_thread_delay(3000);
		API_W25Q128.Read_ID(&id[0]);
		rt_kprintf("QSPIflash 的ID号是 %d he %d \n",id[0],id[1]);
	}
}
#endif
//

				   //
int rt_application_init()
{
    rt_thread_t tid;

    tid = rt_thread_create("init",
        rt_init_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);
#if	w25q_thr_test//测试spiflash
	/* initialize thread */
    rt_thread_init
	(&w25q_thr,
	"w25q_thr",
	w25q_thr_entry,
	RT_NULL,
	&w25q_thr_stack[0],
	sizeof(w25q_thr_stack),
	16,
	10);

    /* startup */
    rt_thread_startup(&w25q_thr);
#endif
	
#if	STM32_USER_USART == 1
	//init user uart1
	extern int usart1_app_system_init(void);
	extern void usart1_app_set_device(const char *device_name);
	usart1_app_system_init();
    usart1_app_set_device( USER_UART_DEVICE_NAME );
#else
	//init user uart2
//	extern int usart2_app_system_init(void);
//	extern void usart2_app_set_device(const char *device_name);
//	usart2_app_system_init();
//    usart2_app_set_device( USER_UART_DEVICE_NAME );	
//	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
#endif
	
//uint8_t il	
	
	key_init();
	
    return 0;	
}

/*@}什么东西*/
