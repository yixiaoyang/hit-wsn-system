/****************************************************************************
** Form interface generated from reading ui file 'SettingDlg.ui'
**
** Created: Thu Mar 31 07:05:03 2011
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef SETTINGDLG_H
#define SETTINGDLG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;

class SettingDlg : public QDialog
{ 
    Q_OBJECT

public:
    SettingDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SettingDlg();

    QPushButton* PushButton_Cancel;
    QPushButton* PushButton_OK;
    QLabel* TextLabel1;
    QComboBox* ComboBox_Boardrate;
    QLabel* Boardrate;
    QComboBox* ComboBox1;
    QLineEdit* EditFrequence;
    QLabel* Enlargement;

public slots:
    virtual void boardrate_select();
    virtual void enlarge_select();
    virtual void cancel_pressed();
    virtual void ok_pressed();

protected:
    QGridLayout* SettingDlgLayout;
    QGridLayout* Layout4;
};

#endif // SETTINGDLG_H
