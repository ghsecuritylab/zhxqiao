#include "users.h"
#include "drivers/pin.h"


static struct rt_thread key_thread;
ALIGN(RT_ALIGN_SIZE)
static char key_stack[0x110];

static void key_thread_entry(void * p)
{
		static uint8_t key_va;

	while(1)
	{
		rt_thread_delay(10);
		key_va = rt_pin_read(KEY_DOWNLOAD_BEGIN);
		
		
		
		if(1)
		{
			usart2_mailbox_send((rt_uint32_t)(&key_va));
		}
	}
}
//
#include "api.h"

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

void key_init(void)
{
    P2DIR &= ~(0x1F);
    P2IES |= (0x1F);
    keyIntEnable();
}

void keyIntEnable(void)
{
    P2IFG = 0;
    P2IE |= (0x1F);
}
void keyIntDisable(void)
{
    P2IE &= ~(0x1F);
}

//==================================================================================================
//| �������� | key_get_code
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡ����
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ɨ���õļ�ֵ
//|----------|--------------------------------------------------------------------------------------
//| ������� | cc 2012-4-27
//==================================================================================================
#pragma optimize=none
static uint8_t key_get_code(void)
{
    if(!(P2IN & (BIT1 + BIT2))){
        return Q_KEY_RFSET_SIG;
    }
    if(!(P2IN & BIT0)){
        return Q_KEY_OK_SIG;
    } 
    if(!(P2IN & BIT1)){
        return Q_KEY_DOWN_SIG;
    }
    if(!(P2IN & BIT2)){
        return Q_KEY_UP_SIG;
    }
    if(!(P2IN & BIT3)){
        return Q_KEY_RETURN_SIG;
    }
    if(!(P2IN & BIT4)){
        return Q_KEY_OK_SIG;
    }
    return KEY_NO;
}


#pragma optimize=none
void key_process(void)
{
    static uint8_t KeyState = 0;//����״̬,״̬��ʹ��
    static uint8_t KeyPrev = 0;//������һ�ΰ���,��ֹ������������
    static uint16_t KeyHoldDly = 0;//��������ʱ��
    static uint8_t KeyBurst = 0;//��־������ʼ
    static uint8_t KeyPress = KEY_NO;//����ֵ,�ĸ�����
    static uint8_t KeyReturn = KEY_NO;//��������ֵ

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

    if(key_value != KEY_NO)//�а���
    {
        qmsg_push2Hmi(key_value);//���Ͱ�����Ϣ������
        qmsg_push2Hmi(Q_KEY_ANY_SIG); //���������Ϣ
        resetTkeyCount();
    }
}
























