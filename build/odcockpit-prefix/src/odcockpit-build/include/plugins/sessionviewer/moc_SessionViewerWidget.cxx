/****************************************************************************
** Meta object code from reading C++ file 'SessionViewerWidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../odcockpit/include/plugins/sessionviewer/SessionViewerWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SessionViewerWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_cockpit__plugins__sessionviewer__SessionViewerWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_cockpit__plugins__sessionviewer__SessionViewerWidget[] = {
    "cockpit::plugins::sessionviewer::SessionViewerWidget\0"
};

void cockpit::plugins::sessionviewer::SessionViewerWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData cockpit::plugins::sessionviewer::SessionViewerWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject cockpit::plugins::sessionviewer::SessionViewerWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_cockpit__plugins__sessionviewer__SessionViewerWidget,
      qt_meta_data_cockpit__plugins__sessionviewer__SessionViewerWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &cockpit::plugins::sessionviewer::SessionViewerWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *cockpit::plugins::sessionviewer::SessionViewerWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *cockpit::plugins::sessionviewer::SessionViewerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_cockpit__plugins__sessionviewer__SessionViewerWidget))
        return static_cast<void*>(const_cast< SessionViewerWidget*>(this));
    if (!strcmp(_clname, "odcore::io::conference::ContainerListener"))
        return static_cast< odcore::io::conference::ContainerListener*>(const_cast< SessionViewerWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int cockpit::plugins::sessionviewer::SessionViewerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
