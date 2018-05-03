
#include "hly_testtool.h"

int IsIsp_fromePCB(comm_state_t * r)
{
    uint16_t Len,Flag;
    Flag = (uint16_t)(((r->tempbuf[_DATA_FLAG] )<<8) + (r->tempbuf[_DATA_FLAG + 1]));
    
    r->tempbuf[_OP_TYPE] = 0xaa;
    Len = r->ucBufLen + 2;
    r->tempbuf[_DATA_LEN]       = (uint8_t)Len >> 8;
    r->tempbuf[_DATA_LEN + 1]   = (uint8_t)Len;
    Flag <<= 1;
    r->tempbuf[_DATA_FLAG]       = (uint8_t)Flag >> 8;
    r->tempbuf[_DATA_FLAG + 1]   = (uint8_t)Flag;
    memcpy(&(r->tempbuf[_DATA_PTR]),r->ucBuf,r->ucBufLen);
    r->ucBufLen += _DATA_PTR;
    API_USART1.giveCRCEnd(r->tempbuf,&(r->ucBufLen));
    
    memcpy(r->ucSend,r->tempbuf ,r->ucBufLen);
    r->ucSendLen = r->ucBufLen;
    
    API_USART2.Usart2ToUsart1(r);
    
    return 1;
}//
//







