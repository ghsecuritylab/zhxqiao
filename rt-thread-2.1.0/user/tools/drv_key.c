#include "users.h"
#include "drivers/pin.h"


static struct rt_thread key_thread;
ALIGN(RT_ALIGN_SIZE)
static char key_stack[0x200];
//struct rt_mailbox mb_key1;
//uint32_t mb_key1_pool[3];

bool KEY_ONCE=true;
/*
����������:��������|����ֵ(�����)
�������԰���:�̰�, ����, ����, ˫��
����ֵ:�������ص�ֵ
��2λ��ʾ����,��6λ��ʾ����ֵ*/

//����Ϊ�����������ú궨��,
#define KEY_DOUBLE_ENABLE       0//1Ϊ���ð���˫��ģʽ

//������������
#define KEY_BURST_FIRST         200//����������ʼ����ʱ�䳤��
#define KEY_BURST_SPACE         10//����������ʱ��������

//˫����������
#define KEY_DOUBLE_SPACE        2//˫��ʱ����,ָ����KEY_DOWN�ļ��

//���ö�ȡ����
#define PIN_RESET				1
#define PIN_OK					rt_pin_read(KEY_DOWNLOAD_BEGIN)
#define PIN_RIGHT				1
#define PIN_LEFT				1
#define PIN_DOWN				rt_pin_read(KEY_UPGRADE_BEGIN)
#define PIN_UP					1


//==================================================================================================
//| �������� | key_get_code
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡ����
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ɨ���õļ�ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | cc 2012-4-27
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
	static uint8_t KeyState = 0;//����״̬,״̬��ʹ��
	static uint8_t KeyPrev = 0;//������һ�ΰ���,��ֹ������������
	static uint16_t KeyHoldDly = 0;//��������ʱ��
	static uint8_t KeyBurst = 0;//��־������ʼ
	static uint8_t KeyPress = KEY_NO;//����ֵ,�ĸ�����
	static uint8_t KeyReturn = KEY_NO;//��������ֵ
	static rt_thread_t reisp=RT_NULL;
	static rt_thread_t reiap=RT_NULL;

//	rt_mb_init(&mb_key1,"kmb1",&mb_key1_pool,sizeof(mb_key1_pool)/4,0);
	while(1)
	{
		rt_thread_delay(10);
		key_va = rt_pin_read(KEY_DOWNLOAD_BEGIN);
				
		#if KEY_DOUBLE_ENABLE == 1
		static uint8_t KeyDbleTmp = 0;//������˫���¼�
		static uint8_t KeyDbleDly = 0;//����˫�����ʱ��
		#endif

		uint8_t key_value = KEY_NO;

		KeyPress = key_get_code();

		#if KEY_DOUBLE_ENABLE == 1
		if(KeyDbleDly > 0)//����˫��������
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
			case 0://������ʼ̬00
				if(KeyPress != KEY_NO)
				{
					KeyState = 1;
					KeyPrev = KeyPress;
				}
				break;
			case 1://����ȷ��̬01
				if(KeyPress != KEY_NO)
				{
					if(KeyPrev != KeyPress)//���ϴΰ�����ͬ,������
					{
						KeyState = 0;
					}
					else
					{
						//˫�������ڰ�������֮ǰ
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

						//����Ϊ�������´���
						KeyState = 2;
						KeyReturn = KEY_PROPERTY_SHORT | KeyPress;//���ذ����̰���ֵ

						//�����ϴΰ������µļ�,����˫����
						#if KEY_DOUBLE_ENABLE == 1
						KeyDbleTmp = KeyPress;
						KeyDbleDly = KEY_DOUBLE_SPACE;
						#endif
					}
				}
				else//����̧��,�Ƕ���,����Ӧ����
				{
					KeyState = 0;
				}
				break;
			case 2://��������̬11
				if(KeyPress != KEY_NO)
				{//����ط�Ҳ������� (KeyPress == KEY_1) || (KeyPress == KEY_2)
				 //�������ԶԲ��������İ�����������
					KeyHoldDly++;
					if((KeyBurst == 1) && (KeyHoldDly > KEY_BURST_SPACE))
					{
						KeyHoldDly = 0;
						KeyReturn = KEY_PROPERTY_BURST | KeyPress;//���������ļ�ֵ
						//key_value = KEY_PROPERTY_SHORT | (KeyReturn & 0x3F);
						key_value = KeyReturn;
						KeyPrev = KeyPress;//��ס�ϴεİ���
						break;
					}

					if(KeyHoldDly > KEY_BURST_FIRST)
					{
						KeyBurst = 1;
						KeyHoldDly = 0;
						KeyReturn = KEY_PROPERTY_LONG | KeyPress;//���س�����ļ�ֵ
						key_value = KeyReturn;
						break;
					}
				}
			case 3://�����ͷ�̬10
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

		if((key_value == KEY_OK_SIG)&&(KEY_ONCE))//�а���
		{
//			key_value = key_value &0x0f;//����������������ͬ
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


//����key�߳�
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



