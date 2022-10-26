//*****************************************************************************
//
// project0.c - Example to demonstrate minimal TivaWare setup

//*****************************************************************************

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

/////////////////ȫ�ֱ���/////////////////////////////////////
uint32_t ui32SysClock;
int KeyTemp;//����ɨ��ֵ
int value;//ʵ��ֵ
int u=0,d=0,l=0,r=0;//��������
int dir;//
int x=0,y=0;//�ƶ�ֵ x=r-l;y=u-d;
int b=0;//�ڵ�,Ĭ�ϲ���,=0
int i,j,k=0;//ͨ�ü���
int flag=0;//������עbomb�Ƿ��ʼ����1�Ѿ���ʼ��
int flag0=0;//flag0=1���ڵ�û�г���flag=0���ڵ�����
int hand=0;//������ע�Ƿ�����

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

//*************************������**************************************

void getvalue(KeyTemp){
    switch(KeyTemp){
           case 8: {// 2
               value=2;
               u+=1;//up
               dir=2;
           break; }
           case 13: {// 4
               value=4;
               l+=1;
               dir=4;
               break;
            }
           case 9: {// 5
               value=5;
               b+=1;
               flag0=1;
               hand=1;//����5
               break;
                    }
           case 5: {// 6
               value=6;
               r+=1;
               dir=6;
               break;
                    }
           case 10: {// 8
               value=8;
               d+=1;
               dir=8;
               break;
                    }

           default: break;
    }//switch
    x=r-l;
    y=u-d;
    if(!b){
        LDC_draw(dir,x,y,0);
    }
    if(hand){//����
        changebomb();
        hand=0;
    }

}

void IntGPIOP0(void ){
    uint32_t IntStatus=GPIOIntStatus(GPIO_PORTP_BASE,true);
    if(IntStatus&GPIO_INT_PIN_2|GPIO_INT_PIN_3|GPIO_INT_PIN_4|GPIO_INT_PIN_5){
     if ((KeyRead() & 0xF0) < 0xF0)//�Ƿ��м�����
     {
     check_key(); // ����check_key(),��ȡ��ֵ
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
/////////////////////////////////////////

        //��ʾ��
        Lcd_init_basic();
        //��ʼ�������˿ں� GPIOP ���жϣ�
        key_init();//������ʼ����顢�¼�
        KeyWrite(0xF0);
        IntMasterEnable();
        IntEnable(INT_GPIOP0);

        GPIOIntTypeSet(GPIO_PORTP_BASE,GPIO_INT_PIN_2|GPIO_INT_PIN_3|GPIO_INT_PIN_4|GPIO_INT_PIN_5,GPIO_FALLING_EDGE);
        GPIOIntClear(GPIO_PORTP_BASE,GPIO_INT_PIN_2|GPIO_INT_PIN_3|GPIO_INT_PIN_4|GPIO_INT_PIN_5);
        GPIOIntRegister(GPIO_PORTP_BASE,IntGPIOP0);
        GPIOIntEnable(GPIO_PORTP_BASE,GPIO_INT_PIN_2|GPIO_INT_PIN_3|GPIO_INT_PIN_4|GPIO_INT_PIN_5);
        LDC_draw(2,0,0,false);
        while(1){
            if(flag0){
                //////�ڵ�
                switch(dir){
                case 2://up
                    {
                        //ֻ��bombͼ
                        LDC_draw(dir, 0, 5,1);
                        //��ʱ
                        delay(0.1*ui32SysClock);

                        for(i=0;i<1024;i++){//flag0ȫ��Ϊ0����ͼΪ1
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
                       if(flag0){
                           //ֻ��bombͼ
                           LDC_draw(dir, 0,-5,1);
                           //��ʱ
                           delay(0.1*ui32SysClock);

                           for(i=0;i<1024;i++){//flagȫ��Ϊ0����ͼΪ1
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
                      if(flag0){
                          //ֻ��bombͼ
                          LDC_draw(dir, -5,0,1);
                          //��ʱ
                          delay(0.1*ui32SysClock);
                          for(i=0;i<1024;i++){//flagȫ��Ϊ0����ͼΪ1
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
                      if(flag0){
                          //ֻ��bombͼ
                          LDC_draw(dir, 5,0,1);
                          //��ʱ
                          delay(0.1*ui32SysClock);

                          for(i=0;i<1024;i++){//flagȫ��Ϊ0����ͼΪ1
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
        }//while
}//main
