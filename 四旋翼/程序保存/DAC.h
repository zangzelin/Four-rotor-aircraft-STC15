#ifndef  __DAC
#define  __DAC

#include"STC15W4K.h"
#include"HD7279.h"
#include"chuankou.h"
#include"PWM.h"


#define CMD_RESET 0xa4
#define CMD_TEST 0xbf
#define DECODE0 0x80
#define DECODE1 0xc8
#define CMD_READ 0x15
#define UNDECODE 0x90
#define RTL_CYCLE 0xa3
#define RTR_CYCLE 0xa2
#define RTL_UNCYL 0xa1
#define RTR_UNCYL 0xa0
#define ACTCTL 0x98
#define SEGON 0xe0
#define SEGOFF 0xc0
#define BLINKCTL 0x88

 


sbit      dacs = P5^ 2;       //片选
sbit     dasclk = P4^ 3;       //时钟
sbit     dadin = P4 ^ 0;       //数据入口


void write5615(WORD da);
void init_5615();
void timer_isr();
void change_5615(u16 ppmv,u8 frequency);
void change_5615fudu(u16 ppmv);
#endif	