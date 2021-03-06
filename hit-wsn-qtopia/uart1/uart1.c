//--------------------------------------------------------------
// xiaoyang linux uart driver test
// 2011.4.24
//
// Work as console,collect the data from uart2.
//		uart0:Debug
//		uart1:free
//		uart2:data path 
//--------------------------------------------------------------

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>		//文件控制定义
#include <termios.h>	//posix中断控制定义
#include <errno.h>		//错误定义

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "pthread.h"
#include "uart1.h"

struct serial_config Uart1_Cfg;
static int serial_fd;

const unsigned int speed_arr[] = {
	B230400, B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300,
	B38400, B19200, B9600, B4800, B2400, B1200, B300
};

const unsigned int name_arr[] = {
	230400, 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300,
	38400, 19200, 9600, 4800, 2400, 1200, 300
};

//-----------------------------------------------
//	print content of uart2.cfg
//-----------------------------------------------
void print_serialread()
{
	printf("serialread.dev is %s\n",Uart1_Cfg.serial_dev);
	printf("Uart1_Cfg.speed is %d\n",Uart1_Cfg.serial_speed);
	printf("Uart1_Cfg.databits is %d\n",Uart1_Cfg.databits);
	printf("Uart1_Cfg.stopbits is %d\n",Uart1_Cfg.stopbits);
	printf("Uart1_Cfg.parity is %c\n",Uart1_Cfg.parity);
}

//-----------------------------------------------
//	read uart2.cfg
//-----------------------------------------------
void read_uart1_cfg()
{
	FILE *serial_fp;
	char tmp[10];

	//读取配置文件
	serial_fp = fopen("/etc/uart1.cfg","r");
	if(NULL == serial_fp)
	{
		printf("can't open /etc/uart1.cfg\r\n");
	}
	else
	{
		fscanf(serial_fp, "DEV=%s\n", Uart1_Cfg.serial_dev);

		fscanf(serial_fp, "SPEED=%s\n", tmp);
		Uart1_Cfg.serial_speed = atoi(tmp);

		fscanf(serial_fp, "DATABITS=%s\n", tmp);
		Uart1_Cfg.databits = atoi(tmp);

		fscanf(serial_fp, "STOPBITS=%s\n", tmp);
		Uart1_Cfg.stopbits = atoi(tmp);

		fscanf(serial_fp, "PARITY=%s\n", tmp);
		Uart1_Cfg.parity = tmp[0];
	}

	fclose(serial_fp);
	printf("uart cfg read over\r\n");
}

//-----------------------------------------------
//	set boardrate
//-----------------------------------------------
void set_speed(int fd)
{
	unsigned int i;
	int status;
	struct termios Opt;

	if(tcgetattr(fd,&Opt) != 0){
		perror("error: set_speed tcgetattr failed!");
		return ;
	}
	
	for( i = 0; i < (unsigned int)sizeof(speed_arr)/sizeof(int); i++)
	{
		if(Uart1_Cfg.serial_speed == name_arr[i])
		{
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if(status != 0)
			{
				perror("error: set_speed tcsetattr failed!");
				return;
			}
			tcflush(fd, TCIOFLUSH);
		}
	}
}


//-----------------------------------------------
//	set other parity
//-----------------------------------------------
int set_parity(int fd)
{
	struct termios options;

	if(tcgetattr(fd, &options) != 0)
	{
		perror("error: set_parity tcgetattr failed!");
		return(FALSE);
	}

	options.c_cflag |= (CLOCAL|CREAD);
	options.c_cflag &=~CSIZE;
	
	//set data bits lenghth
	switch(Uart1_Cfg.databits)
	{
		case 7:
			options.c_cflag |= CS7;
			break;
		case 8:
			options.c_cflag |= CS8;
			break;
		default:
			options.c_cflag |= CS8;
			fprintf(stderr, "Unsupported data size\n");
			return(FALSE);
	}
		
	switch(Uart1_Cfg.parity)
	{
		case 'n':
		case 'N':
			options.c_cflag &= ~PARENB;
			options.c_iflag &= ~INPCK;
			break;
		case 'o':
		case 'O':
			options.c_cflag |= (PARODD | PARENB);
			options.c_iflag |= INPCK;
			break;
		case 'e':
		case 'E':
			options.c_cflag |= PARENB;
			options.c_cflag &= ~PARODD;
			options.c_iflag |= INPCK;
			break;
		default:
			options.c_cflag &= ~PARENB;
			options.c_iflag &= ~INPCK;
			fprintf(stderr, "Unsupported parity\n");
			return(FALSE);
	}
	
	//set stop bits
	switch(Uart1_Cfg.stopbits)
	{
		case 1:
			options.c_cflag &= ~CSTOPB;
			break;
		case 2:
			options.c_cflag |= CSTOPB;perror("error: set_parity tcgetattr failed!");
			break;
		default:
			options.c_cflag &= ~CSTOPB;
			fprintf(stderr, "Unsupported stop bits\n");
			return(FALSE);
	}
	
	if(Uart1_Cfg.parity != 'n')
		options.c_iflag |= INPCK;
	options.c_cc[VTIME] = 0;	//150,15 seconds
	options.c_cc[VMIN] = 0;
	#if 1
	options.c_iflag |= IGNPAR|ICRNL;
	options.c_oflag |= OPOST; 
	options.c_iflag &= ~(IXON|IXOFF|IXANY);                     
	#endif
	tcflush(fd, TCIFLUSH);
	if(tcsetattr(fd, TCSANOW, &options) != 0)
	{
		perror("error: set_parity tcsetattr failed!");
		return(FALSE);
	}
	return(TRUE);
}

//-----------------------------------------------
//	open device
//-----------------------------------------------
int open_dev(char *dev)
{
	int fd = open(dev, O_RDWR, 0);
	if(-1 == fd)
	{
		perror("Can't Open Serial Port");
		return -1;
	}
	else
		return fd;
}

//--------------------------------------------------
//	uart initialization
//--------------------------------------------------
void uart1_init(void)
{
	char *dev;
	unsigned int i;

	read_uart1_cfg();
	print_serialread();

	dev = Uart1_Cfg.serial_dev;
	serial_fd = open_dev(dev);

	if(serial_fd > 0)
		set_speed(serial_fd);		//设置波特率
	else
	{
		printf("Can't Open Serial Port!\n");
		exit(0);
	}
	
	//恢复串口未阻塞状态
	if (fcntl(serial_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		printf("fcntl failed!\n");
		exit(0);
	}
	
	//检查是否是终端设备
#if 1	//如果屏蔽下面这段代码，在串口输入时不会有回显的情况，调用下面这段代码时会出现回显现象。
	if(isatty(STDIN_FILENO)==0)
	{
		printf("standard input is not a terminal device\n");
	}
	else
		printf("isatty success!\n");
#endif
	//设置串口参数
	if(set_parity(serial_fd) == FALSE)
	{
		printf("Set parity Error\n");
		exit(1);
	}
}

//--------------------------------------------------
//	uart test demo
//--------------------------------------------------
void uart1_test()
{
	//unsigned int i;
	char rx_buffer[512];
	char tx_buffer[] = "this is tquart2_init2440 console\n";
	int nread,nwrite;

	nwrite = write(serial_fd,tx_buffer,sizeof(tx_buffer));
	printf("nwrite len=%d\r\n",nwrite);
	while(1)
	{
		if((nread = read(serial_fd,rx_buffer,512))>0)
		{ 
			rx_buffer[nread] = '\0';
			
			printf("\nrecv len:%d\r\n",nread);
			printf("content:%s",rx_buffer);
			printf("\r\n");
		}
	}

	close(serial_fd);
}
/*
int main(int argc, char **argv)
{
	uart1_init();
	uart1_test();	
	return 0;
}
*/
