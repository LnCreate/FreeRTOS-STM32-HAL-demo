# FreeRTOS 移植过程
# ！！！ 有问题开调试，调试就是最有用的，看代码如果不运行是卡在哪里
## 依赖的工具
- VSCode
- Cmake(项目组织工具)
- gcc-arm-none-eabi(交叉编译工具链)
- Openocd (进行elf的烧录)

## 项目的生成
CubeMX生成stm32 hal库的Makefile工程
将FreeRTOS-kernel添加进工程的freertos目录下

## Cmake构建工程
note: 之所以使用cmake是因为freertos以cmake构建，在stm32工程中使用cmake来管理，可以通过add_subdirectory()的方式较为轻易地将freertos加入到stm32工程中
Cmake的构建分为两部分:
1. 根据CubeMX生成的Makefile文件来获得单片机信息，完成基本工程的构建
2. 依照freertos给出的examples/cmake_example中的CMakeLists.txt完成改动，主要需要注意的是，要把模板里的“template”改完自己所使用的芯片型号（这次工作中为`GCC_ARM_CM4F`），要指定`FreeRTOSConfig.h`的位置
```C
// @note: freertos核相对于当前目录的位置
set(FREERTOS_KERNEL_PATH "freertos")

# Add the freertos_config for FreeRTOS-Kernel
add_library(freertos_config INTERFACE)

// @note: 指示FreeRTOSConfig.h的位置
target_include_directories(freertos_config
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/User/Inc
)

if (DEFINED FREERTOS_SMP_EXAMPLE AND FREERTOS_SMP_EXAMPLE STREQUAL "1")
    message(STATUS "Build FreeRTOS SMP example")
    # Adding the following configurations to build SMP template port
    add_compile_options( -DconfigNUMBER_OF_CORES=2 -DconfigUSE_PASSIVE_IDLE_HOOK=0 )
endif()

# Select the heap port.  values between 1-4 will pick a heap.
set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)

// @note: GCC_ARM_CM4F
# Select the native compile PORT
set(FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)

# Adding the FreeRTOS-Kernel subdirectory
add_subdirectory(${FREERTOS_KERNEL_PATH} FreeRTOS-Kernel)


target_compile_options(freertos_kernel PRIVATE
    ### Gnu/Clang C Options
    $<$<COMPILE_LANG_AND_ID:C,GNU>:-fdiagnostics-color=always>
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-fcolor-diagnostics>

    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wall>
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wextra>
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wpedantic>
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Werror>
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wconversion>
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Weverything>

    # Suppressions required to build clean with clang.
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-unused-macros>
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-padded>
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-missing-variable-declarations>
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-covered-switch-default>
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-cast-align> )

target_link_libraries(${PROJECT_NAME}.elf freertos_kernel freertos_config)

set_property(TARGET freertos_kernel PROPERTY C_STANDARD 90)
```
## .VsCode文件夹的配置
### tasks.json
在终端-配置默认生成任务中生成，需要添加cmake:build任务和烧录任务，使用快捷键`ctrl`+`Shift`+`P`可以快捷调出
```C
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Flash STM32",
            "type": "process",
            "command": "openocd",
            "args": [
                "-f",
                "./openocd.cfg",
                "-c",
                "program build/${workspaceRootFolderName}.elf verify reset exit"
            ],
            "dependsOn": [
                "CMake: build"
            ],
            "options": {
                "cwd": "${workspaceRoot}"
            },
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": [
                "$gcc"
            ]
        },
        {
            "type": "cmake",
            "label": "CMake: build",
            "command": "build",
            "targets": [
                "all"
            ],
            "group": "build",
            "problemMatcher": [],
            "detail": "CMake template build task"
        }
    ]
}
```
### c_cpp_properities.json
主要是为了include_path,不让目录一片红

```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "Core/Inc",
                "Drivers/CMSIS/Device/ST/STM32F4xx/Include",
                "Drivers/CMSIS/Include",
                "Drivers/STM32F4xx_HAL_Driver/Inc",
                "Drivers/STM32F4xx_HAL_Driver/Inc/Legacy",
                "freertos/include",
                "freertos/portable/GCC/ARM_CM4F",
                "user/Inc"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE",
                "USE_HAL_DRIVER",
                "STM32F427xx"
            ],
            "compilerPath": "C:\\MinGW\\bin\\gcc.exe",
            "cStandard": "c17",
            "cppStandard": "gnu++14",
            "intelliSenseMode": "windows-gcc-arm",
            "configurationProvider": "ms-vscode.makefile-tools"
        }
    ],
    "version": 4
}
```
### launch.json
调试文件，需要添加一些调试相关的内容，这里使用openocd来进行调试(**非常重要！！！**)
```json
{
    "configurations": [
        {
            "showDevDebugOutput": "parsed",
            "cwd": "${workspaceRoot}",
            "executable": "build/${workspaceRootFolderName}.elf",
            "name": "Debug STM32",
            "request": "launch",
            "type": "cortex-debug",
            "servertype": "openocd",
            "preLaunchTask": "CMake: build",
            "configFiles": [
                "openocd.cfg"
            ],
            "openOCDPreConfigLaunchCommands": [
                "adapter_khz 1800"
            ],
            "runToMain": true,
            "svdFile": "${workspaceRoot}/path/to/your.svd"
        }
    ]
}
```
## FreeRTOSConfig.h
通过该文件进行内核裁剪，这里直接使用的是ST官方给的config文件模板，不多赘述，需要额外关注`configMAX_SYSCALL_INTERRUPT_PRIORITY`,这里报错报了好几轮了

由于在Cubemx里配置时，裸机使用的是`TIM1`时钟，所以可以直接使用`port.c`中的函数对以下进行重定义，并把`stm32f4xx_it.c`中的几个函数注释掉
```C
/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
   standard names. */
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler

/* IMPORTANT: FreeRTOS is using the SysTick as internal time base, thus make sure the system and peripherials are
              using a different time base (TIM based for example).
 */
#define xPortSysTickHandler SysTick_Handler
```

## User文件夹
这里存放主要的用户定义函数
要把这里通过`include_directories(
    User/Inc
)`把用户头文件加进去
### user_tasks.c
这里给了一个测试程序，如有需要，直接去看这个文件，另外，在main.c里要调用`User_FREERTOS_Init()`函数（因为是从官方抄的，可以改个名）


