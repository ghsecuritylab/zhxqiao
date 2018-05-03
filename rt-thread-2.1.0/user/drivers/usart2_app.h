#ifndef _USART2_APP_H_
#define _USART2_APP_H_

#include "users.h"

struct _api_func_uart2
{
//    void (*Set_Bande)(uint32_t c);
    void (*Init)(void );
    void (*check)(void);
    void (*Reply)(comm_state_t * r);
    void (*SendData)(uint8_t *pdata, uint16_t Length);
//    void (*IDLE_IRQ)(void);
//    void (*TxCpltCallback)(void); 
    void (*Usart2ToUsart1)(comm_state_t * r);
	void (*send)(comm_state_t * r);
};

extern const struct  _api_func_uart2 API_USART2;



void * Usart2getTempbuf(void);
void * Usart2getComStruct(void);

//====================================================================
#define IDLE_MAIL				0x01

#define usart2_app_CMD_SIZE	256

struct usart2_app_xieyi
{
	struct rt_semaphore rx_sem;
	struct rt_semaphore idle_sem;
	struct rt_mailbox	u2_mb;
//	uint8_t * buffer;
//	uint16_t buflen;
//	enum input_stat_user stat;

//	rt_uint8_t echo_mode:1;


//	char line[usart2_app_CMD_SIZE];
//	rt_uint8_t line_position;
//	rt_uint8_t line_curpos;

	rt_device_t device;
};

//void usart2_app_set_echo(rt_uint32_t echo);
//rt_uint32_t usart2_app_get_echo(void);

int usart2_app_system_init(void);
void usart2_app_set_device(const char* device_name);
const char* usart2_app_get_device(void);

rt_err_t usart2_mailbox_send(rt_uint32_t value);
rt_err_t usart2_mailbox_recv(rt_uint32_t *value);
rt_err_t usart2_idle_ind(rt_device_t dev, rt_size_t size);



#endif
