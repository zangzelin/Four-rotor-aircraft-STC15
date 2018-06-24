
#ifndef  __CHUANKOU
#define  __CHUANKOU

#include"STC15W4K.h"
#include"HD7279.h"
//????Keil????????Intel?8058????????
//??????,???????11.0592MHz



typedef unsigned char BYTE;
typedef unsigned int WORD;

#define FOSC 24000000L          //????
#define BAUD 115200             //?????

#define NONE_PARITY     0       //???
#define ODD_PARITY      1       //???
#define EVEN_PARITY     2       //???
#define MARK_PARITY     3       //????
#define SPACE_PARITY    4       //????
#define PARITYBIT EVEN_PARITY   //?????
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

extern bit busy;


void init_chuankou();
void Uart();
void SendData(BYTE dat);
void SendString(char *s);
void SendNumber(s16 num);

#endif		 