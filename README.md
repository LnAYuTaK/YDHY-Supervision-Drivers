# 监理设备
## stm32F103RCT6
---
| 引脚        | 引脚说明                                                         |
| ------------ | ------------------------------------------------------------ |
| PC13 |  RGB灯R |
| PC14 | RGB灯G  |
| PC15 | RGB灯B  |
| PD0  | 晶振RCC_OSC_IN  |
| PD1  | 晶振RCC_OSC_OUT  |
| PC0  | 回传数据按键IO  |
| PC1  | 掉电监测中断  |
| PA12 | 流量计1脉冲计数器TIM1  |
| PA15 | 流量计2脉冲计数器TIM2  |
| PD2  | 流量计3脉冲计TIM3  |     
| PA0  | 流量计4脉冲计TIM8  |                               
| PA2  | 4G模块串口TX  |
| PA3  | 4G模块串口RX  |
| PA9  | GPS模块串口TX  |
| PA10 | GPS模块串口RX  |
| PB10 | 调试串口TX  | 
| PB11 | 调试串口RX  |
| PC10 | 串口预留TX  |
| PC11 | 串口预留RX  |
| PA13 | 调试口SWD_IO  |
| PA14 | 调试口SWD_CLK  |
| PA4  | ADC1_IN4  |
| PA5  | ADC1_IN5  |
| PA6  | ADC1_IN6  |
| PA7  | ADC1_IN7  |
## 合宙Air724 4G模块
| 引脚        | 引脚说明                                                         |
| ------------ | ------------------------------------------------------------ |
| RXD1 | stm32串口通讯 |
| TXD1 | stm32串口通讯 |
| RXD3 | DEBUG|
| TXD3 | DEBUG |
| IO14 | 掉电检测 |
### 功能描述
stm32 端作为数据采集采集GPS、压力计、流量计数据 通过解码 协议发送到合宙4G模块进行文件保存，合宙4G模块通过ftp每日回传Sd卡文件。

 


