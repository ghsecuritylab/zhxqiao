#include "users.h"
#include "drivers/pin.h"


static struct rt_thread key_thread;
ALIGN(RT_ALIGN_SIZE)
static char key_stack[0x200];
//struct rt_mailbox mb_key1;
//uint32_t mb_key1_pool[3];

bool KEY_ONCE=true;
/*
按键返回码:按键属性|按键值(或操作)
按键属性包括:短按, 长按, 连发, 双击
按键值:按键返回的值
高2位表示属性,低6位表示按键值*/

//以下为按键部分配置宏定义,
#define KEY_DOUBLE_ENABLE       0//1为启用按键双击模式

//按键连发设置
#define KEY_BURST_FIRST         200//按键连发开始所需时间长度
#define KEY_BURST_SPACE         10//按键连发的时间间隔长度

//双击按键设置
#define KEY_DOUBLE_SPACE        2//双击时间间隔,指两次KEY_DOWN的间隔

//放置读取按键
#define PIN_RESET				1
#define PIN_OK					rt_pin_read(KEY_DOWNLOAD_BEGIN)
#define PIN_RIGHT				1
#define PIN_LEFT				1
#define PIN_DOWN				rt_pin_read(KEY_UPGRADE_BEGIN)
#define PIN_UP					1


//==================================================================================================
//| 函数名称 | key_get_code
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 获取键码
//|----------|--------------------------------------------------------------------------------------
//| 返回参数 | 扫描获得的键值
//|----------|--------------------------------------------------------------------------------------
//| 函数设计 | cc 2012-4-27
//==================================================================================================
//#pragma optimize=none
static uint8_t key_get_code(void)
{
    if(!(PIN_RESET)){
        return KEY_RESET_SIG;
    }
    if(!(PIN_OK)){
        return KEY_OK_SIG;
    }
    if(!(PIN_RIGHT)){
        return KEY_RIGHT_SIG;
    }
    if(!(PIN_LEFT)){
        return KEY_LEFT_SIG;
    }
    if(!(PIN_DOWN)){
        return KEY_DOWN_SIG;
    }
    if(!(PIN_UP)){
        return KEY_UP_SIG;
    }
    return KEY_NO;
}
//

static void key_thread_entry(void * p)
{
	static uint8_t key_va;
	static uint8_t KeyState = 0;//按键状态,状态机使用
	static uint8_t KeyPrev = 0;//保存上一次按键,防止出现乱码现象
	static uint16_t KeyHoldDly = 0;//按键连发时间
	static uint8_t KeyBurst = 0;//标志连发开始
	static uint8_t KeyPress = KEY_NO;//按键值,哪个按键
	static uint8_t KeyReturn = KEY_NO;//按键返回值
	static rt_thread_t reisp=RT_NULL;
	static rt_thread_t reiap=RT_NULL;

//	rt_mb_init(&mb_key1,"kmb1",&mb_key1_pool,sizeof(mb_key1_pool)/4,0);
	while(1)
	{
		rt_thread_delay(10);
		key_va = rt_pin_read(KEY_DOWNLOAD_BEGIN);
				
		#if KEY_DOUBLE_ENABLE == 1
		static uint8_t KeyDbleTmp = 0;//处理按键双击事件
		static uint8_t KeyDbleDly = 0;//按键双击间隔时间
		#endif

		uint8_t key_value = KEY_NO;

		KeyPress = key_get_code();

		#if KEY_DOUBLE_ENABLE == 1
		if(KeyDbleDly > 0)//处理双击按键的
		{
			KeyDbleDly--;
			if(KeyDbleDly == 0)
			{
				KeyDbleTmp = KEY_NO;
			}
		}
		#endif

		switch(KeyState)
		{
			case 0://按键初始态00
				if(KeyPress != KEY_NO)
				{
					KeyState = 1;
					KeyPrev = KeyPress;
				}
				break;
			case 1://按键确认态01
				if(KeyPress != KEY_NO)
				{
					if(KeyPrev != KeyPress)//与上次按键不同,是乱码
					{
						KeyState = 0;
					}
					else
					{
						//双击处理在按键按下之前
						#if KEY_DOUBLE_ENABLE == 1
						if(KeyPress == KeyDbleTmp)
						{
							KeyReturn = KEY_PROPERTY_DOUBLE | KeyPress;
							key_value = KeyReturn;
							KeyDbleTmp = KEY_NO;
							KeyDbleDly = 0;
							break;
						}
						#endif

						//以下为按键按下处理
						KeyState = 2;
						KeyReturn = KEY_PROPERTY_SHORT | KeyPress;//返回按键短按键值

						//保存上次按键按下的键,处理双击用
						#if KEY_DOUBLE_ENABLE == 1
						KeyDbleTmp = KeyPress;
						KeyDbleDly = KEY_DOUBLE_SPACE;
						#endif
					}
				}
				else//按键抬起,是抖动,不响应按键
				{
					KeyState = 0;
				}
				break;
			case 2://按键连续态11
				if(KeyPress != KEY_NO)
				{//这个地方也可以添加 (KeyPress == KEY_1) || (KeyPress == KEY_2)
				 //这样可以对产生连发的按键进行限制
					KeyHoldDly++;
					if((KeyBurst == 1) && (KeyHoldDly > KEY_BURST_SPACE))
					{
						KeyHoldDly = 0;
						KeyReturn = KEY_PROPERTY_BURST | KeyPress;//返回连发的键值
						//key_value = KEY_PROPERTY_SHORT | (KeyReturn & 0x3F);
						key_value = KeyReturn;
						KeyPrev = KeyPress;//记住上次的按键
						break;
					}

					if(KeyHoldDly > KEY_BURST_FIRST)
					{
						KeyBurst = 1;
						KeyHoldDly = 0;
						KeyReturn = KEY_PROPERTY_LONG | KeyPress;//返回长按后的键值
						key_value = KeyReturn;
						break;
					}
				}
			case 3://按键释放态10
				if(KeyPress == KEY_NO)
				{
					KeyState = 0;
					KeyHoldDly = 0;
					KeyBurst = 0;
					if((KeyReturn & KEY_PROPERTY_BURST) != KEY_PROPERTY_BURST && (KeyReturn & KEY_PROPERTY_LONG) != KEY_PROPERTY_LONG)
					{
						key_value = KeyReturn;
					}
				}
				break;
			default:
				break;
		}

		if((key_value == KEY_OK_SIG)&&(KEY_ONCE))//有按键
		{
//			key_value = key_value &0x0f;//连续按与正常按相同
			KEY_ONCE = false;
			extern void usart2_isp_entry(void *parameter);
			reisp = rt_thread_create("isp",usart2_isp_entry,RT_NULL,0x400,30,10);
			
			if(reisp != RT_NULL)
			{
				rt_thread_startup(reisp);
//				rt_thread_delay(2);
//				usart2_mailbox_send((rt_uint32_t)(&key_va));
			}
//			rt_thread_delete(relas);
		}else if((key_value == KEY_DOWN_SIG)&&(KEY_ONCE))
		{
			KEY_ONCE = false;
			
			extern void usart2_iap_entry(void *parameter);
			reiap = rt_thread_create("iap",usart2_iap_entry,RT_NULL,0x400,30,10);
			
			if(reiap != RT_NULL)
			{
				rt_thread_startup(reiap);
//				rt_thread_delay(2);
//				usart2_mailbox_send((rt_uint32_t)(&key_va));
			}
		}
//		uint8_t as;
//		rt_mb_recv(&mb_key1,(rt_uint32_t *)&as,1);
//		if(as == 1)
//			rt_thread_delete(reisp);
//		if(as == 2)
//			rt_thread_delete(reiap);
	}
}
//


//创建key线程
void key_init(void)
{
	rt_err_t relas;
	
	relas = rt_thread_init(&key_thread,"key",key_thread_entry,RT_NULL,&key_stack[0],sizeof(key_stack),5,10);
	
	if(relas == RT_EOK)
	{
		rt_thread_startup(&key_thread);
	}
	
}
//



