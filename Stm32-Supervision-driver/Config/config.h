#ifndef __Config_H
#define	__Config_H
//调试DEBUG 宏
//注释相应宏 可以打开对应调试
#define GPS_DEBUG_DISABLED	

#define PressureGage_DEBUG_DISABLED

#define FlowMeter_DEBUG_DISABLED

#define NET4GState_DEBUG_DISABLED	

// #define GPS_DATA_FILTER_DISABLED


//TUDO
// #define DATA_UART_SEND_DEBUG_DISABLED

// #define DATA_UART_RECV_DEBUG_DISABLED

/* 是否启用 GPS_DEBUG_DISABLED，如果定义了 GPS_DEBUG_DISABLED 则禁用（默认：禁用） */
#if defined(GPS_DEBUG_DISABLED)
	#ifndef GPS_DEBUG_DISABLED
		#define GPS_DEBUG_DISABLED
	#endif
#endif

/* 是否启用 PressureGage_DEBUG_DISABLED，如果定义了 PressureGage_DEBUG_DISABLED 则禁用（默认：禁用） */
#if defined(PressureGage_DEBUG_DISABLED)
	#ifndef PressureGage_DEBUG_DISABLED
		#define PressureGage_DEBUG_DISABLED
	#endif
#endif

/* 是否启用 FlowMeter_DEBUG_DISABLED，如果定义了 FlowMeter_DEBUG_DISABLED 则禁用（默认：禁用） */
#if defined(FlowMeter_DEBUG_DISABLED)
	#ifndef FlowMeter_DEBUG_DISABLED
		#define FlowMeter_DEBUG_DISABLED
	#endif
#endif

//显示当前系统状态
/* 是否启用 NET4GState_DEBUG_DISABLED，如果定义了 NET4GState_DEBUG_DISABLED 则禁用（默认：禁用） */
#if defined(NET4GState_DEBUG_DISABLED)
	#ifndef NET4GState_DEBUG_DISABLED
		#define NET4GState_DEBUG_DISABLED
	#endif
#endif

//是否允许GPS数据过滤
/* 是否启用 GPS_DATA_FILTER_DISABLED，如果定义了GPS_DATA_FILTER_DISABLED 则禁用（默认：禁用） */
#if defined(GPS_DATA_FILTER_DISABLED)
	#ifndef GPS_DATA_FILTER_DISABLED
		#define GPS_DATA_FILTER_DISABLED
	#endif
#endif

//TUDO
// //是否允许DEBUG Send串口数据
// /* 是否启用 DATA_UART_SEND_DEBUG_DISABLED，如果定义了DATA_UART_SEND_DEBUG_DISABLED 则禁用（默认：禁用） */
// #if defined(DATA_UART_SEND_DEBUG_DISABLED)
// 	#ifndef DATA_UART_SEND_DEBUG_DISABLED
// 		#define DATA_UART_SEND_DEBUG_DISABLED
// 	#endif
// #endif

// //是否允许DEBUG Recv串口数据
// /* 是否启用 DATA_UART_RECV_DISABLED，如果定义了DATA_UART_RECV_DISABLED 则禁用（默认：禁用） */
// #if defined(DATA_UART_RECV_DEBUG_DISABLED)
// 	#ifndef DATA_UART_RECV_DEBUG_DISABLED
// 		#define DATA_UART_RECV_DEBUG_DISABLED
// 	#endif
// #endif

#endif /*__Config_H */