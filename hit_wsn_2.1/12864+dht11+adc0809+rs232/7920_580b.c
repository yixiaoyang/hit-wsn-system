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

/******************************��һ����LCD  START******************************/
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
�������ƣ�initinal()
�������ã���ʼ��LCD�ֿ�
������������
��������ֵ����
***************************************************/
void initinal(void)           //LCD�ֿ��ʼ������
{
    delay(40);             //����40MS����ʱ����
    PSB=1;                 //����Ϊ8BIT���ڹ���ģʽ
    delay(1);              //��ʱ
    RES=0;                 //��λ
    delay(1);              //��ʱ
    RES=1;                 //��λ�ø�
    delay(10);
    TransferData(0x30,0);  //Extended Function Set :8BIT����,RE=0: basic instruction set, G=0 :graphic display OFF
    delay(100);            //����100uS����ʱ����
    TransferData(0x30,0);  //Function Set
    delay(37);             ////����37uS����ʱ����
    TransferData(0x08,0);  //Display on Control
    delay(100);            //����100uS����ʱ����
    TransferData(0x10,0);  //Cursor Display Control�������
    delay(100);            //����100uS����ʱ����
    TransferData(0x0C,0);  //Display Control,D=1,��ʾ��
    delay(100);            //����100uS����ʱ����
    TransferData(0x01,0);  //Display Clear
    delay(10);             //����10mS����ʱ����
    TransferData(0x06,0);  //Enry Mode Set,�����������1λ�ƶ�
    delay(100);            //����100uS����ʱ����
}

/********************************************************************
�������ƣ�initinal2()
�������ã���ʼ��LCDͼƬ�⣨�����Ҫ��ʾ����ͼƬ������øó�ʼ����
������������
��������ֵ����
***********************************************************************/
void     initina2(void)           //LCD��ʾͼƬ(��չ)��ʼ������
{
    delay(40);             //����40MS����ʱ����
    PSB=1;                 //����Ϊ8BIT���ڹ���ģʽ
    delay(1);              //��ʱ
    RES=0;                 //��λ
    delay(1);              //��ʱ
    RES=1;                 //��λ�ø�
    delay(10);

    TransferData(0x36,0);  //Extended Function Set RE=1: extended instruction
    delay(100);            //����100uS����ʱ����
    TransferData(0x36,0);  //Extended Function Set:RE=1: extended instruction set
    delay(37);             ////����37uS����ʱ����
    TransferData(0x3E,0);  //EXFUNCTION(DL=8BITS,RE=1,G=1)
    delay(100);            //����100uS����ʱ����
    TransferData(0x01,0);  //CLEAR SCREEN
    delay(100);            //����100uS����ʱ����
}

/********************************************************************
�������ƣ�setline()
�������ã��������������ʾ�е��׵�ַ
����������ȷ�����к�
��������ֵ����
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
�������ƣ�lcd_mesg()
�������ã���ʾ�����ӳ���
��������������ʾ�ַ������׵�ַ	�˺���������ʾCODE���е��ַ�
��������ֵ����
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
�������ƣ�write_dat()
�������ã���ʾ�ַ�
��������������ʾ��CODE�����ַ�
��������ֵ����
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
�������ƣ�TransferData()
�������ã�дָ��/���ݺ���
������������д�ַ��Լ���־λ
��������ֵ����
***********************************************************************/
void    TransferData(char data1,bit DI)  //�������ݻ�������,��DI=0��,��������,��DI=1,��������.
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
�������ƣ�DisplayGraphic()
�������ã���ʾͼƬ����
�����������洢ͼ��CODE���׵�ַ
��������ֵ����
***********************************************************************/
void DisplayGraphic(unsigned char code *adder)
{

    int i,j;
//*******��ʾ�ϰ�����������
    for (i=0;i<32;i++)             //
    {
        TransferData((0x80 + i),0); //SET  ��ֱ��ַ VERTICAL ADD
        TransferData(0x80,0);       //SET  ˮƽ��ַ HORIZONTAL ADD
        for (j=0;j<16;j++)
        {
            TransferData(*adder,1);
            adder++;
        }
    }
//*******��ʾ�°�����������
    for (i=0;i<32;i++)             //
    {
        TransferData((0x80 + i),0); //SET ��ֱ��ַ VERTICAL ADD
        TransferData(0x88,0);       //SET ˮƽ��ַ HORIZONTAL ADD
        for (j=0;j<16;j++)
        {
            TransferData(*adder,1);
            adder++;
        }
    }
}

/********************************************************************
�������ƣ�delayms()
�������ã���ʱ��������ʱ10 X N ����
����������N
��������ֵ����
***********************************************************************/
void    delayms(unsigned int n)            //��ʱ10��n�������
{
    unsigned int i,j;
    for (i=0;i<n;i++)
        for (j=0;j<2000;j++);
}

/********************************************************************
�������ƣ�delay()
�������ã���ʱ��������ʱ������
������������
��������ֵ����
***********************************************************************/
void    delay(unsigned int m)            //��ʱ����
{
    unsigned int i,j;
    for (i=0;i<m;i++)
        for (j=0;j<10;j++);
}
/******************************��һ����LCD  END***********************************************************************************/

/******************************�ڶ�����DHT11  START*******************************************************************************/

sbit DHT11_DATA  = P2^0;

unsigned char U8FLAG;  //�ñ�־��Ҫ���ڼ�ʱ
unsigned char U8temp;  //��ȡ���ݵ�ÿһλֵ
unsigned char U8comdata; //��ȡ���ݣ�8λ����һ��
unsigned char U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp; //δУ��֮ǰ������
unsigned char U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;  //��У��֮�������

/********************************************************************
�������ƣ�Delay()
�������ã���ʱ��������ʱ������
������������
��������ֵ����
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
�������ƣ�Delay_10us()
�������ã���ʱ��������ʱ10 us
������������
��������ֵ����
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
�������ƣ�COM()
�������ã�dht11��ʱ�����ʪ��ֵ
������������
��������ֵ����
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
        //��ʱ������forѭ��
        if (U8FLAG==1)break;
        //�ж�����λ��0����1

        // ����ߵ�ƽ�߹�Ԥ��0�ߵ�ƽֵ������λΪ 1

        U8comdata<<=1;
        U8comdata|=U8temp;        //0
    }//rof

}

/********************************************************************
�������ƣ�RH()
�������ã���DHT11д��������ݵ�
������������
��������ֵ����
***********************************************************************/
void RH(void)
{
    //��������18ms
    DHT11_DATA=0;
    DHT11_Delay(180);
    DHT11_DATA=1;
    //������������������ ������ʱ20us
    DHT11_Delay_10us();
    DHT11_Delay_10us();
    DHT11_Delay_10us();
    DHT11_Delay_10us();
    //������Ϊ���� �жϴӻ���Ӧ�ź�
    DHT11_DATA=1;
    //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������
    if (!DHT11_DATA)		 //T !
    {
        U8FLAG=2;
        //�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����
        while ((!DHT11_DATA)&&U8FLAG++);
        U8FLAG=2;
        //�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
        while ((DHT11_DATA)&&U8FLAG++);
        //���ݽ���״̬
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
        //����У��

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

/******************************�ڶ�����DHT11  END********************************************************************************************/

/******************************�������� ���� START*******************************************************************************************/
sbit CLOCK = P3^7;
sbit EOC = P3^6;
sbit ST = P3^5;
sbit OE = P3^4;

sbit SELECT_A = P3^2;
sbit SELECT_B = P3^3;

uint ADCReg;
/********************************************************************
�������ƣ�delay()
�������ã�adc0809�õ���ͨ��ʱ
����������n
��������ֵ����
***********************************************************************/
void delay_0809(uint n)
{
    uint i,j;
    for (i=0; i<n; i++)
        for (j=0; j<124; j++)
            _nop_();
}

/********************************************************************
�������ƣ�adc_init()
�������ã�adc0809��ʼ�������ö�ʱ��0
����������n
��������ֵ����
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
�������ƣ�timer0int()
�������ã���ʱ��0�жϣ���������ADC0809�Ĺ���ʱ��
������������
��������ֵ����
***********************************************************************/
void timer0int(void) interrupt 1
{
    CLOCK = ! CLOCK;
}
/******************************�������� ���� END  *******************************************************************************************/

/******************************���Ĳ��� RS232 START******************************************************************************************/
/********************************************************************
�������ƣ�put_char()
�������ã���һ���ַ����ͻ�����
�����������������ַ�
��������ֵ����
***********************************************************************/
void put_char(unsigned char data1)
{
    SBUF = data1;               //�������͵��ַ����뷢�ͻ�����
    while (TI == 0);           //�ȴ��������
    TI = 0;                     //�����жϱ�־��0
}

/********************************************************************
�������ƣ�delay_232()
�������ã�������ʱn����
����������n
��������ֵ����
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
�������ƣ�putstring()
�������ã������ַ�������
�����������ַ����׵�ַ
��������ֵ����
***********************************************************************/
void putstring(unsigned char *dat)
{
    while (*dat != '\0')          //�ж��ַ����Ƿ������
    {
        put_char(*dat);        //���͵����ַ�
        dat++;
        //�ַ���ַ��1��ָ������һ���ַ�
        delay_232(5);
    }
}

/********************************************************************
�������ƣ�rs232_init()
�������ã�����ʼ��
������������
��������ֵ����
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
/******************************���Ĳ��� RS232 END********************************************************************************************/

/********************************************************************
* ���� : Main()
* ���� : ������
* ���� : ��
* ��� : ��
***********************************************************************/
unsigned char code welcome[]=
{
    "     HIT-WSN    "
    "        ��ӭ    "
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

    initinal();   		 //����LCD�ֿ��ʼ������
    delay(100);            //����100uS����ʱ����
    lcd_mesg(welcome);      //��ʾ���ĺ���1
    delayms(240);
    delayms(240);

   
    initinal();
    adc_init();

    while (1)
    {
		initinal();
        setline(LINE_1);
        strcpy(ch,"�¶�");
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
        strcpy(ch,"ʪ��");
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
        strcpy(ch,"����");
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
		strcpy(ch,"һ����̼");
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
        strcpy(ch,"����");
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
