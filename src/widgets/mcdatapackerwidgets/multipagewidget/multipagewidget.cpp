/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QComboBox>
#include <QFormLayout>
#include <QStackedWidget>
#include <QFrame>
#include <QLabel>

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

void MultiPageWidget::insertPage(int index, QWidget *page) {
    page->setParent(stackWidget);

    stackWidget->insertWidget(index, page);

    QString title = page->windowTitle();
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

QWidget* MultiPageWidget::widget(int index) {
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
