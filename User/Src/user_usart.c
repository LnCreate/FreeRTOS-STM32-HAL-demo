#include "user_usart.h"

#include <stdio.h>
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
static void StartUSARTTask(void * argument)
{
    printf("USART Task Init OK!\n");
    for(;;)
    {
        vTaskDelay(1000);
    }
}