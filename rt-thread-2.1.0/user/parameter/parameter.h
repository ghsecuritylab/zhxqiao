#ifndef _PARAMETER_H_
#define _PARAMETER_H_
/*---------------------------------------------------------------------------*/

#include <stdint.h>
/*---------------------------------------------------------------------------*/

typedef struct	        // 21 ���ò���
{    
    uint16_t     ucDevTypeNumb;           // 1B �豸���к�
    
    uint16_t     SetLightLevel;           //���� 0-255
    uint16_t     ucVersion  ;             //����汾
    uint16_t     ucDate;                  //��������
    
	uint32_t 	 ispDATA_LEN;	//����isp���������
	uint32_t 	 iapDATA_LEN;	//����iap���������
    
	///////////////=====================================----------------//////////////
	uint16_t	dingweihao;		//
    uint16_t     ThisStructLen;                              //���ṹ��ռ�õ��ֽ���
    uint16_t iCRC_values;                                    //crcֵ
	
}PARAMETER;

/*---------------------------------------------------------------------------*/
extern PARAMETER sys_parameter;

int Parameter_init(void);
int parameter_save(void);
/*---------------------------------------------------------------------------*/

char *   Get_cVersion(void);

/*---------------------------------------------------------------------------*/

#endif /* _PARAMETER_H_ */
