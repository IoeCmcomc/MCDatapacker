#ifndef DATAWIDGETCONTROLLER_H
#define DATAWIDGETCONTROLLER_H

#include <QJsonObject>

QT_BEGIN_NAMESPACE
class QSpinBox;
class QLineEdit;
class QComboBox;
class QRadioButton;
class QCheckBox;
class QButtonGroup;
QT_END_NAMESPACE

class NumberProvider;
class OptionalSpinBox;
class InventorySlot;
class TrueFalseBox;
class ExtendedTableWidget;
class DataWidgetInterface;
class MultiPageWidget;
class DialogDataButton;
class IdTagSelector;

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

template<class T>
using enable_if_is_base_of_qwidget = std::enable_if_t<std::is_base_of_v<QWidget,
                                                                        T> >;

template<class T, typename ...Args,
         typename = enable_if_is_base_of_qwidget<T> >
DataWidgetController * makeController(T *widget, Args&& ... params) {
    return new ControllerWrapper<T>(widget, std::forward<Args>(params)...);
}

struct DataWidgetControllerRecordPair {
    QString               key;
    DataWidgetController *controller = nullptr;

    DataWidgetControllerRecordPair(const QString &key,
                                   DataWidgetController * controller) {
        this->key        = key;
        this->controller = controller;
    }

    template<class T, typename ...Args,
             typename = enable_if_is_base_of_qwidget<T> >
    DataWidgetControllerRecordPair(const QString &key,
                                   T * widget,
                                   Args && ... params) {
        this->key  = key;
        controller = makeController(widget, std::forward<Args>(params)...);
    };
};

class DataWidgetControllerRecord : public DataWidgetController {
public:
    explicit DataWidgetControllerRecord(const bool required = false)
        : DataWidgetController{required} {
    };
    ~DataWidgetControllerRecord() {
        qDeleteAll(m_widgetMappings);
    }

    explicit DataWidgetControllerRecord(
        std::initializer_list<DataWidgetControllerRecordPair> list)
        : DataWidgetController{false} {
        for (auto &&elem: list) {
            m_widgetMappings[elem.key] = elem.controller;
        }
    }

    template<class T, typename ...Args,
             typename = enable_if_is_base_of_qwidget<T> >
    DataWidgetController * addMapping(const QString &key, T *widget,
                                      Args&& ... params) {
        DataWidgetController *controller = makeController(
            widget, std::forward<Args>(params)...);

        m_widgetMappings[key] = controller;
        return controller;
    };
    DataWidgetController * addMapping(const QString &key,
                                      DataWidgetController *controller);

    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;

private:
    QMap<QString, DataWidgetController *> m_widgetMappings;
};

using DWCRecord = DataWidgetControllerRecord;


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
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;
};

class DataWidgetControllerOptionalSpinBox
    : public DataWidgetControllerWidget<OptionalSpinBox> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;
};

class DataWidgetControllerNumberProvider
    : public DataWidgetControllerWidget<NumberProvider> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;
};

class DataWidgetControllerInventorySlot
    : public DataWidgetControllerWidget<InventorySlot> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;
};

class DataWidgetControllerLineEdit
    : public DataWidgetControllerWidget<QLineEdit> {
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;
};

class DataWidgetControllerCheckBox
    : public DataWidgetControllerWidget<QCheckBox> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;
};

class DataWidgetControllerTrueFalseBox
    : public DataWidgetControllerWidget<TrueFalseBox> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;
};

class DataWidgetControllerComboBox : public DataWidgetController {
public:
    DataWidgetControllerComboBox(QComboBox *widget,
                                 const bool required = false,
                                 const int dataRole  = Qt::UserRole +
                                 1);

    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;

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
            new ControllerWrapper<T>(widget,
                                     std::forward<Args>(params)...);
        m_button = btn;
    };
    DataWidgetControllerRadioButton(
        const DataWidgetControllerRadioButton &) =
    delete;
    void operator=(const DataWidgetControllerRadioButton &) = delete;
    ~DataWidgetControllerRadioButton() {
        delete m_controller;
    }

    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;

private:
    DataWidgetController *m_controller = nullptr;
    QRadioButton *m_button             = nullptr;
};


class DataWidgetControllerExtendedTableWidget
    : public DataWidgetControllerWidget<ExtendedTableWidget> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;
};


class DataWidgetControllerDataWidgetInterface
    : public DataWidgetControllerWidget<DataWidgetInterface> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;
};


class DataWidgetControllerDialogDataButton
    : public DataWidgetControllerWidget<DialogDataButton> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;
};


class DataWidgetControllerIdTagSelector
    : public DataWidgetControllerWidget<IdTagSelector> {
public:
    using DataWidgetControllerWidget::DataWidgetControllerWidget;
    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;
};


/*!
    \class StringIndexMap

    \brief A simple data structure to map bi-directionally between
    QString and int.
 */
class StringIndexMap {
public:
    StringIndexMap() = default;
    StringIndexMap(
        std::initializer_list<std::pair<QString, int> > list);

    void insert(const QString &str, const int index);

    int indexOf(const QString &str) const;
    QString stringOf(const int index) const;

private:
    QMap<QString, int> m_stringToIndex;
    QVector<QString> m_indexToString;
};

/*
   template<typename T, typename U>
   class BiMap {
   public:
    BiMap() = default;
    BiMap(std::initializer_list<std::pair<T, U> > list) {
        for (const auto &[first, second]: list) {
            m_firstToSecond[first]  = second;
            m_secondToFirst[second] = first;
        }
    }

    void insert(const T &first, const U &second) {
        m_firstToSecond.insert(first, second);
        m_secondToFirst.insert(second, first);
    }
    U value(const T &key) const {
        const auto it = m_firstToSecond.constFind(key);

        return (it != m_firstToSecond.cend()) ? it.value() : -1;
    }
    T value(const U &key) const {
        const auto it = m_secondToFirst.constFind(key);

        return (it != m_secondToFirst.cend()) ? it.value() : -1;
    }

   private:
    QMap<T, U> m_firstToSecond;
    QMap<U, T> m_secondToFirst;
   };
 */


struct DataWidgetControllerChoicePair {
    QString               key;
    int                   index      = 0;
    DataWidgetController *controller = nullptr;

    DataWidgetControllerChoicePair(const QString &key, const int index,
                                   DataWidgetController * controller) {
        this->key        = key;
        this->index      = index;
        this->controller = controller;
    }

    template<class T, typename ...Args,
             typename = enable_if_is_base_of_qwidget<T> >
    DataWidgetControllerChoicePair(const QString &key, const int index,
                                   T * widget,
                                   Args && ... params) {
        this->key   = key;
        this->index = index;
        controller  = makeController(widget, std::forward<Args>(params)...);
    };
};

void removePrefix(QString &str);

using DWCChoiceInitList = std::initializer_list<DataWidgetControllerChoicePair>;

template<typename Subclass>
class DataWidgetControllerChoices {
public:
    explicit DataWidgetControllerChoices(const QString &choiceKey,
                                         DWCChoiceInitList list = {})
        : m_choiceKey{choiceKey} {
        addChoices(list);
    };

    template<class U, typename ...Args,
             typename = enable_if_is_base_of_qwidget<U> >
    DataWidgetController * addChoice(const QString &key, const int index,
                                     U *widget, Args&& ... params) {
        return addChoice(key, index, makeController(
                             widget, std::forward<Args>(params)...));
    };
    DataWidgetController * addChoice(const QString &key,
                                     const int index,
                                     DataWidgetController *controller) {
        m_indexMap.insert(key, index);
        m_widgetChoices[key] = controller;
        return controller;
    };

    void addChoices(DWCChoiceInitList list) {
        for (const auto &elem: list) {
            addChoice(elem.key, elem.index, elem.controller);
        }
    }

    QString caughtChoice() const {
        return m_caughtChoice;
    }

protected:
    void setValueFromImpl(const QJsonObject &obj,
                          const QString &key) {
        m_caughtChoice.clear();
        if (obj.contains(key) || key.isNull()) {
            const auto &record = key.isNull() ? obj : obj.value(key).toObject();
            if (record.contains(m_choiceKey)) {
                QString &&choice = record.value(m_choiceKey).toString();
                removePrefix(choice);
                const int index = m_indexMap.indexOf(choice);
                if (index != -1) {
                    m_caughtChoice = choice;
                    static_cast<Subclass *>(this)->setCurrentIndex(index);
                    auto *controller = m_widgetChoices[choice];
                    Q_ASSERT(controller != nullptr);
                    controller->setValueFrom(record, {});
                }
            }
        }
    }
    void putValueToImpl(QJsonObject &obj,
                        const QString &key) const {
        QJsonObject  newRecord;
        QJsonObject &record = key.isNull() ? obj : newRecord;

        const QString &choice = m_indexMap.stringOf(
            static_cast<const Subclass *>(this)->currentIndex());

        Q_ASSERT(!choice.isNull());
        record[m_choiceKey] = choice;
        const auto *controller = m_widgetChoices[choice];
        Q_ASSERT(controller != nullptr);

        if (controller->hasAcceptableValue()) {
            controller->putValueTo(record, {});
            if (!key.isNull()) {
                obj[key] = record;
            }
        }
    }

private:
    StringIndexMap m_indexMap;
    QMap<QString, DataWidgetController *> m_widgetChoices;
    QString m_choiceKey;
    QString m_caughtChoice;
};

class DataWidgetControllerMultiPageWidget;
using DWCMultiPageWidgetSuperclass =
    DataWidgetControllerChoices<DataWidgetControllerMultiPageWidget>;
class DataWidgetControllerMultiPageWidget
    : public DataWidgetControllerWidget<MultiPageWidget>,
    public DWCMultiPageWidgetSuperclass {
public:
    DataWidgetControllerMultiPageWidget(MultiPageWidget *widget,
                                        const QString &choiceKey,
                                        DWCChoiceInitList list = {})
        :  DataWidgetControllerWidget(widget),
        DataWidgetControllerChoices(choiceKey, list) {
    };

    void setWidget(MultiPageWidget *widget) {
        this->m_widget = widget;
    }

    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;

private:
    friend DWCMultiPageWidgetSuperclass;

    int currentIndex() const;
    void setCurrentIndex(const int index);
};

class DataWidgetControllerButtonGroup;
using DWCButtonGroupSuperclass =
    DataWidgetControllerChoices<DataWidgetControllerButtonGroup>;
class DataWidgetControllerButtonGroup
    : public DataWidgetController, public DWCButtonGroupSuperclass {
public:
    DataWidgetControllerButtonGroup(QButtonGroup *group,
                                    const QString &choiceKey,
                                    DWCChoiceInitList list = {})
        :  DataWidgetController(),
        DataWidgetControllerChoices(choiceKey, list), m_group{group} {
    };

    virtual bool hasAcceptableValue() const final;
    virtual void setValueFrom(const QJsonObject &obj,
                              const QString &key) final;
    virtual void putValueTo(QJsonObject &obj,
                            const QString &key) const final;

private:
    friend DWCButtonGroupSuperclass;

    QButtonGroup *m_group = nullptr;

    int currentIndex() const;
    void setCurrentIndex(const int index);
};

using DWCBtnGrp = DataWidgetControllerButtonGroup;


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
DEFINE_CONTROLLER_WRAPPER(QCheckBox, DataWidgetControllerCheckBox)
DEFINE_CONTROLLER_WRAPPER(QButtonGroup, DataWidgetControllerButtonGroup)
DEFINE_WRAPPER(OptionalSpinBox)
DEFINE_WRAPPER(NumberProvider)
DEFINE_WRAPPER(InventorySlot)
DEFINE_WRAPPER(TrueFalseBox)
DEFINE_WRAPPER(ExtendedTableWidget)
DEFINE_WRAPPER(DataWidgetInterface)
DEFINE_WRAPPER(MultiPageWidget)
DEFINE_WRAPPER(DialogDataButton)
DEFINE_WRAPPER(IdTagSelector)

#endif // DATAWIDGETCONTROLLER_H
