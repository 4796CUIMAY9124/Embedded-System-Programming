
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "drivers/pinout.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/fpu.h"
#include "utils/uartstdio.h"
#include "driverlib/crc.h"
#include "driverlib/i2c.h"



//*****************************************************************************
uint32_t g_ui32SysClock;
int i=0;
uint8_t de;
uint8_t  dataBuffer[6];        //定义数组
int16_t x;                   //加速度变量16位
int16_t y;
int16_t z;
uint16_t I2CState;
float gx=0;
float gy=0;
float gz=0;
float g=0;

float zx=0;
float zy=0;
float zz=0;

int ggx[3];
int ggy[3];
int ggz[3];

char sx[5];
char sy[5];
char sz[5];

#define slaveAddress 0x53    //0x53好像是另一个可用从机地址
#define firstRegAddress 0x32   //DATAX0地址
#define devid 0x00               //


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
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************



//*****************************************************************************
//
// Print "Hello World!" to the UART on the Intelligent UART Module.
//
//*****************************************************************************


uint16_t  SLAVE_rx_data;
uint16_t  MASTER_rx_data[2];
int
main(void)
                                      {
    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                SYSCTL_CFG_VCO_480), 120000000);

    Lcd_init_basic();


///////////////使能
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
//上拉电阻 弱上拉
   // GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_2,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
   // GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_3,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

    I2CMasterInitExpClk(I2C0_BASE, g_ui32SysClock, false);//时钟使能
    I2CMasterIntClear(I2C0_BASE);//

//从机打开&设置地址




    while(1)
    {
        //读出ADXL345模块的内部寄存器0x00(DEVID)的值，用示波器记录SDA和
        //SCL线上的波形，指出SDA上传输的数据的什么，哪部分是主机发送的，哪部分是从机发送的

        //设置DATA_FORMAT寄存器和POWER_CTL寄存器寄存器，开始加速度的测量。读出DATAX0、DATAX1、DATAY0、DATAY1
        //DATAZ0和DATAZ1寄存器的值，并转换位XYZ轴的加速度。

        //把XYZ三个方向的重力加速度，显示在LCD显示屏上
/*
        while(I2CMasterBusBusy(I2C0_BASE)){SysCtlDelay(400);};
         while(I2CMasterBusy(I2C0_BASE)){SysCtlDelay(400);};
         I2CMasterSlaveAddrSet(I2C0_BASE, slaveAddress, 0);
         I2CMasterDataPut(I2C0_BASE,devid);//11100101
         I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);//0x03
         do{SysCtlDelay(400);}while(I2CMasterBusy(I2C0_BASE));
         I2CMasterSlaveAddrSet(I2C0_BASE, slaveAddress, 1);
         I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);//0x07
         do{SysCtlDelay(400);}while(I2CMasterBusy(I2C0_BASE));
         while(I2CMasterBusBusy(I2C0_BASE)){SysCtlDelay(400);};
         de=I2CMasterDataGet(I2C0_BASE);
         i=7;
         I2CState=I2CMasterErr(I2C0_BASE);


*/
        //fahui
        while(I2CMasterBusBusy(I2C0_BASE)){SysCtlDelay(400);};
         while(I2CMasterBusy(I2C0_BASE)){SysCtlDelay(400);};
         I2CMasterSlaveAddrSet(I2C0_BASE, slaveAddress, 0);
         I2CMasterDataPut(I2C0_BASE, 0x31);
         I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);//0x03
         do{SysCtlDelay(400);}while(I2CMasterBusy(I2C0_BASE));
         I2CMasterSlaveAddrSet(I2C0_BASE, slaveAddress, 0);
         I2CMasterDataPut(I2C0_BASE, 0x08);
         I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);//0x05
         do{SysCtlDelay(400);}while(I2CMasterBusy(I2C0_BASE));
         while(I2CMasterBusBusy(I2C0_BASE)){SysCtlDelay(400);};
        I2CState=I2CMasterErr(I2C0_BASE);

        while(I2CMasterBusBusy(I2C0_BASE)){SysCtlDelay(400);};
                 while(I2CMasterBusy(I2C0_BASE)){SysCtlDelay(400);};
                 I2CMasterSlaveAddrSet(I2C0_BASE, slaveAddress, 0);
                 I2CMasterDataPut(I2C0_BASE, 0x2D);
                 I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);//0x03
                 do{SysCtlDelay(400);}while(I2CMasterBusy(I2C0_BASE));
                 I2CMasterSlaveAddrSet(I2C0_BASE, slaveAddress, 0);
                 I2CMasterDataPut(I2C0_BASE, 0x08);
                 I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);//0x05
                 do{SysCtlDelay(400);}while(I2CMasterBusy(I2C0_BASE));
                 while(I2CMasterBusBusy(I2C0_BASE)){SysCtlDelay(400);};
                I2CState=I2CMasterErr(I2C0_BASE);

        while(I2CMasterBusBusy(I2C0_BASE)){SysCtlDelay(400);};
                 while(I2CMasterBusy(I2C0_BASE)){SysCtlDelay(400);};
        //devid = HWREG(I2C0_BASE + 0x00) ;    //0x00偏移地址   读取DEVID寄存器的值
        //PB2 PB3 SDA SCL
        I2CMasterSlaveAddrSet(I2C0_BASE, slaveAddress, 0);
        I2CMasterDataPut(I2C0_BASE, firstRegAddress);
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
        do{SysCtlDelay(400);}while(I2CMasterBusy(I2C0_BASE));
        for(i=0;i<6;i++){
        if(i==0){
        I2CMasterSlaveAddrSet(I2C0_BASE, slaveAddress, 1);
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);//0x0b
        }else if(i==5){
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);//0x05
        }else{
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);//0x09
        }
        do{SysCtlDelay(400);}while(I2CMasterBusy(I2C0_BASE));
        dataBuffer[i]=I2CMasterDataGet(I2C0_BASE);
        I2CState=I2CMasterErr(I2C0_BASE);
        }

        x = ((short)dataBuffer[1] << 8) + dataBuffer[0];
        y = ((short)dataBuffer[3] << 8) + dataBuffer[2];
        z = ((short)dataBuffer[5] << 8) + dataBuffer[4];
        gx=x*0.004*9.8;
        gy=y*0.004*9.8;
        gz=z*0.004*9.8;

        if(gx<0) zx=-gx;
        else zx=gx;

        if(gy<0) zy=-gy;
                else zy=gy;

        if(gz<0) zz=-gz;
                else zz=gz;

        ggx[0]=(int)zx/10;
        ggy[0]=(int)zy/10;
        ggz[0]=(int)zz/10;

        ggx[1]=(int)zx-ggx[0]*10;
        ggy[1]=(int)zy-ggy[0]*10;
        ggz[1]=(int)zz-ggz[0]*10;

        ggx[2]=(int)(zx*10)-ggx[0]*100-ggx[1]*10;
        ggy[2]=(int)(zy*10)-ggy[0]*100-ggy[1]*10;
        ggz[2]=(int)(zz*10)-ggz[0]*100-ggz[1]*10;

        sx[0]=ggx[0]+48;
        sx[1]=ggx[1]+48;
        sx[3]=ggx[2]+48;

        sy[0]=ggy[0]+48;
        sy[1]=ggy[1]+48;
        sy[3]=ggy[2]+48;

        sz[0]=ggz[0]+48;
        sz[1]=ggz[1]+48;
        sz[3]=ggz[2]+48;

        sx[2]='.';
        sy[2]='.';
        sz[2]='.';

        sx[4]='\0';
                sy[4]='\0';
                sz[4]='\0';

        LCD_gotoXY(1,0);
       LCD_sendstr(sx);
       LCD_gotoXY(2,0);
      LCD_sendstr(sy);
        LCD_gotoXY(3,0);
      LCD_sendstr(sz);
      delay(1*g_ui32SysClock);

    }

}
