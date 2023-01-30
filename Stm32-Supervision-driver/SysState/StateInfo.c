#include "StateInfo.h"
#include "led.h"
#include "stdio.h"
ModuleStateClass_t  ModuleState = { {true},{true},{true},{true},{true}};

SysState GetSysState(ModuleStateClass_t *moduleState)
{
  if((moduleState->GPSModle.State)
    &&(moduleState->NET_4gModule.State)
    &&(moduleState->ReSendModule.State)
    &&(moduleState->VoltageModule.State)
    &&(moduleState->SDCardModle.State))
  {
    return Normal;
  }
  //根据优先级判断当前状态灯显示
  else if(!(moduleState->VoltageModule.State))
  {
    return Voltage_Error;
  }
  else if(!(moduleState->GPSModle.State))
  {
    return GPS_Error;
  }
  else if(!(moduleState->NET_4gModule.State))
  {
    return NET_4g_Error;
  }
  else if(!(moduleState->SDCardModle.State))
  {
    return SDCard_Error;
  }
  else if(!(moduleState->ReSendModule.State))
  {
    return ReSendData;
  }
  return 0;
}

