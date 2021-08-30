#ifndef EXTENDEDTABLEWIDGET_H
#define EXTENDEDTABLEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

#include "../plugin/extendedtablewidget/extendedtablewidgetplugin.h"

QT_BEGIN_NAMESPACE
class QTableWidget;
class QFrame;
class QToolButton;
QT_END_NAMESPACE

namespace Ui {
    class ExtendedTableWidget;
}

class ExtendedTableWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QTableWidget * tableWidget READ tableWidget WRITE setTableWidget)
    Q_PROPERTY(QFrame * container READ container WRITE setContainer)
    Q_PROPERTY(bool isAddingItem READ isAddingItem WRITE setAddingItem)
    Q_PROPERTY(
        QStringList columnTitles READ columnTitles WRITE setColumnTitles)

    struct ColumnMapping {
        QString  jsonKey;
        QWidget *editor = nullptr;
    };

public:

    ExtendedTableWidget(QWidget *parent = 0);

    QTableWidget *tableWidget() const;
    QFrame *container() const;

    bool isAddingItem() const;
    QStringList columnTitles() const;

    void appendColumnMapping(const QString &jsonKey, QWidget *editor);

public slots:
    void setContainer(QFrame *widget);
    void setTableWidget(QTableWidget *widget);
    void setAddingItem(bool isAddingItem);
    void setColumnTitles(const QStringList &columnTitles);

protected:
    void changeEvent(QEvent *e) override;

private:
    Ui::ExtendedTableWidget *ui;

    QStringList m_columnTitles;
    QVector<ColumnMapping> m_columnMappings;
    QVBoxLayout *m_layout = nullptr;
    bool m_isAddingItem   = false;

    void retranslateUi();

private slots:
    void onAddBtn();
    void onRemoveBtn();
    void onApplyBtn();
    void onCancelBtn();
    void updateRemoveBtn();
};

#endif /* EXTENDEDTABLEWIDGET_H */
