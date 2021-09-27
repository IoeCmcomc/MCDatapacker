#include "extendedtablewidget.h"
#include "ui_extendedtablewidget.h"

#include "../numberprovider/numberproviderdelegate.h"
#include "comboboxdelegate.h"

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

void ExtendedTableWidget::fromJson(const QJsonObject &root) {
    clear();

    int row = 0;
    for (auto it = root.constBegin(); it != root.constEnd(); ++it) {
        const int cols = ui->__qt__passive_table->columnCount();

        if ((m_jsonMode == JsonMode::SimpleMap) && (cols == 2)) {
            loadItemFromJson(row, 0, it.key());
            loadItemFromJson(row, 1, it.value());
        } else {
            loadItemFromJson(row, 0, it.key());
            const QJsonObject &&obj = it.value().toObject();
            for (auto objIt = obj.constBegin();
                 objIt != obj.constEnd(); ++objIt) {
                const QString &&key   = objIt.key();
                const int       index = indexOfKeyInMapping(key);
                if (index != -1)
                    loadItemFromJson(row, index, objIt.value());
                else if (!key.isEmpty())
                    qWarning() << "Undefined JSON key:" << key;
            }
        }
        ++row;
    }
}

void ExtendedTableWidget::setJsonMode(const JsonMode &jsonMode) {
    m_jsonMode = jsonMode;
}

void ExtendedTableWidget::setGameVersion(const QVersionNumber &version) {
    m_gameVersion = version;
}

void ExtendedTableWidget::clear() {
    ui->__qt__passive_table->clearContents();
    ui->__qt__passive_table->setRowCount(0);
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

    setAddingItem(m_isAddingItem);
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
    const int curCol = m_columnMappings.size() - 1;
    if (!gameVerLim.first.isNull() && gameVerLim.first > m_gameVersion) {
        editor->hide();
        ui->__qt__passive_table->setColumnHidden(curCol,
                                                 true);
    } else {
        if (qobject_cast<NumberProvider*>(editor)) {
            ui->__qt__passive_table->setItemDelegateForColumn(
                curCol, new NumberProviderDelegate(this));
        } else if (auto *combobox = qobject_cast<QComboBox*>(editor)) {
            ui->__qt__passive_table->setItemDelegateForColumn(
                curCol, new ComboboxDelegate(combobox, this));
        }
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

    if (qobject_cast<NumberProvider*>(widget)) {
        return item->data(NumberProviderRole).toJsonValue();
    } else if (qobject_cast<QSpinBox*>(widget)) {
        return item->data(Qt::EditRole).toInt();
    } else if (qobject_cast<QLineEdit*>(widget)) {
        return item->text();
    } else if (const auto *editor = qobject_cast<QComboBox*>(widget)) {
        const int    index = item->data(ComboboxIndexRole).toInt();
        const auto &&vari  = editor->itemData(index, ComboboxDataRole);
        if (!vari.isNull()) {
            return vari.toJsonValue();
        } else {
            return editor->itemData(index, Qt::DisplayRole).toJsonValue();
        }
    } else if (qobject_cast<QCheckBox*>(widget)) {
        return item->data(Qt::EditRole).toBool();
    }
    return QJsonValue::Null;
}

int ExtendedTableWidget::indexOfKeyInMapping(const QString &key) const {
    auto it = std::find_if(
        m_columnMappings.constBegin(), m_columnMappings.constEnd(),
        [&key](const ColumnMapping &mapping) -> bool {
        return key == mapping.jsonKey;
    });
    const int index = (it != m_columnMappings.constEnd())
                          ? (it - m_columnMappings.constBegin()) : -1;

    return index;
}

void ExtendedTableWidget::loadItemFromJson(int row, int col,
                                           const QJsonValue &value) {
    if (row >= ui->__qt__passive_table->rowCount())
        ui->__qt__passive_table->insertRow(row);

    auto *item = new QTableWidgetItem();

    auto *widget = m_columnMappings[col].editor;
    if (qobject_cast<NumberProvider*>(widget)) {
        item->setData(NumberProviderRole, value);
    } else if (qobject_cast<QSpinBox*>(widget)) {
        item->setData(Qt::DisplayRole, value.toInt());
    } else if (qobject_cast<QLineEdit*>(widget)) {
        item->setText(value.toString());
    } else if (auto *editor = qobject_cast<QComboBox*>(widget)) {
        int index = editor->findData(value.toVariant(),
                                     ExtendedRole::ComboboxDataRole);
        if (index == -1)
            index = editor->findText(value.toString());
        if (index == -1)
            return;

        item->setText(editor->itemData(index, Qt::DisplayRole).toString());
        item->setData(Qt::DecorationRole,
                      editor->itemData(index, Qt::DecorationRole));
        item->setData(ComboboxIndexRole, index);
    } else if (qobject_cast<QCheckBox*>(widget)) {
        item->setData(Qt::EditRole, value);
    }

    ui->__qt__passive_table->setItem(row, col, item);
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
    for (const auto &mapping : qAsConst(m_columnMappings)) {
        if (auto lineEdit = qobject_cast<QLineEdit*>(mapping.editor)) {
            if (lineEdit->text().isEmpty())
                return;
        }
    }

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
            item->setData(ComboboxIndexRole, editor->currentIndex());
        } else if (auto *editor = qobject_cast<QCheckBox*>(widget)) {
            item->setData(Qt::EditRole, editor->isChecked());
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
