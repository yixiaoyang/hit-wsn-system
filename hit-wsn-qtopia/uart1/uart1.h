#ifndef UART1_H
#define UART1_H

#define FALSE		0
#define TRUE		1

#define WORDLEN		32

struct serial_config
{
	unsigned char serial_dev[WORDLEN];
	unsigned int serial_speed;
	unsigned char databits;
	unsigned char stopbits;
	unsigned char parity;
};

#endif