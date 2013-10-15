/****************************************************************************
** first meta object code from reading C++ file 'first.h'
**
** Created: Mon May 9 03:52:28 2011
**      by: The Qt MOC ($Id: qt/src/moc/moc.y   2.3.12   edited 2005-10-27 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "first.h"
#include <qmetaobject.h>
#include <qapplication.h>
#ifdef QWS
#include <qobjectdict.h>
#endif



const char *first::className() const
{
    return "first";
}

QMetaObject *first::metaObj = 0;

#ifdef QWS
static class first_metaObj_Unloader {
public:
    ~first_metaObj_Unloader()
    {
         if ( objectDict )
             objectDict->remove( "first" );
    }
} first_metaObj_unloader;
#endif

void first::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QWidget::className(), "QWidget") != 0 )
	badSuperclassWarning("first","QWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString first::tr(const char* s)
{
    return qApp->translate( "first", s, 0 );
}

QString first::tr(const char* s, const char * c)
{
    return qApp->translate( "first", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* first::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (first::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    typedef void (first::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (first::*m1_t2)();
    typedef void (QObject::*om1_t2)();
    typedef void (first::*m1_t3)();
    typedef void (QObject::*om1_t3)();
    m1_t0 v1_0 = &first::open_console;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &first::user_button;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &first::open_setting;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    m1_t3 v1_3 = &first::languageChange;
    om1_t3 ov1_3 = (om1_t3)v1_3;
    QMetaData *slot_tbl = QMetaObject::new_metadata(4);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(4);
    slot_tbl[0].name = "open_console()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "user_button()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "open_setting()";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Protected;
    slot_tbl[3].name = "languageChange()";
    slot_tbl[3].ptr = (QMember)ov1_3;
    slot_tbl_access[3] = QMetaData::Protected;
    metaObj = QMetaObject::new_metaobject(
	"first", "QWidget",
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
