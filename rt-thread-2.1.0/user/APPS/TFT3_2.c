#include "tft3_2.h" 
#include "sys.h"


//=============================================================================================================

//定义背光
#define LCD_BL(n)		\
	{if(n)				\
	{PCout(7)=0;}		\
	else 				\
	{PCout(7)=1; }}						
//
//定义触摸屏片选
#define LCD_T(n)		\
	{if(n)				\
	{PBout(12)=0;}		\
	else 				\
	{PBout(12)=1; }}						
//	
//定义SD卡片选
#define LCD_SD(n)		\
	{if(n)				\
	{PBout(12)=0;}		\
	else 				\
	{PBout(12)=1; }}						
//	
//定义触摸屏片选
#define LCD_FLASH(n)		\
	{if(n)				\
	{PBout(12)=0;}		\
	else 				\
	{PBout(12)=1; }}						
//	
	
static void init(void)
{
	LCD_BL(0);
//	PCout(7) = 1;
}

/**************************************************************************************/
const struct  _tftlcd3_2_ API_LCD3_2 = 
{
	init,
};
//











