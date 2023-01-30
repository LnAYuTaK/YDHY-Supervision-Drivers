#ifndef __ProtocolPackag_H
#define	__ProtocolPackag_H

#include "stdio.h"
#include "stdint.h"
#include "gps.h"
#include <malloc.h>
#include "math.h"
#include "MeteringDrivers.h"
#include "stdbool.h"
//大小端转换
#define sw16(A) ((((uint16_t)(A) & 0xff00) >> 8) | (((uint16_t)(A) & 0x00ff) << 8))
#define sw32(A) ((((uint32_t)(A) & 0xff000000) >> 24) | (((uint32_t)(A) & 0x00ff0000) >> 8) | (((uint32_t)(A) & 0x0000ff00) << 8) | (((uint32_t)(A) & 0x000000ff) << 24))
#define sw64(A) ((uint64_t)(\
				(((uint64_t)(A)& (uint64_t)0x00000000000000ffULL) << 56) | \
				(((uint64_t)(A)& (uint64_t)0x000000000000ff00ULL) << 40) | \
				(((uint64_t)(A)& (uint64_t)0x0000000000ff0000ULL) << 24) | \
				(((uint64_t)(A)& (uint64_t)0x00000000ff000000ULL) << 8) | \
				(((uint64_t)(A)& (uint64_t)0x000000ff00000000ULL) >> 8) | \
				(((uint64_t)(A)& (uint64_t)0x0000ff0000000000ULL) >> 24) | \
				(((uint64_t)(A)& (uint64_t)0x00ff000000000000ULL) >> 40) | \
				(((uint64_t)(A)& (uint64_t)0xff00000000000000ULL) >> 56) ))

#define u64 uint64_t
#define SW16(A)	sw16(((uint16_t)A))	
#define SW32(A)	sw32(((uint32_t)A))
#define SW64(A)	sw64(((uint64_t)A))	

typedef enum 
{
   PACK_OK         = 0,
   PACK_ERROR      = 1
}PackState;

//发送 4G模块通信包
#pragma pack(push, 1)
typedef struct ProtocolPackag_s
{
   uint16_t  PackHead;      //包头 Header
   uint32_t  StartTimeStamp;//上电时间戳
   uint16_t  DirverId;      //设备ID
   uint32_t  Lon;           //经度                                                                                                                                                                                                                            
   uint32_t  Lat;           //纬度
   int16_t   Elv;           //高度
   uint16_t  Speed;         //速度
   uint32_t  GpsTimeStamp;  //Gps数据时间戳
   uint16_t  flowMeter1;    //流速计1 
   uint16_t  flowMeter2;    //流速计2 
   uint16_t  flowMeter3;    //流速计3
   uint16_t  flowMeter4;    //流速计4
   uint16_t  PressureGage1; //压力计1 
   uint16_t  PressureGage2; //压力计2
   uint16_t  PressureGage3; //压力计3
   uint16_t  PressureGage4; //压力计4
   uint32_t  SerialPadding; //串口预留数据
   uint32_t  FlightDistance;//本架次行驶距离	
   uint8_t   CheckSum;      //校验和
}Pack_t;
#pragma pack(pop)

//接收4G模块状态通信包
#pragma pack(push, 1)
//State 0 ERROR 
typedef struct Net4GPack_s
{
  uint16_t  PackHead; //包头 Header
  uint8_t   SDCardState;//Sd卡状态 
  uint8_t   Net4GState;//4G模块网络状态  
  uint8_t   UploadState;//ftp上传状态
  uint8_t   CheckSum;  //校验和
   /* data */
}Net4GPack_t;
#pragma pack(pop)

//FTP 任务通信包
#pragma pack(push, 1)
typedef struct ftpTaskPack_s
{
   uint16_t  PackHead; 

}ftpTaskPack_t;
#pragma pack(pop)

Pack_t * MakePack(Gps_Msg_t *msg,MeterMsg_t *drvmsg);
//GPS数据是否有效无效就不发//
PackState CheckGpsState(Gps_Msg_t *msg);

PackState Net4G_decode(Net4GPack_t *msg);


#endif  //__ProtocolPackag_H