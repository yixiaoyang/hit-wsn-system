/****************************************************************************
** Form interface generated from reading ui file 'first.ui'
**
** Created: Fri May 13 16:08:25 2011
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************
** xiaoyang yi@2011.5.13
** NerwUI 
****************************************************************************/
#ifndef FIRST_H
#define FIRST_H

#include "config.h"
#include "SettingDlg.h"

class first : public QWidget
{
	Q_OBJECT

	public:
		first( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
		~first();

		QPushButton* user_b;
		QPushButton* button_setting;
		QPushButton* button_console;
		QPushButton* ButtonMoreFuntion_2;
		QPushButton* ButtonMoreFuntion;
		QPushButton* Button_MoreSetting_2;
		QPushButton* close;
		QPushButton* Button_MoreSetting;
		QLabel* text_title;

	public slots:
		virtual void open_console();
		virtual void user_button();

	protected:

	protected slots:
		virtual void open_setting();
		virtual void languageChange();

	private:
		QPixmap image0;
		QPixmap image1;
		QPixmap image2;
		QPixmap image3;
		QPixmap image4;
		QPixmap image5;
		QPixmap image6;

};

#endif // FIRST_H
