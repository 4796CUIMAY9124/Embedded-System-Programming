#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/eeprom.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/timer.h"
#include "key.h"
#include "inc/tm4c1294ncpdt.h"
#include "12864.h"

/////////////////全局变量/////////////////////////////////////
uint32_t ui32SysClock;
int KeyTemp;//键盘扫描值
int value;//实际值
int u=0,d=0,l=0,r=0;//上下左右
int dir;//
int x=0,y=0;//移动值 x=r-l;y=u-d;
int b=0;//炮弹,默认不开,=0
int i,j,k=0;//通用计数
int flag=0;//用来标注bomb是否初始化，1已经初始化
int flag0=0;//flag0=1，炮弹没有出框，flag=0，炮弹出框
int hand=0;//用来标注是否按下五

extern unsigned char bomb[];

// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*************************函数区**************************************

void getvalue(KeyTemp)
{
    switch(KeyTemp)
    {
        case 8: 
            {// 2
                value=2;
                u+=1;//up
                dir=2;
                break; 
            }
        case 13: 
            {// 4
                value=4;
                l+=1;
                dir=4;
                break;
            }
        case 9: 
            {// 5
                value=5;
                b+=1;
                flag0=1;
                hand=1;//按下5
                break;
            }
        case 5: 
            {// 6
                value=6;
                r+=1;
                dir=6;
                break;
            }
        case 10: 
            {// 8
                value=8;
                d+=1;
                dir=8;
                break;
            }            
        default: break;
    }
    x=r-l;
    y=u-d;
    if(!b)
    {
        LDC_draw(dir,x,y,0);
    }
    if(hand)
    {//按下
        changebomb();
        hand=0;
    }
}

void IntGPIOP0(void )
{
    uint32_t IntStatus=GPIOIntStatus(GPIO_PORTP_BASE,true);
    if(IntStatus&GPIO_INT_PIN_2|GPIO_INT_PIN_3|GPIO_INT_PIN_4|GPIO_INT_PIN_5)
    {
        if ((KeyRead() & 0xF0) < 0xF0)//是否有键按下
        {
            check_key(); // 调用check_key(),获取键值
        }
        KeyTemp=key_val;
        getvalue(KeyTemp);//value
        KeyWrite(0xF0);
        GPIOIntClear(GPIO_PORTP_BASE,IntStatus);
        return;
    }
    GPIOIntClear(GPIO_PORTP_BASE,IntStatus);
}

int
main(void)
{
    //
    // Run from the PLL at 120 MHz.
    //
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                       SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_PLL |
                                       SYSCTL_CFG_VCO_480), 120000000);
    //显示屏
    Lcd_init_basic();
    //初始化按键端口和 GPIOP 的中断：
    key_init();//包含初始、检查、事件
    KeyWrite(0xF0);
    IntMasterEnable();
    IntEnable(INT_GPIOP0);
    
    GPIOIntTypeSet(GPIO_PORTP_BASE,GPIO_INT_PIN_2|GPIO_INT_PIN_3|GPIO_INT_PIN_4|GPIO_INT_PIN_5,GPIO_FALLING_EDGE);
    GPIOIntClear(GPIO_PORTP_BASE,GPIO_INT_PIN_2|GPIO_INT_PIN_3|GPIO_INT_PIN_4|GPIO_INT_PIN_5);
    GPIOIntRegister(GPIO_PORTP_BASE,IntGPIOP0);
    GPIOIntEnable(GPIO_PORTP_BASE,GPIO_INT_PIN_2|GPIO_INT_PIN_3|GPIO_INT_PIN_4|GPIO_INT_PIN_5);
    LDC_draw(2,0,0,false);
    while(1)
    {
        if(flag0)
        {
            ////////////////炮弹///////////////
            switch(dir)
            {
                case 2://up
                    {
                        //只改bomb图
                        LDC_draw(dir, 0, 5,1);
                        //延时
                        delay(0.1*ui32SysClock);

                        for(i=0;i<1024;i++)
                        {//flag0全空为0，有图为1
                            if( bomb[i]!=0x00)
                            {
                                flag0=1;
                                break;
                            }
                            flag0=0;b=0;k=0;
                        }
                           break;
                    }
                case 8://d
                    {
                        if(flag0)
                        {
                            //只改bomb图
                            LDC_draw(dir, 0,-5,1);
                            //延时
                            delay(0.1*ui32SysClock);
                            
                            for(i=0;i<1024;i++)
                            {//flag全空为0，有图为1
                                if( bomb[i]!=0x00)
                                {
                                    flag0=1;
                                    break;
                                }
                                flag0=0;b=0;k=0;
                            }
                        }
                        break;
                    }
                case 4://l
                    {
                      if(flag0)
                      {
                          //只改bomb图
                          LDC_draw(dir, -5,0,1);
                          //延时
                          delay(0.1*ui32SysClock);
                          for(i=0;i<1024;i++)
                          {//flag全空为0，有图为1
                              if( bomb[i]!=0x00)
                              {
                                  flag0=1;
                                  break;
                              }
                              flag0=0;b=0;k=0;
                          }
                      }
                      break;
                    }
                case 6://r
                    {
                        if(flag0)
                        {
                            //只改bomb图
                            LDC_draw(dir, 5,0,1);
                            //延时
                            delay(0.1*ui32SysClock);
                            
                            for(i=0;i<1024;i++)
                            {//flag全空为0，有图为1
                                if( bomb[i]!=0x00)
                                {
                                    flag0=1;
                                    break;
                                }
                                flag0=0;b=0;k=0;
                            }
                        }
                        break;
                    }
            }
            delay(0.3*ui32SysClock);
        }
    }
}
