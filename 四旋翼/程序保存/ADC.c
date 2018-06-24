#include"STC15W4K.h"
#include"HD7279.h"
#include"chuankou.h"
#include"PWM.h"
#include"ADC.h"

extern BYTE ch;                        //ADC通道号 
u16 sum1 = 0;
extern u8 adcdata[100];
u16 adccount = 0;

void adc_isr() interrupt 5 using 1
{
	
		if( adccount >=1 )
		{
			adccount = 1;
			EADC = 0;
		}
		else
		{
			
			ADC_CONTR &= !ADC_FLAG;          					 //	清除ADC中断标志
			adcdata[adccount] = ADC_RES;          //读取高8位结果并发送到串口
			adccount++;
			ADC_CONTR = ADC_POWER | ADC_SPEEDHH | ADC_START | ch;
		}
      
    
}


void Init_ADC()
{
    P1ASF = 0xff;                   //设置P1口为AD口
    ADC_RES = 0;                    //清除结果寄存器
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
    DelayADC(2);                       //ADC上电并延时                    //ADC上电并延时
}

void DelayADC(WORD n)
{
    WORD x;

    while (n--)
    {
        x = 50000;
        while (x--);
    }
}
u16 getadc(u8 chin)
{
	u16 result = 0;
		ADC_CONTR = ADC_POWER | ADC_SPEEDLL | chin | ADC_START;
    _nop_();                        //等待4个NOP
    _nop_();
    _nop_();
    _nop_();
    while (!(ADC_CONTR & ADC_FLAG));//等待ADC转换完成
    ADC_CONTR &= ~ADC_FLAG;         //Close ADC

    result =  ADC_RES*4+ADC_RESL;                 //返回ADC结果

	return result;
}

