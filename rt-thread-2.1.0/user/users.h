#ifndef _USERS_H_
#define _USERS_H_
//------------�����ǰ�����ͷ�ļ�------------------------------------------------
#if 1   //------------�����ǰ�����ͷ�ļ�----------------------
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "Parameter/Parameter.h"

#include "stm32f4xx.h"
#include <rthw.h>
#include <rtthread.h>

#include "sys.h"
#include "tools/drv_key.h"
#include <drivers/pin.h>
//=========================================================================

#if 1   //------------�����Ƕ��������---------------------
#define w25q_thr_test		0
#define user_uart1_en		1
//GPIO
#define	PCB_POWER			2
#define	PCB_BOOT0			6
#define	PCB_RESET			7
#define	PCB_LEDS_POWER		11
#define	KEY_DOWNLOAD_BEGIN	8
#define KEY_UPGRADE_BEGIN	9
//


// ֡λ��
#define _CTRL_CODE          1
#define _CTRL_SUN           2
#define _OP_TYPE            3
#define _DATA_LEN           4
#define _DATA_FLAG          6
#define _DATA_PTR           8
#define _CMD_LEN            9
/*---------------------------------------------------------------------------*/
// ֡����
#define _FRAME_START   	0x7B
#define _FRAME_ESC		0x40
#define _FRAME_END		0x7D
/*---------------------------------------------------------------------------*/

// ������
#define _READ1          	0x55
#define _READ1_ANSWER   	0xAA

#define _WRITE1         	0x66
#define _WRITE1_ANSWER  	0x99

#define _WRITE2         	0xE6
/*---------------------------------------------------------------------------*/

// ��Ӧ��
#define _WRITE_SUCCESS  		0x00
#define _WRITE_FAULT_MASK       0x80
/*---------------------------------------------------------------------------*/

//�����ĸ�Э��Ĵ���
#define _STMAPPCODE         0xC0        //STM32��APPЭ��
#define _STMIAPCODE         0xD0        //STM32��IAPЭ��
#define _STMISPCODE         0xE0        //STM32��ISPЭ��
#define _STMISPRESE         0xE1        //STM32��ISPЭ��,;

//-------------------------------------------------------------------------------------
#endif   //------------�����Ƕ��������---------------------
//
#include "tools\lc-switch.h"
//
typedef enum
{
    NONMODE,
    ISPMODE = 66,
    IAPMODE,
    APPMODE
}_XIEYI_MODE_;
//
typedef struct _comm_state
{
    uint8_t *ucBuf;
    uint8_t *ucSend;
    uint8_t *tempbuf;
/*---------------------------------------------------------------------------*/    
    uint16_t ucBufLen;
    uint16_t ucSendLen;
    uint8_t  receive_flag:1;
    uint8_t  dmasend_flag:1;
    uint8_t  uart2_rec_flag:1;
    uint8_t  uart2_send_flag:1;
    uint8_t  uart1_rec_uart2_flag:1;
    uint8_t  uart1_send_uart2_flag:1;
    uint8_t  uart2_rec_uart1_flag:1;
    uint8_t  uart2_send_uart1_flag:1;
    //8���ֽڽ���
/*---------------------------------------------------------------------------*/
	lc_t lc;
/*---------------------------------------------------------------------------*/
    _XIEYI_MODE_ mode;
/*---------------------------------------------------------------------------*/
} comm_state_t;
//

/***********drivers**************/
#include "drivers\usart1_app.h"
#include "drivers\usart2_app.h"
#include "drivers\spi1.h"


/***********APPS**************/
#include "APPS\W25Q128FVSIGTR.h"
#include "APPS\tft3_2.h"


/***********agreement**************/
//#include "agreement\hly_testtool.h"
#include "agreement\7b7dxieyi.h"
//#include "agreement\appxieyi.h"
//#include "agreement\iapxieyi.h"

/***********parameter**************/


#if 0   //------------����������cube��ͷ�ļ�----------------------
#include "iwdg.h"
#endif  //------------����������cube��ͷ�ļ�---------------
/***********TOOLS**************/
#include "tools\crc16.h"
#include "tools\delay.h"

#endif  //------------�����ǰ�����ͷ�ļ�---------------
//------------�����ǰ�����ͷ�ļ�------------------------------------------------










#if 1   //------------������ȫ�ֱ���---------------
extern  uint8_t FLASH_sector_buf[0x1000];//4K    Ϊ��������flash����
#endif  //------------������ȫ�ֱ���---------------

#endif 

