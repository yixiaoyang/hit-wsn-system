/****************************************************************************
** MonitorDlg meta object code from reading C++ file 'Monitor.h'
**
** Created: Fri May 27 11:38:15 2011
**      by: The Qt MOC ($Id: qt/src/moc/moc.y   2.3.12   edited 2005-10-27 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/
/*
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "Monitor.h"
#include <qmetaobject.h>
#include <qapplication.h>
#ifdef QWS
#include <qobjectdict.h>
#endif



const char *MonitorDlg::className() const
{
    return "MonitorDlg";
}

QMetaObject *MonitorDlg::metaObj = 0;

#ifdef QWS
static class MonitorDlg_metaObj_Unloader {
public:
    ~MonitorDlg_metaObj_Unloader()
    {
         if ( objectDict )
             objectDict->remove( "MonitorDlg" );
    }
} MonitorDlg_metaObj_unloader;
#endif

void MonitorDlg::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QDialog::className(), "QDialog") != 0 )
	badSuperclassWarning("MonitorDlg","QDialog");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString MonitorDlg::tr(const char* s)
{
    return qApp->translate( "MonitorDlg", s, 0 );
}

QString MonitorDlg::tr(const char* s, const char * c)
{
    return qApp->translate( "MonitorDlg", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* MonitorDlg::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QDialog::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (MonitorDlg::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    typedef void (MonitorDlg::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    m1_t0 v1_0 = &MonitorDlg::textChanged;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &MonitorDlg::ok_pressed;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    QMetaData *slot_tbl = QMetaObject::new_metadata(2);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(2);
    slot_tbl[0].name = "textChanged()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "ok_pressed()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"MonitorDlg", "QDialog",
	slot_tbl, 2,
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
*/
/****************************************************************************
** MonitorDlg meta object code from reading C++ file 'Monitor.h'
**
** Created: Fri May 13 09:58:28 2011
**      by: The Qt MOC ($Id: qt/src/moc/moc.y   2.3.12   edited 2005-10-27 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "Monitor.h"
#include <qmetaobject.h>
#include <qapplication.h>
#ifdef QWS
#include <qobjectdict.h>
#endif



const char *MonitorDlg::className() const
{
    return "MonitorDlg";
}

QMetaObject *MonitorDlg::metaObj = 0;

#ifdef QWS
static class MonitorDlg_metaObj_Unloader {
public:
    ~MonitorDlg_metaObj_Unloader()
    {
         if ( objectDict )
             objectDict->remove( "MonitorDlg" );
    }
} MonitorDlg_metaObj_unloader;
#endif

void MonitorDlg::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QDialog::className(), "QDialog") != 0 )
	badSuperclassWarning("MonitorDlg","QDialog");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString MonitorDlg::tr(const char* s)
{
    return qApp->translate( "MonitorDlg", s, 0 );
}

QString MonitorDlg::tr(const char* s, const char * c)
{
    return qApp->translate( "MonitorDlg", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* MonitorDlg::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QDialog::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (MonitorDlg::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    typedef void (MonitorDlg::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (MonitorDlg::*m1_t2)();
    typedef void (QObject::*om1_t2)();
    typedef void (MonitorDlg::*m1_t3)();
    typedef void (QObject::*om1_t3)();
    m1_t0 v1_0 = &MonitorDlg::flush_data_slot;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &MonitorDlg::flush_page_slot;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &MonitorDlg::ok_pressed;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    m1_t3 v1_3 = &MonitorDlg::textChanged;
    om1_t3 ov1_3 = (om1_t3)v1_3;
    QMetaData *slot_tbl = QMetaObject::new_metadata(4);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(4);
    slot_tbl[0].name = "flush_data_slot()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "flush_page_slot()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "ok_pressed()";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Public;
    slot_tbl[3].name = "textChanged()";
    slot_tbl[3].ptr = (QMember)ov1_3;
    slot_tbl_access[3] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"MonitorDlg", "QDialog",
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
