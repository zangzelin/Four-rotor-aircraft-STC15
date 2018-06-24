#ifndef  __CONTRAL
#define  __CONTRAL

#include"STC15W4K.h"
#include"HD7279.h"
#include"chuankou.h"
#include"PWM.h"
#include"CONTRaL.h"


#define maxfre  110
#define minfre  5
#define maxmVpp 5000
#define minmVpp 50

s16 contral_pid(s16 error,s16 lastseeor);
u16 limit_fudu(u16 fudu );


#endif	