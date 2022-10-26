#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

//****************************************************************************
//
// System clock rate in Hz.
//
//****************************************************************************
uint32_t g_ui32SysClock;
uint32_t cThisChar;
uint8_t d3,d2,d1,d0;
char s1[8],s2[8],s3[8],s4[8];
int i;
//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void
UARTIntHandler(void)
{
    uint32_t ui32Status;
    //
    // Get the interrrupt status.
    //
    ui32Status = ROM_UARTIntStatus(UART0_BASE, true);
    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART0_BASE, ui32Status);
    //
    // Loop while there are characters in the receive FIFO.
    //
    while(ROM_UARTCharsAvail(UART0_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        ROM_UARTCharPutNonBlocking(UART0_BASE,
                                   ROM_UARTCharGetNonBlocking(UART0_BASE));
        //
        // Blink the LED to show a character transfer is occuring.
        //
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
        //
        // Delay for 1 millisecond.  Each SysCtlDelay is about 3 clocks.
        //
        SysCtlDelay(g_ui32SysClock / (1000 * 3));
        //
        // Turn off the LED
        //
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);
    }
}
//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        ROM_UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);
    }
}

void SHOW(){
      s1[0]='L';//LED1:O
      s1[1]='E';
      s1[2]='D';
      s1[3]='1';
      s1[4]=':';
      s1[5]='O';
      if(d0==1)
      {
      s1[6]='N';//1,N''.0,FF
      s1[7]=' ';
      }
      else
      {
          s1[6]='F';//1,N''.0,FF
          s1[7]='F';
      }

              s2[0]='L';//LED1:O
                s2[1]='E';
                s2[2]='D';
                s2[3]='2';
                s2[4]=':';
                s2[5]='O';
                if(d1==1)
                {
                s2[6]='N';//1,N''.0,FF
                s2[7]=' ';
                }
                else
                {
                    s2[6]='F';//1,N''.0,FF
                      s2[7]='F';
                }

       s3[0]='L';
       s3[1]='E';
       s3[2]='D';
       s3[3]='3';
       s3[4]=':';
       s3[5]='O';
       if(d2){
       s3[6]='N';s3[7]=' ';}
       else {s3[6]='F';s3[7]='F';}


       s4[0]='L';
       s4[1]='E';
       s4[2]='D';
       s4[3]='4';
       s4[4]=':';
       s4[5]='O';
       if(d3){
       s4[6]='N';s4[7]=' ';}
       else {s4[6]='F';s4[7]='F';}
}
//*****************************************************************************
//
// 中断
//
//*****************************************************************************
// 中断函数IntGPIOj  使能见下
void IntGPIOj(void)
{
    //首先判断是那个引脚产生的中断
    if(GPIOIntStatus(GPIO_PORTJ_AHB_BASE,true)==PIO_INT_PIN_0)
    {
        //延迟一段时间后，检查引脚是否还是低电平
        SysCtlDelay(g_ui32SysClock/30);
        //如果是低电平，则反转输出
        if(GPIOPinRead(GPIO_PORTJ_AHB_BASE,GPIO_PIN_0)==0)
        {
            //按下0
            SHOW();
            for(i=0;i<8;i++)
            {
                UARTCharPut(UART0_BASE, s1[i]);
            }
            for(i=0;i<8;i++)
            {
                UARTCharPut(UART0_BASE, s2[i]);
            }
            for(i=0;i<8;i++)
            {
                UARTCharPut(UART0_BASE, s3[i]);
            }
            for(i=0;i<8;i++)
            {
                UARTCharPut(UART0_BASE, s4[i]);
            }
        }
        //清除中断，来什么中断就清除什么中断
        GPIOIntClear(GPIO_PORTJ_AHB_BASE,GPIO_INT_PIN_0);
    }
}
//*****************************************************************************
int
main(void)
{
    //
    // Set the clocking to run directly from the crystal at 120MHz.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);
    //灯使能 PORTN0.1   PORTF0.4
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0|GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0|GPIO_PIN_1, 0x00);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, 0x00);
    //SW1初始化 PORTJ
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_AHB_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTJ_AHB_BASE,GPIO_PIN_0,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

   //PORTJ 开启总中断和NVIC里的GPIOJ中断
    IntMasterEnable();
    IntEnable(INT_GPIOJ);
    //配置中断
    GPIOIntTypeSet(GPIO_PORTJ_AHB_BASE,GPIO_INT_PIN_0,GPIO_FALLING_EDGE);
    GPIOIntClear(GPIO_PORTJ_AHB_BASE,GPIO_INT_PIN_0);
    GPIOIntRegister(GPIO_PORTJ_AHB_BASE,IntGPIOj);
    GPIOIntEnable(GPIO_PORTJ_AHB_BASE,GPIO_INT_PIN_0);

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    //
    // Enable the GPIO pins for the LED (PN0).
    //
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    //
    // Enable the peripherals used by this example.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable processor interrupts.
    //
    ROM_IntMasterEnable();

    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    ROM_UARTConfigSetExpClk(UART0_BASE, g_ui32SysClock, 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    //
    // Enable the UART interrupt.
    //
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    //
    // Loop forever echoing data through the UART.
    //
    while(1)
    {
        //基础部分
        //PA0 R ,PA1 T
        cThisChar = UARTCharGet(UART0_BASE);//基础部分
        // UARTCharPut(UART0_BASE, cThisChar);

        //PF4  PF0 PN1 PN0
        d0=cThisChar&0x01;
        d1=(cThisChar&0x02)>>1;
        d2=(cThisChar&0x04)>>2;
        d3=(cThisChar&0x08)>>3;

        if(d0)
        {
            //d0控制PN0
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
        }
        else GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);

        if(d1)
        {
            //d1控制PN1
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
        }
        else GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);

        if(d2)
        {
            //d2控制PF0
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);
        }
        else GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);

        if(d3)
        {
            //d3控制PF4
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);
        }
        else GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0);
    }
}
