#ifndef __LED_H
#define __LED_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "gpio.h"
#include "usart.h"
#include "StateInfo.h"
#include "stdbool.h"
typedef struct ledDriver_s
{
  void (* SetState)(SysState state);
}ledDriver_t;
//全局LED 设备
extern ledDriver_t led;
ledDriver_t  ledDriverInit();
#ifdef __cplusplus
}
#endif
#endif //__LED_H
