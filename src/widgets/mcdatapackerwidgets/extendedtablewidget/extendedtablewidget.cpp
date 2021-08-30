#include "extendedtablewidget.h"
#include "ui_extendedtablewidget.h"

#include "../numberprovider/numberprovider.h"

#include <QTableWidget>
#include <QGridLayout>
#include <QFrame>
#include <QItemSelectionModel>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

ExtendedTableWidget::ExtendedTableWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::ExtendedTableWidget) {
    ui->setupUi(this);
    m_layout = qobject_cast<QVBoxLayout*>(layout());

    setContainer(new QFrame(this));
    setAddingItem(false);

    connect(ui->addBtn, &QToolButton::clicked,
            this, &ExtendedTableWidget::onAddBtn);
    connect(ui->removeBtn, &QToolButton::clicked,
            this, &ExtendedTableWidget::onRemoveBtn);
    connect(ui->applyBtn, &QToolButton::clicked,
            this, &ExtendedTableWidget::onApplyBtn);
    connect(ui->cancelBtn, &QToolButton::clicked,
            this, &ExtendedTableWidget::onCancelBtn);
    connect(ui->__qt__passive_table->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &ExtendedTableWidget::updateRemoveBtn);

    updateRemoveBtn();
}

QTableWidget *ExtendedTableWidget::tableWidget() const {
    return ui->__qt__passive_table;
}

void ExtendedTableWidget::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);

    if (e->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
}

QFrame *ExtendedTableWidget::container() const {
    return ui->container;
}

void ExtendedTableWidget::setContainer(QFrame *widget) {
    widget->setFrameShape(QFrame::StyledPanel);
    widget->setFrameShadow(QFrame::Sunken);
    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    policy.setVerticalStretch(1);
    widget->setSizePolicy(policy);

    m_layout->removeWidget(ui->container);
    ui->container->deleteLater();
    m_layout->addWidget(widget);

    ui->container = widget;
}

void ExtendedTableWidget::setTableWidget(QTableWidget *widget) {
    auto &&policy = widget->sizePolicy();

    policy.setVerticalStretch(1);
    widget->setSizePolicy(policy);
    widget->setObjectName(QStringLiteral("__qt__passive_table"));

    m_layout->removeWidget(ui->__qt__passive_table);
    ui->__qt__passive_table->deleteLater();
    m_layout->insertWidget(0, widget);

    ui->__qt__passive_table = widget;
}

bool ExtendedTableWidget::isAddingItem() const {
    return m_isAddingItem;
}

void ExtendedTableWidget::setAddingItem(bool isAddingItem) {
    m_isAddingItem = isAddingItem;

    ui->container->setVisible(isAddingItem);
    ui->stackedWidget->setCurrentIndex((isAddingItem) ? 1 : 0);
}

QStringList ExtendedTableWidget::columnTitles() const {
    return m_columnTitles;
}

void ExtendedTableWidget::setColumnTitles(const QStringList &columnTitles) {
    m_columnTitles = columnTitles;

    const int size = columnTitles.size();

    ui->__qt__passive_table->setColumnCount(size);

    for (int i = 0; i < size; ++i) {
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setText(columnTitles[i]);
        ui->__qt__passive_table->setHorizontalHeaderItem(i, __qtablewidgetitem);
    }
}

void ExtendedTableWidget::appendColumnMapping(const QString &jsonKey,
                                              QWidget *editor) {
    m_columnMappings << ColumnMapping{ jsonKey, editor };
}

void ExtendedTableWidget::retranslateUi() {
    ui->retranslateUi(this);

    for (int i = 0; i < m_columnTitles.size(); ++i) {
        QTableWidgetItem *___qtablewidgetitem =
            ui->__qt__passive_table->horizontalHeaderItem(i);
        ___qtablewidgetitem->setText(m_columnTitles[i]);
    }
}


void ExtendedTableWidget::onAddBtn() {
    setAddingItem(true);
}

void ExtendedTableWidget::onRemoveBtn() {
    const auto &&indexes =
        ui->__qt__passive_table->selectionModel()->selectedRows();
    auto it = indexes.constEnd();

    while (it != indexes.constBegin()) {
        --it;
        ui->__qt__passive_table->removeRow(it->row());
    }
    updateRemoveBtn();
}

void ExtendedTableWidget::onApplyBtn() {
    setAddingItem(false);

    const int row = ui->__qt__passive_table->rowCount();
    ui->__qt__passive_table->insertRow(row);

    for (int col = 0; col < ui->__qt__passive_table->columnCount(); ++col) {
        auto *item = new QTableWidgetItem();

        auto *widget = m_columnMappings[col].editor;
        if (auto *editor = qobject_cast<NumberProvider*>(widget)) {
            item->setData(Qt::UserRole + 1, editor->toJson());
        } else if (auto *editor = qobject_cast<QSpinBox*>(widget)) {
            item->setData(Qt::EditRole, editor->value());
        } else if (auto *editor = qobject_cast<QLineEdit*>(widget)) {
            item->setText(editor->text());
        } else if (auto *editor = qobject_cast<QComboBox*>(widget)) {
            item->setData(Qt::DisplayRole,
                          editor->currentData(Qt::DisplayRole));
            item->setData(Qt::DecorationRole,
                          editor->currentData(Qt::DecorationRole));
            item->setData(Qt::EditRole, editor->currentData(Qt::EditRole));
            item->setData(Qt::UserRole + 1, editor->currentIndex());
            item->setText(editor->currentText());
        } else if (auto *editor = qobject_cast<QCheckBox*>(widget)) {
            item->setCheckState(editor->checkState());
        }

        ui->__qt__passive_table->setItem(row, col, item);
    }

    ui->__qt__passive_table->setItem(
        ui->__qt__passive_table->rowCount() - 1, 0,
        new QTableWidgetItem(ui->__qt__passive_table->rowCount() - 1));
}

void ExtendedTableWidget::onCancelBtn() {
    setAddingItem(false);
}

void ExtendedTableWidget::updateRemoveBtn() {
    ui->removeBtn->setEnabled(
        ui->__qt__passive_table->selectionModel()->selection().size() > 0);
}
