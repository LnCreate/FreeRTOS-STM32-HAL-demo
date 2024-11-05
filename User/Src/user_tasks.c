#include "user_tasks.h"

#include <FreeRTOS.h>
#include <stdint.h>
#include <stdio.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>

#include "main.h"
#include "stm32_hal_legacy.h"
#include "stm32f4xx_hal_tim.h"
#include "tim.h"

#include "user_usart.h"

TaskHandle_t TaskLEDHandle;
static void StartLEDTask(void * argument);

SemaphoreHandle_t xMutex = NULL;

void User_FREERTOS_Init(void) {

    xMutex = xSemaphoreCreateMutex();
    User_USART_Init();
    xTaskCreate(
        StartLEDTask,       /* 任务入口函数 */
        "TaskLow",          /* 任务名称 */
        configMINIMAL_STACK_SIZE, /* 任务栈大小 */
        NULL,                /* 任务参数 */
        tskIDLE_PRIORITY + 1,/* 任务优先级 */
        &TaskLEDHandle);           /* 任务句柄 */

    vTaskStartScheduler();
}

static void StartLEDTask(void * argument)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    int pwmVal = 0;
    int pwmDir = 5;
    printf("LED Task Init OK!\n");
    for(;;)
    {
        // while (pwmVal< 1000){
        //     pwmVal++;
        //     __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, pwmVal);    //修改比较值，修改占空比
        //     // TIM3->CCR1 = pwmVal;    与上方相同
        //     vTaskDelay(1);
        // }
        // while (pwmVal){
        //     pwmVal--;
        //     __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, pwmVal);    //修改比较值，修改占空比
        //      // TIM3->CCR1 = pwmVal;     与上方相同
        //     vTaskDelay(1);
        // }
        if(pwmVal<=1000 && pwmVal>=0){
            pwmVal += pwmDir;
        }else{
            pwmDir = -pwmDir;
            pwmVal += pwmDir;
        }       
        __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, pwmVal);    //修改比较值，修改占空比
        vTaskDelay(10);
    }
}

#if ( configCHECK_FOR_STACK_OVERFLOW > 0 )

    void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                        char * pcTaskName )
    {
        /* Check pcTaskName for the name of the offending task,
         * or pxCurrentTCB if pcTaskName has itself been corrupted. */
        ( void ) xTask;
        ( void ) pcTaskName;
    }

#endif /* #if ( configCHECK_FOR_STACK_OVERFLOW > 0 ) */