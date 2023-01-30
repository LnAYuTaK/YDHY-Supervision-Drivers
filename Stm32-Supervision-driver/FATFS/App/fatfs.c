/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
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
#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */  
/* USER CODE BEGIN Variables */
#include "stdio.h"
#include "config.h"
#include "StateInfo.h"
FRESULT res;
UINT bTW;
UINT bW;  
FILINFO fno; 
/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  #ifdef SDCardTest 
  res = f_mount(&SDFatFS, "0:", 1);
  if(res)
  {   
    printf(" mount error : %d \r\n",res);
  }
  else
  {
    printf(" mount sucess!!! \r\n");
  }
  #endif
  /* USER CODE END Init */
}

int SaveDataToSDCard(Pack_t *pack)
{
  nmeaTIME time = TimeStamp2DateTime(pack->GpsTimeStamp);
  char Temp[20] = {0};
  int j = sprintf(Temp,"%d",time.year);
  j    += sprintf(Temp+j,"%d",time.mon);
  j    += sprintf(Temp+j,"%d\n",time.day);
  if(f_open(&SDFile,Temp,FA_WRITE|FA_OPEN_ALWAYS)== FR_OK)
  {
     printf("OK\n");
  }
  else
  {
    printf("False\n");
  }
    return 0;
}


/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */

/* USER CODE END Application */
