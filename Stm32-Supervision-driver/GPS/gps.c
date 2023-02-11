#include "gps.h"
#include <string.h>
#include "UserApp.h"
#include "time.h"
#include "math.h"
#include "config.h"
#include "usart.h"
#define EARTH_RADIUS 6378.137//地球半径
#define PI 3.14159265358979323846 //圆周率

//GPS原始数据DEBUG
void trace(const char *str, int str_size)
{
    uint16_t i;
    printf("\r\nTrace: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
    printf("\n");
}

void error(const char *str, int str_size)
{
    uint16_t i;
    printf("\r\nError: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
    printf("\n");
}

void gps_info(const char *str, int str_size)
{
    uint16_t i;
    printf("\r\nInfo: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
    printf("\n");
}
// char * buf  To String 
void DebugStr(const char *str, int str_size)
{
    uint16_t i;
    printf("\r\nDebugStr: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));    
    printf("\n");
}   
//计算角度
static double rad(double d)
{
   return d * PI / 180.0;
}
//GPS 根据经纬度差值计算距离
double GetDistance(double lat1, double lng1, double lat2, double lng2)
{
    double radLat1 = rad(lat1);
    double radLat2 = rad(lat2);
    double a = radLat1 - radLat2;
    double b = rad(lng1) - rad(lng2);
    double s = 2 * asin(sqrt(pow(sin(a/2),2) +
    cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
    s = s * EARTH_RADIUS;
    s = llroundl(s*10000) / 10000.0;
    return s;
}
//判断闰年
static uint8_t IsLeapYear(uint8_t iYear) 
{ 
    uint16_t Year; 
    Year = 2000+iYear; 
    if((Year&3)==0) 
    { 
        return ((Year%400==0) || (Year%100!=0)); 
    } 
    return 0;     
}
//GMT 转换时间
void  GMTconvert(nmeaTIME *SourceTime, nmeaTIME *ConvertTime, uint8_t GMT,uint8_t AREA) 
{ 
    uint32_t  YY,MM,DD,hh,mm,ss;       
    if(GMT==0)    return;               
    if(GMT>12)    return;                   
    YY    =    SourceTime->year;            
    MM    =    SourceTime->mon;               
    DD    =    SourceTime->day;               
    hh    =    SourceTime->hour;             
    mm    =    SourceTime->min;             
    ss    =    SourceTime->sec;           
    if(AREA)                      
    { 
        if(hh+GMT<24)    hh    +=    GMT;
        else                      
        { 
            hh    =    hh+GMT-24;  
            if(MM==1 || MM==3 || MM==5 || MM==7 || MM==8 || MM==10)  
            { 
                if(DD<31)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==4 || MM==6 || MM==9 || MM==11)               
            { 
                if(DD<30)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==2)  
            { 
                if((DD==29) || (DD==28 && IsLeapYear(YY)==0)) 
                {
                    DD    =    1; 
                    MM    ++; 
                } 
                else    DD++; 
            } 
            else if(MM==12)   
            { 
                if(DD<31)    DD++; 
                else      
                {               
                    DD    =    1; 
                    MM    =    1; 
                    YY    ++; 
                } 
            } 
        } 
    } 
    else 
    {     
        if(hh>=GMT)
        {
        hh  -=  GMT;    
        }
        else
        {               
            hh    =    hh+24-GMT;     
            if(MM==2 || MM==4 || MM==6 || MM==8 || MM==9 || MM==11)   
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    31; 
                    MM    --; 
                } 
            } 
            else if(MM==5 || MM==7 || MM==10 || MM==12)            
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    30; 
                    MM    --; 
                } 
            } 
            else if(MM==3)    
            { 
                if((DD==1) && IsLeapYear(YY)==0)               
                { 
                    DD    =    28; 
                    MM    --; 
                } 
                else    DD--; 
            } 
            else if(MM==1)   
            { 
                if(DD>1)    DD--; 
                else       
                {               
                    DD    =    31; 
                    MM    =    12; 
                    YY    --; 
                } 
            } 
        } 
    }         
    ConvertTime->year   =    YY;               
    ConvertTime->mon    =    MM;              
    ConvertTime->day    =    DD;               
    ConvertTime->hour   =    hh;                
    ConvertTime->min    =    mm;             
    ConvertTime->sec    =    ss;                
} 
//时间戳转换时间
nmeaTIME  TimeStamp2DateTime(uint32_t Stamp)
{
    nmeaTIME  stDateTime;
    time_t TimeStamp = Stamp;
    struct tm* pstTime = localtime(&TimeStamp);
    stDateTime.year = pstTime->tm_year + 1900;
    stDateTime.mon = pstTime->tm_mon + 1;
    stDateTime.day = pstTime->tm_mday;
    stDateTime.hour = pstTime->tm_hour;
    stDateTime.min = pstTime->tm_min;
    stDateTime.sec = pstTime->tm_sec;
    return stDateTime;
}

uint32_t DateTimeToTimeStamp (unsigned int year, unsigned int mon,
					unsigned int day, unsigned int hour,
					unsigned int min, unsigned int sec)     
{
    if (0 >= (int) (mon -= 2)){    /**//* 1..12 -> 11,12,1..10 */
         mon += 12;      /**//* Puts Feb last since it has leap day */
         year -= 1;
    }
 
    return (((
             (unsigned long) (year/4 - year/100 + year/400 + 367*mon/12 + day) +
             year*365 - 719499
          )*24 + hour /**//* now have hours */
       )*60 + min /**//* now have minutes */
    )*60 + sec; /**//* finally seconds */
}

//GPS解码  
int GPS_decode()
{
    double deg_lat;//转换成[degree].[degree]格式的纬度
    double deg_lon;//转换成[degree].[degree]格式的经度
    nmeaINFO info;          //GPS解码后得到的信息
    nmeaPARSER parser;      //解码时使用的数据结构  
    /* 设置用于输出调试信息的函数 */
    // nmea_property()->trace_func = &trace;
    // nmea_property()->error_func = &error;
    // nmea_property()->info_func = &gps_info;
    /* 初始化GPS数据结构 */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);
    nmea_parse(&parser, (const char*)&(RxBuffer),sizeof(RxBuffer), &info);
    deg_lat = nmea_ndeg2degree(info.lat);
    deg_lon = nmea_ndeg2degree(info.lon);
    //GMT时间换算成北京时间
    #ifndef GPS_DEBUG_DISABLED
    printf("\r\n纬度:%f,经度%f\r\n",deg_lat,deg_lon);
    printf("海拔高度：%f 米\n", info.elv);
    printf("速度：%f km/h\n", info.speed);
    printf("\r\nPDOP:%f,HDOP:%f,VDOP:%f\r\n",info.PDOP,info.HDOP,info.VDOP);
    #endif
    Gps_Msg_t msg;
    msg.Lat     = deg_lat;
    msg.Lon     = deg_lon;
    msg.Height  = info.elv;
    msg.Hdop    = info.HDOP;
    msg.UtcTime = info.utc;
    msg.Speed   = info.speed;
    //转发到任务里边
    osMessagePut(GpsMsgHandle,(uint32_t)&msg,0);
    nmea_parser_destroy(&parser);
    return 0;
}