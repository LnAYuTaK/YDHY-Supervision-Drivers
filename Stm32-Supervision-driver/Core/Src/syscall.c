#include <stdio.h>
#include <sys/stat.h>
#include "usart.h"
#include "stm32f1xx_hal.h"
#ifdef __cplusplus
extern "C"
{
#endif
    // 重定向到串口3
    int _write(int fd, uint8_t *pBuffer, int size)
    {
        for (int i = 0; i < size; i++)
        {
            while (!(USART1->SR & USART_SR_TXE))
            {
            }
            HAL_UART_Transmit(&huart3, &pBuffer[i], 1, 0xFFFF);
        }
        return size;
    }
 
    int _read(int fd, uint8_t *pBuffer, int size)
    {
        for (int i = 0; i < size; i++)
        {
            while ((USART1->SR & USART_SR_RXNE) == 0)
            {
            }
            pBuffer[i] = HAL_UART_Receive(&huart3, &pBuffer[i], 1, 0xFFFF);
        }
        return size;
    }
 
    caddr_t _sbrk(int increment)
    {
        extern char end asm("end");
        register char *pStack asm("sp");
 
        static char *s_pHeapEnd;
 
        if (!s_pHeapEnd)
            s_pHeapEnd = &end;
 
        if (s_pHeapEnd + increment > pStack)
            return (caddr_t)-1;
 
        char *pOldHeapEnd = s_pHeapEnd;
        s_pHeapEnd += increment;
        return (caddr_t)pOldHeapEnd;
    }
 
    int _close(int fd)
    {
        return -1;
    }
 
    int _lseek(int fd1, int fd2, int fd3)
    {
        return -1;
    }
 
    int _fstat(int fd, struct stat *pStat)
    {
        pStat->st_mode = S_IFCHR;
        return 0;
    }
 
    int _isatty(int fd)
    {
        return 1;
    }
 
#ifdef __cplusplus
}
#endif