#include "truefalsebox.h"

TrueFalseBox::TrueFalseBox(QWidget *parent) : QCheckBox(parent) {
    setCheckState(Qt::PartiallyChecked);
}

void TrueFalseBox::unset() {
    setCheckState(Qt::PartiallyChecked);
}

void TrueFalseBox::insertToJsonObject(QJsonObject &obj, const QString &key) {
    if (checkState() != Qt::PartiallyChecked)
        obj.insert(key, isChecked());
}

void TrueFalseBox::setupFromJsonObject(const QJsonObject &obj,
                                       const QString &key) {
    if (obj.contains(key))
        setChecked(obj[key].toBool());
    else
        setCheckState(Qt::PartiallyChecked);
}

void TrueFalseBox::nextCheckState() {
    if (isTristate()) {
        switch (checkState()) {
        case Qt::PartiallyChecked:
            setCheckState(Qt::Checked);
            break;

        case Qt::Checked:
            setCheckState(Qt::Unchecked);
            break;

        case Qt::Unchecked:
            setCheckState(Qt::PartiallyChecked);
            break;
        }
    } else {
        QCheckBox::nextCheckState();
    }
}
