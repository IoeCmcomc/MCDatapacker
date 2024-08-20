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
#include <QJsonArray>
#include <QTimer>


ExtendedTableWidget::ExtendedTableWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::ExtendedTableWidget) {
    ui->setupUi(this);
    m_layout = qobject_cast<QVBoxLayout *>(layout());

    ui->table->setObjectName(QStringLiteral("__qt__passive_table"));
    ui->addBtn->setObjectName(QStringLiteral("__qt__passive_addBtn"));
    ui->cancelBtn->setObjectName(QStringLiteral("__qt__passive_cancelBtn"));

    setContainer(new QFrame(this));

    connect(ui->addBtn, &QToolButton::clicked,
            this, &ExtendedTableWidget::onAddBtn);
    connect(ui->removeBtn, &QToolButton::clicked,
            this, &ExtendedTableWidget::onRemoveBtn);
    connect(ui->applyBtn, &QToolButton::clicked,
            this, &ExtendedTableWidget::onApplyBtn);
    connect(ui->cancelBtn, &QToolButton::clicked,
            this, &ExtendedTableWidget::onCancelBtn);
    connect(ui->table->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this, &ExtendedTableWidget::updateRemoveBtn);

    updateRemoveBtn();
    setAddingItem(false);
/*
      QHeaderView* headerView = ui->table->horizontalHeader();
      headerView->setSectionResizeMode(QHeaderView::ResizeToContents);
 */
}

QTableWidget * ExtendedTableWidget::tableWidget() const {
    return ui->table;
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

QJsonObject ExtendedTableWidget::toJsonObject() const {
    if (m_jsonMode == JsonMode::List || m_jsonMode == JsonMode::SimpleList)
        return QJsonObject();

    QJsonObject ret;

    for (int row = 0; row < ui->table->rowCount(); ++row) {
        const int cols = ui->table->columnCount();
        if ((m_jsonMode == JsonMode::SimpleMap) && (cols == 2)) {
            const QString &&key   = itemDataToJson(row, 0).toString();
            const auto    &&value = itemDataToJson(row, 1);
            if (!value.isNull())
                ret[key] = value;
        } else {
            QJsonObject     obj;
            const QString &&key = ui->table->item(row, 0)->text();
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

QJsonArray ExtendedTableWidget::toJsonArray() const {
    if (m_jsonMode == JsonMode::SimpleMap || m_jsonMode == JsonMode::ComplexMap)
        return QJsonArray();

    QJsonArray ret;
    const int  cols = ui->table->columnCount();

    for (int row = 0; row < ui->table->rowCount(); ++row) {
        if (m_jsonMode == JsonMode::SimpleList) {
            ret << itemDataToJson(row, 0);
        } else if (m_jsonMode == JsonMode::List) {
            QJsonObject obj;
            for (int col = 0; col < cols; ++col) {
                const auto &&value = itemDataToJson(row, col);
                if (!value.isNull())
                    obj[m_columnMappings[col].jsonKey] = value;
            }
            if (!obj.isEmpty())
                ret << obj;
        }
    }
    return ret;
}

void ExtendedTableWidget::fromJson(const QJsonObject &root) {
    clear();

    int row = 0;
    for (auto it = root.constBegin(); it != root.constEnd(); ++it) {
        const int cols = ui->table->columnCount();

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

void ExtendedTableWidget::fromJson(const QJsonArray &root) {
    clear();
    const int cols = ui->table->columnCount();

    if (m_jsonMode == JsonMode::List) {
        int row = 0;
        for (const auto &jsonRef : root) {
            const QJsonObject &&obj = jsonRef.toObject();
            for (auto objIt = obj.constBegin();
                 objIt != obj.constEnd(); ++objIt) {
                const QString &&key   = objIt.key();
                const int       index = indexOfKeyInMapping(key);
                if (index != -1) {
                    loadItemFromJson(row, index, objIt.value());
                } else if (!key.isEmpty()) {
                    qWarning() << "Undefined JSON key:" << key;
                }
            }
            ++row;
        }
    } else if (m_jsonMode == JsonMode::SimpleList && (cols == 1)) {
        int row = 0;
        for (const auto &jsonRef : root) {
            loadItemFromJson(row, 0, jsonRef);
            ++row;
        }
    }
}

void ExtendedTableWidget::setJsonMode(const JsonMode &jsonMode) {
    m_jsonMode = jsonMode;
    emit jsonModeChanged();
}

void ExtendedTableWidget::setGameVersion(const QVersionNumber &version) {
    m_gameVersion = version;
}

void ExtendedTableWidget::clear() {
    ui->table->clearContents();
    ui->table->setRowCount(0);
}

QFrame * ExtendedTableWidget::container() const {
    return ui->container;
}

void ExtendedTableWidget::setContainer(QFrame *widget) {
    Q_ASSERT(widget != nullptr);
    widget->setFrameShape(QFrame::StyledPanel);
    widget->setFrameShadow(QFrame::Sunken);
    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    policy.setVerticalStretch(1);
    widget->setSizePolicy(policy);

    auto *oldContainer = ui->container;
    if (oldContainer != widget) {
        m_layout->removeWidget(oldContainer);
        // Calling oldContainer->deleteLater() directly will not work
        // in the context of a modal dialog
        QTimer::singleShot(0, oldContainer, &QObject::deleteLater);
        m_layout->addWidget(widget);

        ui->container = widget;
        emit containerChanged();
    }

    setAddingItem(m_isAddingItem);
}

void ExtendedTableWidget::setTableWidget(QTableWidget *widget) {
    auto &&policy = widget->sizePolicy();

    policy.setVerticalStretch(1);
    widget->setSizePolicy(policy);
    widget->setObjectName(QStringLiteral("__qt__passive_table"));

    m_layout->removeWidget(ui->table);
    ui->table->deleteLater();
    m_layout->insertWidget(0, widget);

    ui->table = widget;

    emit tableWidgetChanged();
}

bool ExtendedTableWidget::isAddingItem() const {
    return m_isAddingItem;
}

void ExtendedTableWidget::setAddingItem(bool isAddingItem) {
    ui->container->setVisible(isAddingItem);
    ui->stackedWidget->setCurrentIndex((isAddingItem) ? 1 : 0);

    m_isAddingItem = isAddingItem;

    emit addingItemChanged();
}

QStringList ExtendedTableWidget::columnTitles() const {
    return m_columnTitles;
}

int ExtendedTableWidget::rowCount() const {
    return ui->table->rowCount();
}

void ExtendedTableWidget::setColumnTitles(const QStringList &columnTitles) {
    m_columnTitles = columnTitles;

    const int size = columnTitles.size();

    ui->table->setColumnCount(size);

    for (int i = 0; i < size; ++i) {
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setText(columnTitles[i]);
        ui->table->setHorizontalHeaderItem(i, __qtablewidgetitem);
    }

    emit columnTitlesChanged();
}

void ExtendedTableWidget::appendColumnMapping(const QString &jsonKey,
                                              QWidget *editor,
                                              VersionPair gameVerLim) {
#define     CHECK_EDITOR_CLASS_AND_SET_ENUM(Class) \
        if (qobject_cast<Class *>(editor)) editorClass = EditorClass::Class

    EditorClass editorClass = EditorClass::Unknown;
    CHECK_EDITOR_CLASS_AND_SET_ENUM(NumberProvider);
    else CHECK_EDITOR_CLASS_AND_SET_ENUM(QSpinBox);
    else CHECK_EDITOR_CLASS_AND_SET_ENUM(QLineEdit);
    else CHECK_EDITOR_CLASS_AND_SET_ENUM(QComboBox);
    else CHECK_EDITOR_CLASS_AND_SET_ENUM(QCheckBox);

    m_columnMappings <<
        ColumnMapping{ jsonKey, editor, editorClass, gameVerLim };

#undef CHECK_EDITOR_CLASS_AND_SET_ENUM

    const int curCol = m_columnMappings.size() - 1;

    if (!gameVerLim.first.isNull() && gameVerLim.first > m_gameVersion) {
        editor->hide();
        ui->table->setColumnHidden(curCol, true);
    } else {
        if (qobject_cast<NumberProvider *>(editor)) {
            ui->table->setItemDelegateForColumn(
                curCol, new NumberProviderDelegate(this));
        } else if (auto *combobox = qobject_cast<QComboBox *>(editor)) {
            ui->table->setItemDelegateForColumn(
                curCol, new ComboboxDelegate(combobox, this));
        }
    }
}

void ExtendedTableWidget::retranslateUi() {
    ui->retranslateUi(this);

    for (int i = 0; i < m_columnTitles.size(); ++i) {
        QTableWidgetItem *___qtablewidgetitem =
            ui->table->horizontalHeaderItem(i);
        ___qtablewidgetitem->setText(m_columnTitles[i]);
    }
}

QJsonValue ExtendedTableWidget::itemDataToJson(int row, int col) const {
    auto *item   = ui->table->item(row, col);
    auto *widget = m_columnMappings[col].editor;

    switch (m_columnMappings[col].editorClass) {
        case EditorClass::NumberProvider: {
            return item->data(NumberProviderRole).toJsonValue();
        }

        case EditorClass::QSpinBox: {
            return item->data(Qt::EditRole).toInt();
        }

        case EditorClass::QLineEdit: {
            return item->text();
        }

        case EditorClass::QComboBox: {
            const auto  *editor = qobject_cast<QComboBox *>(widget);
            const int    index  = item->data(ComboboxIndexRole).toInt();
            const auto &&vari   = editor->itemData(index, ComboboxDataRole);
            if (!vari.isNull()) {
                return vari.toJsonValue();
            } else {
                return editor->itemData(index, Qt::DisplayRole).toJsonValue();
            }
        }

        case EditorClass::QCheckBox: {
            return item->data(Qt::EditRole).toBool();
        }

        default:
            return QJsonValue::Null;
    }
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
    if (row >= ui->table->rowCount())
        ui->table->insertRow(row);

    auto *item = new QTableWidgetItem();

    auto *widget = m_columnMappings[col].editor;

    switch (m_columnMappings[col].editorClass) {
        case EditorClass::NumberProvider: {
            item->setData(NumberProviderRole, value);
            break;
        }

        case EditorClass::QSpinBox: {
            item->setData(Qt::DisplayRole, value.toInt());
            break;
        }

        case EditorClass::QLineEdit: {
            item->setText(value.toString());
            break;
        }

        case EditorClass::QComboBox: {
            auto *editor = qobject_cast<QComboBox *>(widget);
            int   index  = editor->findData(value.toVariant(),
                                            ExtendedRole::ComboboxDataRole);
            if (index == -1)
                index = editor->findText(value.toString());
            if (index == -1) {
                delete item;
                return;
            }

            item->setText(editor->itemData(index, Qt::DisplayRole).toString());
            item->setData(Qt::DecorationRole,
                          editor->itemData(index, Qt::DecorationRole));
            item->setData(ComboboxIndexRole, index);
            break;
        }

        case EditorClass::QCheckBox: {
            item->setData(Qt::EditRole, value.toBool());
            break;
        }

        default: {
            break;
        }
    }

    ui->table->setItem(row, col, item);
}


void ExtendedTableWidget::onAddBtn() {
    setAddingItem(true);
}

void ExtendedTableWidget::onRemoveBtn() {
    const auto &&indexes =
        ui->table->selectionModel()->selectedRows();
    auto it = indexes.constEnd();

    while (it != indexes.constBegin()) {
        --it;
        ui->table->removeRow(it->row());
    }
    updateRemoveBtn();
}

void ExtendedTableWidget::onApplyBtn() {
    for (const auto &mapping : qAsConst(m_columnMappings)) {
        if (auto lineEdit = qobject_cast<QLineEdit *>(mapping.editor)) {
            if (lineEdit->text().isEmpty())
                return;
        }
    }

    setAddingItem(false);

    const int row = ui->table->rowCount();
    ui->table->insertRow(row);

    for (int col = 0; col < ui->table->columnCount(); ++col) {
        auto *item = new QTableWidgetItem();

        auto *widget = m_columnMappings[col].editor;
        if (auto *editor = qobject_cast<NumberProvider *>(widget)) {
            item->setData(NumberProviderRole, editor->toJson());
        } else if (auto *editor = qobject_cast<QSpinBox *>(widget)) {
            item->setData(Qt::EditRole, editor->value());
        } else if (auto *editor = qobject_cast<QLineEdit *>(widget)) {
            item->setText(editor->text());
        } else if (auto *editor = qobject_cast<QComboBox *>(widget)) {
            item->setText(editor->currentText());
            item->setData(Qt::DecorationRole,
                          editor->currentData(Qt::DecorationRole));
            item->setData(ComboboxIndexRole, editor->currentIndex());
        } else if (auto *editor = qobject_cast<QCheckBox *>(widget)) {
            item->setData(Qt::EditRole, editor->isChecked());
        }

        ui->table->setItem(row, col, item);
    }

/*
      ui->table->setItem(
          ui->table->rowCount() - 1, 0,
          new QTableWidgetItem(ui->table->rowCount() - 1));
 */
}

void ExtendedTableWidget::onCancelBtn() {
    setAddingItem(false);
}

void ExtendedTableWidget::updateRemoveBtn() {
    ui->removeBtn->setEnabled(
        ui->table->selectionModel()->selection().size() > 0);
}
