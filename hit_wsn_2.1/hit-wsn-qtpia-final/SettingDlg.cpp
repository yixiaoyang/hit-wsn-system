/****************************************************************************
** Form implementation generated from reading ui file 'SettingDlg.ui'
**
** Created: Thu Mar 31 07:05:03 2011
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************
** New UI
**
** Create:By xiaoyangyi @2011.6.7
****************************************************************************/
#include "SettingDlg.h"

#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#define MAX_ITEM	7
const char* combox_item[MAX_ITEM] ={
	"1200",
	"2400",
	"4800",
	"9600",
	"38400",
	"57600",
	"115200"
};

#define MAX_ENLARGE		5
const char* combox1_item[MAX_ENLARGE]= {
	"0.25",
	"0.5",
	"1",
	"2",
	"4"
};
/* 
 *  Constructs a SettingDlg which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SettingDlg::SettingDlg( QWidget* parent,  const char* name, bool modal, WFlags 
fl )    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "SettingDlg" );
    resize( 320, 241 ); 
    setMinimumSize( QSize( 320, 240 ) );
    setMaximumSize( QSize( 320, 250 ) );
    setCaption( tr( "Setting" ) );
    SettingDlgLayout = new QGridLayout( this ); 
    SettingDlgLayout->setSpacing( 6 );
    SettingDlgLayout->setMargin( 11 );

    PushButton_Cancel = new QPushButton( this, "PushButton_Cancel" );
    PushButton_Cancel->setText( tr( "Cancel" ) );

    SettingDlgLayout->addWidget( PushButton_Cancel, 1, 1 );

    PushButton_OK = new QPushButton( this, "PushButton_OK" );
    PushButton_OK->setText( tr( "OK" ) );

    SettingDlgLayout->addWidget( PushButton_OK, 1, 0 );

    Layout4 = new QGridLayout; 
    Layout4->setSpacing( 6 );
    Layout4->setMargin( 0 );
	
    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setText( tr( "Fresh Frequence" ) );

    Layout4->addWidget( TextLabel1, 0, 0 );

    ComboBox_Boardrate = new QComboBox( FALSE, this, "ComboBox_Boardrate" );

    Layout4->addWidget( ComboBox_Boardrate, 2, 1 );

    Boardrate = new QLabel( this, "Boardrate" );
    Boardrate->setText( tr( "Boardrate Setting" ) );

    Layout4->addWidget( Boardrate, 2, 0 );

    ComboBox1 = new QComboBox( FALSE, this, "ComboBox1" );

    Layout4->addWidget( ComboBox1, 1, 1 );

    EditFrequence = new QLineEdit( this, "EditFrequence" );

    Layout4->addWidget( EditFrequence, 0, 1 );

    Enlargement = new QLabel( this, "Enlargement" );
    Enlargement->setText( tr( "Enlargement" ) );

    Layout4->addWidget( Enlargement, 1, 0 );

    SettingDlgLayout->addMultiCellLayout( Layout4, 0, 0, 0, 1 );

    // signals and slots connections
	connect( ComboBox1, SIGNAL( activated(int) ), this, SLOT( enlarge_select() ) );    
	connect( ComboBox_Boardrate, SIGNAL( activated(int) ), this, SLOT( boardrate_select() ) );    
	connect( PushButton_OK, SIGNAL( clicked() ), this, SLOT( ok_pressed() ) );
	connect( PushButton_Cancel, SIGNAL( pressed() ), this, SLOT( cancel_pressed() ) );
	
	//setting combox
	for(int i = 0; i < MAX_ITEM;i++){
		ComboBox_Boardrate->insertItem(combox_item[i]);	
	}
	//enlarge setting
	for(int j = 0; j < MAX_ENLARGE;j++){
		ComboBox1->insertItem(combox1_item[j]);	
	}
}

/*  
 *  Destroys the object and frees any allocated resources
 */
SettingDlg::~SettingDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

void SettingDlg::boardrate_select()
{
    qWarning( "SettingDlg::boardrate_select(): Not implemented yet!" );
}

void SettingDlg::enlarge_select()
{
    qWarning( "SettingDlg::enlarge_select(): Not implemented yet!" );
}

void SettingDlg::cancel_pressed()
{
    qWarning( "SettingDlg::cancel_pressed(): Not implemented yet!" );
}

void SettingDlg::ok_pressed()
{
    qWarning( "SettingDlg::ok_pressed(): Not implemented yet!" );
}

