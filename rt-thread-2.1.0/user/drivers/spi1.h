#ifndef	_SPI1_H_
#define _SPI1_H_
#include "sys.h"



uint8_t SPI1_Receive(uint8_t * buf,uint32_t len,uint32_t timeout);
uint8_t SPI1_Transmit(uint8_t * buf,uint32_t len,uint32_t timeout);
int stm32_spi1_init(void);

#endif

