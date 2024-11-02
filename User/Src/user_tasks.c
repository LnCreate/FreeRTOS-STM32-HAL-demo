#include <FreeRTOS.h>
#include <stdint.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
#include "main.h"
#include "stm32f4xx_hal_tim.h"
#include "tim.h"

#include "user_tasks.h"

TaskHandle_t TaskHighHandle;
#define TASK_HIGH_STACK_SIZE                    128

TaskHandle_t TaskLowHandle;
#define TASK_LOW_STACK_SIZE                    128


static void StartTaskHigh(void * argument);
static void StartTaskLow(void * argument);

SemaphoreHandle_t xMutex = NULL;

void User_FREERTOS_Init(void) {

    xMutex = xSemaphoreCreateMutex();
    xTaskCreate(
        StartTaskHigh,       /* 任务入口函数 */
        "TaskHigh",          /* 任务名称 */
        configMINIMAL_STACK_SIZE, /* 任务栈大小 */
        NULL,                /* 任务参数 */
        tskIDLE_PRIORITY + 2,/* 任务优先级 */
        &TaskHighHandle);           /* 任务句柄 */
    xTaskCreate(
        StartTaskLow,       /* 任务入口函数 */
        "TaskLow",          /* 任务名称 */
        configMINIMAL_STACK_SIZE, /* 任务栈大小 */
        NULL,                /* 任务参数 */
        tskIDLE_PRIORITY + 1,/* 任务优先级 */
        &TaskLowHandle);           /* 任务句柄 */

    vTaskStartScheduler();
}

static void StartTaskHigh(void * argument)
{
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
    for(;;)
    {
        // HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
        vTaskDelay(2000);
    }
}
static void StartTaskLow(void * argument)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    int pwmVal = 0;
    int pwmDir = 5;
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