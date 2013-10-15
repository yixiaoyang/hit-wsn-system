#ifndef UART1_H
#define UART1_H

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

#ifndef FALSE 
#define FALSE		0
#endif
#ifndef TRUE
#define TRUE		1
#endif

#define WORDLEN		32

#define TRANS_OK	"@OK"
struct serial_config
{
	unsigned char serial_dev[WORDLEN];
	unsigned int serial_speed;
	unsigned char databits;
	unsigned char stopbits;
	unsigned char parity;
};


#endif
