#include "delay.h"

void delay_ms(uint32_t a)
{
//    LL_mDelay(a);
	rt_thread_delay(a);
}//

void delay_us(uint32_t a)
{
	for(uint32_t j=0;j<a;j++)
		for(uint8_t i=0;i<14;i++);
}//
//



