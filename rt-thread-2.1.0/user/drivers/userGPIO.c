#include "usergpio.h"
#include <rtdevice.h>

//#define SPI1_W25Q_PIN			(0)
#define SPI2_LCD_T_PIN			(3)
#define LCD_BL_PIN				(1)


//#define SPI1_W25Q_CS(n)			rt_pin_write(SPI1_W25Q_PIN,n)
#define SPI2_LCD_T_CS(n)		rt_pin_write(SPI2_LCD_T_PIN,n)
//#define SPI2_LCD_W25Q_CS(n)		rt_pin_write(SPI2_CS_PIN,n)
//#define SPI2_LCD_SD_CS(n)		rt_pin_write(SPI2_CS_PIN,n)


#define LCD_BL(n)		rt_pin_write(LCD_BL_PIN,n)



int bsp_gpio_init(void)
{
//	rt_pin_mode(SPI1_W25Q_PIN,PIN_MODE_OUTPUT);
	rt_pin_mode(SPI2_LCD_T_PIN,PIN_MODE_OUTPUT_PP);
	rt_pin_mode(LCD_BL_PIN,PIN_MODE_OUTPUT_OD);
	
//	SPI1_W25Q_CS(1);
	SPI2_LCD_T_CS(1);
	LCD_BL(0);
	
	return 0;
}




