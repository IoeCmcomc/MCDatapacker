#include "datawidgetinterface.h"
#include "ui_datawidgetinterface.h"

#include <QJsonObject>
#include <QShortcut>

DataWidgetInterface::DataWidgetInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataWidgetInterface) {
    ui->setupUi(this);
    ui->scrollAreaWidgetContents->setLayout(&m_layout);

    connect(ui->addBtn, &QToolButton::clicked,
            this, &DataWidgetInterface::addAfterCurrent);
    connect(ui->removeBtn, &QToolButton::clicked,
            this, &DataWidgetInterface::removeCurrent);
    connect(ui->scrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &DataWidgetInterface::onScrollAreaScrolled);
    connect(ui->scrollBar, &QScrollBar::valueChanged,
            this, &DataWidgetInterface::onSliderMoved);

    connect(new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Up), this),
            &QShortcut::activated, this, &DataWidgetInterface::moveToPrevStep);
    connect(new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Down), this),
            &QShortcut::activated, this, &DataWidgetInterface::moveToNextStep);
    connect(new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_PageUp), this),
            &QShortcut::activated, this, &DataWidgetInterface::moveToPrevPage);
    connect(new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_PageDown), this),
            &QShortcut::activated, this, &DataWidgetInterface::moveToNextPage);

    ui->scrollArea->verticalScrollBar()->hide();
}

DataWidgetInterface::~DataWidgetInterface() {
    delete ui;
}

void DataWidgetInterface::setMainWidget(QWidget *widget) {
    widget->setParent(this);

    widget->sizeHint().rheight() = widget->minimumHeight();
    m_layout.addWidget(widget, 0);
    m_mainWidget = widget;

    addAfterCurrent();
}

void DataWidgetInterface::addAfterCurrent() {
    qDebug() << "addAfterCurrent" << ui->scrollBar->maximum() <<
        (ui->scrollBar->maximum() + ui->scrollBar->pageStep() -
         (m_json.isEmpty()));
    const int computedMax = ui->scrollBar->maximum() +
                            ui->scrollBar->pageStep() -
                            (m_json.isEmpty());
    ui->scrollBar->setMaximum(computedMax);
    qDebug() << ui->scrollBar->maximum() << m_json << m_currentIndex;
    m_json.insert(m_currentIndex + 1, QJsonObject());
    ui->scrollBar->setValue((m_currentIndex + 1) * ui->scrollBar->pageStep());
    onSliderMoved(ui->scrollBar->value());
    emit setterCallRequested({});
}

void DataWidgetInterface::removeCurrent() {
    m_json.removeAt(m_currentIndex);

    ui->scrollBar->setMaximum(
        ui->scrollBar->maximum() - ui->scrollBar->pageStep() +
        (m_json.isEmpty()));
    if (m_currentIndex >= m_json.size()) {
        setCurrentIndex(m_json.size() - 1);
    } else {
        loadData(m_currentIndex);
    }
    updateStates();
}

void DataWidgetInterface::onSliderMoved(int value) {
    const auto areaScrollbar = ui->scrollArea->verticalScrollBar();

    qDebug() << "onSliderMoved" << value;
    /*areaScrollbar->blockSignals(true); */
    if (const int index = value / ui->scrollBar->pageStep();
        index != m_currentIndex) {
        setCurrentIndex(index);
    }

    m_reactToSignal = false;
    if (ui->scrollBar->maximum() != 0) {
        areaScrollbar->setValue(
            (value % ui->scrollBar->pageStep()) * areaScrollbar->maximum() /
            (ui->scrollBar->pageStep() / ui->scrollBar->singleStep()));
    }
    m_reactToSignal = true;
    /*
       areaScrollbar->blockSignals(false);
       areaScrollbar->triggerAction(QScrollBar::SliderMove);
     */
    updateStates();
}

void DataWidgetInterface::onScrollAreaScrolled(int value) {
    const auto areaScrollbar = ui->scrollArea->verticalScrollBar();
    const int  computedValue =
        m_currentIndex * ui->scrollBar->pageStep() + value *
        (ui->scrollBar->pageStep() / ui->scrollBar->singleStep()) /
        areaScrollbar->maximum();

    qDebug() << "onScrollAreaScrolled" << value << computedValue;
    if (m_reactToSignal) {
        ui->scrollBar->blockSignals(true);
        ui->scrollBar->setValue(computedValue);
        ui->scrollBar->blockSignals(false);
    }
}

void DataWidgetInterface::moveToPrevStep() {
    ui->scrollBar->triggerAction(QScrollBar::SliderSingleStepSub);
}

void DataWidgetInterface::moveToNextStep() {
    ui->scrollBar->triggerAction(QScrollBar::SliderSingleStepAdd);
}

void DataWidgetInterface::moveToPrevPage() {
    ui->scrollBar->triggerAction(QScrollBar::SliderPageStepSub);
}

void DataWidgetInterface::moveToNextPage() {
    ui->scrollBar->triggerAction(QScrollBar::SliderPageStepAdd);
}

void DataWidgetInterface::loadData(int index) {
    if (index < m_json.size() && index >= 0 && index != m_currentIndex) {
        /* Call the mapped setter function with the specified data */
        emit setterCallRequested(m_json[index].toObject());
    }
}

void DataWidgetInterface::saveData(int index) {
    if (index < m_json.size() && index >= 0) {
        emit getterCallRequested();
        /* The current index's data should have been set to the mapped getter's return value */
    }
}

void DataWidgetInterface::updateStates() {
    const bool arrEmpty = m_json.isEmpty();

    ui->removeBtn->setDisabled(arrEmpty);
    m_mainWidget->setDisabled(arrEmpty);

    ui->label->setText(QString("%1/%2").arg(m_currentIndex +
                                            1).arg(m_json.size()));
}

QJsonArray DataWidgetInterface::json() const {
    return m_json;
}

void DataWidgetInterface::setJson(const QJsonArray &json) {
    m_json = json;
}

int DataWidgetInterface::getCurrentIndex() const {
    return m_currentIndex;
}

void DataWidgetInterface::setCurrentIndex(int currentIndex) {
    qDebug() << "setCurrentIndex" << currentIndex;
    saveData(m_currentIndex);
    loadData(currentIndex);
    m_currentIndex = currentIndex;
    updateStates();
}
