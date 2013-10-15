/****************************************************************************
** Form interface generated from reading ui file 'Monitor.ui'
**
** Created: Fri May 27 11:38:15 2011
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************
** xiaoyang
**		add @2011.6.7
**
****************************************************************************/
#ifndef MONITORDLG_H
#define MONITORDLG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QComboBox;
class QPushButton;

class MonitorDlg : public QDialog
{ 
    Q_OBJECT

public:
    MonitorDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~MonitorDlg();

    QComboBox* ComboBox1;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
	
public slots:
	virtual void textChanged();
    virtual void ok_pressed();
	virtual void flush_page_slot();
	virtual void flush_data_slot();
protected:
    QGridLayout* MonitorDlgLayout;
    QVBoxLayout* Layout9;
    QHBoxLayout* Layout8;
	
	
	//timer .etc
	QTimer *timer;
	QTimer *data_timer;
	
	void paintEvent( QPaintEvent *);
	void paint_gridding(QPainter *p);
	void drawPrimitives(QPainter *p);
	int gridding_space;
	
	//offset of paint
	int xval_offset;
	int yval_offset;
	bool if_paint_gridding;
	float scaling;
	
	QPoint zero_point;
	QString current_text;
	int current_item;
};

#endif // MONITORDLG_H
