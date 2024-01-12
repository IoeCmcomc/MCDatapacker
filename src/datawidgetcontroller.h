#ifndef DATAWIDGETCONTROLLER_H
#define DATAWIDGETCONTROLLER_H

#include <QJsonObject>

QT_BEGIN_NAMESPACE
class QSpinBox;
class QLineEdit;
class QComboBox;
class QRadioButton;
QT_END_NAMESPACE

class NumberProvider;
class OptionalSpinBox;
class InventorySlot;
class TrueFalseBox;
class ExtendedTableWidget;
class DataWidgetInterface;

class DataWidgetController {
public:
    DataWidgetController(const bool required = false);
    virtual ~DataWidgetController() {
    };

    virtual bool hasAcceptableValue() const {
        return !m_required;
    };
    virtual void setValueFrom(const QJsonObject &obj, const QString &key) = 0;
    virtual void putValueTo(QJsonObject &obj, const QString &key) const   = 0;

    bool isRequired() const;

protected:
    bool m_required = false;
};

template<class T>
struct _ControllerWrapper;

template<class T>
using ControllerWrapper = typename _ControllerWrapper<T>::type;

class DataWidgetControllerRecord : public DataWidgetController {
public:
    explicit DataWidgetControllerRecord(const bool required = false)
        : DataWidgetController{required} {
    };
    ~DataWidgetControllerRecord() {
        qDeleteAll(m_widgetMappings);
    }

    template<class T, typename ...Args>
    DataWidgetController * addMapping(const QString &key, T *widget,
                                      Args&& ... params) {
        DataWidgetController *controller = new ControllerWrapper<T>(
            widget, std::forward<Args>(params)...);

        m_widgetMappings[key] = controller;
        return controller;
    };
    DataWidgetController * addMapping(const QString &key,
                                      DataWidgetController *controller) {
        m_widgetMappings[key] = controller;
        return controller;
    };

    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj, const QString &key) final;
    virtual void putValueTo(QJsonObject &obj, const QString &key) const final;

private:
    QMap<QString, DataWidgetController *> m_widgetMappings;
};


template<class T>
class DataWidgetControllerWidget : public DataWidgetController {
public:
    DataWidgetControllerWidget(T *widget, const bool required = false)
        : DataWidgetController{required}, m_widget{widget} {
    };

protected:
    T *m_widget = nullptr;
};

/*
   using SetCallbackFunc =
    std::function<void (DataWidgetController *that, const QJsonObject &obj,
                        const QString &key)>;
   using PutCallbackFunc =
    std::function<void (const DataWidgetController *that, QJsonObject &obj,
                        const QString &key)>;


   class Callback : public DataWidgetController {
   public:
    explicit Callback(DataWidgetController *controller)
        : DataWidgetController{false}, m_controller{controller} {
    };
    ~Callback() {
        delete m_controller;
    }

    Callback * whenSetValue(SetCallbackFunc func) {
        m_setCallback = func;
        return this;
    }
    Callback * whenPutValue(PutCallbackFunc func) {
        m_putCallback = func;
        return this;
    }

    void setValueFrom(const QJsonObject &obj, const QString &key) final {
        m_setCallback(this, obj, key);
    };
    void putValueTo(QJsonObject &obj, const QString &key) const final {
        m_putCallback(this, obj, key);
    };

   protected:
    DataWidgetController *m_controller = nullptr;
    SetCallbackFunc m_setCallback      = nullptr;
    PutCallbackFunc m_putCallback      = nullptr;
   };
 */

class DataWidgetControllerSpinBox
    : public DataWidgetControllerWidget<QSpinBox> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj, const QString &key) final;
    virtual void putValueTo(QJsonObject &obj, const QString &key) const final;
};

class DataWidgetControllerOptionalSpinBox
    : public DataWidgetControllerWidget<OptionalSpinBox> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj, const QString &key) final;
    virtual void putValueTo(QJsonObject &obj, const QString &key) const final;
};

class DataWidgetControllerNumberProvider
    : public DataWidgetControllerWidget<NumberProvider> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj, const QString &key) final;
    virtual void putValueTo(QJsonObject &obj, const QString &key) const final;
};

class DataWidgetControllerInventorySlot
    : public DataWidgetControllerWidget<InventorySlot> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj, const QString &key) final;
    virtual void putValueTo(QJsonObject &obj, const QString &key) const final;
};

class DataWidgetControllerLineEdit
    : public DataWidgetControllerWidget<QLineEdit> {
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj, const QString &key) final;
    virtual void putValueTo(QJsonObject &obj, const QString &key) const final;
};

class DataWidgetControllerTrueFalseBox
    : public DataWidgetControllerWidget<TrueFalseBox> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj, const QString &key) final;
    virtual void putValueTo(QJsonObject &obj, const QString &key) const final;
};

class DataWidgetControllerComboBox : public DataWidgetController {
public:
    DataWidgetControllerComboBox(QComboBox *widget,
                                 const bool required = false,
                                 const int dataRole  = Qt::UserRole + 1);

    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj, const QString &key) final;
    virtual void putValueTo(QJsonObject &obj, const QString &key) const final;

private:
    QComboBox *m_widget = nullptr;
    int m_dataRole      = Qt::UserRole + 1;
};

class DataWidgetControllerRadioButton : public DataWidgetController {
public:
    template<class T, typename ...Args>
    DataWidgetControllerRadioButton(QRadioButton *btn, T *widget,
                                    Args&& ... params) {
        m_controller =
            new ControllerWrapper<T>(widget, std::forward<Args>(params)...);
        m_button = btn;
    };
    ~DataWidgetControllerRadioButton() {
        delete m_controller;
    }

    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj, const QString &key) const final;

private:
    DataWidgetController *m_controller = nullptr;
    QRadioButton *m_button             = nullptr;
};


class DataWidgetControllerExtendedTableWidget
    : public DataWidgetControllerWidget<ExtendedTableWidget> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj, const QString &key) final;
    virtual void putValueTo(QJsonObject &obj, const QString &key) const final;
};


class DataWidgetControllerDataWidgetInterface
    : public DataWidgetControllerWidget<DataWidgetInterface> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj, const QString &key) final;
    virtual void putValueTo(QJsonObject &obj, const QString &key) const final;
};


template<class T>
struct _ControllerWrapper {
    using type = void;
};

#define DEFINE_CONTROLLER_WRAPPER(Class, Wrapper) \
        template<>                                \
        struct _ControllerWrapper<Class> {        \
            using type = Wrapper;                 \
        };
#define DEFINE_WRAPPER(Class) \
        DEFINE_CONTROLLER_WRAPPER(Class, DataWidgetController ## Class)

DEFINE_CONTROLLER_WRAPPER(QSpinBox, DataWidgetControllerSpinBox)
DEFINE_CONTROLLER_WRAPPER(QLineEdit, DataWidgetControllerLineEdit)
DEFINE_CONTROLLER_WRAPPER(QComboBox, DataWidgetControllerComboBox)
DEFINE_CONTROLLER_WRAPPER(QRadioButton, DataWidgetControllerRadioButton)
DEFINE_WRAPPER(NumberProvider)
DEFINE_WRAPPER(InventorySlot)
DEFINE_WRAPPER(TrueFalseBox)
DEFINE_WRAPPER(ExtendedTableWidget)
DEFINE_WRAPPER(DataWidgetInterface)

#endif // DATAWIDGETCONTROLLER_H
