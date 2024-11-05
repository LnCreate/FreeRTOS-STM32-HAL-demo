#include "user_usart.h"

#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "stm32f4xx_hal_uart.h"
#include "usart.h"         // 包含 USART 配置的头文件
#include "FreeRTOS.h"
#include "task.h"

// 声明外部的 UART_HandleTypeDef
extern UART_HandleTypeDef huart2;

/*******************串口重定向******************/
// 重定义 fputc 函数，实现 printf 的重定向
int fputc(int ch, FILE *f)
{
    // 使用 HAL 库的函数发送一个字符
    if (HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        // 发送失败，可以在这里处理错误
        return -1;
    }
    return ch;
}

// 重定义 _write 函数，以适配某些编译器的需求
int _write(int file, char *ptr, int len)
{
    for(int i = 0; i < len; i++)
    {
        fputc(*ptr++, NULL);
    }
    return len;
}


/*************创建串口任务***************/
static void StartUSARTTask(void * argument);
static TaskHandle_t TaskUSARTHandle;
void User_USART_Init(void)
{
    xTaskCreate(
        StartUSARTTask,       /* 任务入口函数 */
        "TaskUSART",          /* 任务名称 */
        configMINIMAL_STACK_SIZE, /* 任务栈大小 */
        NULL,                /* 任务参数 */
        tskIDLE_PRIORITY + 2,/* 任务优先级 */
        &TaskUSARTHandle);           /* 任务句柄 */
}

/***************串口运行任务**************/
#define RXBUFFERSIZE  256     //最大接收字节数
static char RxBuffer[RXBUFFERSIZE];   //接收数据
static uint8_t aRxBuffer;			//接收中断缓冲
static uint8_t Uart2_Rx_Cnt = 0;		//接收缓冲计数
static void StartUSARTTask(void * argument)
{
    if(HAL_UART_Receive_IT(&huart2, &aRxBuffer, 1) != HAL_OK)
    {
        printf("UART Task Init Error!");
        Error_Handler();
    }
    printf("USART Task Init OK!\n");
    for(;;)
    {
        // 这里可以用来进行一些必要性信息的显示和发送
        vTaskDelay(1000);
    }
}

/*****************接收中断回调函数********************/
// 需要先调用一次串口中断接收函数，以触发第一次中断
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
 
	if(Uart2_Rx_Cnt >= 255)  //溢出判断
	{
		Uart2_Rx_Cnt = 0;
        memset(RxBuffer,0x00,sizeof(RxBuffer)); 
		printf("数据溢出"); 	
        
	}
	else
	{
		RxBuffer[Uart2_Rx_Cnt++] = aRxBuffer;   //接收数据转存
	
		if((RxBuffer[Uart2_Rx_Cnt-1] == 0x0A)&&(RxBuffer[Uart2_Rx_Cnt-2] == 0x0D)) //判断结束位
		{
			HAL_UART_Transmit(&huart2, (uint8_t *)&RxBuffer, Uart2_Rx_Cnt,0xFFFF); //将收到的信息发送出去
            while(HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_TX);//检测UART发送结束
			Uart2_Rx_Cnt = 0;
			memset(RxBuffer,0x00,sizeof(RxBuffer)); //清空数组
		}
	}
	
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);   //再开启接收中断
}