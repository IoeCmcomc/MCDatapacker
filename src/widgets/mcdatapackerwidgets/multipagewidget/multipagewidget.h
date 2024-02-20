/****************************************************************************
**
** Parts of this file, which was part of the examples of the Qt Toolkit,
** was licensed under BSD license.
**
****************************************************************************/

#ifndef MULTIPAGEWIDGET_H
#define MULTIPAGEWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QComboBox;
class QStackedWidget;
class QLabel;
class QFrame;
QT_END_NAMESPACE

/*! [0] */
class MultiPageWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(
        int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(
        QString pageTitle READ pageTitle WRITE setPageTitle STORED false NOTIFY pageTitleChanged)
    Q_PROPERTY(QString labelText READ labelText WRITE setLabelText STORED false)
    Q_PROPERTY(
        bool showSeparator READ showSeparator WRITE setShowSeparator STORED false)

public:
    explicit MultiPageWidget(QWidget *parent = nullptr);

    QSize sizeHint() const override;

    int count() const;
    int currentIndex() const;
    QString labelText() const;
    bool showSeparator() const;
    QWidget * widget(int index);
    QString pageTitle() const;

public slots:
    void addPage(QWidget *page);
    void insertPage(int index, QWidget *page);
    void removePage(int index);
    void setPageTitle(QString const &newTitle);
    void setCurrentIndex(int index);
    void setLabelText(const QString &str);
    void setShowSeparator(bool value);
    void setPageHidden(int index, bool hidden);
    void setPageHidden(QWidget *page, bool hidden);

private slots:
    void pageWindowTitleChanged();

signals:
    void currentIndexChanged(int index);
    void pageTitleChanged(const QString &title);
    void currentPageHiddenChanged(bool hidden);

private:

    QLabel *label               = nullptr;
    QComboBox *comboBox         = nullptr;
    QFrame *hLine               = nullptr;
    QStackedWidget *stackWidget = nullptr;
};
/*! [0] */

#endif
