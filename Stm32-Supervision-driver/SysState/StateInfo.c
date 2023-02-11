#include "StateInfo.h"
#include "led.h"
#include "stdio.h"
#include "UserApp.h"
//全局
ModuleStateClass_t  ModuleState = { {true},{false},{true},{true},{true}};

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
    //根据优先级判断当前状态灯显示
  else if(!(moduleState->ReSendModule.State))
  {
    return ReSendData;
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

  return 0;
}

