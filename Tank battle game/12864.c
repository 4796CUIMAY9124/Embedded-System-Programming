/*
 * 12864.c
 *
 *  Created on: 2016��5��16��
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
extern unsigned char bombtemp0[];//����֮ǰ��ͼ��
extern unsigned char bombtemp[];//����֮���ͼ��
extern unsigned char const bombu[];
extern unsigned char const bombd[];
extern unsigned char const bombl[];
extern unsigned char const bombr[];

extern unsigned char sum[];
/******************************************
��������: delay
��    ��: ��ʱ����
��    ��: ��
����ֵ  : ��
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
��������: Lcd_init_basic()
��    ��: 12864�˿ڳ�ʼ��
��    ��: ��
����ֵ  : ��
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

//Lcd_instruction(0x30) :�����趨;
LCD_Senddata(CMD, 0x30);


//Lcd_instruction(0x0C) :display on;
LCD_Senddata(CMD, 0x0C);

//Lcd_instruction(0x01) :clear;
LCD_Senddata(CMD, 0x01);


//Lcd_instruction(0x06)��������趨;
LCD_Senddata(CMD, 0x06);

}



/******************************************
��������: Lcd_clear()
��    ��: 12864����
��    ��: ��
����ֵ  : ��
********************************************/
void Lcd_clear(){

	LCD_Senddata(CMD, 0x01);

}




/******************************************
��������: LCD_Senddata
��    ��: ��12864Һ��д��һ���ֽ����ݻ���ָ��
��    ��: DatCmd--ΪDATʱ�����ݣ�ΪCMDʱ��ָ��
	  	  dByte--Ϊд��12864�����ݻ���ָ��
����ֵ  : ��
********************************************/


void LCD_Senddata(uint32_t DatCmad,uint32_t dByte)
{

	while(LCD_busycheck());

	if(DatCmad==CMD)
		LCD_RS_L;//ָ�����
	else
		LCD_RS_H;//���ݲ���

	LCD_RW_L;
/////// д���� /////////
	uint32_t temp=0;
	temp=(dByte&0x01)<<4;
	GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,temp);//��DB0дdByte�����λ
	temp=(dByte&0x02)<<4;
	GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_5,temp);//��DB1дdByte�ĵ����ڶ�λ
	temp=(dByte&0x04)>>1;
	GPIOPinWrite(GPIO_PORTH_BASE,GPIO_PIN_1,temp);//��DB2дdByte�ĵ�������λ
	temp=(dByte&0x08)<<1;
	GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_4,temp);//��DB3дdByte�ĵ�������λ
	temp=dByte&0x10;
	GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_4,temp);//��DB4дdByte�ĵ�������λ
	temp=dByte&0x20;
	GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_5,temp);//��DB5дdByte�ĵ�������λ
    temp=(dByte&0x40)>>2;
	GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_4,temp);//��DB6дdByte�ĵ�������λ
	temp=(dByte&0x80)>>2;
	GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_5,temp);//��DB7дdByte�ĵ����ڰ�λ
	LCD_EN_H;
	delay(120);
	LCD_EN_L;

}

/*******************************************
��������: LCD12864_sendstr
��    ��: ��12864Һ��д��һ���ַ���
��    ��: ptString--�ַ���ָ��
����ֵ  : ��
********************************************/
void LCD_sendstr(uint8_t *ptString)
{
	while((*ptString)!='\0')		 //�ַ���δ����һֱд
	{
		LCD_Senddata(DAT,*ptString++);
	}
}

/*******************************************
��������: LCD12864_gotoXY
��    ��: �ƶ���ָ��λ��
��    ��: Row--ָ������
	  	  Col--ָ������
����ֵ  : ��
********************************************/
void LCD_gotoXY(uint32_t Row, uint32_t Col)
{
	switch (Row)		  //ѡ����
	{
		case 2:
			LCD_Senddata(CMD, LCD_L2 + Col); break;	//д���2�е�ָ����
		case 3:
			LCD_Senddata(CMD, LCD_L3 + Col); break;	//д���3�е�ָ����
		case 4:
			LCD_Senddata(CMD, LCD_L4 + Col); break;	//д���4�е�ָ����
		default:
			LCD_Senddata(CMD, LCD_L1 + Col); break;	//д���1�е�ָ����
	}
}


/*******************************************
��������: LDC_draw
��    ��: ��ͼ���Ժ���
��    ��:dir,x,y

����ֵ  : ��
********************************************/
void LDC_draw(int dir,int x,int y,int b)
{
    int i,j;
    int r,c;//��¼����
	LCD_Senddata(CMD, 0x34);//��λ���ݣ�����ָ�����

	//�޸�ͼ��,�������� ��bΪ1.\2\3..��ֻ��bombͼ��bΪ0����������
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

	////x����xΪ�������ص�����
   	if(x>0){
   	    char out=0x00;//���ƺ󱻼���ȥ��ֵ
        char outb=0x00;//���ƺ󱻼���ȥ��ֵ

   	    for(i=0;i<x;i++){
   	        for(r=0;r<64;r++){
   	            for(c=0;c<16;c++){//������
   	                //BitmapDottemp[r][c]=BitmapDot[r][c]/2|out;//���ƺ�ƴ����һ�����һλ
   	                //out=BitmapDot[r][c]&0x01;//��ȡ���һλ
   	                BitmapDottemp[r*16+c]=(BitmapDot[r*16+c] >> 1) | out;//���ƺ�ƴ����һ�����һλ
   	                out=BitmapDot[r*16+c] & 0x01;//��ȡ���һλ
   	                out = out << 7;//��һ�����һλ��ɵ�һλ


   	             if(c!=0){
   	              bombtemp0[r*16+c]=(bomb[r*16+c] >> 1) | outb;//���ƺ�ƴ����һ�����һλ
   	             }
   	             else{
   	              bombtemp0[r*16+c]=(bomb[r*16+c] >> 1);
   	             }
   	             outb=bomb[r*16+c] & 0x01;//��ȡ���һλ
   	             outb = outb << 7;//��һ�����һλ��ɵ�һλ

   	            }
   	        }
   	        for(j=0;j<1024;j++){
   	            BitmapDot[j]=BitmapDottemp[j];
   	            bomb[j]=bombtemp0[j];
   	        }
   	    }
   	}
   	else if(x<0){//����
   	    char out=0x00;//���ƺ󱻼���ȥ��ֵ
        char outb=0x00;//���ƺ󱻼���ȥ��ֵ

   	    for(i=0;i<(-x);i++){
   	             for(r=0;r<64;r++){
   	                 for(c=15;c>=0;c--){//���ҵ���
   	                     BitmapDottemp[r*16+c]=BitmapDot[r*16+c]*2|out;//���ƺ�ƴ�Ӻ�һ����һλ
   	                     out=BitmapDot[r*16+c]&0x80;//��ȡ��һλ
   	                     out=out>>7;//��һ����һλ������һλ

   	                     if(c!=15){
   	                                    bombtemp0[r*16+c]=(bomb[r*16+c] << 1) | outb;//���ƺ�ƴ����һ�����һλ
   	                                   }
   	                     else{
   	                                    bombtemp0[r*16+c]=(bomb[r*16+c] << 1);
   	                                   }
   	                     outb=bomb[r*16+c]&0x80;//��ȡ��һλ
   	                     outb=outb>>7;//��һ����һλ������һλ

   	                 }
   	             }
   	             for(j=0;j<1024;j++){
   	                          BitmapDot[j]=BitmapDottemp[j];
   	                          bomb[j]=bombtemp0[j];
   	                      }
   	    }
   	}
   	////y����
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
	    /////bomb>=1,ֻ��bombͼ
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
    ////x����xΪ�������ص�����
    if(x>0){
        char outb=0x00;//���ƺ󱻼���ȥ��ֵ

        for(i=0;i<x;i++){
            for(r=0;r<64;r++){
                for(c=0;c<16;c++){//������

                    if(c!=0){
                                      bombtemp[r*16+c]=(bomb[r*16+c] >> 1) | outb;//���ƺ�ƴ����һ�����һλ
                                     }
                                     else{
                                      bombtemp[r*16+c]=(bomb[r*16+c] >> 1);
                                     }
                                     outb=bomb[r*16+c] & 0x01;//��ȡ���һλ
                                     outb = outb << 7;//��һ�����һλ��ɵ�һλ
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
    else if(x<0){//����
        char outb=0x00;//���ƺ󱻼���ȥ��ֵ
        for(i=0;i<(-x);i++){
                 for(r=0;r<64;r++){
                     for(c=15;c>=0;c--){//���ҵ���

                         if(c!=15){
                             bombtemp[r*16+c]=(bomb[r*16+c] << 1) | outb;//���ƺ�ƴ����һ�����һλ
                             }
                         else{
                             bombtemp[r*16+c]=(bomb[r*16+c] << 1);
                         }
                         outb=bomb[r*16+c]&0x80;//��ȡ��һλ
                         outb=outb>>7;//��һ����һλ������һλ
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
    ////y����
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

	if(b){//��ʾ����
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
	LCD_Senddata(CMD, 0x34);//��λ���ݣ�����ָ��������رջ�ͼ
	for(col=0;col<32;col++){

//		LCD_Senddata(CMD, 0x34);//��λ���ݣ�����ָ��������رջ�ͼ
		LCD_Senddata(CMD, 0x80 + col);//���봹ֱ�е�ַ
		LCD_Senddata(CMD, 0x80);//�е�ַ���㿪ʼ
		for(row=0;row<16;row++){
			LCD_Senddata(DAT,*(sum+16*col+row));
		}
		for(row=0;row<16;row++){
			LCD_Senddata(DAT,*(sum+16*(col+32)+row));
		}


//		LCD_Senddata(CMD, 0x36);//��λ���ݣ�����ָ�������������ͼ

	}

	LCD_Senddata(CMD, 0x36);//��λ���ݣ�����ָ�������������ͼ
	LCD_Senddata(CMD, 0x30);//��λ���ݣ�����ָ��
}
/*******************************************
��������: changebomb
��    ��: �ı�bombͼ
��    ��:

����ֵ  : ��
********************************************/
void changebomb(){
    int m;
    if(!flag){//δ��ʼ��
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

    else if (flag){//�Ѿ���ʼ��
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
��������: LDC_draw_bomb
��    ��: ��ͼ���Ժ���
��    ��:dir,x,y

����ֵ  : ��
********************************************/
/*
void LDC_draw_bomb(int dir,int x,int y){
    int flag0=1;//flag=1���ڵ�û�г���flag=0���ڵ�����
    int i,j;

        //////�ڵ�
        switch(dir){
                case 2://up
                    {
                        flag0=1;
                        for(j=0;(j<14)&&(flag0);j++){
                            //ֻ��bombͼ
                            LDC_draw(dir, x, y+j*5,1);
                            //��ʱ
                            delay(ui32SysClock);

                            for(i=0;i<1024;i++){//flagȫ��Ϊ0����ͼΪ1
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
                        //ֻ��bombͼ
                        LDC_draw(dir, x, y-j*5,1);
                        //��ʱ
                        delay(ui32SysClock);

                        for(i=0;i<1024;i++){//flagȫ��Ϊ0����ͼΪ1
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
                    //ֻ��bombͼ
                    LDC_draw(dir, x-j*10, y,1);
                    //��ʱ
                    delay(ui32SysClock*1);
                    for(i=0;i<1024;i++){//flagȫ��Ϊ0����ͼΪ1
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
                    //ֻ��bombͼ
                    LDC_draw(dir, x+j*10, y,1);
                    //��ʱ
                    delay(ui32SysClock*1);
                    for(i=0;i<1024;i++){//flagȫ��Ϊ0����ͼΪ1
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
	LCD_RS_L;//ָ�����
	LCD_RW_H;
	delay(120);
	LCD_EN_H;
	delay(120);
	tmp=GPIOPinRead(GPIO_PORTN_BASE,GPIO_PIN_5);//��DB7дdByte�ĵ����ڰ�λ
	LCD_EN_L;
	delay(120);
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE,GPIO_PIN_5);//PN5
	return tmp && 1;

}

