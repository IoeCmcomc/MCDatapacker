#include "datawidgetcontroller.h"

#include "numberprovider.h"
#include "optionalspinbox.h"
#include "inventoryslot.h"
#include "truefalsebox.h"
#include "extendedtablewidget.h"
#include "datawidgetinterface.h"
#include "multipagewidget.h"

#include "modelfunctions.h"
#include "globalhelpers.h"

#include <QSpinBox>
#include <QJsonArray>
#include <QLineEdit>
#include <QComboBox>
#include <QRadioButton>

DataWidgetController::DataWidgetController(const bool required)
    : m_required{required} {
}

bool DataWidgetController::isRequired() const {
    return m_required;
}


bool DataWidgetControllerRecord::hasAcceptableValue() const {
    bool valid = true;

    for (const auto *controller: m_widgetMappings) {
        valid &= controller->hasAcceptableValue();
    }

    return valid;
}

void DataWidgetControllerRecord::setValueFrom(const QJsonObject &obj,
                                              const QString &key) {
    if (obj.contains(key) || key.isNull()) {
        const auto &record = key.isNull() ? obj : obj.value(key).toObject();
        for (auto it = record.constBegin(); it != record.constEnd(); ++it) {
            const QString &&pairKey = it.key();
            if (!m_widgetMappings.contains(pairKey)) {
                continue;
            }
            m_widgetMappings.value(pairKey)->setValueFrom(record, pairKey);
        }
    }
}

void DataWidgetControllerRecord::putValueTo(QJsonObject &obj,
                                            const QString &key) const {
    QJsonObject  newRecord;
    QJsonObject &record = key.isNull() ? obj : newRecord;

    for (auto it = m_widgetMappings.cbegin(); it != m_widgetMappings.cend();
         ++it) {
        if (it.value()->hasAcceptableValue()) {
            it.value()->putValueTo(record, it.key());
        } else if (it.value()->isRequired()) {
            obj = {};
            break;
        }
    }
    if (!key.isNull()) {
        obj[key] = record;
    }
}


bool DataWidgetControllerSpinBox::hasAcceptableValue() const {
    return m_widget->isEnabled() && m_widget->hasAcceptableInput();
}

void DataWidgetControllerSpinBox::setValueFrom(const QJsonObject &obj,
                                               const QString &key) {
    if (obj.contains(key)) {
        m_widget->setValue(obj.value(key).toInt());
    } else if (!m_widget->specialValueText().isEmpty()) {
        m_widget->setValue(m_widget->minimum());
    }
}

void DataWidgetControllerSpinBox::putValueTo(QJsonObject &obj,
                                             const QString &key) const {
    if (m_widget->specialValueText().isEmpty()
        || (m_widget->value() > m_widget->minimum())) {
        obj[key] = m_widget->value();
    }
}


bool DataWidgetControllerOptionalSpinBox::hasAcceptableValue() const {
    return m_widget->isEnabled() && !m_widget->isUnset();
}

void DataWidgetControllerOptionalSpinBox::setValueFrom(const QJsonObject &obj,
                                                       const QString &key) {
    if (obj.contains(key)) {
        m_widget->setValue(obj.value(key).toInt());
    }
}

void DataWidgetControllerOptionalSpinBox::putValueTo(QJsonObject &obj,
                                                     const QString &key) const {
    obj[key] = m_widget->value();
}


bool DataWidgetControllerNumberProvider::hasAcceptableValue() const {
    return m_widget->isEnabled() && !m_widget->isCurrentlyUnset();
}

void DataWidgetControllerNumberProvider::setValueFrom(const QJsonObject &obj,
                                                      const QString &key) {
    if (obj.contains(key)) {
        m_widget->fromJson(obj.value(key));
    }
}

void DataWidgetControllerNumberProvider::putValueTo(QJsonObject &obj,
                                                    const QString &key) const {
    obj[key] = m_widget->toJson();
}


bool DataWidgetControllerInventorySlot::hasAcceptableValue() const {
    return m_widget->isEnabled() && !m_widget->isEmpty();
}

void DataWidgetControllerInventorySlot::setValueFrom(const QJsonObject &obj,
                                                     const QString &key) {
    if (obj.contains(key)) {
        m_widget->clearItems();
        if (m_widget->getAcceptMultiple()) {
            if (const auto &&value = obj.value(key); value.isArray()) {
                const auto &&items = value.toArray();
                for (const auto &item: items) {
                    m_widget->appendItem(item.toString());
                }
            }
        } else {
            m_widget->setItem(InventoryItem(obj[key].toString()));
        }
    }
}

void DataWidgetControllerInventorySlot::putValueTo(QJsonObject &obj,
                                                   const QString &key) const {
    if (m_widget->getAcceptMultiple()) {
        QJsonArray ids;
        for (const auto &item: qAsConst(m_widget->getItems())) {
            ids << item.getNamespacedID();
        }
        obj[key] = ids;
    } else {
        obj[key] = m_widget->getItem().getNamespacedID();
    }
}


bool DataWidgetControllerLineEdit::hasAcceptableValue() const {
    return m_widget->isEnabled() && !m_widget->text().isEmpty();
}

void DataWidgetControllerLineEdit::setValueFrom(const QJsonObject &obj,
                                                const QString &key) {
    if (obj.contains(key)) {
        m_widget->setText(obj.value(key).toString());
    }
}

void DataWidgetControllerLineEdit::putValueTo(QJsonObject &obj,
                                              const QString &key) const {
    obj[key] = m_widget->text();
}


bool DataWidgetControllerCheckBox::hasAcceptableValue() const {
    return m_widget->isEnabled();
}

void DataWidgetControllerCheckBox::setValueFrom(const QJsonObject &obj,
                                                const QString &key) {
    m_widget->setChecked(obj.value(key).toBool());
}

void DataWidgetControllerCheckBox::putValueTo(QJsonObject &obj,
                                              const QString &key) const {
    obj[key] = m_widget->isChecked();
}


bool DataWidgetControllerTrueFalseBox::hasAcceptableValue() const {
    return m_widget->isEnabled();
}

void DataWidgetControllerTrueFalseBox::setValueFrom(const QJsonObject &obj,
                                                    const QString &key) {
    m_widget->setupFromJsonObject(obj, key);
}

void DataWidgetControllerTrueFalseBox::putValueTo(QJsonObject &obj,
                                                  const QString &key) const {
    m_widget->insertToJsonObject(obj, key);
}


DataWidgetControllerComboBox::DataWidgetControllerComboBox(
    QComboBox *widget, const bool required,
    const int dataRole) : DataWidgetController{required}, m_widget{widget},
    m_dataRole{dataRole} {
}

bool DataWidgetControllerComboBox::hasAcceptableValue() const {
    return m_widget->isEnabled() &&
           (!m_widget->itemData(0, m_dataRole).isNull()
            || m_widget->currentIndex() != 0);
}

void DataWidgetControllerComboBox::setValueFrom(const QJsonObject &obj,
                                                const QString &key) {
    if (obj.contains(key)) {
        setComboValueFrom(m_widget, obj[key], m_dataRole);
    }
}

void DataWidgetControllerComboBox::putValueTo(QJsonObject &obj,
                                              const QString &key) const {
    const auto &&data = m_widget->currentData(m_dataRole);

    if (!data.isNull()) {
        obj[key] = data.toJsonValue();
    } else {
        obj[key] = m_widget->currentData(Qt::DisplayRole).toString();
    }
}


bool DataWidgetControllerRadioButton::hasAcceptableValue() const {
    return m_controller->hasAcceptableValue() && m_button->isChecked();
}

void DataWidgetControllerRadioButton::setValueFrom(const QJsonObject &obj,
                                                   const QString &key) {
    m_controller->setValueFrom(obj, key);
}

void DataWidgetControllerRadioButton::putValueTo(QJsonObject &obj,
                                                 const QString &key) const {
    m_controller->putValueTo(obj, key);
    m_button->setChecked(obj.contains(key));
}


bool DataWidgetControllerExtendedTableWidget::hasAcceptableValue() const {
    return m_widget->isEnabled() && m_widget->rowCount() > 0;
}

void DataWidgetControllerExtendedTableWidget::setValueFrom(
    const QJsonObject &obj, const QString &key) {
    if (obj.contains(key)) {
        if (m_widget->jsonMode() == ExtendedTableWidget::JsonMode::List) {
            m_widget->fromJson(obj.value(key).toArray());
        } else {
            m_widget->fromJson(obj.value(key).toObject());
        }
    }
}

void DataWidgetControllerExtendedTableWidget::putValueTo(
    QJsonObject &obj, const QString &key) const {
    if (m_widget->jsonMode() == ExtendedTableWidget::JsonMode::List) {
        obj[key] = m_widget->toJsonArray();
    } else {
        obj[key] = m_widget->toJsonObject();
    }
}

bool DataWidgetControllerDataWidgetInterface::hasAcceptableValue() const {
    return m_widget->isEnabled() && m_widget->hasEntries();
}

void DataWidgetControllerDataWidgetInterface::setValueFrom(
    const QJsonObject &obj, const QString &key) {
    if (obj.contains(key)) {
        m_widget->setJson(obj.value(key).toArray());
    }
}

void DataWidgetControllerDataWidgetInterface::putValueTo(
    QJsonObject &obj, const QString &key) const {
    obj[key] = m_widget->json();
}

StringIndexMap::StringIndexMap(std::initializer_list<std::pair<QString,
                                                               int> > list) {
    const int maxIndex = std::max_element(list.begin(), list.end())->second;

    m_indexToString.reserve(maxIndex + 1);

    for (const auto &[str, index]: list) {
        m_stringToIndex[str]   = index;
        m_indexToString[index] = str;
    }
}

void StringIndexMap::insert(const QString &str, const int index) {
    m_stringToIndex.insert(str, index);
    if (index <= m_indexToString.size()) {
        m_indexToString.resize(index + 1);
    }
    m_indexToString[index] = str;
}

int StringIndexMap::indexOf(const QString &str) const {
    const auto it = m_stringToIndex.constFind(str);

    return (it != m_stringToIndex.cend()) ? it.value() : -1;
}

QString StringIndexMap::stringOf(const int index) const {
    return (index > 0 && index < m_indexToString.size())
               ? m_indexToString.at(index) : QString();
}

bool DataWidgetControllerMultiPageWidget::hasAcceptableValue() const {
    return DataWidgetControllerWidget<MultiPageWidget>::hasAcceptableValue() &&
           (m_widget->count() > 0);
}

void DataWidgetControllerMultiPageWidget::setValueFrom(const QJsonObject &obj,
                                                       const QString &key) {
    m_caughtChoice.clear();
    if (obj.contains(key) || key.isNull()) {
        const auto &record = key.isNull() ? obj : obj.value(key).toObject();
        if (record.contains(m_choiceKey)) {
            QString &&choice = record.value(m_choiceKey).toString();
            Glhp::removePrefix(choice);
            const int index = m_indexMap.indexOf(choice);
            if (index != -1) {
                m_caughtChoice = choice;
                m_widget->setCurrentIndex(index);
                auto *controller = m_widgetChoices[choice];
                Q_ASSERT(controller != nullptr);
                controller->setValueFrom(record, {});
            }
        }
    }
}

void DataWidgetControllerMultiPageWidget::putValueTo(QJsonObject &obj,
                                                     const QString &key) const {
    QJsonObject  newRecord;
    QJsonObject &record = key.isNull() ? obj : newRecord;

    const QString &choice = m_indexMap.stringOf(m_widget->currentIndex());

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
