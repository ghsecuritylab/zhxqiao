#include "users.h"
// ----------------------------------------------------------------------------

#include <string.h>

// ----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////////////////////////////

PARAMETER sys_parameter =				// 21 配置参数
{    
    .ucDevTypeNumb  = 0x00,				// 1B 设备序列号
    .ucVersion      = 0x01,                             // 1B 程序版本
    .ucDate         = 0x0201,                           // 2B 程序修改日期
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
        
	/*设置LCD显示的对比度*/
//	Set_ContrastLevel(sys_parameter.SetContrastLevel);

	/* 设置可以显示的温度数据的页数，最多4页，一页12个点 */
//	Set_TheDataPage(sys_parameter.SetPage);

	/* 设置背光超时时间 */
//	BlackLight_Set(sys_parameter.SetBacklightTimeout);

	/* 设置按键音 */
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
	// 软件版本号
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




