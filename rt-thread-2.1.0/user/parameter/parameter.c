#include "users.h"
// ----------------------------------------------------------------------------

#include <string.h>

// ----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////////////////////////////

PARAMETER sys_parameter =				// 21 ���ò���
{    
    .ucDevTypeNumb  = 0x00,				// 1B �豸���к�
    .ucVersion      = 0x01,                             // 1B ����汾
    .ucDate         = 0x0201,                           // 2B �����޸�����
    .SetLightLevel = 125,
};
// ----------------------------------------------------------------------------
//cfs_coffee_arch_write(const void *buf, unsigned int size, cfs_offset_t offset)
//cfs_coffee_arch_read(void *buf, unsigned int size, cfs_offset_t offset)
//W25Qx_SYS_PARA
int
Parameter_init(void)
{
	PARAMETER Parameter;

	uint16_t PARA_len;

	PARA_len =  sizeof(PARAMETER);

	if(PARA_len>1024)return 1;
	
	API_W25Q128.Read((void* )&Parameter,W25Qx_SYS_PARA,PARA_len);
        
//        cfs_coffee_arch_read(&Parameter,PARA_len,0);

//	AT24C64_API.Read_Sequential(ParameterFirstAddress,&Parameter,PARA_len) ;

	if(( Parameter.dingweihao == 0xaaaa)&&(Parameter.ThisStructLen == PARA_len))
	{
            memcpy(&sys_parameter,&Parameter,PARA_len);
        }	
        else
        {                
			parameter_save();
        }
    // ------------------------------------------------------------------------

//        process_start(&PAR_process, NULL);
        
	/*����LCD��ʾ�ĶԱȶ�*/
//	Set_ContrastLevel(sys_parameter.SetContrastLevel);

	/* ���ÿ�����ʾ���¶����ݵ�ҳ�������4ҳ��һҳ12���� */
//	Set_TheDataPage(sys_parameter.SetPage);

	/* ���ñ��ⳬʱʱ�� */
//	BlackLight_Set(sys_parameter.SetBacklightTimeout);

	/* ���ð����� */
//	Set_Beep_Enable(sys_parameter.SetButtonTone);

	/*  */
	/*  */
	/*  */
	/*  */
	/*  */
    return 0;
}
// ----------------------------------------------------------------------------

int
parameter_save(void)
{
    // ------------------------------------------------------------------------
	uint16_t PARA_len;
	char flag=0;

	PARA_len =  sizeof(sys_parameter);

	sys_parameter.ThisStructLen = PARA_len;
	sys_parameter.dingweihao = 0xaaaa;
	flag = (char)API_W25Q128.Write((void* )&sys_parameter,W25Qx_SYS_PARA,PARA_len);
          
	PARAMETER Parameter;

	if(PARA_len>1024)return 1;
	
	API_W25Q128.Read((void* )&Parameter,W25Qx_SYS_PARA,PARA_len);
//	flag = AT24C64_API.Write_Data(ParameterFirstAddress,&sys_parameter,PARA_len);

//	Parameter_init();

	return flag;
}
// ----------------------------------------------------------------------------

char * Get_cVersion(void)
{
	// ����汾��
	return "CLDL1.0.1R1";
}
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

//---------------------------------------------------------------------------

/*---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/*---------------------------------------------------------------------------*/




