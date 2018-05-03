#include "users.h"



//SPI1速度设置函数
//SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2时钟一般为84Mhz：
void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	SPI1->CR1&=0XFFC7;//位3-5清零，用来设置波特率
	SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPI1,ENABLE); //使能SPI1
} 
//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
uint8_t SPI1_ReadWriteByte(uint8_t TxData,uint32_t timeout)
{		 			 
	uint32_t j=0;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
	{
		for(uint16_t i=0;i<0xff;i++);
		if(j>timeout)
			return 1;
		j++;
	}//等待发送区空  
	
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个byte  数据
		
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
	{
		for(uint16_t i=0;i<0xff;i++);
		if(j>timeout)
			return 1;
		j++;
	} //等待接收完一个byte  
 
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据	
}
//

void SPI_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);  //开启时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);  //开启时钟
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  //引脚初始化
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //引脚初始化
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
	PAout(15) = 1;
	
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1);  //打开引脚的复用功能
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);
}
//

void SPI_Config(void)
{
    SPI_GPIOConfig();
	
	//这里只针对SPI口初始化
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//复位SPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//停止复位SPI1
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);  //时钟
	
	SPI_InitTypeDef SPI_InitStructure;
    
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //全双工模式
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;   //作为主机使用
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   //数据长度8
    SPI_InitStructure.SPI_CPOL  = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //软件设置NSS功能
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1,&SPI_InitStructure);
    SPI_Cmd(SPI1,ENABLE);
	
//	SPI1_ReadWriteByte(0xff,1000);//启动传输	
}//
//
//void MySPI_SendData(char da)
//{
//    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);
//    SPI_SendData(SPI1,da);
//}
////

//uint8_t MySPI_ReceiveData(void)
//{
//    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET);
//    return SPI_ReceiveData(SPI1);
//}
//

uint8_t SPI1_Transmit(uint8_t * buf,uint32_t len,uint32_t timeout)
{
	for(uint32_t i=0;i<len;i++)
		SPI1_ReadWriteByte(*buf++,timeout);
	
	return 0;
}
//
uint8_t SPI1_Receive(uint8_t * buf,uint32_t len,uint32_t timeout)
{
	for(uint32_t i=0;i<len;i++)
		* buf++ = SPI1_ReadWriteByte(0xff,timeout);
	
	return 0;
}
//
int stm32_spi1_init(void)
{
	SPI_Config();
	return 0;
}
//

