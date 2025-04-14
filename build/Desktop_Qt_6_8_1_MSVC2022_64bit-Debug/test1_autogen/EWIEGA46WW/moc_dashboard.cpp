/****************************************************************************
** Meta object code from reading C++ file 'dashboard.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../dashboard.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dashboard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN9DashboardE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN9DashboardE = QtMocHelpers::stringData(
    "Dashboard",
    "valueChanged",
    "",
    "value",
    "rangeChanged",
    "dashboardDoubleClicked",
    "dashboardSettingsChanged",
    "objectName",
    "QMap<QString,QVariant>",
    "settings",
    "customVariableFormulaChanged",
    "formula",
    "refreshUI",
    "updateValue",
    "minValue",
    "maxValue",
    "precision",
    "unit",
    "title",
    "titleColor",
    "scaleColor",
    "pointerStyle",
    "PointerStyle",
    "pointerColor",
    "animation",
    "animationStep",
    "variableName",
    "customVariable"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN9DashboardE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
      15,   73, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   56,    2, 0x06,   16 /* Public */,
       4,    0,   59,    2, 0x06,   18 /* Public */,
       5,    0,   60,    2, 0x06,   19 /* Public */,
       6,    2,   61,    2, 0x06,   20 /* Public */,
      10,    2,   66,    2, 0x06,   23 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      12,    0,   71,    2, 0x0a,   26 /* Public */,
      13,    0,   72,    2, 0x08,   27 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 8,    7,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    7,   11,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags, notifyId, revision
       3, QMetaType::Double, 0x00015103, uint(0), 0,
      14, QMetaType::Double, 0x00015103, uint(1), 0,
      15, QMetaType::Double, 0x00015103, uint(1), 0,
      16, QMetaType::Int, 0x00015103, uint(-1), 0,
      17, QMetaType::QString, 0x00015103, uint(-1), 0,
      18, QMetaType::QString, 0x00015103, uint(-1), 0,
      19, QMetaType::QColor, 0x00015103, uint(-1), 0,
      20, QMetaType::QColor, 0x00015103, uint(-1), 0,
      21, 0x80000000 | 22, 0x0001510b, uint(-1), 0,
      23, QMetaType::QColor, 0x00015103, uint(-1), 0,
      24, QMetaType::Bool, 0x00015103, uint(-1), 0,
      25, QMetaType::Double, 0x00015103, uint(-1), 0,
      26, QMetaType::QString, 0x00015103, uint(-1), 0,
      11, QMetaType::QString, 0x00015103, uint(-1), 0,
      27, QMetaType::Bool, 0x00015103, uint(-1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject Dashboard::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ZN9DashboardE.offsetsAndSizes,
    qt_meta_data_ZN9DashboardE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN9DashboardE_t,
        // property 'value'
        QtPrivate::TypeAndForceComplete<double, std::true_type>,
        // property 'minValue'
        QtPrivate::TypeAndForceComplete<double, std::true_type>,
        // property 'maxValue'
        QtPrivate::TypeAndForceComplete<double, std::true_type>,
        // property 'precision'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'unit'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'title'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'titleColor'
        QtPrivate::TypeAndForceComplete<QColor, std::true_type>,
        // property 'scaleColor'
        QtPrivate::TypeAndForceComplete<QColor, std::true_type>,
        // property 'pointerStyle'
        QtPrivate::TypeAndForceComplete<PointerStyle, std::true_type>,
        // property 'pointerColor'
        QtPrivate::TypeAndForceComplete<QColor, std::true_type>,
        // property 'animation'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'animationStep'
        QtPrivate::TypeAndForceComplete<double, std::true_type>,
        // property 'variableName'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'formula'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'customVariable'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Dashboard, std::true_type>,
        // method 'valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'rangeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'dashboardDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'dashboardSettingsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QMap<QString,QVariant> &, std::false_type>,
        // method 'customVariableFormulaChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'refreshUI'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateValue'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Dashboard::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Dashboard *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->valueChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 1: _t->rangeChanged(); break;
        case 2: _t->dashboardDoubleClicked(); break;
        case 3: _t->dashboardSettingsChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QMap<QString,QVariant>>>(_a[2]))); break;
        case 4: _t->customVariableFormulaChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->refreshUI(); break;
        case 6: _t->updateValue(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (Dashboard::*)(double );
            if (_q_method_type _q_method = &Dashboard::valueChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (Dashboard::*)();
            if (_q_method_type _q_method = &Dashboard::rangeChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (Dashboard::*)();
            if (_q_method_type _q_method = &Dashboard::dashboardDoubleClicked; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _q_method_type = void (Dashboard::*)(const QString & , const QMap<QString,QVariant> & );
            if (_q_method_type _q_method = &Dashboard::dashboardSettingsChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _q_method_type = void (Dashboard::*)(const QString & , const QString & );
            if (_q_method_type _q_method = &Dashboard::customVariableFormulaChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->getValue(); break;
        case 1: *reinterpret_cast< double*>(_v) = _t->getMinValue(); break;
        case 2: *reinterpret_cast< double*>(_v) = _t->getMaxValue(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->getPrecision(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->getUnit(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->getTitle(); break;
        case 6: *reinterpret_cast< QColor*>(_v) = _t->getTitleColor(); break;
        case 7: *reinterpret_cast< QColor*>(_v) = _t->getScaleColor(); break;
        case 8: *reinterpret_cast< PointerStyle*>(_v) = _t->getPointerStyle(); break;
        case 9: *reinterpret_cast< QColor*>(_v) = _t->getPointerColor(); break;
        case 10: *reinterpret_cast< bool*>(_v) = _t->getAnimation(); break;
        case 11: *reinterpret_cast< double*>(_v) = _t->getAnimationStep(); break;
        case 12: *reinterpret_cast< QString*>(_v) = _t->getVariableName(); break;
        case 13: *reinterpret_cast< QString*>(_v) = _t->getFormula(); break;
        case 14: *reinterpret_cast< bool*>(_v) = _t->isCustomVariable(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setValue(*reinterpret_cast< double*>(_v)); break;
        case 1: _t->setMinValue(*reinterpret_cast< double*>(_v)); break;
        case 2: _t->setMaxValue(*reinterpret_cast< double*>(_v)); break;
        case 3: _t->setPrecision(*reinterpret_cast< int*>(_v)); break;
        case 4: _t->setUnit(*reinterpret_cast< QString*>(_v)); break;
        case 5: _t->setTitle(*reinterpret_cast< QString*>(_v)); break;
        case 6: _t->setTitleColor(*reinterpret_cast< QColor*>(_v)); break;
        case 7: _t->setScaleColor(*reinterpret_cast< QColor*>(_v)); break;
        case 8: _t->setPointerStyle(*reinterpret_cast< PointerStyle*>(_v)); break;
        case 9: _t->setPointerColor(*reinterpret_cast< QColor*>(_v)); break;
        case 10: _t->setAnimation(*reinterpret_cast< bool*>(_v)); break;
        case 11: _t->setAnimationStep(*reinterpret_cast< double*>(_v)); break;
        case 12: _t->setVariableName(*reinterpret_cast< QString*>(_v)); break;
        case 13: _t->setFormula(*reinterpret_cast< QString*>(_v)); break;
        case 14: _t->setCustomVariable(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *Dashboard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Dashboard::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN9DashboardE.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Dashboard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void Dashboard::valueChanged(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Dashboard::rangeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Dashboard::dashboardDoubleClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Dashboard::dashboardSettingsChanged(const QString & _t1, const QMap<QString,QVariant> & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Dashboard::customVariableFormulaChanged(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
