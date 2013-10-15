/****************************************************************************
** SettingDlg meta object code from reading C++ file 'SettingDlg.h'
**
** Created: Thu Mar 31 07:05:03 2011
**      by: The Qt MOC ($Id: qt/src/moc/moc.y   2.3.12   edited 2005-10-27 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "SettingDlg.h"
#include <qmetaobject.h>
#include <qapplication.h>
#ifdef QWS
#include <qobjectdict.h>
#endif



const char *SettingDlg::className() const
{
    return "SettingDlg";
}

QMetaObject *SettingDlg::metaObj = 0;

#ifdef QWS
static class SettingDlg_metaObj_Unloader {
public:
    ~SettingDlg_metaObj_Unloader()
    {
         if ( objectDict )
             objectDict->remove( "SettingDlg" );
    }
} SettingDlg_metaObj_unloader;
#endif

void SettingDlg::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QDialog::className(), "QDialog") != 0 )
	badSuperclassWarning("SettingDlg","QDialog");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString SettingDlg::tr(const char* s)
{
    return qApp->translate( "SettingDlg", s, 0 );
}

QString SettingDlg::tr(const char* s, const char * c)
{
    return qApp->translate( "SettingDlg", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* SettingDlg::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QDialog::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (SettingDlg::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    typedef void (SettingDlg::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (SettingDlg::*m1_t2)();
    typedef void (QObject::*om1_t2)();
    typedef void (SettingDlg::*m1_t3)();
    typedef void (QObject::*om1_t3)();
    m1_t0 v1_0 = &SettingDlg::boardrate_select;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &SettingDlg::enlarge_select;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &SettingDlg::cancel_pressed;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    m1_t3 v1_3 = &SettingDlg::ok_pressed;
    om1_t3 ov1_3 = (om1_t3)v1_3;
    QMetaData *slot_tbl = QMetaObject::new_metadata(4);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(4);
    slot_tbl[0].name = "boardrate_select()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "enlarge_select()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "cancel_pressed()";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Public;
    slot_tbl[3].name = "ok_pressed()";
    slot_tbl[3].ptr = (QMember)ov1_3;
    slot_tbl_access[3] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"SettingDlg", "QDialog",
	slot_tbl, 4,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    metaObj->set_slot_access( slot_tbl_access );
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    return metaObj;
}
