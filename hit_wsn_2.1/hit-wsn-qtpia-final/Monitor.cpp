/****************************************************************************
** Form implementation generated from reading ui file 'Monitor.ui'
**
** Created: Fri May 27 12:52:06 2011
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************
** xiaoyang yi@2011.5.27
**		add monitor
***************************************************************************/
#include "Monitor.h"
#include "config.h"
#include "datatrans.h"

#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

//extern global params
extern int fresh_time;
//from main.cpp
extern int dot_pos[MAX_MACHINE][2];//[0]:x,[1]:y
//data storage for WSN
extern WsnData GWsnData[MAX_HISTORY_DATA];
//date for end_device
extern char all_addr[MAX_MACHINE][17];
//machine count
extern int dot_count;
extern int current_data_index;
//current node record
extern int current_node;

#define COMBOX_NUM	6
const QString combox_item[COMBOX_NUM] = {
	"ALL",
	"TEMP",
	"HUMIDITY",
	"SMOKE",
	"CO",
	"CH4"	
};

typedef struct _element_value{
	float x_element_value;//value of a width
	float y_element_value;//value of a width	
	int x_base;
	int y_base;
}ElementVal;

ElementVal e_val[COMBOX_NUM] ={
	{0,0,0,0},
	{0.1,0.5,20,180},//for temperature
	{0.1,1,20,180},//for HUMIDITY
	{0.1,0.05,20,180},//for smoke
	{0.1,0.05,20,180},//for CO
	{0.1,0.05,20,180} //for CH4
};

/* 
 *  Constructs a MonitorDlg which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
MonitorDlg::MonitorDlg( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "MonitorDlg" );
    resize( 320, 240 ); 
    setMinimumSize( QSize( 320, 240 ) );
    setMaximumSize( QSize( 470, 240 ) );
	
    setCaption( tr( "Monitor" ) );
    setSizeGripEnabled( TRUE );
    MonitorDlgLayout = new QGridLayout( this ); 
    MonitorDlgLayout->setSpacing( 6 );
    MonitorDlgLayout->setMargin( 11 );

    Layout9 = new QVBoxLayout; 
    Layout9->setSpacing( 0 );
    Layout9->setMargin( 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout9->addItem( spacer );

    Layout8 = new QHBoxLayout; 
    Layout8->setSpacing( 6 );
    Layout8->setMargin( 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout8->addItem( spacer_2 );

    ComboBox1 = new QComboBox( FALSE, this, "ComboBox1" );
    ComboBox1->setMinimumSize( QSize( 0, 30 ) );
    ComboBox1->setMaximumSize( QSize( 32767, 30 ) );
    Layout8->addWidget( ComboBox1 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setMinimumSize( QSize( 50, 30 ) );
    buttonOk->setMaximumSize( QSize( 50, 30 ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout8->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setMinimumSize( QSize( 50, 30 ) );
    buttonCancel->setMaximumSize( QSize( 55, 30 ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAutoDefault( TRUE );
    Layout8->addWidget( buttonCancel );
    Layout9->addLayout( Layout8 );

    MonitorDlgLayout->addLayout( Layout9, 0, 0 );

	//combox
	for(int i = 0; i < COMBOX_NUM; i++){
		ComboBox1->insertItem(combox_item[i]);
	}
	
    // signals and slots connections
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( ok_pressed() ) );
    connect( ComboBox1, SIGNAL( activated(int) ), this, SLOT( textChanged() ) );
	
	//space between grids
	gridding_space = 10;
	
	//new a timer
	timer = new QTimer( this, "timer" ); 
	connect( timer, SIGNAL( timeout() ), this, SLOT( flush_page_slot() ) ); 
	timer->start(fresh_time);//about 20ms 

	data_timer = new QTimer( this, "data_timer" );
	connect( data_timer, SIGNAL( timeout() ), this, SLOT( flush_data_slot() ) ); 
	data_timer->start(3000);//about 5s  
	
	//set tooltips enable,you must enable mouse trace
	setMouseTracking(true);
	
	//offset value
	yval_offset = -4;
	xval_offset = 0;
	if_paint_gridding = true;
	scaling =1;
	current_item = 0;
	
	current_data_index = 0;
	
	zero_point.setX(20);
	zero_point.setY(180);
}

/*  
 *  Destroys the object and frees any allocated resources
 */
MonitorDlg::~MonitorDlg()
{
    // no need to delete child widgets, Qt does it all for us
	delete timer;
}

void MonitorDlg::textChanged()
{
	current_text = ComboBox1->currentText();
	current_item = ComboBox1->currentItem(); 
	//qWarning( "MonitorDlg::textChanged(): %d",current_item );
	repaint(TRUE);
}

void MonitorDlg::ok_pressed()
{
    //trigger the repaint event 
	repaint(TRUE);
}

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.12
//	Input:	void
//	Output:	void
//	Note:	when timer timeout,the slot is called
//--------------------------------------------------------------
void MonitorDlg::flush_page_slot()
{
	//trigger the repaint event 
	if((current_item < 0) |( current_item > COMBOX_NUM))
		current_item = 0;
	repaint(TRUE);
}

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.12
//	Input:	QPainter of the Dialog object
//	Output:	void
//	Note:	paint the Rects(nodes)
//--------------------------------------------------------------
void MonitorDlg::drawPrimitives(QPainter *p)
{
	QBrush b1( Qt::white );
	//int rect_width = 12;
	int x_pos;
	int y_pos;
	int next_x_pos;
	int next_y_pos;
	
	p->setPen( Qt::blue );
	p->setBrush( b1 );
	
	//text
	if(current_item == 0){
		char msg[64];
		const int  msg_len = 64;
		QString qs;
		
		memset(msg,msg_len,'\0');
		sprintf(msg,"IEEE:%s\r\n",all_addr[current_node]);
		qs = tr(msg);
		p->drawText(30,20,qs);
		
		memset(msg,msg_len,'\0');
		sprintf(msg,"temperature:%f `C",GWsnData[current_data_index].temp);
		qs = tr(msg);
		p->drawText(30,45,qs);
		
		memset(msg,msg_len,'\0');
		sprintf(msg,"humidity:%f",GWsnData[current_data_index].humidity);
		qs = tr(msg);
		p->drawText(30,70,qs);
		
		memset(msg,msg_len,'\0');
		sprintf(msg,"smoke%f",GWsnData[current_data_index].smoke);
		qs = tr(msg);
		p->drawText(30,95,qs);
		
		memset(msg,msg_len,'\0');
		sprintf(msg,"CO:%f",GWsnData[current_data_index].co);
		qs = tr(msg);
		p->drawText(30,120,qs);
		
		memset(msg,msg_len,'\0');
		sprintf(msg,"CH4:%f",GWsnData[current_data_index].ch4);
		qs = tr(msg);
		p->drawText(30,145,qs);
	}
	//temp
	else if(current_item == 1){
		yval_offset = 0;
		for(int i = 0; i < MAX_HISTORY_DATA-1; i++)
		{
			if(i < current_data_index){
				p->setPen( Qt::green);
			}
			if(i == current_data_index){
				p->setPen( Qt::red);
			}
			else{
				p->setPen(Qt::blue);
			}
			x_pos = i*gridding_space + zero_point.x();
			y_pos = zero_point.y() - ((int)(GWsnData[i].temp/e_val[current_item].y_element_value)
					-(int)(yval_offset/e_val[current_item].y_element_value));
		
			next_x_pos = (i+1)*gridding_space + zero_point.x();
			next_y_pos = zero_point.y() - ((int)(GWsnData[i+1].temp/e_val[current_item].y_element_value)
					-(int)(yval_offset/e_val[current_item].y_element_value));
			
			p->drawLine(x_pos,y_pos,next_x_pos,next_y_pos);
		
		}
	}
	//HUMIDITY
	else if(current_item == 2){
		yval_offset = 0;
		for(int i = 0; i < MAX_HISTORY_DATA-1; i++)
		{
			if(i < current_data_index){
				p->setPen( Qt::green);
			}
			if(i == current_data_index){
				p->setPen( Qt::red);
			}
			else{
				p->setPen(Qt::blue);
			}
			x_pos = i*gridding_space + zero_point.x();
			y_pos = zero_point.y() - ((int)(GWsnData[i].humidity/e_val[current_item].y_element_value)
					-(int)(yval_offset/e_val[current_item].y_element_value));
		
			next_x_pos = (i+1)*gridding_space + zero_point.x();
			next_y_pos = zero_point.y() - ((int)(GWsnData[i+1].humidity/e_val[current_item].y_element_value)
					-(int)(yval_offset/e_val[current_item].y_element_value));
			
			p->drawLine(x_pos,y_pos,next_x_pos,next_y_pos);
		}
	}
	//smoke,0~5
	else if(current_item == 3){
		yval_offset = 0;
		for(int i = 0; i < MAX_HISTORY_DATA-1; i++)
		{
			if(i < current_data_index){
				p->setPen( Qt::green);
			}
			if(i == current_data_index){
				p->setPen( Qt::red);
			}
			else{
				p->setPen(Qt::blue);
			}
			x_pos = i*gridding_space + zero_point.x();
			y_pos = zero_point.y() - ((int)(GWsnData[i].smoke/e_val[current_item].y_element_value)
					-(int)(yval_offset/e_val[current_item].y_element_value));
		
			next_x_pos = (i+1)*gridding_space + zero_point.x();
			next_y_pos = zero_point.y() - ((int)(GWsnData[i+1].smoke/e_val[current_item].y_element_value)
					-(int)(yval_offset/e_val[current_item].y_element_value));
			
			p->drawLine(x_pos,y_pos,next_x_pos,next_y_pos);
		
		}
	}
	//CO,0~5
	else if(current_item == 4){
		yval_offset = 0;
		for(int i = 0; i < MAX_HISTORY_DATA-1; i++)
		{
			if(i < current_data_index){
				p->setPen( Qt::green);
			}
			if(i == current_data_index){
				p->setPen( Qt::red);
			}
			else{
				p->setPen(Qt::blue);
			}
			x_pos = i*gridding_space + zero_point.x();
			y_pos = zero_point.y() - ((int)(GWsnData[i].co/e_val[current_item].y_element_value)
					-(int)(yval_offset/e_val[current_item].y_element_value));
		
			next_x_pos = (i+1)*gridding_space + zero_point.x();
			next_y_pos = zero_point.y() - ((int)(GWsnData[i+1].co/e_val[current_item].y_element_value)
					-(int)(yval_offset/e_val[current_item].y_element_value));
			
			p->drawLine(x_pos,y_pos,next_x_pos,next_y_pos);
		
		}
	}
	//CH4,0~5
	else if(current_item == 5){
			yval_offset = 0;
		for(int i = 0; i < MAX_HISTORY_DATA-1; i++)
		{
			if(i < current_data_index){
				p->setPen( Qt::green);
			}
			if(i == current_data_index){
				p->setPen( Qt::red);
			}
			else{
				p->setPen(Qt::blue);
			}
			x_pos = i*gridding_space + zero_point.x();
			y_pos = zero_point.y() - ((int)(GWsnData[i].ch4/e_val[current_item].y_element_value)
					-(int)(yval_offset/e_val[current_item].y_element_value));
		
			next_x_pos = (i+1)*gridding_space + zero_point.x();
			next_y_pos = zero_point.y() - ((int)(GWsnData[i+1].ch4/e_val[current_item].y_element_value)
					-(int)(yval_offset/e_val[current_item].y_element_value));
			
			p->drawLine(x_pos,y_pos,next_x_pos,next_y_pos);
		
		}
	}
	else{
	}
}

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.27
//	Input:	QPainter of the Dialog object
//	Output:	void
//	Note:	paint the grids
//--------------------------------------------------------------
void MonitorDlg::paint_gridding(QPainter *p)
{
	int i;
	int cur_x_off = 0;
	int cur_y_off = 0;
	
	if(current_item != 0){
		QBrush b1( Qt::gray );
		p->setPen( Qt::red );
		p->setBrush(b1);
		
		p->drawLine( zero_point.x(), zero_point.y(), X, zero_point.y());
		p->drawLine( zero_point.x(),0 ,zero_point.x(),zero_point.y());
		
		//draw "--" line
		for(i = 0; ((i*gridding_space) < zero_point.y()); i++)
		{
			cur_y_off = gridding_space*i;
			p->drawLine( zero_point.x(), zero_point.y()-cur_y_off,
						zero_point.x()+4,zero_point.y()-cur_y_off);
			if((i != 0) && (i%2==0) )
				p->drawText(0, zero_point.y()-cur_y_off+5, 
							QString::number(yval_offset + (int)((cur_y_off) * e_val[current_item].y_element_value))); 
		}
		
		//draw "1"line
		for(i = 0; i*gridding_space+zero_point.x() < X; i++)
		{
			cur_x_off = gridding_space*i;
			p->drawLine( cur_x_off + zero_point.x(), zero_point.y(),
						cur_x_off + zero_point.x(), zero_point.y()-4 );
			if((i != 0) && (i%4==0) )
				p->drawText(cur_x_off + zero_point.x()-5, zero_point.y()+10, 
							QString::number(xval_offset + (int)((cur_x_off) * e_val[current_item].x_element_value))); 
		}
		
		//draw text
		p->drawText(zero_point.x()-10,zero_point.y()+10,QString::number(0));
	}else{
		
	}
}

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.27
//	Input:	repaint
//	Output:	void
//	Note:	repaint event handle
//--------------------------------------------------------------
void MonitorDlg::paintEvent( QPaintEvent *e)
{
	e = e;
	QPainter paint(this);
	scaling = 1;

	if(if_paint_gridding)
		paint_gridding(&paint);
	drawPrimitives(&paint);
			
	paint.end();
}

void MonitorDlg::flush_data_slot()
{	
	if(current_node < 0)
		return ;

	DataTrans data_trans;
	/*
	printf("flush_data dot_count=%d\r\n",dot_count);
	if(data_trans.get_all_addr() < 0){
		DEBUG_QWARNING("error:get all adress in flush_data");
		return ;
	}*/
	
	if(data_trans.get_data(current_node,current_data_index) < 0){
		DEBUG_PRINT("node:%d,ieee:%s,get data error!\r\n",current_node,all_addr[current_node]);
	}
	
	//current_data_index++;
	//current_data_index = current_data_index % MAX_HISTORY_DATA;

}
