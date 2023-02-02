#ifndef __GPS_H
#define	__GPS_H
//负责GPS 解码与写入文件 
#include "Middlewares\Third_Party\Nmea\include\nmea\nmea.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
//原始的GPS 数据
typedef struct GpsMsg_s
{                                                                                                                                                                                                                                                                                                                          ;
   double Lat;
   double Lon;
   double Height;
   double Speed;
   double Hdop;  
   nmeaTIME UtcTime;
}Gps_Msg_t;

int GPS_decode();

void DebugStr(const char *str, int str_size);

nmeaTIME  TimeStamp2DateTime(uint32_t Stamp);

uint32_t DateTimeToTimeStamp (unsigned int year, unsigned int mon,
					unsigned int day, unsigned int hour,
					unsigned int min, unsigned int sec);   

double GetDistance(double lat1, double lng1, double lat2, double lng2);

void  GMTconvert(nmeaTIME *SourceTime, 
                 nmeaTIME *ConvertTime, 
                 uint8_t GMT,
                 uint8_t AREA);
#endif