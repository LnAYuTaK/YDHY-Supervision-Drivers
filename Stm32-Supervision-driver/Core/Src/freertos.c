/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "UserApp.h"
#include "stdio.h"
#include "stdlib.h"
// #include "wwdg.h"
// #include "led.h"
#include "gps.h"
#include "ProtocolPack.h"
#include "fatfs.h"
#include "led.h"
#include "StateInfo.h"
#include "tim.h"
#include "MeteringDrivers.h"
#include "adc.h"
#include "string.h"
#include "gpio.h"
#include "config.h"
#include "timers.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osMessageQId GpsMsgHandle ;
osMessageQDef(GpsQueue,5, sizeof(Gps_Msg_t));

osMessageQId DriversMsgHandle;
osMessageQDef(DriversQueue,5, sizeof(MeterMsg_t));

osThreadId defaultTaskHandle;

osThreadId myTask02Handle;

osThreadId drvRecvHandle; 

TimerHandle_t NetStateTimer;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartDrvRecvTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */

void TimerCallBack( TimerHandle_t xTimer )
{
  ModuleState.NET_4gModule.State = false;
}

void MX_FREERTOS_Init(void) {
  //临界保护下任务保证任务完整创建
  //开启临界区
  taskENTER_CRITICAL();
  //核心任务//
  //心跳定时检测4G模块状态 
  NetStateTimer= xTimerCreate("4GModuleState",  
                               2500,            
                               pdTRUE,         
                               (void*)1,        
                               TimerCallBack 
                              );
  osThreadDef(defaultTask, StartDefaultTask, osPriorityHigh, 0,512);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
  GpsMsgHandle = osMessageCreate(osMessageQ(GpsQueue),defaultTaskHandle);
  /* definition and creation of myTask02 */
  //指示灯任务
  osThreadDef(myTask02, StartTask02, osPriorityBelowNormal, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);
  //流速计 压力计任务//
  osThreadDef(drvRecvTask, StartDrvRecvTask, osPriorityNormal, 0, 190);
  drvRecvHandle = osThreadCreate(osThread(drvRecvTask), NULL);
  DriversMsgHandle = osMessageCreate(osMessageQ(DriversQueue),drvRecvHandle);
  //关闭临界区
  xTimerStart(NetStateTimer,20);
  taskEXIT_CRITICAL(); 
  /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  接收gps信号解码打包协议串口转发
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osEvent evt = osMessageGet(GpsMsgHandle,osWaitForever);
    osEvent evt2 = osMessageGet(DriversMsgHandle,osWaitForever);
    if ((evt.status == osEventMessage)&&(evt2.status == osEventMessage))
    {
      if(RecvFlag == 1)
      {
        Gps_Msg_t* gpsmeg = (Gps_Msg_t*)evt.value.p;
        MeterMsg_t* drvmsg = (MeterMsg_t*)evt2.value.p;
        //判断下gps数据有效性 
        // if(CheckGpsState(gpsmeg)!= PACK_ERROR)
        // {
          //协议打包                                                                                                                                                                                            
          Pack_t *pack = MakePack(gpsmeg,drvmsg);                                                                                     
          //发送给4G模块
          HAL_UART_Transmit(&huart1,(uint8_t *)pack,sizeof(Pack_t),0x200);
          RecvFlag = 0;
          free(pack);
          pack = NULL;
        // }
      }
    }
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief 状态灯显示任务
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  //初始化LED 状态灯系统
  ledDriverInit();
  for(;;)
  { 
    //检测灯状态
    led.SetState(GetSysState(&ModuleState));
  }
  /* USER CODE END StartTask02 */
}
/**
* @brief 流量计 压力计获取任务
* @param argument: Not used
* @retval None
*/
void StartDrvRecvTask(void const * argument)
{
  HAL_TIM_Base_Start(&htim1); 
  HAL_TIM_Base_Start(&htim2); 
  HAL_TIM_Base_Start(&htim3); 
  HAL_TIM_Base_Start(&htim8); 
  for(;;)
  {
    MeterMsg_t msg; 
    memset(&msg,0,sizeof(MeterMsg_t));
    msg.flowMeter1 = (uint16_t)(htim1.Instance->CNT);
    msg.flowMeter2 = (uint16_t)(htim2.Instance->CNT);
    msg.flowMeter3 = (uint16_t)(htim3.Instance->CNT);
    msg.flowMeter4 = (uint16_t)(htim8.Instance->CNT);
    #ifndef FlowMeter_DEBUG_DISABLED
    printf("FlowMeter1: %hd\n",msg.flowMeter1);
    printf("FlowMeter2: %hd\n",msg.flowMeter2);
    printf("FlowMeter3: %hd\n",msg.flowMeter3);
    printf("FlowMeter4: %hd\n",msg.flowMeter4);
    #endif 
    msg.PressureGage1  = AD_Buf[0];
    msg.PressureGage2  = AD_Buf[1];
    msg.PressureGage3  = AD_Buf[2];
    msg.PressureGage4  = AD_Buf[3];
    #ifndef PressureGage_DEBUG_DISABLED
    printf("PressureGage1: %d\n",msg.PressureGage1);
    printf("PressureGage2: %d\n",msg.PressureGage2);
    printf("PressureGage3: %d\n",msg.PressureGage3);
    printf("PressureGage4: %d\n",msg.PressureGage4);
    #endif
    htim1.Instance->CNT = 0;
    htim2.Instance->CNT = 0;
    htim3.Instance->CNT = 0;
    htim8.Instance->CNT = 0;
    osMessagePut(DriversMsgHandle,(uint32_t)&msg,50);
    osDelay(1000);
  }
}
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

