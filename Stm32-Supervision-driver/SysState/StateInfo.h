#ifndef __SysState_H
#define __SysState_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "stdbool.h"
//获取返回当前设备状态//
//枚举数值越大优先级越高// 
/*  电压报警    6 
    GPS错误     5
    内存卡错误   4   
    4G错误      3  
    补发数据    2 
    正常工作    1 
*/
#define GreenOneFlash    1 
#define BlueOneFlash     2 
#define RedTwoFlash      3
#define RedthreeFlash    4
#define RedOneFlash      5
#define RedLight         6
//系统状态警告灯  
typedef enum {
  Normal        = GreenOneFlash, 
  ReSendData    = BlueOneFlash, 
  SDCard_Error  = RedTwoFlash ,
  NET_4g_Error  = RedthreeFlash,
  GPS_Error     = RedOneFlash, 
  Voltage_Error = RedLight
}SysState;
//State TRUE  OK 
//State FAlSE ERROR
typedef struct ModuleState_s
{
  bool State;
}ModuleState_t;
//各个模块状态
typedef struct ModuleStateClass
{
  ModuleState_t ReSendModule;
  ModuleState_t NET_4gModule;
  ModuleState_t GPSModle;
  ModuleState_t SDCardModle;
  ModuleState_t VoltageModule;
}ModuleStateClass_t;
//获取系统当前应该显示的状态
SysState GetSysState(ModuleStateClass_t *moduleState);
//当前系统各个模块状态结构体
extern ModuleStateClass_t  ModuleState;
#ifdef __cplusplus
}
#endif
#endif 