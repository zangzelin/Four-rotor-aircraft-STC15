#include"STC15W4K.h"
#include"HD7279.h"
#include"chuankou.h"
#include"PWM.h"
#include"CONTRaL.h"
#include"oled.h"


/*
made by lzjs
used in 2016 college students electronic design contest
// UART1 		timer1
// ticyic 	timer0
// AD 			timer2
*/


void main(void)
{

RES = 1;
DC = 0;
CS = 0;
	Initial_M096128x64_ssd1306();
	
	while(1)
	{
//	SCL = 1;	SDA = 1;		 //  Delay_50ms(1);
// 	Delay_50ms(1);	
//    SCL = 0;	 SDA = 0;
//	Delay_50ms(1);
		fill_picture(0xff);//全屏显示
		Delay_50ms(25);
		fill_picture(0xf0);//半屏熄灭；出现一条一条的亮线
		Delay_50ms(25);
		Picture();//显示一张图片
		Delay_50ms(25);

		

	}
}



