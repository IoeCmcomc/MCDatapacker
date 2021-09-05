#include "extendedtablewidget.h"
#include "ui_extendedtablewidget.h"

#include "../numberprovider/numberproviderdelegate.h"

#include <QTableWidget>
#include <QVBoxLayout>
#include <QFrame>
#include <QItemSelectionModel>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QJsonObject>


ExtendedTableWidget::ExtendedTableWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::ExtendedTableWidget) {
    ui->setupUi(this);
    m_layout = qobject_cast<QVBoxLayout*>(layout());

    setContainer(new QFrame(this));

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
            this, &ExtendedTableWidget::updateRemoveBtn);

    updateRemoveBtn();
    setAddingItem(false);
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

ExtendedTableWidget::JsonMode ExtendedTableWidget::jsonMode() const {
    return m_jsonMode;
}

QJsonObject ExtendedTableWidget::toJson() const {
    QJsonObject ret;

    for (int row = 0; row < ui->__qt__passive_table->rowCount(); ++row) {
        const int cols = ui->__qt__passive_table->columnCount();
        if ((m_jsonMode == JsonMode::SimpleMap) && (cols == 2)) {
            const QString &&key =
                ui->__qt__passive_table->item(row, 0)->text();
            const auto &&value = itemDataToJson(row, 1);
            if (!value.isNull())
                ret[key] = value;
        } else {
            QJsonObject     obj;
            const QString &&key = ui->__qt__passive_table->item(row, 0)->text();
            for (int col = 1; col < cols; ++col) {
                const auto &&value = itemDataToJson(row, col);
                if (!value.isNull())
                    obj[m_columnMappings[col].jsonKey] = value;
            }
            if (!obj.isEmpty())
                ret[key] = obj;
        }
    }
    return ret;
}

void ExtendedTableWidget::setJsonMode(const JsonMode &jsonMode) {
    m_jsonMode = jsonMode;
}

void ExtendedTableWidget::setGameVersion(const QVersionNumber &version) {
    m_gameVersion = version;
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
                                              QWidget *editor,
                                              VersionPair gameVerLim) {
    m_columnMappings << ColumnMapping{ jsonKey, editor, gameVerLim };
    if (!gameVerLim.first.isNull() && gameVerLim.first > m_gameVersion) {
        editor->hide();
        ui->__qt__passive_table->setColumnHidden(m_columnMappings.size() - 1,
                                                 true);
    }
}

void ExtendedTableWidget::retranslateUi() {
    ui->retranslateUi(this);

    for (int i = 0; i < m_columnTitles.size(); ++i) {
        QTableWidgetItem *___qtablewidgetitem =
            ui->__qt__passive_table->horizontalHeaderItem(i);
        ___qtablewidgetitem->setText(m_columnTitles[i]);
    }
}

QJsonValue ExtendedTableWidget::itemDataToJson(int row, int col) const {
    auto *item = ui->__qt__passive_table->item(row, col);

    auto *widget = m_columnMappings[col].editor;

    if (const auto *editor = qobject_cast<NumberProvider*>(widget)) {
        return item->data(Qt::EditRole).toJsonValue();
    } else if (auto *editor = qobject_cast<QSpinBox*>(widget)) {
        return item->data(Qt::EditRole).toJsonValue();
    } else if (auto *editor = qobject_cast<QLineEdit*>(widget)) {
        return item->text();
    } else if (auto *editor = qobject_cast<QComboBox*>(widget)) {
        return item->data(ComboboxDataRole).toJsonValue();
    } else if (auto *editor = qobject_cast<QCheckBox*>(widget)) {
        return item->checkState() == Qt::Checked;
    }
    return QJsonValue::Null;
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
            item->setData(NumberProviderRole, editor->toJson());
        } else if (auto *editor = qobject_cast<QSpinBox*>(widget)) {
            item->setData(Qt::EditRole, editor->value());
        } else if (auto *editor = qobject_cast<QLineEdit*>(widget)) {
            item->setText(editor->text());
        } else if (auto *editor = qobject_cast<QComboBox*>(widget)) {
            item->setText(editor->currentText());
            item->setData(Qt::DecorationRole,
                          editor->currentData(Qt::DecorationRole));
            item->setData(ComboboxDataRole,
                          editor->currentData(ComboboxDataRole));
            item->setData(ComboboxIndexRole, editor->currentIndex());
        } else if (auto *editor = qobject_cast<QCheckBox*>(widget)) {
            item->setCheckState(editor->checkState());
        }

        ui->__qt__passive_table->setItem(row, col, item);
    }

/*
      ui->__qt__passive_table->setItem(
          ui->__qt__passive_table->rowCount() - 1, 0,
          new QTableWidgetItem(ui->__qt__passive_table->rowCount() - 1));
 */
}

void ExtendedTableWidget::onCancelBtn() {
    setAddingItem(false);
}

void ExtendedTableWidget::updateRemoveBtn() {
    ui->removeBtn->setEnabled(
        ui->__qt__passive_table->selectionModel()->selection().size() > 0);
}
