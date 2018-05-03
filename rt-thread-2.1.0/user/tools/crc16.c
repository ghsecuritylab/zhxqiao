#include "crc16.h"

uint16_t GetCrc16(uint8_t *pucData, uint16_t wDataLen)
{
    uint8_t ucCheck, ucHigh, ucLow;
    uint16_t i, j, wCrc;
    wCrc = 0xFFFF;
    for(i=0; i<wDataLen; i++)
    {
        wCrc ^= pucData[i];
        for(j=0; j<8; j++)
        {
            ucCheck = wCrc & 1;
            wCrc >>= 1;
            wCrc &= 0x7FFF;
            if (1 == ucCheck)
            {
                wCrc ^= 0xA001;
            }
        }
    }
    ucHigh = wCrc & 0x00FF;
    ucLow  = wCrc >> 8;
    wCrc   = (ucHigh << 8) | ucLow;
    return wCrc;
}



