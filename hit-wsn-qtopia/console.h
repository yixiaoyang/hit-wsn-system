/****************************************************************************
** Form interface generated from reading ui file 'console.ui'
**
** Created: Wed Feb 2 14:32:45 2011
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef CONSOLE_H
#define CONSOLE_H

#include "config.h"
#include "datatrans.h"

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.12
//	global params
//--------------------------------------------------------------
#define		ALL_SCALING		11
#define		SMALL10			0.1
#define 	SMALL8 			0.125
#define 	SMALL5 			0.2
#define 	SMALL4	 		0.25
#define 	SMALL2 			0.5
#define 	NORMAL 			1
#define 	LARGER2 		1.5
#define 	LARGER4 		2
#define 	LARGER5 		3
#define 	LARGER8 		4
#define 	LARGER10 		5

//data trans status
#define 	TRANS_IDEL		0
#define 	TRANS_BUSY		1

//drag  status
#define DRAG_L_IDEL				0
#define DRAG_L_PRESSED			1
//#define DRAG_L_RELEASED			2

class Console : public QDialog
{ 
    Q_OBJECT

public:
    Console( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~Console();

    QFrame* Frame6;
    QFrame* Frame8;
    QPushButton* PushButton6;
    QPushButton* PushButton10;
    QPushButton* PushButton9;
    QPushButton* PushButton3;
    QLabel* TextLabel2;

public slots:
    virtual void down();
    virtual void left();
    virtual void refresh();
    virtual void right();
    virtual void setting();
    virtual void up();
    virtual void zoom_in();
    virtual void zoom_out();
	virtual void flush_buffer();
	virtual void flush_data();

protected slots:
		virtual void languageChange();
		
protected:
    QGridLayout* ConsoleLayout;
	
	//timer .etc
	QTimer *timer;
	QTimer *data_timer;
	bool if_paint_gridding;
	float scaling;
	int scaling_index ;
	int data_trans_status;
	//for map draging
	QPoint last_qp;
	unsigned char drag_status;
	int x_offset;
	int y_offset;
	
	//nomarl funtions
	int open_data_trans();
	int check_trans_status();
	
	//event handle
	bool event( QEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void dragEnterEvent(QDragEnterEvent*);
	void dropEvent(QDropEvent*);
	void resizeEvent(QResizeEvent *);

public:
	void paintEvent( QPaintEvent *);
	void paint_gridding(QPainter *p);
	int gridding_space;
	
private:
	void drawPrimitives( QPainter *p);
};

#endif // CONSOLE_H
