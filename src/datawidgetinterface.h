#ifndef DATAWIDGETINTERFACE_H
#define DATAWIDGETINTERFACE_H

#include <QJsonArray>
#include <QVBoxLayout>
#include <QWidget>

namespace Ui {
    class DataWidgetInterface;
}

class DataWidgetInterface : public QWidget
{
    Q_OBJECT

public:
    explicit DataWidgetInterface(QWidget *parent = nullptr);
    ~DataWidgetInterface();

    void setMainWidget(QWidget *widget);

    QJsonArray json() const;
    void setJson(const QJsonArray &json);

    int getCurrentIndex() const;
    void setCurrentIndex(int currentIndex);

    template<typename Class = QObject, typename Func>
    void mapToSetter(const Class *that, Func funcPtr) {
        connect(this, &DataWidgetInterface::setterCallRequested,
                that, funcPtr);
    }

    template <typename Sender, typename Func1, typename Func2>
    void addConnection(const Sender* sender, Func1 signal, Func2 slot) {
        connect(sender, signal, slot);
    }

    template<typename Class = QObject>
    void mapToGetter(QJsonObject (Class::*funcPtr)() const, Class *that) {
        connect(this, &DataWidgetInterface::getterCallRequested,
                [this, that, funcPtr]() {
            if (m_currentIndex < m_json.size() && m_currentIndex >= 0)
                m_json[m_currentIndex] = (that->*funcPtr)();
        });
    }
    template<typename Class = QWidget>
    void setupMainWidget(Class *widget) {
        mapToSetter<Class>(&Class::fromJson, widget);
        mapToGetter<Class>(&Class::toJson, widget);
    }

public slots:
    void addAfterCurrent();
    void removeCurrent();
    void onSliderMoved(int value);
    void onScrollAreaScrolled(int value);


signals:
    void getterCallRequested();
    void setterCallRequested(const QJsonObject &obj);
    void test(int);
/*    void resetCallRequested(); */

private:
    QVBoxLayout m_layout;
    Ui::DataWidgetInterface *ui;
    QJsonArray m_json;
    QWidget *m_mainWidget = nullptr;
    int m_currentIndex    = -1;
    bool m_reactToSignal  = true;

    void loadData(int index);
    void saveData(int index);
    void updateStates();
};

#endif /* DATAWIDGETINTERFACE_H */
