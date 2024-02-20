/****************************************************************************
**
** Parts of this file, which was part of the examples of the Qt Toolkit,
** was licensed under BSD license.
**
****************************************************************************/

#include <QComboBox>
#include <QFormLayout>
#include <QStackedWidget>
#include <QFrame>
#include <QLabel>
#include <QListView>
#include <QStandardItemModel>

#include "multipagewidget.h"

MultiPageWidget::MultiPageWidget(QWidget *parent)
    : QWidget(parent),
    label(new QLabel),
    comboBox(new QComboBox),
    hLine(new QFrame),
    stackWidget(new QStackedWidget) {
    comboBox->setObjectName(QStringLiteral("__qt__passive_comboBox"));

    connect(comboBox, QOverload<int>::of(&QComboBox::activated),
            this, &MultiPageWidget::setCurrentIndex);

    hLine->setFrameShape(QFrame::HLine);
    hLine->setFrameShadow(QFrame::Sunken);

    label->setText("Page:");
    label->hide();

    auto *layout = new QFormLayout(this);
    layout->addRow(label, comboBox);
    layout->addRow(hLine);
    layout->addRow(stackWidget);
    layout->setContentsMargins(0, 0, 0, 0);
}

QSize MultiPageWidget::sizeHint() const {
    return QSize(200, 150);
}

void MultiPageWidget::addPage(QWidget *page) {
    insertPage(count(), page);
}

void MultiPageWidget::removePage(int index) {
    QWidget *w = stackWidget->widget(index);

    stackWidget->removeWidget(w);

    comboBox->removeItem(index);
}

int MultiPageWidget::count() const {
    return stackWidget->count();
}

int MultiPageWidget::currentIndex() const {
    return stackWidget->currentIndex();
}

QString MultiPageWidget::labelText() const {
    return (label->isVisible()) ? label->text() : QString();
}

void MultiPageWidget::setLabelText(const QString &str) {
    label->setHidden(str.isEmpty());
    label->setText(str);
}

bool MultiPageWidget::showSeparator() const {
    return hLine->isVisible();
}

void MultiPageWidget::setShowSeparator(bool value) {
    hLine->setVisible(value);
}

void MultiPageWidget::setPageHidden(int index, bool hidden) {
    if (auto *view = qobject_cast<QListView *>(comboBox->view())) {
        auto *model = qobject_cast<QStandardItemModel *>(comboBox->model());
        Q_ASSERT(model != nullptr);
        view->setRowHidden(index, hidden);
        model->item(index, 0)->setEnabled(!hidden);

        emit currentPageHiddenChanged(hidden);
    }
}

void MultiPageWidget::setPageHidden(QWidget *page, bool hidden) {
    if (const int i = stackWidget->indexOf(page); i != -1) {
        setPageHidden(i, hidden);
    }
}

void MultiPageWidget::insertPage(int index, QWidget *page) {
    page->setParent(stackWidget);

    stackWidget->insertWidget(index, page);

    QString &&title = page->windowTitle();
    if (title.isEmpty()) {
        title = tr("Page %1").arg(comboBox->count() + 1);
        page->setWindowTitle(title);
    }
    connect(page, &QWidget::windowTitleChanged,
            this, &MultiPageWidget::pageWindowTitleChanged);
    comboBox->insertItem(index, title);
}

void MultiPageWidget::setCurrentIndex(int index) {
    if (index != currentIndex()) {
        stackWidget->setCurrentIndex(index);
        comboBox->setCurrentIndex(index);
        emit currentIndexChanged(index);
    }
}

void MultiPageWidget::pageWindowTitleChanged() {
    QWidget  *page  = qobject_cast<QWidget *>(sender());
    const int index = stackWidget->indexOf(page);

    comboBox->setItemText(index, page->windowTitle());
}

QWidget * MultiPageWidget::widget(int index) {
    return stackWidget->widget(index);
}

QString MultiPageWidget::pageTitle() const {
    if (const QWidget *currentWidget = stackWidget->currentWidget())
        return currentWidget->windowTitle();

    return QString();
}

void MultiPageWidget::setPageTitle(QString const &newTitle) {
    if (QWidget *currentWidget = stackWidget->currentWidget())
        currentWidget->setWindowTitle(newTitle);
    emit pageTitleChanged(newTitle);
}
