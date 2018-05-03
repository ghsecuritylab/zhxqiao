#ifndef _HLY_TEST_TOOLS_H_
#define _HLY_TEST_TOOLS_H_

#include "users.h"


// ÷°Œª÷√
#define _CTRL_CODE          1
#define _CTRL_SUN           2
#define _OP_TYPE            3
#define _DATA_LEN           4
#define _DATA_FLAG          6
#define _DATA_PTR           8
#define _CMD_LEN            9
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
struct _api_func_test
{
    void (*code)(comm_state_t * r);
};

extern const struct  _api_func_test API_TEST_TOOLS_XIE_YI;


int IsIsp_fromePCB(comm_state_t * r);


#endif
