/****************************************************************************
**	Data Tranfer Class
**
	** Created:2011.5.13
**      by:  xiaoyang yi

****************************************************************************/

#ifndef DATATRANS_H
#define DATATRANS_H


#include "uart1.h"
#include "config.h"
//#include "config.h"
#include <qthread.h>
#include <qapplication.h>

//try to get data, failed if try more than TRY_COUNT times
#define TRY_COUNT	1

//status
#define STATUS_FAILED	-1
#define STATUS_OK		1

//buffer size defination
#define RX_BUF_SIZE		512
#define TX_BUF_SIZE		512
			
//data end flag
#define TRANS_END			"]]\n\0"
#define CMD_ALL_ADDR		"[[RA\n\0"
#define CMD_THIS_DATA		"[[TD\n\0"

class QThread;

class DataTrans
{
	Q_OBJECT
	public:
		DataTrans();
		~DataTrans();
		int data_send();
		int data_rcv();
		int get_status();
		int uart1_init();
		int get_data(int node_no,int place);
		int get_all_addr();		
		int read_uart();
		//buffer for data trans
		char rx_buffer[RX_BUF_SIZE];
		char tx_buffer[TX_BUF_SIZE];
		//unsigned int which_node;
	protected:
		int status;
		int serial_fd;
		struct serial_config Uart1_Cfg;
		void print_serialread();
		void read_uart1_cfg();
		void set_speed(int fd);
		int set_parity(int fd);
		int open_dev(unsigned char *dev);
		
};


#endif
