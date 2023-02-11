#include "led.h"
#include "cmsis_os.h"
#include "stdio.h"
ledDriver_t led;
//绿灯慢闪
void  SetGreenOneFlash()
{
  HAL_GPIO_TogglePin(GPIOC,RGB_GPIO_G_Pin);
  HAL_GPIO_WritePin(GPIOC,RGB_GPIO_B_Pin,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC,RGB_GPIO_R_Pin,GPIO_PIN_SET);
  osDelay(500);
}
//蓝灯慢闪
void  SetBlueOneFlash()
{
  HAL_GPIO_TogglePin(GPIOC,RGB_GPIO_B_Pin);
  HAL_GPIO_WritePin(GPIOC,RGB_GPIO_G_Pin,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC,RGB_GPIO_R_Pin,GPIO_PIN_SET);
  osDelay(500);
}
//红灯慢闪
void  SetRedOneFlash()
{
  HAL_GPIO_TogglePin(GPIOC,RGB_GPIO_R_Pin);
  HAL_GPIO_WritePin(GPIOC,RGB_GPIO_B_Pin,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC,RGB_GPIO_G_Pin,GPIO_PIN_SET);
  osDelay(500);
}
//红灯双闪
void  SetRedTwoFlash()
{
  HAL_GPIO_TogglePin(GPIOC,RGB_GPIO_R_Pin);
  HAL_GPIO_WritePin(GPIOB,RGB_GPIO_B_Pin,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC,RGB_GPIO_G_Pin,GPIO_PIN_SET);
  osDelay(300);
}
//红灯三闪
void  SetRedThreeFlash()
{
  HAL_GPIO_TogglePin(GPIOC,RGB_GPIO_R_Pin);
  HAL_GPIO_WritePin(GPIOC,RGB_GPIO_B_Pin,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC,RGB_GPIO_G_Pin,GPIO_PIN_SET);
  osDelay(100);
}
//红灯常亮
void  SetRedLight()
{
  HAL_GPIO_WritePin(GPIOC,RGB_GPIO_R_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC,RGB_GPIO_G_Pin,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC,RGB_GPIO_B_Pin,GPIO_PIN_SET);
}

//设置系统灯状态//多状态优先级判断
void SetSysState(SysState state)
{
  //根据各个模块状态判断当前系统状态
  //再次设置的时候根据优先级判断系统的状态
  switch (state)
  {
  case Normal:
    SetGreenOneFlash();
  break;
  case ReSendData:
    SetBlueOneFlash();
  break;
  case SDCard_Error:
    SetRedTwoFlash();
  break;
  case NET_4g_Error:
    SetRedThreeFlash();
  break;
  case GPS_Error:
    SetRedOneFlash();
  break;
  case Voltage_Error:
    SetRedLight();
  break;
  default:
  break;
  }
}
//led驱动初始化//开始设置为正常工作
ledDriver_t  ledDriverInit()
{
  led.SetState  = SetSysState;
  return  led;
}



