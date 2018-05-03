#ifndef __DRV_KEY_H__
#define __DRV_KEY_H__
#include <stdbool.h>

extern bool KEY_ONCE;
//无键按下
#define KEY_NO                  0
#define KEY_YES                 1
//按键状态
#define KEY_RESET_SIG	0x10
#define KEY_OK_SIG		0x11
#define KEY_RIGHT_SIG	0x12
#define KEY_LEFT_SIG	0x13
#define KEY_DOWN_SIG	0x14
#define KEY_UP_SIG		0x15

//按键属性
#define KEY_PROPERTY_SHORT      0x00//短按
#define KEY_PROPERTY_LONG       0x40//长按
#define KEY_PROPERTY_BURST      0x80//连发
#define KEY_PROPERTY_DOUBLE     0xC0//双击

void key_init(void);
//void keyIntEnable(void);
//void keyIntDisable(void);
//void key_process(void);

#endif//__DRV_KEY_H__

