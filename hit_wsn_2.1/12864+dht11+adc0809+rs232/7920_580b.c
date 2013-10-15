/*
DHT11_DATA---------------P2^0

LCD_PSB------------------P2^2
LCD_RES------------------P2^4
LCD_RS-------------------P2^5
LCD_WRD------------------P2^6
LCD_E--------------------P2^7
LCD_DATA-----------------P0

ADC0809_CLOCK------------P3^7
ADC0809_EOC--------------P3^6
ADC0809_ST---------------P3^5
ADC0809_OE---------------P3^4
ADC0809_SELECT_A---------P3^2
ADC0809_SELECT_B---------P3^3
ADC0809_SELECT_C---------GND
ADC0809_DATA-------------P1

RS232_TX-----------------P3^1
*/
#include <reg52.h>
#include <string.h>
#include <intrins.h>

#define uchar unsigned char
#define uint  unsigned int

/******************************第一部分LCD  START******************************/
#define LINE_1  0x80
#define LINE_2	0x90
#define LINE_3  0x88
#define LINE_4  0x98


sbit RS  = P2^5;
sbit WRD = P2^6;
sbit E   = P2^7;
sbit PSB = P2^2;
sbit RES = P2^4;

void TransferData(char data1,bit DI);
void display(void);
void display_grapic(void);
void delayms(uint n);
void DisplayLine(uchar line1,uchar line2);
void DisplayGraphic(uchar code *adder);
void delay(uint m);
void lcd_mesg(uchar code *adder1);
void setline(char line);
void write_string(uchar *string);
/***************************************************
函数名称：initinal()
函数作用：初始化LCD字库
函数参数：无
函数返回值：无
***************************************************/
void initinal(void)           //LCD字库初始化程序
{
    delay(40);             //大于40MS的延时程序
    PSB=1;                 //设置为8BIT并口工作模式
    delay(1);              //延时
    RES=0;                 //复位
    delay(1);              //延时
    RES=1;                 //复位置高
    delay(10);
    TransferData(0x30,0);  //Extended Function Set :8BIT设置,RE=0: basic instruction set, G=0 :graphic display OFF
    delay(100);            //大于100uS的延时程序
    TransferData(0x30,0);  //Function Set
    delay(37);             ////大于37uS的延时程序
    TransferData(0x08,0);  //Display on Control
    delay(100);            //大于100uS的延时程序
    TransferData(0x10,0);  //Cursor Display Control光标设置
    delay(100);            //大于100uS的延时程序
    TransferData(0x0C,0);  //Display Control,D=1,显示开
    delay(100);            //大于100uS的延时程序
    TransferData(0x01,0);  //Display Clear
    delay(10);             //大于10mS的延时程序
    TransferData(0x06,0);  //Enry Mode Set,光标从右向左加1位移动
    delay(100);            //大于100uS的延时程序
}

/********************************************************************
函数名称：initinal2()
函数作用：初始化LCD图片库（即如果要显示的是图片必须调用该初始化）
函数参数：无
函数返回值：无
***********************************************************************/
void     initina2(void)           //LCD显示图片(扩展)初始化程序
{
    delay(40);             //大于40MS的延时程序
    PSB=1;                 //设置为8BIT并口工作模式
    delay(1);              //延时
    RES=0;                 //复位
    delay(1);              //延时
    RES=1;                 //复位置高
    delay(10);

    TransferData(0x36,0);  //Extended Function Set RE=1: extended instruction
    delay(100);            //大于100uS的延时程序
    TransferData(0x36,0);  //Extended Function Set:RE=1: extended instruction set
    delay(37);             ////大于37uS的延时程序
    TransferData(0x3E,0);  //EXFUNCTION(DL=8BITS,RE=1,G=1)
    delay(100);            //大于100uS的延时程序
    TransferData(0x01,0);  //CLEAR SCREEN
    delay(100);            //大于100uS的延时程序
}

/********************************************************************
函数名称：setline()
函数作用：将光标移至待显示行的首地址
函数参数：确定的行号
函数返回值：无
***********************************************************************/
void setline(char line)
{
    /*
    if(line == 1)
        TransferData(0x80,0);  //Set Graphic Display RAM Address
    else if(line == 2)
    	TransferData(0x90,0);
    else if(line == 3)
    	TransferData(0x88,0);
    else
    */
    TransferData(line,0);
    delay(100);
}

/********************************************************************
函数名称：lcd_mesg()
函数作用：显示汉字子程序
函数参数：待显示字符串的首地址	此函数用于显示CODE区中的字符
函数返回值：无
***********************************************************************/
void   lcd_mesg(unsigned char code *adder1)
{
    unsigned char i;
    TransferData(0x80,0);  //Set Graphic Display RAM Address
    delay(100);
    for (i=0;i<32;i++)
    {
        TransferData(*adder1,1);
        adder1++;
    }

    TransferData(0x90,0);  //Set Graphic Display RAM Address
    delay(100);
    for (i=32;i<64;i++)
    {
        TransferData(*adder1,1);
        adder1++;
    }
}

/********************************************************************
函数名称：write_dat()
函数作用：显示字符
函数参数：待显示非CODE区的字符
函数返回值：无
***********************************************************************/
void write_string(uchar *string)
{
    unsigned char len, i;
    len = strlen(string);
    for (i=0;i<len;i++)
    {
        TransferData(*string,1);
        string++;
    }
}

/********************************************************************
函数名称：TransferData()
函数作用：写指令/数据函数
函数参数：待写字符以及标志位
函数返回值：无
***********************************************************************/
void    TransferData(char data1,bit DI)  //传送数据或者命令,当DI=0是,传送命令,当DI=1,传送数据.
{
    WRD=0;
    RS=DI;
    delay(1);
    P0=data1;
    E=1;
    delay(1);
    E=0;
}

/********************************************************************
函数名称：DisplayGraphic()
函数作用：显示图片函数
函数参数：存储图像CODE的首地址
函数返回值：无
***********************************************************************/
void DisplayGraphic(unsigned char code *adder)
{

    int i,j;
//*******显示上半屏内容设置
    for (i=0;i<32;i++)             //
    {
        TransferData((0x80 + i),0); //SET  垂直地址 VERTICAL ADD
        TransferData(0x80,0);       //SET  水平地址 HORIZONTAL ADD
        for (j=0;j<16;j++)
        {
            TransferData(*adder,1);
            adder++;
        }
    }
//*******显示下半屏内容设置
    for (i=0;i<32;i++)             //
    {
        TransferData((0x80 + i),0); //SET 垂直地址 VERTICAL ADD
        TransferData(0x88,0);       //SET 水平地址 HORIZONTAL ADD
        for (j=0;j<16;j++)
        {
            TransferData(*adder,1);
            adder++;
        }
    }
}

/********************************************************************
函数名称：delayms()
函数作用：延时函数，延时10 X N 毫秒
函数参数：N
函数返回值：无
***********************************************************************/
void    delayms(unsigned int n)            //延时10×n毫秒程序
{
    unsigned int i,j;
    for (i=0;i<n;i++)
        for (j=0;j<2000;j++);
}

/********************************************************************
函数名称：delay()
函数作用：延时函数，延时？？？
函数参数：无
函数返回值：无
***********************************************************************/
void    delay(unsigned int m)            //延时程序
{
    unsigned int i,j;
    for (i=0;i<m;i++)
        for (j=0;j<10;j++);
}
/******************************第一部分LCD  END***********************************************************************************/

/******************************第二部分DHT11  START*******************************************************************************/

sbit DHT11_DATA  = P2^0;

unsigned char U8FLAG;  //该标志主要用于计时
unsigned char U8temp;  //读取数据的每一位值
unsigned char U8comdata; //读取数据（8位合在一起）
unsigned char U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp; //未校验之前的数据
unsigned char U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;  //经校验之后的数据

/********************************************************************
函数名称：Delay()
函数作用：延时函数，延时？？？
函数参数：无
函数返回值：无
***********************************************************************/
void DHT11_Delay(unsigned int j)
{
    unsigned char i;
    for (;j>0;j--)
    {
        for (i=0;i<27;i++);

    }
}

/********************************************************************
函数名称：Delay_10us()
函数作用：延时函数，延时10 us
函数参数：无
函数返回值：无
***********************************************************************/
void  DHT11_Delay_10us(void)
{
    unsigned char i;
    i--;
    i--;
    i--;
    i--;
    i--;
    i--;
}

/********************************************************************
函数名称：COM()
函数作用：dht11按时序读温湿度值
函数参数：无
函数返回值：无
***********************************************************************/
void  COM(void)
{

    unsigned char i;
    for (i=0;i<8;i++)
    {

        U8FLAG=2;
        //----------------------
        //----------------------

        while ((!DHT11_DATA)&&U8FLAG++);
        DHT11_Delay_10us();
        DHT11_Delay_10us();
        //	DHT11_Delay_10us();
        U8temp=0;
        if (DHT11_DATA)U8temp=1;
        U8FLAG=2;
        while ((DHT11_DATA)&&U8FLAG++);

        //----------------------
        //----------------------
        //超时则跳出for循环
        if (U8FLAG==1)break;
        //判断数据位是0还是1

        // 如果高电平高过预定0高电平值则数据位为 1

        U8comdata<<=1;
        U8comdata|=U8temp;        //0
    }//rof

}

/********************************************************************
函数名称：RH()
函数作用：向DHT11写命令、读数据等
函数参数：无
函数返回值：无
***********************************************************************/
void RH(void)
{
    //主机拉低18ms
    DHT11_DATA=0;
    DHT11_Delay(180);
    DHT11_DATA=1;
    //总线由上拉电阻拉高 主机延时20us
    DHT11_Delay_10us();
    DHT11_Delay_10us();
    DHT11_Delay_10us();
    DHT11_Delay_10us();
    //主机设为输入 判断从机响应信号
    DHT11_DATA=1;
    //判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行
    if (!DHT11_DATA)		 //T !
    {
        U8FLAG=2;
        //判断从机是否发出 80us 的低电平响应信号是否结束
        while ((!DHT11_DATA)&&U8FLAG++);
        U8FLAG=2;
        //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
        while ((DHT11_DATA)&&U8FLAG++);
        //数据接收状态
        COM();
        U8RH_data_H_temp=U8comdata;
        COM();
        U8RH_data_L_temp=U8comdata;
        COM();
        U8T_data_H_temp=U8comdata;
        COM();
        U8T_data_L_temp=U8comdata;
        COM();
        U8checkdata_temp=U8comdata;
        DHT11_DATA=1;
        //数据校验

        U8temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
        if (U8temp==U8checkdata_temp)
        {
            U8RH_data_H=U8RH_data_H_temp;
            U8RH_data_L=U8RH_data_L_temp;
            U8T_data_H=U8T_data_H_temp;
            U8T_data_L=U8T_data_L_temp;
            U8checkdata=U8checkdata_temp;
        }//fi
    }//fi

}

/******************************第二部分DHT11  END********************************************************************************************/

/******************************第三部分 烟雾 START*******************************************************************************************/
sbit CLOCK = P3^7;
sbit EOC = P3^6;
sbit ST = P3^5;
sbit OE = P3^4;

sbit SELECT_A = P3^2;
sbit SELECT_B = P3^3;

uint ADCReg;
/********************************************************************
函数名称：delay()
函数作用：adc0809用的普通延时
函数参数：n
函数返回值：无
***********************************************************************/
void delay_0809(uint n)
{
    uint i,j;
    for (i=0; i<n; i++)
        for (j=0; j<124; j++)
            _nop_();
}

/********************************************************************
函数名称：adc_init()
函数作用：adc0809初始化，设置定时器0
函数参数：n
函数返回值：无
***********************************************************************/
void adc_init(void)
{
    TMOD=0x02;
    TH0=0x14;
    TL0=0x00;
    IE=0x82;
    TR0=1;
    delay_0809(10);
}

/********************************************************************
函数名称：timer0int()
函数作用：定时器0中断，用于设置ADC0809的工作时钟
函数参数：无
函数返回值：无
***********************************************************************/
void timer0int(void) interrupt 1
{
    CLOCK = ! CLOCK;
}
/******************************第三部分 烟雾 END  *******************************************************************************************/

/******************************第四部分 RS232 START******************************************************************************************/
/********************************************************************
函数名称：put_char()
函数作用：将一个字符发送缓冲器
函数参数：待发送字符
函数返回值：无
***********************************************************************/
void put_char(unsigned char data1)
{
    SBUF = data1;               //将待发送的字符送入发送缓冲器
    while (TI == 0);           //等待发送完成
    TI = 0;                     //发送中断标志请0
}

/********************************************************************
函数名称：delay_232()
函数作用：串口延时n毫秒
函数参数：n
函数返回值：无
***********************************************************************/
void delay_232(uint count)
{
    uint i,j;
    for (i=0;i<count;i++)
    {
        for (j=0;j<110;j++);
    }
}

/********************************************************************
函数名称：putstring()
函数作用：发送字符串函数
函数参数：字符串首地址
函数返回值：无
***********************************************************************/
void putstring(unsigned char *dat)
{
    while (*dat != '\0')          //判断字符串是否发送完毕
    {
        put_char(*dat);        //发送单个字符
        dat++;
        //字符地址加1，指向先下一个字符
        delay_232(5);
    }
}

/********************************************************************
函数名称：rs232_init()
函数作用：串口始化
函数参数：无
函数返回值：无
***********************************************************************/
void rs232_init()
{
    SCON = 0x40;
    TMOD = 0x20;
    PCON = 0x00;
    TL1  = 0xfd;
    TH1  = 0xfd;
    TI   = 0;
    TR1  = 1;
    delay_232(200);
}
/******************************第四部分 RS232 END********************************************************************************************/

/********************************************************************
* 名称 : Main()
* 功能 : 主函数
* 输入 : 无
* 输出 : 无
***********************************************************************/
unsigned char code welcome[]=
{
    "     HIT-WSN    "
    "        欢迎    "
    "     welcome    "
    "            2010"
};

unsigned int chartoint(unsigned char ch)
{
    unsigned int temp = 0;
    unsigned int i;

    for (i=0;i<8;i++)
    {
        if (ch & 0x80)
            temp = temp * 2 + 1;
        else
            temp = temp * 2;
        ch = ch << 1;
    }
    return temp;
}

void    main(void)
{
    unsigned char ch[16];
    unsigned char wendu[6],shidu[6],yanwu[6],co[6],jiawan[6];
	unsigned char send[30];	   
    unsigned int wendu_h, wendu_l,shidu_h,shidu_l;

    initinal();   		 //调用LCD字库初始化程序
    delay(100);            //大于100uS的延时程序
    lcd_mesg(welcome);      //显示中文汉字1
    delayms(240);
    delayms(240);

   
    initinal();
    adc_init();

    while (1)
    {
		initinal();
        setline(LINE_1);
        strcpy(ch,"温度");
        write_string((unsigned char *)ch);
        
        RH();
        wendu_h = chartoint(U8T_data_H);
        wendu_l = chartoint(U8T_data_L);
        wendu[0] = wendu_h / 10 + '0';
        wendu[1] = wendu_h % 10 + '0';
        wendu[2] = '.';
        wendu[3] = wendu_l / 10 + '0';
        wendu[4] = 'C';
        setline(LINE_2);
        write_string((unsigned char *)wendu);
		delay_232(1000);

		setline(LINE_3);
        strcpy(ch,"湿度");
        write_string((unsigned char *)ch);
        shidu_h = chartoint(U8RH_data_H);
        shidu_l = chartoint(U8RH_data_L);
        shidu[0] = shidu_h / 10 + '0';
        shidu[1] = shidu_h % 10 + '0';
        shidu[2] = '.';
        shidu[3] = shidu_l / 10 + '0';
        shidu[4] = '%';
        setline(LINE_4);
        write_string((unsigned char *)shidu);
		delay_232(1000);

		SELECT_A = 0;
		SELECT_B = 0;
		ST = 0;
        ST = 1;
        while (EOC){}
        ST = 0;
        while (!EOC){}
        OE=1;

        ADCReg = P1;
        delay_0809(100);
        OE = 0;
        ADCReg = ADCReg*100/51;

        yanwu[0] = ADCReg/100+'0';
        yanwu[1] = '.';
        yanwu[2] = (ADCReg%100)/10+'0';
        yanwu[3] = ADCReg%10+'0';
        yanwu[4] = 'V';

        initinal();
        setline(LINE_1);
        strcpy(ch,"烟雾");
        write_string((unsigned char *)ch);
        setline(LINE_2);
        write_string((unsigned char *)yanwu);
		delay_232(1000);

		SELECT_A = 1;
		SELECT_B = 0;
        ST = 0;
        ST = 1;
        while (EOC){}
        ST = 0;
        while (!EOC){}
        OE=1;

        ADCReg = P1;
        delay_0809(100);
        OE = 0;
        ADCReg = ADCReg*100/51;

        co[0] = ADCReg/100+'0';
        co[1] = '.';
        co[2] = (ADCReg%100)/10+'0';
        co[3] = ADCReg%10+'0';
        co[4] = 'V';
		setline(LINE_3);
		strcpy(ch,"一氧化碳");
		write_string((unsigned char *)ch);
		setline(LINE_4);
		write_string((unsigned char *)co);
		delay_232(1000);

		SELECT_A = 0;
		SELECT_B = 1;
        ST = 0;
        ST = 1;
        while (EOC){}
        ST = 0;
        while (!EOC){}
        OE=1;

        ADCReg = P1;
        delay_0809(100);
        OE = 0;
        ADCReg = ADCReg*100/51;

        jiawan[0] = ADCReg/100+'0';
        jiawan[1] = '.';
        jiawan[2] = (ADCReg%100)/10+'0';
        jiawan[3] = ADCReg%10+'0';
        jiawan[4] = 'V';

        initinal();
        setline(LINE_1);
        strcpy(ch,"甲烷");
        write_string((unsigned char *)ch);
        setline(LINE_2);
        write_string((unsigned char *)jiawan); 
		delay_232(1000);

		send[0] = wendu[0];send[1] = wendu[1];send[2] = wendu[2];send[3] = wendu[3];send[4] = wendu[4];send[5] = ' ';
		send[6] = shidu[0];send[7] = shidu[1];send[8] = shidu[2];send[9] = shidu[3];send[10] = shidu[4];send[11] = ' ';
		send[12] = yanwu[0];send[13] = yanwu[1];send[14] = yanwu[2];send[15] = yanwu[3];send[16] = yanwu[4];send[17] = ' ';
		send[18] = co[0];send[19] = co[1];send[20] = co[2];send[21] = co[3];send[22] = co[4];send[23] = ' ';
		send[24] = jiawan[0];send[25] = jiawan[1];send[26] = jiawan[2];send[27] = jiawan[3];send[28] = jiawan[4];send[29] = ' ';
		rs232_init();
		delayms(10);
		putstring((unsigned char *)send);
		//putstring("0123456789");
		delayms(100);
    }
}
