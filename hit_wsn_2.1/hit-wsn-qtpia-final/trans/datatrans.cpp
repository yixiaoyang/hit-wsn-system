/****************************************************************************
**	Data Tranfer Class
**
	** Created:2011.5.13
**      by:  xiaoyang yi

****************************************************************************/

#include "datatrans.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

const unsigned int speed_arr[] = {
	B230400, B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300,
	B38400, B19200, B9600, B4800, B2400, B1200, B300
};

const unsigned int name_arr[] = {
	230400, 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300,
	38400, 19200, 9600, 4800, 2400, 1200, 300
};

//from main.cpp
extern int dot_pos[MAX_MACHINE][2];//[0]:x,[1]:y

//data storage for WSN
extern WsnData GWsnData[MAX_HISTORY_DATA];

//date for end_device
extern char all_addr[MAX_MACHINE][17];

//machine count
extern int dot_count;

/*
 * reauest all adress from WSN
 * reauest format:	"[[RA"
 * reaonse format:	"#1234#"
 * end signal:		"]]"
 */
int DataTrans::get_all_addr()
{
	int nread;
	int nwrite;
	
	if(uart1_init() < 0){
		printf("errno=%d,%s\r\n",errno,strerror(errno));
		return -1;
	}
	strcpy(tx_buffer,CMD_ALL_ADDR);//command:request address
	nwrite = write(serial_fd,tx_buffer,strlen(tx_buffer));
	DEBUG_PRINT("send data:%s",tx_buffer);	
	//for(int i = 0 ; i < 4; i++)
	while(1)
	{
		//usleep(1000);	
		if( (nread = read(serial_fd,rx_buffer,RX_BUF_SIZE)) > 0){
		DEBUG_PRINT("get all addr():len:%d,content:%s\r\n",nread,rx_buffer);
		DEBUG_PRINT("\r\n");
		/*	
		if(nread >= 6)//adress message
		{
			if( (rx_buffer[0] == '#') && (rx_buffer[5] == '#') ){
				for(char j = 0; j < 4; j++){
					all_addr[dot_count][j] = rx_buffer[j+1];
				}
				dot_count++;
				DEBUG_PRINT("dot_count=%d\r\n",dot_count);
			}
		}
		else if(nread >= 2)
		{ 	
			//"]]" as end signal
			if( rx_buffer[0]== ']' && rx_buffer[1]==']') {
				DEBUG_PRINT("dot_count=%d,exit!\r\n",dot_count);	
				break;
			}
		}else if(nread < 0){
			printf("errno=%d,%s",errno,strerror(errno));
			DEBUG_PRINT("error:unknown\r\n");
			close(serial_fd);
			return -1;
		}else{
		}
	*/
		}

	}

	close(serial_fd);
	return 1;
}
/*
 * reauest the data from WSN
 * reauest format:	"[[RD#1234#"
 * reaonse format:	"#tmp:humidity:smoke:co:ch4#"
 * end signal:		"]]"
 */
int DataTrans::get_data(int node_no,int place)
{
	int nread;
	int nwrite;
	char cmd[23] = "[[RD";
	
	if(node_no > dot_count){
		return -1;
	}
	
	if(place > MAX_HISTORY_DATA-1 ){
		return -1;
	}
	
	cmd[4] = '#';
	for(int i = 0 ; i < 16; i++){
		cmd[4+i] = all_addr[node_no][i];
	}
	cmd[21] = '#';
	cmd[22] = '\0';
	
	if(uart1_init() < 0){
		return -1;
	}
	
	DEBUG_PRINT("request data:%s",cmd);
	strcpy(tx_buffer,cmd);//command:request address
	nwrite = write(serial_fd,tx_buffer,strlen(tx_buffer));
	
	//for(int i = 0 ; i < 4; i++)
	while(1)
	{
		//usleep(1000);	
		//nread = read(serial_fd,rx_buffer,RX_BUF_SIZE);
		if( (nread = read(serial_fd,rx_buffer,RX_BUF_SIZE)) > 0){
		DEBUG_PRINT("len:,content:%s",nread,rx_buffer);
		DEBUG_PRINT("\r\n");
			
		if(nread >= 8)//data message
		{
			char temp[8];
			unsigned char m = 0;
			unsigned char n = 0;
			
			rx_buffer[nread-1] = '\0';	
			for(int i = 0; i < nread; i++ ){
				rx_buffer[i] = rx_buffer[i+1];	
			}	
		
			//get temperature data
			for( m = 0, n = 0 ; (m < nread) && (rx_buffer[m] != ':'); m++, n++){
				temp[n] = rx_buffer[m];
			}
			temp[n+1] = '\0';
			GWsnData[place].temp = atof(temp);
			m++;
			DEBUG_PRINT("temp=%.2f,m=%d\r\n",GWsnData[place].temp,m);
			
			//get humidity data
			for( n = 0 ; (m < nread) && (rx_buffer[m] != ':'); m++, n++){
				temp[n] = rx_buffer[m];
			}
			temp[n+1] = '\0';
			GWsnData[place].humidity = atof(temp);
			m++;
			DEBUG_PRINT("humidity=%.2f,m=%d\r\n",GWsnData[place].humidity,m);
			
			//get smoke data
			for( n = 0 ; (m < nread) && (rx_buffer[m] != ':'); m++, n++){
				temp[n] = rx_buffer[m];
			}
			temp[n+1] = '\0';
			GWsnData[place].smoke = atof(temp);
			m++;
			DEBUG_PRINT("smoke=%.2f,m=%d\r\n",GWsnData[place].smoke,m);
			
			//get co data
			for(  n = 0 ; (m < nread) && (rx_buffer[m] != ':'); m++, n++){
				temp[n] = rx_buffer[m];
			}
			temp[n+1] = '\0';
			GWsnData[place].co = atof(temp);
			m++;
			DEBUG_PRINT("co=%.2f,m=%d\r\n",GWsnData[place].co,m);
			
			//get ch4 data
			memset(temp,8,0);
			for( n = 0 ; (m < nread) && (rx_buffer[m] != ':') && (rx_buffer[m] != '\0' ); m++, n++){
				temp[n] = rx_buffer[m];
			}
			temp[n+1] = '\0';
			GWsnData[place].ch4 = atof(temp);
			m++;
			DEBUG_PRINT("ch4=%.2f,m=%d\r\n",GWsnData[place].ch4,m);
		}
		else if(nread >= 2)
		{ 	
			//"]]" as end signal
			if( rx_buffer[0]== ']' && rx_buffer[1]==']') {
				DEBUG_PRINT("dot_count=%d,exit!\r\n",dot_count);	
				break;
			}
		}else if(nread < 0){
			DEBUG_PRINT("error:unknown\r\n");
			close(serial_fd);
			return -1;
		}else{
		}
		}
	}

	close(serial_fd);
	return 1;
}

/*
 * read into buffer
 *
 */
int DataTrans::read_uart()
{
	int nread = 0;
	int total = 0;
	while(nread = read(serial_fd,rx_buffer+total,RX_BUF_SIZE)) > 0)
	{
		if(rx_buffer[total+nread-1] == '\n' | 	rx_buffer[total+nread-1] == '\0')
		{
			total += nread;
			rx_buffer[total+nread-1] = '\0';
			break;
		}
		total += nread;	
	}
	return total; 
}
/*
 * reauest all adress from WSN
 * reauest format:	"[[RA"
 * reaonse format:	"#1234567890123456#"
 * end signal:		"]]"
 */
int DataTrans::data_send()
{
	int nread;
	int nwrite;
	
	if(uart1_init() < 0){
		return -1;
	}
	
	while(1)
	{
		printf("print msg:");
		scanf("%s",tx_buffer);
		nwrite = write(serial_fd,tx_buffer,sizeof(tx_buffer));
		DEBUG_PRINT("nwrite len=%d\r\n",nwrite);

		while(1)
		{
			if((nread = read(serial_fd,rx_buffer,RX_BUF_SIZE))>0)
			{
				if(nread > 2){ 
					rx_buffer[nread] = '\0';
					DEBUG_PRINT("\nrecv len:%d,content:%s\r\n",nread,rx_buffer);
					//break;
				}
			}
		}
	}
	
	close(serial_fd);
	return 1;
}

/*
	strcpy(tx_buffer,"i am qtopia!");
	nwrite = write(serial_fd,tx_buffer,sizeof(tx_buffer));
	DEBUG_PRINT("nwrite len=%d\r\n",nwrite);
	//for(int i = 0 ; i < 4; i++)
	while(1)
	{
		//usleep(1000);	
		if((nread = read(serial_fd,rx_buffer,RX_BUF_SIZE))>0)
		{ 
			rx_buffer[nread] = '\0';
			//echo	
			nwrite = write(serial_fd,rx_buffer,sizeof(rx_buffer));
	
			DEBUG_PRINT("\nrecv len:%d\r\n",nread);
			DEBUG_PRINT("content:%s",rx_buffer);
			DEBUG_PRINT("\r\n");
			if( rx_buffer[0]== '[' && rx_buffer[1]=='[') {
				printf("exit!\r\n");	
				break;
			}
		}else{
			//	nwrite = write(serial_fd,"no data!\r\n",sizeof("no data!\r\n"));
		}
	}

	close(serial_fd);
	return 1;
}
*/
int DataTrans::data_rcv()
{
}

//
DataTrans::DataTrans()
{
	memset(rx_buffer,'\0',RX_BUF_SIZE);
	memset(tx_buffer,'\0',TX_BUF_SIZE);
}

//
DataTrans::~DataTrans()
{
}


//-----------------------------------------------
//	print content of uart2.cfg
//-----------------------------------------------
void DataTrans::print_serialread()
{
	DEBUG_PRINT("serialread.dev is %s\n",Uart1_Cfg.serial_dev);
	DEBUG_PRINT("Uart1_Cfg.speed is %d\n",Uart1_Cfg.serial_speed);
	DEBUG_PRINT("Uart1_Cfg.databits is %d\n",Uart1_Cfg.databits);
	DEBUG_PRINT("Uart1_Cfg.stopbits is %d\n",Uart1_Cfg.stopbits);
	DEBUG_PRINT("Uart1_Cfg.parity is %c\n",Uart1_Cfg.parity);
}

//-----------------------------------------------
//	read uart2.cfg
//-----------------------------------------------
void DataTrans::read_uart1_cfg()
{
	FILE *serial_fp;
	char tmp[10];

	//读取配置文件
	serial_fp = fopen("/etc/uart1.cfg","r");
	if(NULL == serial_fp)
	{
		DEBUG_PRINT("can't open /etc/uart1.cfg\r\n");
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
	DEBUG_PRINT("uart cfg read over\r\n");
}

//-----------------------------------------------
//	set boardrate
//-----------------------------------------------
void DataTrans::set_speed(int fd)
{
	unsigned int i;
	int status;
	struct termios Opt;

	if(tcgetattr(fd,&Opt) != 0){
		DEBUG_PRINT("error: set_speed tcgetattr failed!");
		return ;
	}
	
	for( i = 0; i < sizeof(speed_arr)/sizeof(int); i++)
	{
		if(Uart1_Cfg.serial_speed == name_arr[i])
		{
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if(status != 0)
			{
				DEBUG_PRINT("error: set_speed tcsetattr failed!");
				return;
			}
			tcflush(fd, TCIOFLUSH);
		}
	}
}


//-----------------------------------------------
//	set other parity
//-----------------------------------------------
int DataTrans::set_parity(int fd)
{
	struct termios options;

	if(tcgetattr(fd, &options) != 0)
	{
		DEBUG_PRINT("error: set_parity tcgetattr failed!");
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
			DEBUG_PRINT("Unsupported data size\n");
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
			DEBUG_PRINT("Unsupported parity\n");
			return(FALSE);
	}
	
	//set stop bits
	switch(Uart1_Cfg.stopbits)
	{
		case 1:
			options.c_cflag &= ~CSTOPB;
			break;
		case 2:
			options.c_cflag |= CSTOPB;DEBUG_PRINT("error: set_parity tcgetattr failed!");
			break;
		default:
			options.c_cflag &= ~CSTOPB;
			DEBUG_PRINT( "Unsupported stop bits\n");
			return(FALSE);
	}
	
	if(Uart1_Cfg.parity != 'n')
		options.c_iflag |= INPCK;
	options.c_cc[VTIME] = 100;	//10 seconds delay if not recieved data
	options.c_cc[VMIN] = 64;
#if 1
	options.c_iflag |= IGNPAR|ICRNL;
	options.c_oflag |= OPOST; 
	options.c_iflag &= ~(IXON|IXOFF|IXANY);                     
#endif

	//ignore \n and \0
	options.c_oflag &= ~(INLCR | IGNCR | ICRNL);
	options.c_oflag &= ~(ONLCR | OCRNL);
	//send directly
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	tcflush(fd, TCIFLUSH);
	if(tcsetattr(fd, TCSANOW, &options) != 0)
	{
		DEBUG_PRINT("error: set_parity tcsetattr failed!");
		return(FALSE);
	}
	return(TRUE);
}

//-----------------------------------------------
//	open device
//-----------------------------------------------
int DataTrans::open_dev(unsigned char *dev)
{
	int fd = open((const char*)dev, O_RDWR, 0);
	if(-1 == fd)
	{
		DEBUG_PRINT("Can't Open Serial Port");
		return -1;
	}
	else
		return fd;
}

//--------------------------------------------------
//	uart initialization
//--------------------------------------------------
int DataTrans::uart1_init()
{
	unsigned char *dev;

	read_uart1_cfg();
	print_serialread();

	dev = Uart1_Cfg.serial_dev;
	serial_fd = open_dev(dev);

	if(serial_fd > 0)
		set_speed(serial_fd);		
	else
	{
		DEBUG_PRINT("Can't Open Serial Port!\n");
		return -1;
	}
	
	if (fcntl(serial_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		DEBUG_PRINT("fcntl failed!\n");
		return -1;
	}
	
	#if 1
	if(isatty(STDIN_FILENO)==0)
	{
		DEBUG_PRINT("standard input is not a terminal device\n");
	}
		else
			DEBUG_PRINT("isatty success!\n");
	#endif
	if(set_parity(serial_fd) == FALSE)
	{
		DEBUG_PRINT("Set parity Error\n");
		return 1;
	}
	return 1;
}
