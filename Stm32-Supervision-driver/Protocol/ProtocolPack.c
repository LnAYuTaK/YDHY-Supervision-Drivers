#include "ProtocolPack.h"
#include "stddef.h"
#include "gps.h"
#include "time.h"
#include "StateInfo.h"
// PackHead Buffer 
static const uint8_t PackHeadBuf[2] = {0xFB,0x90};
// 上电时间戳记录
static int timeFlag ;
static uint32_t LastLat ;
static uint32_t LastLon ;
static uint32_t StartTimeTemp ;
static double flightDis = 0;
// 检查gps 数据是否有效
// 经纬度不为0 日期超过2023年且HDOP小于5
PackState CheckGpsState(Gps_Msg_t *msg)
{
   nmeaTIME beiJingTime; 
   GMTconvert(&(msg->UtcTime),&beiJingTime,8,1);
   if((msg->Hdop<=5) && (msg->Lat != 0)&& (msg->Lon!=0)&&(beiJingTime.year+1900 >= 2023))
   { 
      ModuleState.GPSModle.State= true;                             
      return PACK_OK;
   }
   else
   {
      ModuleState.GPSModle.State= false;
      return PACK_ERROR;
   }
}
//检测压力计和流速计是否有数值
PackState CheckDriverState(MeterMsg_t *drvmsg)
{
   return PACK_OK;
}

//根据4G模块状态切换指示灯
PackState Net4G_decode(Net4GPack_t *msg)
{
    uint8_t * header = (uint8_t *)&(msg->PackHead);
    if( (header[0]== 0xFB )&& (header[1] == 0x90)) 
    {
         if(msg->Net4GState == 0)
         {
            ModuleState.NET_4gModule.State = true;
         }
         else if (msg->Net4GState == 1)
         {
           ModuleState.NET_4gModule.State = false;
         }
         if(msg->SDCardState == 0)
         {
            ModuleState.SDCardModle.State = true;
         }
         else if (msg->SDCardState == 1)
         {
            ModuleState.SDCardModle.State = false;
         } 
         if(msg->UploadState == 0)
         {
             ModuleState.ReSendModule.State = true;
         }
         else if(msg->UploadState == 1)
         {
            ModuleState.ReSendModule.State = false;
         }
         return PACK_OK;
    }
    else
    {
      return PACK_ERROR;
    }
}
//协议打包
Pack_t * MakePack(Gps_Msg_t *msg,MeterMsg_t *drvmsg)
{
   uint32_t TimeStamp = DateTimeToTimeStamp(msg->UtcTime.year+1900,
                msg->UtcTime.mon,
                msg->UtcTime.day,
                msg->UtcTime.hour,
                msg->UtcTime.min,
                msg->UtcTime.sec);
   //这里申请了别忘了外边释放！！
   Pack_t *pack  =(Pack_t *) malloc(sizeof(Pack_t)); 
   //Pack Head
   memcpy(pack,PackHeadBuf,2);
   //上电时间戳(只记录一次)
   if(timeFlag == 0 ){
      StartTimeTemp = (TimeStamp);
      timeFlag = 1 ;
   }
   pack->StartTimeStamp = SW32(StartTimeTemp);
   //设备ID
   pack->DirverId = SW16((uint16_t)(1));
   //经度
   pack->Lon      = SW32(((msg->Lon)*(10000000)));
   //纬度
   pack->Lat      = SW32(((msg->Lat)*(10000000)));
   //高度
   pack->Elv      = __REVSH(((msg->Height)*(10)));
   //速度
   pack->Speed    = SW16(((msg->Speed)*(100)));
   //当前GPS时间戳(不断更新)
   pack->GpsTimeStamp = SW32(TimeStamp); 
   //流速计
   pack->flowMeter1 = SW16(drvmsg->flowMeter1);
   pack->flowMeter2 = SW16(drvmsg->flowMeter2);
   pack->flowMeter3 = SW16(drvmsg->flowMeter3);
   pack->flowMeter4 = SW16(drvmsg->flowMeter4);
   //压力计
   pack->PressureGage1 = SW16(drvmsg->PressureGage1);
   pack->PressureGage2 = SW16(drvmsg->PressureGage2);
   pack->PressureGage3 = SW16(drvmsg->PressureGage3);
   pack->PressureGage4 = SW16(drvmsg->PressureGage4);
   //预留数据
   pack->SerialPadding = SW32(0);
   //本架次行驶距离
   double Nowlon =    msg->Lon;
   double Nowlat =    msg->Lat;
   if(CheckDriverState(drvmsg)!= PACK_ERROR)
   {
      //结算距离
      if((LastLon != 0)||(LastLat!=0))
      {
        flightDis = flightDis+ GetDistance(LastLon,LastLat,Nowlon,Nowlat);
      }
   }
   pack->FlightDistance = (SW32((uint32_t)flightDis));
   //更新经纬度
   LastLon = msg->Lon;
   LastLat = msg->Lat;
   //校验 不包含包头
   uint8_t buf[sizeof(Pack_t)-2] = {0};
   uint8_t checkSumTemp = 0;
   memcpy(buf,&(pack->StartTimeStamp),sizeof(buf));
   for(int i = 0 ; i < sizeof(buf);i++)
   {
      checkSumTemp  += buf[i];
   }
   pack->CheckSum = checkSumTemp;
   return pack;
}




