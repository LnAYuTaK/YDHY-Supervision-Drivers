#ifndef __MeteringDrivers_H
#define	__MeteringDrivers_H
#include "UserApp.h"
#include "adc.h" 
typedef struct MeterMsg_s
{
    uint16_t  flowMeter1;    //流速计1 
    uint16_t  flowMeter2;    //流速计2 
    uint16_t  flowMeter3;    //流速计3
    uint16_t  flowMeter4;    //流速计4
    uint16_t  PressureGage1; //压力计1 
    uint16_t  PressureGage2; //压力计2
    uint16_t  PressureGage3; //压力计3
    uint16_t  PressureGage4; //压力计4
}MeterMsg_t;
extern  uint16_t AD_Buf[40];
#endif