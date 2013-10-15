/****************************************************************************
** xiaoyang yi
** global configure file
** 2010.2.8 
****************************************************************************/
#ifndef CONFIG_H
#define CONFIG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qvariant.h>
#include <qpainter.h>
#include <qbrush.h>
#include <qstatusbar.h>
#include <qevent.h>
#include <qmessagebox.h> 
#include <qtimer.h> 
#include <qstring.h>
#include <qpoint.h>
#include <qscrollbar.h>
#include <qstyle.h>
#include <qregion.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qpixmap.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qimage.h>
#include <qapplication.h>
#include <qdialog.h>
#include <qthread.h>
#include <qcursor.h>
#include <qapplication.h>
#include <qtopia/qpeapplication.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
//space of the grid
#define DEF_SPACE	20
#define MAX_SPACE	80
#define MIN_SPACE	5

//size of LCD
#define X		800
#define Y 		600

//max nodes
#define MAX_MACHINE	128

//default fresh time 
#define DEF_FRESH	512

//debug switch
#define DEBUG		1
#define DEBUG_PRINT	\
	if(DEBUG)\
		printf

#define DEBUG_QWARNING	\
	if(DEBUG)\
		qWarning

//class defination
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QLabel;
class QFrame;
class QDropEvent;
class QDragEnterEvent;

#define MAX_HISTORY_DATA	45
//global struct
typedef struct wsn_data{
	float temp;
	float humidity;
	float smoke;
	float co;
	float ch4;
}WsnData; 

#endif 
