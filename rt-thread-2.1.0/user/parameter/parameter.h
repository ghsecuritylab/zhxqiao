#ifndef _PARAMETER_H_
#define _PARAMETER_H_
/*---------------------------------------------------------------------------*/

#include <stdint.h>
/*---------------------------------------------------------------------------*/

typedef struct	        // 21 配置参数
{    
    uint16_t     ucDevTypeNumb;           // 1B 设备序列号
    
    uint16_t     SetLightLevel;           //亮度 0-255
    uint16_t     ucVersion  ;             //程序版本
    uint16_t     ucDate;                  //程序日期
    
	uint32_t 	 ispDATA_LEN;	//保存isp代码的总数
	uint32_t 	 iapDATA_LEN;	//保存iap代码的总数
    
	///////////////=====================================----------------//////////////
	uint16_t	dingweihao;		//
    uint16_t     ThisStructLen;                              //本结构体占用的字节数
    uint16_t iCRC_values;                                    //crc值
	
}PARAMETER;

/*---------------------------------------------------------------------------*/
extern PARAMETER sys_parameter;

int Parameter_init(void);
int parameter_save(void);
/*---------------------------------------------------------------------------*/

char *   Get_cVersion(void);

/*---------------------------------------------------------------------------*/

#endif /* _PARAMETER_H_ */
