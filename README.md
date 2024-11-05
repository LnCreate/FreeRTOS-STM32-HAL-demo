# FreeRTOS的UART通信实现
- [x] 所有实现均位于user_usart.c/.h中，不需要在其他文件中进行配置
- [x] 使用轮询对usart2的串口重定向, 怎么处理microLib
- [x] 串口中断接收，并返回相同的内容给发送方
- [ ] 串口DMA接收方式
- [ ] 串口的DMA发送方式 
- [ ] IDLE方式的接收
- [ ] Robomaster TypeA自带的IMU

## UART接收流程
```mermaid
flowchart TD
    A[初始化 HAL 库] --> B[配置系统时钟]
    B --> C[初始化 GPIO]
    C --> D[初始化 USART2]
    D --> E[启用 USART2 接收中断]
    E --> F[进入主循环]
    F --> G{发生 UART 中断?}
    G -- 是 --> H[调用 USART2_IRQHandler]
    H --> I[执行 HAL_UART_IRQHandler]
    I --> J[触发 HAL_UART_RxCpltCallback]
    J --> K[处理接收到的数据]
    K --> L[重新启动接收中断]
    L --> F
    G -- 否 --> F
```
串口中断接收，要先在初始化里启用一次接收中断函数，以触发第一次中断

