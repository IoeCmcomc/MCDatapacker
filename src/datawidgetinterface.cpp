#include "datawidgetinterface.h"
#include "ui_datawidgetinterface.h"

#include "stylesheetreapplier.h"

#include <QJsonObject>
#include <QMouseEvent>
#include <QShortcut>
#include <QPropertyAnimation>

DataWidgetInterface::DataWidgetInterface(QWidget *parent) :
    QFrame(parent), ui(new Ui::DataWidgetInterface) {
    ui->setupUi(this);
    ui->scrollAreaWidgetContents->setLayout(&m_layout);
    ui->moveUpBtn->hide();
    ui->moveDownBtn->hide();

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
    ui->scrollArea->verticalScrollBar()->setStyleSheet(QStringLiteral(
                                                           "QScrollBar{width:0px;}"));
    ui->scrollArea->verticalScrollBar()->installEventFilter(styleSheetReapplier);

    m_sidebarRect = QRect(ui->sidebar->pos(), ui->sidebar->size());
    m_animation   = new QPropertyAnimation(ui->sidebar, "maximumWidth");
    m_animation->setEasingCurve(QEasingCurve::InCubic);

    updateStates();
}

DataWidgetInterface::~DataWidgetInterface() {
    delete ui;
    delete m_animation;
}

void DataWidgetInterface::setMainWidget(QWidget *widget) {
    widget->setParent(this);
    widget->setMouseTracking(true);
    const auto &&children = widget->findChildren<QWidget *>(
        QString(), Qt::FindDirectChildrenOnly);
    for (auto *child : children) {
        child->setMouseTracking(true);
    }

    /*widget->sizeHint().rheight() = widget->minimumHeight(); */
    m_layout.addWidget(widget, 0);
    m_mainWidget = widget;

    updateStates();
    adjustSize();
}

QWidget * DataWidgetInterface::mainWidget() const {
    return m_mainWidget;
}

void DataWidgetInterface::addAfterCurrent() {
/*
      qDebug() << "addAfterCurrent" << ui->scrollBar->maximum() <<
          (ui->scrollBar->maximum() + ui->scrollBar->pageStep() -
           (m_json.isEmpty()));
 */
    const int computedMax = ui->scrollBar->maximum() +
                            ui->scrollBar->pageStep() -
                            (m_json.isEmpty());

    ui->scrollBar->setMaximum(computedMax);
    /*qDebug() << ui->scrollBar->maximum() << m_json << m_currentIndex; */
    m_json.insert(m_currentIndex + 1, QJsonObject());
    ui->scrollBar->setValue((m_currentIndex + 1) * ui->scrollBar->pageStep());
    onSliderMoved(ui->scrollBar->value());
    /*emit setterCallRequested({}); */
    loadData(m_currentIndex);

    emit entriesCountChanged(m_json.size());
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

    emit entriesCountChanged(m_json.size());
}

void DataWidgetInterface::mouseMoveEvent(QMouseEvent *e) {
    QWidget::mouseMoveEvent(e);

/*
      qDebug() << "DataWidgetInterface::mouseMoveEvent" << m_sidebarRect <<
          e->pos() << m_sidebarRect.contains(e->pos());
 */

    if (m_sidebarRect.contains(e->pos()))
        showSidebar();
    else
        hideSidebar();
}

void DataWidgetInterface::enterEvent(QEvent *e) {
    QWidget::enterEvent(e);
    if (const auto *event = static_cast<QEnterEvent *>(e)) {
        if (!m_sidebarRect.contains(event->pos()))
            hideSidebar();
    }

    /*qDebug() << "DataWidgetInterface::enterEvent" << e; */
}

void DataWidgetInterface::leaveEvent(QEvent *e) {
    QWidget::leaveEvent(e);
    showSidebar();
    /*qDebug() << "DataWidgetInterface::leaveEvent" << e; */
}

void DataWidgetInterface::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);

    m_animation->stop();
    ui->sidebar->setMaximumWidth(m_sidebarRect.width());

    /*m_sidebarRect.setTopLeft(ui->sidebar->pos()); */
    m_sidebarRect =
        QRect(ui->sidebar->pos(),
              QSize(m_sidebarRect.size().width(), ui->sidebar->height()));

    /*qDebug() << "DataWidgetInterface::resizeEvent" << m_sidebarRect; */
}

void DataWidgetInterface::changeEvent(QEvent *e) {
    QFrame::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateStates();
    }
}

void DataWidgetInterface::onSliderMoved(int value) {
    auto *areaScrollbar = ui->scrollArea->verticalScrollBar();

    /*qDebug() << "onSliderMoved" << value; */
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

    if (areaScrollbar->maximum() <= 0)
        return;

    const int computedValue =
        m_currentIndex * ui->scrollBar->pageStep() + value *
        (ui->scrollBar->pageStep() / ui->scrollBar->singleStep()) /
        areaScrollbar->maximum();

    /*qDebug() << "onScrollAreaScrolled" << value << computedValue; */
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

void DataWidgetInterface::sidebarAnimFinished() {
    if (m_sidebarHiding) {
        m_sidebarHiding = false;
        ui->sidebar->hide();
    }
}

void DataWidgetInterface::loadData(int index) {
    if (index < m_json.size() && index >= 0) {
        /* Call the mapped setter function with the specified data */
        emit setterCallRequested(m_json.at(index));
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
    ui->scrollBar->setDisabled(arrEmpty);
    if (m_mainWidget) {
        setEnabled(true);
        m_mainWidget->setDisabled(arrEmpty);
    } else {
        setDisabled(true);
    }

    ui->label->setText(QString("%1/%2").arg(m_currentIndex +
                                            1).arg(m_json.size()));
}

void DataWidgetInterface::showSidebar() {
    /*qDebug() << "DataWidgetInterface::showSidebar"; */
    show();

    m_animation->stop();
    m_animation->setDuration(m_sidebarSlideTime);
    m_animation->setStartValue(ui->sidebar->width());
    m_animation->setEndValue(m_sidebarRect.width());
    m_animation->start();

    m_sidebarHiding = false;
}

void DataWidgetInterface::hideSidebar() {
    /*qDebug() << "DataWidgetInterface::hideSidebar"; */
    m_animation->stop();
    m_animation->setDuration(m_sidebarSlideTime);
    m_animation->setStartValue(ui->sidebar->width());
    m_animation->setEndValue(0);
    m_animation->start();
    m_sidebarHiding = true;
}

QJsonArray DataWidgetInterface::json() {
    saveData(m_currentIndex);
    return m_json;
}

void DataWidgetInterface::setJson(const QJsonArray &json) {
    m_json = json;

    m_currentIndex = m_json.size() - 1;
    const int computedMax = ui->scrollBar->pageStep() * m_json.size() -
                            ((m_json.size() > 0) * 2 - 1);
    ui->scrollBar->setMaximum(computedMax);
    if (!m_json.isEmpty()) {
        ui->scrollBar->setValue((m_json.size() - 1) *
                                ui->scrollBar->pageStep());
        onSliderMoved(ui->scrollBar->value());
        loadData(m_currentIndex);
    } else {
        updateStates();
    }

    emit entriesCountChanged(m_json.size());
}

int DataWidgetInterface::getCurrentIndex() const {
    return m_currentIndex;
}

void DataWidgetInterface::setCurrentIndex(int currentIndex) {
    /*qDebug() << "setCurrentIndex" << currentIndex; */
    saveData(m_currentIndex);
    loadData(currentIndex);
    m_currentIndex = currentIndex;
    updateStates();
}
