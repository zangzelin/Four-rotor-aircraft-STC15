
#include"STC15W4K.h"
#include"HD7279.h"
#include"chuankou.h"

//extern bit busy;


void init_chuankou()
{
    P0M0 = 0x00;
    P0M1 = 0x00;
    P1M0 = 0x00;
    P1M1 = 0x00;
    P2M0 = 0x00;
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    P4M0 = 0x00;
    P4M1 = 0x00;
    P5M0 = 0x00;
    P5M1 = 0x00;
    P6M0 = 0x00;
    P6M1 = 0x00;
    P7M0 = 0x00;
    P7M1 = 0x00;

    ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
    


#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;                //8??????
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;                //9??????,??????1
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;                //9??????,??????0
#endif

    AUXR = 0x40;                //???1?1T??
    TMOD = 0x00;                //???1???0(16?????)
    TL1 = (65536 - (FOSC/4/BAUD));   //????????
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
    TR1 = 1;                    //???1????
    ES = 1;                     //??????
    EA = 1;
	//PS = 1;
    
	}

	void Uart() interrupt 4 using 1
{
    if (RI)
    {
        RI = 0;                 //??RI?
        P0 = SBUF;              //P0??????
        P22 = RB8;              //P2.2?????
    }
    if (TI)
    {
        TI = 0;                 //??TI?
        busy = 0;               //????
    }
}

void SendData(BYTE dat)
{
    while (busy);               //???????????
    ACC = dat;                  //?????P (PSW.0)
    if (P)                      //??P??????
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;                //??????0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;                //??????1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;                //??????1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;                //??????0
#endif
    }
    busy = 1;
    SBUF = ACC;                 //????UART?????
}

void SendString(char *s)
{
    while (*s)                  //?????????
    {
        SendData(*s++);         //??????
    }
}

void SendNumber(s16 num)
{
	u8 number[5] = {0,0,0,0,0};
	u8 digit = 0;
	u8 i = 0;
	
	if(num<0)
		SendData('-'),num = -num;
		
	while(num)
	{
		number[i++] = num%10;
		num = num/10;
	}
	if(i == 0)
		SendData('0');
	else
	{		
		while(i--)
		{
			SendData(number[i]+'0');
		}
  }
	SendString("\t");

}