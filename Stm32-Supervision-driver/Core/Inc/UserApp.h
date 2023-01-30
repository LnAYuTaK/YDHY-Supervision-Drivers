#ifndef __UserApp_H__
#define __UserApp_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "cmsis_os.h"
#include "usart.h"

extern osMessageQId GpsMsgHandle;
extern osMessageQId DriversMsgHandle;

extern osThreadId defaultTaskHandle;

extern osThreadId myTask02Handle;

extern osThreadId drvRecvHandle; 

#ifdef __cplusplus
}
#endif
#endif //__UserApp_H__