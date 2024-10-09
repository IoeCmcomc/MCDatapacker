#include "idtagselector.h"
#include "ui_idtagselector.h"

#include "idtagselectordialog.h"

#include <QJsonObject>
#include <QDebug>
#include <QVector>
#include <QJsonArray>
#include <QMouseEvent>
#include <QLineEdit>
#include <QStringListModel>
#include <QCompleter>
#include <QListView>
#include <QStandardItemModel>
#include <QRegularExpressionValidator>

Q_GLOBAL_STATIC(QStringListModel, staticEmptyModel);

static void setComboRowHidden(QComboBox *comboBox, const int row, bool hidden) {
    if (auto *view = qobject_cast<QListView *>(comboBox->view())) {
        auto *model = static_cast<QStandardItemModel *>(comboBox->model());
        Q_ASSERT(model != nullptr);
        view->setRowHidden(row, hidden);
        model->item(row, 0)->setEnabled(!hidden);
    }
}

IdTagSelector::IdTagSelector(QWidget *parent) :
    QFrame(parent), ui(new Ui::IdTagSelector), m_idModel{staticEmptyModel},
    m_tagModel{staticEmptyModel} {
    ui->setupUi(this);
    ui->typeCombo->setObjectName(QStringLiteral("__qt__passive_comboBox"));
    ui->comboBox->completer()->setFilterMode(Qt::MatchContains);
    ui->comboBox->completer()->setCompletionMode(QCompleter::PopupCompletion);
    ui->comboBox->setValidator(
        new QRegularExpressionValidator(QRegularExpression{
        QStringLiteral(R"([a-z0-9-_.]+:[a-z0-9-_.\/]+)") },
                                        ui->comboBox));
    setModes(Id | Tag | ListOfIds);
    setIdDisplayName(tr("ID"));


    connect(ui->dataBtn->button(), &QPushButton::clicked,
            this, &IdTagSelector::dataBtnClicked);
    connect(ui->typeCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &IdTagSelector::onTypeComboIndexChanged);
    connect(ui->comboBox, &QComboBox::editTextChanged,
            this, &IdTagSelector::onTextEdited);
}

IdTagSelector::~IdTagSelector() {
    delete ui;
}

void IdTagSelector::mouseReleaseEvent(QMouseEvent *event) {
    const auto *cursorWidget = qApp->widgetAt(mapToGlobal(event->pos()));

    if (cursorWidget != nullptr
        && qobject_cast<const QLineEdit *>(cursorWidget) == nullptr) {
        emit editingFinished();
        return;
    }
    QFrame::mouseReleaseEvent(event);
}

void IdTagSelector::dataBtnClicked() {
    auto *dialog = new IdTagSelectorDialog(this);

    dialog->setIdDisplayName(m_idDisplayName);
    dialog->setIdModel(m_idModel);
    dialog->fromJson(ui->dataBtn->data().toJsonArray());
    if (dialog->exec()) {
        ui->dataBtn->setData(dialog->toJson());
    }
}

void IdTagSelector::onTypeComboIndexChanged(const int index) {
    ui->typeCombo->blockSignals(true);
    switch (index) {
        case 0: {
            setCurrentMode(Id);
            break;
        }
        case 1: {
            setCurrentMode(Tag);
            break;
        }
        case 2: {
            setCurrentMode(ListOfIds);
            break;
        }
    }
    ui->typeCombo->blockSignals(false);
}

void IdTagSelector::onTextEdited(const QString &text) {
    const int i = ui->comboBox->findData(text, Qt::EditRole,
                                         Qt::MatchFixedString);

    if (i != -1) {
        qDebug() << text << i;
        ui->comboBox->setCurrentIndex(i);
    }
}

QString IdTagSelector::idDisplayName() const {
    return m_idDisplayName;
}

void IdTagSelector::setIdDisplayName(const QString &newIdDisplayName) {
    m_idDisplayName = newIdDisplayName;

    emit idDisplayNameChanged(newIdDisplayName);
}

void IdTagSelector::setIdModel(QAbstractListModel *model) {
    m_idModel = model;
    if (m_currentMode == Id) {
        ui->comboBox->setModel(model);
    }
    ui->comboBox->setEditText({});
}

void IdTagSelector::setTagModel(QAbstractListModel *model) {
    m_tagModel = model;
    if (m_currentMode == Tag) {
        ui->comboBox->setModel(model);
    }
    ui->comboBox->setEditText({});
}

bool IdTagSelector::hasData() const {
    if (m_currentMode == ListOfIds) {
        return !ui->dataBtn->data().isNull();
    } else {
        return !ui->comboBox->currentText().isEmpty();
    }
}

QString IdTagSelector::id() const {
    return (m_currentMode == Id) ? ui->comboBox->currentText() : QString();
}

void IdTagSelector::setId(const QString &id) {
    setCurrentMode(Id);
    ui->comboBox->setEditText(id);
}

QString IdTagSelector::tag() const {
    return (m_currentMode == Tag) ? ui->comboBox->currentText() : QString();
}

void IdTagSelector::setTag(const QString &tag) {
    setCurrentMode(Tag);
    ui->comboBox->setEditText(tag);
}

QJsonArray IdTagSelector::ids() const {
    if (m_currentMode == ListOfIds) {
        return ui->dataBtn->data().toJsonArray();
    } else {
        return {};
    }
}

void IdTagSelector::setIds(const QJsonArray &ids) {
    setCurrentMode(ListOfIds);
    ui->dataBtn->setData(ids);
}

void IdTagSelector::fromJson(const QJsonValue &value) {
    ui->comboBox->setEditText({});

    if (value.isArray()) {
        setCurrentMode(ListOfIds);
        ui->dataBtn->setData(value.toArray());
    } else if (value.isString()) {
        QString &&str = value.toString();
        if (str.startsWith('#')) {
            str.remove('#');
            setCurrentMode(Tag);
        } else {
            setCurrentMode(Id);
        }

        ui->comboBox->setEditText(str);
    }
}

QJsonValue IdTagSelector::toJson() {
    QJsonValue value;

    switch (currentMode()) {
        case Id: {
            return ui->comboBox->currentText();
        }
        case Tag: {
            return ui->comboBox->currentText().isEmpty()
                       ? QString() : QString('#' + ui->comboBox->currentText());
        }
        case ListOfIds: {
            return ui->dataBtn->data().toJsonArray();
        }
        default: {
            Q_UNREACHABLE();
            return {};
        }
    }
}

IdTagSelector::Modes IdTagSelector::getModes() const {
    return m_modes;
}

void IdTagSelector::setModes(const IdTagSelector::Modes &value) {
    m_modes = value;
    if (!m_modes.testFlag(Id)) {
        ui->stackedWidget->setCurrentIndex(0);
        if (!m_modes.testFlag(Tag))
            ui->stackedWidget->setCurrentIndex(1);
    }

    ui->typeCombo->setHidden(value != 0 && (value & (value - 1)) == 0);
    setComboRowHidden(ui->typeCombo, 0, !value.testFlag(Id));
    setComboRowHidden(ui->typeCombo, 1, !value.testFlag(Tag));
    setComboRowHidden(ui->typeCombo, 2, !value.testFlag(ListOfIds));
    emit modesChanged(m_modes);
}

DialogDataButton * IdTagSelector::dataBtn() {
    return ui->dataBtn;
}

IdTagSelector::Mode IdTagSelector::currentMode() const {
    return m_currentMode;
}

void IdTagSelector::setCurrentMode(const Mode &value) {
    if (!(m_modes & value)) {
        return;
    }

    m_currentMode = value;
    switch (value) {
        case Id: {
            ui->typeCombo->setCurrentIndex(0);
            ui->stackedWidget->setCurrentIndex(0);
            ui->comboBox->setModel(m_idModel);
            ui->comboBox->completer()->setModel(m_idModel);
            break;
        }
        case Tag: {
            ui->typeCombo->setCurrentIndex(1);
            ui->stackedWidget->setCurrentIndex(0);
            ui->comboBox->setModel(m_tagModel);
            ui->comboBox->completer()->setModel(m_tagModel);
            break;
        }
        case ListOfIds: {
            ui->typeCombo->setCurrentIndex(2);
            ui->stackedWidget->setCurrentIndex(1);
            break;
        }
    }

    emit currentModeChanged(value);
}
