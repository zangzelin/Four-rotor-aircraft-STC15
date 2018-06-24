#include"STC15W4K.h"
#include"HD7279.h"
#include"chuankou.h"
#include"PWM.h"
#include"ADC.h"

extern BYTE ch;                        //ADCͨ���� 
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
			
			ADC_CONTR &= !ADC_FLAG;          					 //	���ADC�жϱ�־
			adcdata[adccount] = ADC_RES;          //��ȡ��8λ��������͵�����
			adccount++;
			ADC_CONTR = ADC_POWER | ADC_SPEEDHH | ADC_START | ch;
		}
      
    
}


void Init_ADC()
{
    P1ASF = 0xff;                   //����P1��ΪAD��
    ADC_RES = 0;                    //�������Ĵ���
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
    DelayADC(2);                       //ADC�ϵ粢��ʱ                    //ADC�ϵ粢��ʱ
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
    _nop_();                        //�ȴ�4��NOP
    _nop_();
    _nop_();
    _nop_();
    while (!(ADC_CONTR & ADC_FLAG));//�ȴ�ADCת�����
    ADC_CONTR &= ~ADC_FLAG;         //Close ADC

    result =  ADC_RES*4+ADC_RESL;                 //����ADC���

	return result;
}

