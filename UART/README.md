# UART+GPIO+中断函数

• 通过虚拟串口发送指令，控制开发板上四个LED灯D1, D2, D3, D4的亮灭 

• 按下USR_SW1按键后，TM4C1294控制器向上位机报告四个LED灯D1, D2, D3, D4的亮灭状态，格式为：LED1:ON, LED2:OFF …
