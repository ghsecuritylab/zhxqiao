#ifndef _USART1_APP_H_
#define _USART1_APP_H_

//#include "usart.h"
//#include "tools\crc16.h"
#include "users.h"





struct _api_func_uart
{
	void (*Init)(void );
	void (*check)(void);
	void (*Reply)(comm_state_t * r);
	uint8_t (*Check_Compte)(bool a,comm_state_t * r);
	void (*SendData)(uint8_t *pdata, uint16_t Length);
//	void (*ispBegin)(comm_state_t * r);
	void (*giveCRCEnd)(uint8_t * buf,uint16_t * len);
	void (*send)(comm_state_t * r);
//        void (*IDLE_IRQ)(void);
//        void (*TxCpltCallback)(void);  
        void (*Usart1ToUsart2)(comm_state_t * r);
        bool (*jiaZhuanYi)(comm_state_t * r,bool y);
};

extern const struct  _api_func_uart API_USART1;

//======================================================

//#include <rtthread.h>

#define usart1_app_CMD_SIZE	256

enum input_stat_user
{
	WAIT_NORMAL,
	WAIT_SPEC_KEY,
	WAIT_FUNC_KEY,
};
struct usart1_app_xieyi
{
	struct rt_semaphore rx_sem;

	enum input_stat_user stat;

	rt_uint8_t echo_mode:1;


//	char line[usart1_app_CMD_SIZE];
//	rt_uint8_t line_position;
//	rt_uint8_t line_curpos;

	rt_device_t device;
};

void usart1_app_set_echo(rt_uint32_t echo);
rt_uint32_t usart1_app_get_echo(void);

int usart1_app_system_init(void);
void usart1_app_set_device(const char* device_name);
const char* usart1_app_get_device(void);





#endif
