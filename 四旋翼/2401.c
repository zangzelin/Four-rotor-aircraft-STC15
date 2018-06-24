/***********************************************************/

/*nRF2401学习板V1.0                                        */

/*功能描述：nRF2401无线数据传输                            */

/*芯片    ：STC89C52RC  晶振 11.0592M                      */

/*版本    ：V1.0                                           */

/***********************************************************/

//测试OK，功能描述:

//8个LED接在P0口，低电平有效

//按键1是LED加 按键2LED减  按键3 LED全亮 按键4 LED全灭

//两块同样的板子能够通过无线控制实现LED显示同步，串口输出数据

#include <reg51.h>

#include <intrins.h>

#define uchar unsigned char

#define uint  unsigned int

#define ulong unsigned long


//端口定义

sbit nRF2401_PWR_UP = P1^4;

sbit nRF2401_CE     = P1^5;

sbit nRF2401_CS     = P1^7;

//////////////////////

                      

///////LED定义//////////

sbit LED1  = P0^0;

sbit LED2  = P0^1;

sbit LED3  = P0^2;

sbit LED4  = P0^3;

sbit LED5  = P0^4;

sbit LED6  = P0^5;

sbit LED7  = P0^6;

sbit LED8  = P0^7;

///////按键定义////////////

sbit K1  = P3^3;

sbit K2  = P3^4;

sbit K3  = P3^5;

sbit K4  = P3^6;


sbit speaker=P3^7;//蜂鸣器输出引脚


//Configuration Word       datasheet(page17 of 37)

//Reserved for testing

#define TEST_2         0x8E     //MSB    D143~D136 

#define TEST_1         0x08     //       D135~D128 

#define TEST_0         0x1C     //       D127~D120 


#define DATA2_W        2*8      //通道2 payload数据长度设置(bit)

#define DATA1_W        28*8      //通道1 payload数据长度设置(bit) 

//NOTE: DATAx_W(bits)<=256 - ADDR_W - CRC                datasheet(page17 of 37)  

#define ADDR2_4      0x00//通道2接收地址   

#define ADDR2_3      0x00

#define ADDR2_2      0x00

#define ADDR2_1      0x00

#define ADDR2_0      0x01

               

#define ADDR1_4      0x00//通道1接收地址 

#define ADDR1_3      0x00

#define ADDR1_2      0x00

#define ADDR1_1      0x34

#define ADDR1_0      0x12


#define ADDR_W       2*8    //发送/接收地址宽度(单位:Bit) 

#define CRC_L        0x1    //CRC 模式  0:8 位  1:16 位 

#define CRC_EN       0x1    //CRC 校验  0:禁用  1:启用 


#define RX2_EN       0x0    //双通道功能0:禁用  1:启用 

#define CM           0x1    //0:Direct mode   1:ShockBurst mode 

#define RFDR_SB      0x0    //传输速率0:250kbps 1:1Mbps(requires 16MHZ crystal)

#define XO_F         0x3    //晶振频率 D12-D10(000:4MHZ  001:8MHZ  010:12MHZ  011:16MHZ  100:20MHZ) 

#define RF_PWR       0x3    //发射模式的输出功率D9-D8(00:-20dBm  01:-10dBm  10:-5dBm  11:0dBm)


#define RF_CH        0x2    //Channel 发射频率设置Channel=2400MHz+RF_CH * 1.0MHz  datasheet(page22 of 37)  

#define RXEN         0x0    //0:transmit mode    1:receive mode


uchar code nRF2401_Config_Word[18] =

{

TEST_2, TEST_1, TEST_0, DATA2_W, DATA1_W, ADDR2_4, ADDR2_3, ADDR2_2,

ADDR2_1,ADDR2_0,ADDR1_4,ADDR1_3, ADDR1_2, ADDR1_1, ADDR1_0,(ADDR_W<<2 | CRC_L<<1 | CRC_EN),

(RX2_EN<<7 | CM<<6 | RFDR_SB<<5 | XO_F <<2 | RF_PWR),(RF_CH<<1 | RXEN)

};


uchar code Channel_Addr[]={0x34,0x12,0x00,0x00,0x00};//设置通道1地址，发射和接收必须匹配

uchar nRF2401_Data[32]; //定义nRF2401数据发送接收临时存储地址

uchar nRF2401_flag;//定义标志位，bit7-bit2 暂时保留 | bit0:是否有接收数据标志位 1:有 0:无

                                                  //| bit1:是否有按键数据标志位 1:有 0:无

/*****************************************/

void delayms(uint count); //延时程序

void speakertest(void);   //蜂鸣器程序

uchar nRF2401_Read_Byte(void); //读字节

void nRF2401_Write_Byte(uchar Send_Byte);//写字节

void nRF2401_config(void);   //参数配置

void nRF2401_Transmit_Mode(void); //设置为发射模式

void nRF2401_Receive_Mode(void);  //设置为接收模式

void nRF2401_Transmit_Data(uchar TxBuf[]);  //nRF2401发送数据子程序

void nRF2401_Receive_Data(uchar *data_temp);//nRF2401接收数据子程序

void init_rs232(void);           //串口初始化

void keytest(void);  //按键测试程序

/*****************************************/

/*************延时子程序start*****************/

void delayms(unsigned int count)

{

unsigned int i,j;

for(i=0;i<count;i++)

for(j=0;j<120;j++);

}

/**************延时子程序end****************/

/* 串口通信初始化设置 */

void init_rs232(void)

{

SCON=0x50;

PCON=0x00;

TH1=0xFD;    //波特率 9600  11.0592MHZ 时候

TL1=0XFD;

TMOD=0X20;

TR1=1;    

}

void speakertest(void)//蜂鸣器程序

{

unsigned char i;

  for(i=0;i<100;i++)

{

  speaker=0;

  delayms(1);

  speaker=1;

  delayms(1);

  }

}

/*****************************************/

void keytest(void)  //按键测试程序

{

////////按键K1判断程序////////

  while(K1==0) 

  {

  delayms(100);//按键去抖

   if(K1==1) 

   {

   nRF2401_flag|=0x02; //按键标志位nRF2401_flag_bit1=1

   speakertest();

   nRF2401_Data[0]++;

   }

   else

   {;}

  }

/////////////////////////////

////////按键K2判断程序////////

  while(K2==0) 

  {

   delayms(100);//按键去抖

   if(K2==1) 

   {

   nRF2401_flag|=0x02; //按键标志位nRF2401_flag_bit1=1

   speakertest();

   nRF2401_Data[0]--;

   }

   else

   {nRF2401_flag&=0xfd; } //清除按键标志位nRF2401_flag_bit1=0

  }

/////////////////////////////

////////按键K3判断程序////////

  while(K3==0) 

  {

  delayms(100);//按键去抖

   if(K3==1) 

   {

   nRF2401_flag|=0x02; //按键标志位nRF2401_flag_bit1=1

   speakertest();

   nRF2401_Data[0]=0x00;

   }

   else

    {nRF2401_flag&=0xfd; } //清除按键标志位nRF2401_flag_bit1=0

  }

/////////////////////////////

////////按键K4判断程序////////

  while(K4==0) 

  {

  delayms(100);//按键去抖

   if(K4==1) 

   {

   nRF2401_flag|=0x02; //按键标志位nRF2401_flag_bit1=1

   speakertest();

   nRF2401_Data[0]=0xff;  

   }

   else

   {nRF2401_flag&=0xfd; } //清除按键标志位nRF2401_flag_bit1=0

  }


  if((nRF2401_flag&0x02)==0x02)//判断是否有按键按下

   {

   P0=~nRF2401_Data[0];

   nRF2401_Transmit_Mode();        //设置为发送模式

   nRF2401_Transmit_Data(nRF2401_Data); //发送数据

   nRF2401_Receive_Mode();         //设置为接收模式

   nRF2401_flag&=0xfd; //清除按键标志位nRF2401_flag_bit1=0  

  }

  else

  {;}

   

}

/************读数据子程序***********/

uchar nRF2401_Read_Byte(void)

{

  uchar i,j,temp;

  temp=0x00;

  for (i=0;i<8;i++)

   {

      CLK1=1;

      _nop_();

      if (DATA==1) j=1;

      else j=0;

      temp=(temp<<1)|j;

      CLK1=0;

   }

  return temp;

}

/************写数据子程序***********/

void nRF2401_Write_Byte(uchar Send_Byte)

{

uchar i;

for (i=0;i<8;i++)

   {

      Send_Byte=Send_Byte<<1;

   DATA=CY; //读进位数据

      CLK1=1;

      _nop_();   

      CLK1=0;

   }

}



/*********配置参数子程序***********/

void nRF2401_config(void)

{

uchar i; 

//////stand by///////////// 

nRF2401_PWR_UP=1;

    nRF2401_CE=0;

    nRF2401_CS=0;

/////////////////////////////      datasheet(page24 of 37)

delayms(3);// PWR_DWN -> ST_BY_mode | MAX 3ms | name: Tpd2sby | 

nRF2401_CS=1;     //进入配置模式Configuration

////18个字节的配置字////////////// datasheet(page17 of 37)

for (i=0;i<18;i++)

{

     nRF2401_Write_Byte(nRF2401_Config_Word);

}

delayms(1);

nRF2401_CS=0;    //nRF2401_CS 置低使配置有效  datasheet(page26 of 37);

}


/*********设置发射模式子程序*********/

void nRF2401_Transmit_Mode(void)

{

nRF2401_PWR_UP=1; //进入配置模式

nRF2401_CE=0;

nRF2401_CS=1;

delayms(1); 

//RXEN(bit0)=0 transmit mode ; datasheet(page22 of 37);

DATA =0;

CLK1=1; 

_nop_();

CLK1=0;

//设置为Active(Tx)模式 

nRF2401_CE=1;

nRF2401_CS=0;

    delayms(1); 

}

/*********设置接收模式子程序*********/

void nRF2401_Receive_Mode(void)

{

nRF2401_PWR_UP = 1; //进入配置模式

nRF2401_CE=0;

nRF2401_CS=1;

delayms(1);

//RXEN(bit0)=1 receive mode ; datasheet(page22 of 37);

DATA =1;

CLK1=1;

_nop_();

CLK1=0;

//设置为Active(Rx)模式

    nRF2401_CE=1;

nRF2401_CS=0;

delayms(1);

}


//接收方通道硬件地址


/********发送数据子程序*********/

void nRF2401_Transmit_Data(uchar TxBuf[]) //datasheet(page22 of 37);

{

uchar i;

nRF2401_CE=1;

delayms(1);

for (i=0;i< (ADDR_W/8);i++)//写入接收地址(按字节对齐)

{

  nRF2401_Write_Byte(Channel_Addr);

}

for (i=0;i<(DATA1_W/8);i++)//写入需要发送的数据(按字节对齐)

{

  nRF2401_Write_Byte(TxBuf);

}

nRF2401_CE=0; //nRF2401_CE 置低使发送有效

delayms(1);

}


/********接收数据子程序*********/

void nRF2401_Receive_Data(uchar *data_temp)

{

uchar i;

if (DR1==1) //接收完毕后，DR1自动为0  datasheet(page28 of 37);

{

    for (i=0; i<DATA1_W/8; i++)

    {

  *data_temp=nRF2401_Read_Byte();

  data_temp++;

     }

  nRF2401_flag|=0x01; //接收标志位nRF2401_flag_bit0=1

}

else

{nRF2401_flag&=0xfe;}//接收标志位nRF2401_flag_bit0=0

}




void main(void)

{

    delayms(1000);//开机延时

nRF2401_Data[0] = 0;

    init_rs232();  //串口初始化

    TI=1;          //串口发射有效

nRF2401_config(); //nRF2401初始化配置

    delayms(100);

speakertest();  //蜂鸣器叫一声


nRF2401_Transmit_Mode();            //设置为发送模式

nRF2401_Transmit_Data(nRF2401_Data);//发送测试数据

nRF2401_Receive_Mode();             //设置为接收模式

SBUF=0x55; //串口输出0x55 测试是否有正确的串口数据

delayms(100);

while (1)

{   

  nRF2401_Receive_Data(nRF2401_Data);//接收数据，并存放到nRF2401_Data[32]数组中

  if (nRF2401_flag&0x01==0x01) //判断是否有数据收到

  {

   P0=~nRF2401_Data[0];

      SBUF=nRF2401_Data[0];         

  }

        else

  {;}

      keytest();  //调用按键子程序，无线发送数据 

}

}