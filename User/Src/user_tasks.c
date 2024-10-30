#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
#include "main.h"

#include "user_tasks.h"

TaskHandle_t TaskHighHandle;
#define TASK_HIGH_STACK_SIZE                    128

TaskHandle_t TaskLowHandle;
#define TASK_LOW_STACK_SIZE                    128


static void StartTaskHigh(void * argument);
static void StartTaskLow(void * argument);

void User_FREERTOS_Init(void) {

    xTaskCreate(
        StartTaskHigh,       /* 任务入口函数 */
        "TaskHigh",          /* 任务名称 */
        configMINIMAL_STACK_SIZE, /* 任务栈大小 */
        NULL,                /* 任务参数 */
        tskIDLE_PRIORITY + 1,/* 任务优先级 */
        &TaskHighHandle);           /* 任务句柄 */
    xTaskCreate(
        StartTaskLow,       /* 任务入口函数 */
        "TaskLow",          /* 任务名称 */
        configMINIMAL_STACK_SIZE, /* 任务栈大小 */
        NULL,                /* 任务参数 */
        tskIDLE_PRIORITY + 2,/* 任务优先级 */
        &TaskLowHandle);           /* 任务句柄 */

    vTaskStartScheduler();
}

static void StartTaskHigh(void * argument)
{
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
    for(;;)
    {
        HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
        vTaskDelay(200);
    }
}
static void StartTaskLow(void * argument)
{
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
    for(;;)
    {
        HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
        vTaskDelay(1000);
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