#ifndef DATAWIDGETINTERFACE_H
#define DATAWIDGETINTERFACE_H

#include <QJsonArray>
#include <QVBoxLayout>
#include <QFrame>

class QPropertyAnimation;

namespace Ui {
    class DataWidgetInterface;
}

class DataWidgetInterface : public QFrame
{
    Q_OBJECT

public:
    explicit DataWidgetInterface(QWidget *parent = nullptr);
    ~DataWidgetInterface();

    void setMainWidget(QWidget *widget);
    QWidget *mainWidget() const;

    QJsonArray json();
    void setJson(const QJsonArray &json);

    int getCurrentIndex() const;
    void setCurrentIndex(int currentIndex);

    inline int entriesCount() const {
        return m_json.size();
    }
    inline bool hasEntries() const {
        return !m_json.isEmpty();
    }

    template<typename Class = QObject, typename Func>
    void mapToSetter(Class *that, Func funcPtr) {
        connect(this, &DataWidgetInterface::setterCallRequested,
                that, funcPtr);
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
        setMainWidget(widget);
        connect(this, &DataWidgetInterface::setterCallRequested,
                widget, &Class::fromJson);
        mapToGetter<Class>(&Class::toJson, widget);
    }

public slots:
    void addAfterCurrent();
    void removeCurrent();

signals:
    void getterCallRequested();
    void setterCallRequested(const QJsonObject &obj);
    void entriesCountChanged(int value);

protected:
    void mouseMoveEvent(QMouseEvent *e) override;
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void changeEvent(QEvent *e) override;

private slots:
    void onSliderMoved(int value);
    void onScrollAreaScrolled(int value);
    void moveToPrevStep();
    void moveToNextStep();
    void moveToPrevPage();
    void moveToNextPage();
    void sidebarAnimFinished();

private:
    QVBoxLayout m_layout;
    QRect m_sidebarRect;
    QJsonArray m_json;
    Ui::DataWidgetInterface *ui;
    QWidget *m_mainWidget           = nullptr;
    QPropertyAnimation *m_animation = nullptr;
    int m_currentIndex              = -1;
    bool m_reactToSignal            = true;
    bool m_sidebarHiding            = false;
    const int m_sidebarSlideTime    = 50;

    void loadData(int index);
    void saveData(int index);
    void updateStates();
    void showSidebar();
    void hideSidebar();
};

#endif /* DATAWIDGETINTERFACE_H */
