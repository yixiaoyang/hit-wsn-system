/****************************************************************************
** Form interface generated from reading ui file 'first.ui'
**
** Created: 四  3月 31 00:26:20 2011
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef FIRST_H
#define FIRST_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QPushButton;

class first : public QWidget
{
    Q_OBJECT

public:
    first( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~first();

    QPushButton* button_console;
    QPushButton* button_setting;
    QPushButton* Button_MoreSetting;
    QPushButton* close;
    QPushButton* user_b;
    QLabel* text_title;
    QPushButton* ButtonMoreFuntion_2;
    QPushButton* ButtonMoreFuntion;
    QPushButton* Button_MoreSetting_2;

public slots:
    virtual void open_console();
    virtual void user_button();

protected:
    QGridLayout* firstLayout;

protected slots:
    virtual void languageChange();
private:
    QPixmap image0;
    QPixmap image1;
    QPixmap image2;
    QPixmap image3;
    QPixmap image4;
    QPixmap image5;
    QPixmap image6;
    QPixmap image7;

};

#endif // FIRST_H
