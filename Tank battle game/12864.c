/*
 * 12864.c
 *
 *  Created on: 2016年5月16日
 *      Author: Administrator
 */


#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include"inc/hw_types.h"
#include"inc/hw_memmap.h"
#include"driverlib/timer.h"
#include"driverlib/interrupt.h"
#include"driverlib/systick.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include <string.h>
#include "12864.h"

//#include "2.h"

void Lcd_init_basic();
void LCD_Senddata(uint32_t DatCmad,uint32_t dByte);
void delay(uint32_t num);
void LCD_sendstr(uint8_t *ptString);
void LCD_gotoXY(uint32_t Row, uint32_t Col);
/*
extern unsigned char BitmapDot[64][16];
extern unsigned char BitmapDottemp[64][16];
extern unsigned char const BitmapDotup[64][16];
extern unsigned char const BitmapDotdown[64][16];
extern unsigned char const BitmapDotleft[64][16];
extern unsigned char const BitmapDotright[64][16];
*/
extern unsigned int ui32SysClock;
//extern int b;
extern int flag;
extern int dir;

extern unsigned char BitmapDot[];
extern unsigned char BitmapDottemp[];
extern unsigned char const BitmapDotup[];
extern unsigned char const BitmapDotdown[];
extern unsigned char const BitmapDotleft[];
extern unsigned char const BitmapDotright[];

extern unsigned char bomb[];
extern unsigned char bombtemp0[];//发射之前的图形
extern unsigned char bombtemp[];//发射之后的图形
extern unsigned char const bombu[];
extern unsigned char const bombd[];
extern unsigned char const bombl[];
extern unsigned char const bombr[];

extern unsigned char sum[];
/******************************************
函数名称: delay
功    能: 延时函数
参    数: 无
返回值  : 无
********************************************/
void delay(uint32_t num){

 TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
  TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE,TIMER_A,num-1);
TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
 TimerEnable(TIMER0_BASE, TIMER_A);
 while(!TimerIntStatus(TIMER0_BASE,false)){

	}
}

/******************************************
函数名称: Lcd_init_basic()
功    能: 12864端口初始化
参    数: 无
返回值  : 无
********************************************/
void Lcd_init_basic(){

SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

LCD_RS_RW;
LCD_EN_PSB;
LCD_RST;

LCD_DB0_DB1;
LCD_DB2;
LCD_DB3;
LCD_DB4_DB5;
LCD_DB6_DB7;
LCD_PSB_EN;
delay(4800000);
LCD_RST_NO;
delay(1200);
LCD_RST_EN;

//Lcd_instruction(0x30) :功能设定;
LCD_Senddata(CMD, 0x30);


//Lcd_instruction(0x0C) :display on;
LCD_Senddata(CMD, 0x0C);

//Lcd_instruction(0x01) :clear;
LCD_Senddata(CMD, 0x01);


//Lcd_instruction(0x06)：进入点设定;
LCD_Senddata(CMD, 0x06);

}



/******************************************
函数名称: Lcd_clear()
功    能: 12864清屏
参    数: 无
返回值  : 无
********************************************/
void Lcd_clear(){

	LCD_Senddata(CMD, 0x01);

}




/******************************************
函数名称: LCD_Senddata
功    能: 向12864液晶写入一个字节数据或者指令
参    数: DatCmd--为DAT时是数据，为CMD时是指令
	  	  dByte--为写入12864的数据或者指令
返回值  : 无
********************************************/


void LCD_Senddata(uint32_t DatCmad,uint32_t dByte)
{

	while(LCD_busycheck());

	if(DatCmad==CMD)
		LCD_RS_L;//指令操作
	else
		LCD_RS_H;//数据操作

	LCD_RW_L;
/////// 写操作 /////////
	uint32_t temp=0;
	temp=(dByte&0x01)<<4;
	GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,temp);//向DB0写dByte的最低位
	temp=(dByte&0x02)<<4;
	GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_5,temp);//向DB1写dByte的倒数第二位
	temp=(dByte&0x04)>>1;
	GPIOPinWrite(GPIO_PORTH_BASE,GPIO_PIN_1,temp);//向DB2写dByte的倒数第三位
	temp=(dByte&0x08)<<1;
	GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_4,temp);//向DB3写dByte的倒数第四位
	temp=dByte&0x10;
	GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_4,temp);//向DB4写dByte的倒数第五位
	temp=dByte&0x20;
	GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_5,temp);//向DB5写dByte的倒数第六位
    temp=(dByte&0x40)>>2;
	GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_4,temp);//向DB6写dByte的倒数第七位
	temp=(dByte&0x80)>>2;
	GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_5,temp);//向DB7写dByte的倒数第八位
	LCD_EN_H;
	delay(120);
	LCD_EN_L;

}

/*******************************************
函数名称: LCD12864_sendstr
功    能: 向12864液晶写入一个字符串
参    数: ptString--字符串指针
返回值  : 无
********************************************/
void LCD_sendstr(uint8_t *ptString)
{
	while((*ptString)!='\0')		 //字符串未结束一直写
	{
		LCD_Senddata(DAT,*ptString++);
	}
}

/*******************************************
函数名称: LCD12864_gotoXY
功    能: 移动到指定位置
参    数: Row--指定的行
	  	  Col--指定的列
返回值  : 无
********************************************/
void LCD_gotoXY(uint32_t Row, uint32_t Col)
{
	switch (Row)		  //选择行
	{
		case 2:
			LCD_Senddata(CMD, LCD_L2 + Col); break;	//写入第2行的指定列
		case 3:
			LCD_Senddata(CMD, LCD_L3 + Col); break;	//写入第3行的指定列
		case 4:
			LCD_Senddata(CMD, LCD_L4 + Col); break;	//写入第4行的指定列
		default:
			LCD_Senddata(CMD, LCD_L1 + Col); break;	//写入第1行的指定列
	}
}


/*******************************************
函数名称: LDC_draw
功    能: 画图测试函数
参    数:dir,x,y

返回值  : 无
********************************************/
void LDC_draw(int dir,int x,int y,int b)
{
    int i,j;
    int r,c;//记录行列
	LCD_Senddata(CMD, 0x34);//八位数据，扩充指令操作

	//修改图形,两个都改 。b为1.\2\3..，只改bomb图；b为0，两个都改
	if(!b){
	    switch(dir){
	    case 2://up
	        for(i=0;i<1024;i++) {
	            BitmapDot[i]=BitmapDotup[i];
	            bomb[i]=bombu[i];
	        }
	    break;
	    case 8://d
	        for(i=0;i<1024;i++){
	        BitmapDot[i]=BitmapDotdown[i];
            bomb[i]=bombd[i];
	        }
	    break;
	    case 4://l
	        for(i=0;i<1024;i++){   // BitmapDot[i/16][i%16]=BitmapDotup[i];
	        BitmapDot[i]=BitmapDotleft[i];
	        bomb[i]=bombl[i];
	        }
	        break;
	    case 6://r
	        for(i=0;i<1024;i++){
	            BitmapDot[i]=BitmapDotright[i];
	            bomb[i]=bombr[i];
	        }
	        break;
   	}

	////x方向，x为右移像素点数量
   	if(x>0){
   	    char out=0x00;//右移后被挤出去的值
        char outb=0x00;//右移后被挤出去的值

   	    for(i=0;i<x;i++){
   	        for(r=0;r<64;r++){
   	            for(c=0;c<16;c++){//从左到右
   	                //BitmapDottemp[r][c]=BitmapDot[r][c]/2|out;//右移后拼接上一级最后一位
   	                //out=BitmapDot[r][c]&0x01;//求取最后一位
   	                BitmapDottemp[r*16+c]=(BitmapDot[r*16+c] >> 1) | out;//右移后拼接上一级最后一位
   	                out=BitmapDot[r*16+c] & 0x01;//求取最后一位
   	                out = out << 7;//上一级最后一位变成第一位


   	             if(c!=0){
   	              bombtemp0[r*16+c]=(bomb[r*16+c] >> 1) | outb;//右移后拼接上一级最后一位
   	             }
   	             else{
   	              bombtemp0[r*16+c]=(bomb[r*16+c] >> 1);
   	             }
   	             outb=bomb[r*16+c] & 0x01;//求取最后一位
   	             outb = outb << 7;//上一级最后一位变成第一位

   	            }
   	        }
   	        for(j=0;j<1024;j++){
   	            BitmapDot[j]=BitmapDottemp[j];
   	            bomb[j]=bombtemp0[j];
   	        }
   	    }
   	}
   	else if(x<0){//左移
   	    char out=0x00;//左移后被挤出去的值
        char outb=0x00;//左移后被挤出去的值

   	    for(i=0;i<(-x);i++){
   	             for(r=0;r<64;r++){
   	                 for(c=15;c>=0;c--){//从右到左
   	                     BitmapDottemp[r*16+c]=BitmapDot[r*16+c]*2|out;//左移后拼接后一级第一位
   	                     out=BitmapDot[r*16+c]&0x80;//求取第一位
   	                     out=out>>7;//后一级第一位变成最后一位

   	                     if(c!=15){
   	                                    bombtemp0[r*16+c]=(bomb[r*16+c] << 1) | outb;//右移后拼接上一级最后一位
   	                                   }
   	                     else{
   	                                    bombtemp0[r*16+c]=(bomb[r*16+c] << 1);
   	                                   }
   	                     outb=bomb[r*16+c]&0x80;//求取第一位
   	                     outb=outb>>7;//后一级第一位变成最后一位

   	                 }
   	             }
   	             for(j=0;j<1024;j++){
   	                          BitmapDot[j]=BitmapDottemp[j];
   	                          bomb[j]=bombtemp0[j];
   	                      }
   	    }
   	}
   	////y方向
   	if(y>0){
   	    for(r=0;r<64;r++){
   	        for(c=0;c<16;c++){
   	            if(r<64-y) {
   	                BitmapDottemp[r*16+c]=BitmapDot[16*(r+y)+c];
   	                bombtemp0[r*16+c]=bomb[16*(r+y)+c];
   	            }
   	            else if(r>=64-y){
   	                BitmapDottemp[r*16+c]=BitmapDot[16*(r+y-64)+c];
   	                bombtemp0[r*16+c]=0x00;
   	            }
   	        }
   	    }
        for(i=0;i<1024;i++){
            BitmapDot[i]=BitmapDottemp[i];
            bomb[i]=bombtemp0[i];
        }
   	}
   	else if(y<0){
   	    for(r=0;r<64;r++){
   	             for(c=0;c<16;c++){
   	                 if(r>=-y) {
   	                     BitmapDottemp[r*16+c]=BitmapDot[16*(r+y)+c];
   	                     bombtemp0[r*16+c]=bomb[16*(r+y)+c];
   	                 }
   	                 else if(r<-y){
   	                     BitmapDottemp[r*16+c]=BitmapDot[16*(r+y+64)+c];
   	                     bombtemp0[r*16+c]=0x00;
   	                 }
   	             }
   	    }
   	    for(i=0;i<1024;i++){
   	             BitmapDot[i]=BitmapDottemp[i];
   	             bomb[i]=bombtemp0[i];
   	         }
   	}
	}

	else if(b){
	    /////bomb>=1,只改bomb图
	    /*
	    switch(dir){
	    case 2://up
        for(i=0;i<1024;i++) {
            bomb[i]=bombu[i];
        }
        break;
    case 8://d
        for(i=0;i<1024;i++){
            bomb[i]=bombd[i];
        }
        break;
    case 4://l
        for(i=0;i<1024;i++){   // BitmapDot[i/16][i%16]=BitmapDotup[i];
            bomb[i]=bombl[i];
        }
        break;
    case 6://r
        for(i=0;i<1024;i++){
            bomb[i]=bombr[i];
        }
        break;
    }
*/
    ////x方向，x为右移像素点数量
    if(x>0){
        char outb=0x00;//右移后被挤出去的值

        for(i=0;i<x;i++){
            for(r=0;r<64;r++){
                for(c=0;c<16;c++){//从左到右

                    if(c!=0){
                                      bombtemp[r*16+c]=(bomb[r*16+c] >> 1) | outb;//右移后拼接上一级最后一位
                                     }
                                     else{
                                      bombtemp[r*16+c]=(bomb[r*16+c] >> 1);
                                     }
                                     outb=bomb[r*16+c] & 0x01;//求取最后一位
                                     outb = outb << 7;//上一级最后一位变成第一位
                   /*
                 if(c>x){
                     bombtemp[r*16+c]=bomb[16*(r)+c-x];
                 }
                 else {
                     bombtemp[r*16+c]=0x00;
                 }*/

                }
            }
            for(j=0;j<1024;j++){
                bomb[j]=bombtemp[j];
            }
        }
    }
    else if(x<0){//左移
        char outb=0x00;//左移后被挤出去的值
        for(i=0;i<(-x);i++){
                 for(r=0;r<64;r++){
                     for(c=15;c>=0;c--){//从右到左

                         if(c!=15){
                             bombtemp[r*16+c]=(bomb[r*16+c] << 1) | outb;//右移后拼接上一级最后一位
                             }
                         else{
                             bombtemp[r*16+c]=(bomb[r*16+c] << 1);
                         }
                         outb=bomb[r*16+c]&0x80;//求取第一位
                         outb=outb>>7;//后一级第一位变成最后一位
                                             /*
                         if(c<16+x){
                                           bombtemp[r*16+c]=bomb[16*(r)+c-x];
                         }
                         else {
                                           bombtemp[r*16+c]=0x00;
                         }*/
                     }
                 }
              for(j=0;j<1024;j++){
                              bomb[j]=bombtemp[j];
                          }
        }
    }
    ////y方向
    if(y>0){
        for(r=0;r<64;r++){
            for(c=0;c<16;c++){
                if(r<64-y) {
                    bombtemp[r*16+c]=bomb[16*(r+y)+c];
                }
                else if(r>64-y){
                    bombtemp[r*16+c]=0x00;
                }
            }
        }
        for(i=0;i<1024;i++){
            bomb[i]=bombtemp[i];
        }
    }
    else if(y<0){
        for(r=0;r<64;r++){
                 for(c=0;c<16;c++){
                     if(r>-y) {
                         bombtemp[r*16+c]=bomb[16*(r+y)+c];
                     }
                     else if(r<-y){
                         bombtemp[r*16+c]=0x00;
                     }
                 }
        }
     for(i=0;i<1024;i++){
                 bomb[i]=bombtemp[i];
             }
    }
	}

	if(b){//显示两者
	    for(i=0;i<1024;i++){
	                    sum[i]=bomb[i] | BitmapDot[i];
	    }
	}
	else{
	    for(i=0;i<1024;i++){
	                            sum[i]=BitmapDot[i];
	    }
	}

	int col,row;
	LCD_Senddata(CMD, 0x34);//八位数据，扩充指令操作，关闭绘图
	for(col=0;col<32;col++){

//		LCD_Senddata(CMD, 0x34);//八位数据，扩充指令操作，关闭绘图
		LCD_Senddata(CMD, 0x80 + col);//输入垂直行地址
		LCD_Senddata(CMD, 0x80);//列地址从零开始
		for(row=0;row<16;row++){
			LCD_Senddata(DAT,*(sum+16*col+row));
		}
		for(row=0;row<16;row++){
			LCD_Senddata(DAT,*(sum+16*(col+32)+row));
		}


//		LCD_Senddata(CMD, 0x36);//八位数据，扩充指令操作，开启绘图

	}

	LCD_Senddata(CMD, 0x36);//八位数据，扩充指令操作，开启绘图
	LCD_Senddata(CMD, 0x30);//八位数据，基本指令
}
/*******************************************
函数名称: changebomb
功    能: 改变bomb图
参    数:

返回值  : 无
********************************************/
void changebomb(){
    int m;
    if(!flag){//未初始化
    switch(dir){
    case 2://up
        for(m=0;m<1024;m++) {
            bomb[m]=bombu[m];
        }
        break;
    case 8://d
        for(m=0;m<1024;m++){
            bomb[m]=bombd[m];
        }
        break;
    case 4://l
        for(m=0;m<1024;m++){   // BitmapDot[i/16][i%16]=BitmapDotup[i];
            bomb[m]=bombl[m];
        }
        break;
    case 6://r
        for(m=0;m<1024;m++){
            bomb[m]=bombr[m];
        }
        break;
    }
    flag=1;
    }

    else if (flag){//已经初始化
        switch(dir){
            case 2://up
                for(m=0;m<1024;m++) {
                    bomb[m]=bombtemp0[m]|bomb[m];
                }
                break;
            case 8://d
                for(m=0;m<1024;m++){
                    bomb[m]=bombtemp0[m]|bomb[m];
                }
                break;
            case 4://l
                for(m=0;m<1024;m++){   // BitmapDot[i/16][i%16]=BitmapDotup[i];
                    bomb[m]=bombtemp0[m]|bomb[m];
                }
                break;
            case 6://r
                for(m=0;m<1024;m++){
                    bomb[m]=bombtemp0[m]|bomb[m];
                }
                break;
            }
    }
}


/*******************************************
函数名称: LDC_draw_bomb
功    能: 画图测试函数
参    数:dir,x,y

返回值  : 无
********************************************/
/*
void LDC_draw_bomb(int dir,int x,int y){
    int flag0=1;//flag=1，炮弹没有出框，flag=0，炮弹出框
    int i,j;

        //////炮弹
        switch(dir){
                case 2://up
                    {
                        flag0=1;
                        for(j=0;(j<14)&&(flag0);j++){
                            //只改bomb图
                            LDC_draw(dir, x, y+j*5,1);
                            //延时
                            delay(ui32SysClock);

                            for(i=0;i<1024;i++){//flag全空为0，有图为1
                                        if( bomb[i]!=0x00)
                                        {
                                            flag0=1;
                                            break;
                                        }
                                        flag0=0;
                            }
                        }
                        b-=1;
                        break;
                    }
                case 8://d
                {
                    flag0=1;
                    for(j=0;(j<14)&&(flag0);j++){
                        //只改bomb图
                        LDC_draw(dir, x, y-j*5,1);
                        //延时
                        delay(ui32SysClock);

                        for(i=0;i<1024;i++){//flag全空为0，有图为1
                                                        if( bomb[i]!=0x00)
                                                        {
                                                            flag0=1;
                                                            break;
                                                        }
                                                        flag0=0;
                        }
                    }
                    b-=1;
                    break;
                }
            case 4://l
            {
                flag0=1;
                for(j=0;(j<10)&&(flag0);j++){
                    //只改bomb图
                    LDC_draw(dir, x-j*10, y,1);
                    //延时
                    delay(ui32SysClock*1);
                    for(i=0;i<1024;i++){//flag全空为0，有图为1
                                                            if( bomb[i]!=0x00)
                                                            {
                                                                flag0=1;
                                                                break;
                                                            }
                                                            flag0=0;
                                                }


                }
                b-=1;
                break;
            }
            case 6://r
            {
                flag0=1;
                for(j=0;(j<10)&&(flag0);j++){
                    //只改bomb图
                    LDC_draw(dir, x+j*10, y,1);
                    //延时
                    delay(ui32SysClock*1);
                    for(i=0;i<1024;i++){//flag全空为0，有图为1
                                                            if( bomb[i]!=0x00)
                                                            {
                                                                flag0=1;
                                                                break;
                                                            }
                                                            flag0=0;
                                                }


                }
                b-=1;
                break;
            }
        }
}
*/

/////******************************************
int LCD_busycheck(){

	int tmp;
	GPIOPinTypeGPIOInput(GPIO_PORTN_BASE,GPIO_PIN_5);//PN5
	LCD_RS_L;//指令操作
	LCD_RW_H;
	delay(120);
	LCD_EN_H;
	delay(120);
	tmp=GPIOPinRead(GPIO_PORTN_BASE,GPIO_PIN_5);//向DB7写dByte的倒数第八位
	LCD_EN_L;
	delay(120);
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE,GPIO_PIN_5);//PN5
	return tmp && 1;

}

