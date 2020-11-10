#include "rawjsontextedit.h"
#include "ui_rawjsontextedit.h"

#include <QToolButton>
#include <QSignalBlocker>
#include <QPixmap>
#include <QPalette>
#include <QPainter>
#include <QKeyEvent>
#include <QSizePolicy>
#include <QLineEdit>
#include <QDebug>

RawJsonTextEdit::RawJsonTextEdit(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::RawJsonTextEdit) {
    ui->setupUi(this);

    ui->textEdit->installEventFilter(this);

    initColorMenu();
    QPixmap pixmap(16, 16);
    pixmap.fill(currTextColor);
    ui->colorBtn->setIcon(pixmap);

    connect(ui->boldBtn, &QToolButton::toggled, this,
            &RawJsonTextEdit::setBold);
    connect(ui->italicBtn, &QToolButton::toggled,
            this, &RawJsonTextEdit::setItalic);
    connect(ui->underlineBtn, &QToolButton::toggled,
            this, &RawJsonTextEdit::setUnderline);
    connect(ui->strikeBtn, &QToolButton::toggled,
            this, &RawJsonTextEdit::setStrike);
    connect(ui->colorBtn, &QToolButton::toggled,
            this, &RawJsonTextEdit::colorBtnToggled);
    connect(ui->textEdit, &QTextEdit::cursorPositionChanged,
            this, &RawJsonTextEdit::updateFormatButtons);
}

RawJsonTextEdit::~RawJsonTextEdit() {
    delete ui;
}

QTextEdit* RawJsonTextEdit::getTextEdit() {
    return ui->textEdit;
}

void RawJsonTextEdit::setDarkMode(bool value) {
    QPalette textEditPatt = ui->textEdit->palette();

    textEditPatt.setColor(QPalette::Base, (value) ? Qt::darkGray : Qt::white);
    textEditPatt.setColor(QPalette::Text, (value) ? Qt::white : Qt::black);
    ui->textEdit->setPalette(textEditPatt);

    isDarkMode = value;
}

void RawJsonTextEdit::setOneLine(bool value) {
    ui->textEdit->setVerticalScrollBarPolicy(
        (value) ? Qt::ScrollBarAlwaysOff : Qt::ScrollBarAsNeeded);
    if (value) {
        QLineEdit *lineEdit = new QLineEdit();
        ui->textEdit->setFixedHeight(lineEdit->sizeHint().height());
    } else {
        ui->textEdit->setMaximumHeight(0);
    }

    auto sizePolicy = ui->textEdit->sizePolicy();
    sizePolicy.setVerticalPolicy(
        (value) ? QSizePolicy::Fixed : QSizePolicy::Expanding);
    ui->textEdit->setSizePolicy(sizePolicy);

    ui->textEdit->adjustSize();

    isOneLine = value;
}

void RawJsonTextEdit::setBold(bool bold) {
    ui->textEdit->setFontWeight((bold) ? QFont::Bold : QFont::Normal);
}

void RawJsonTextEdit::setItalic(bool italic) {
    ui->textEdit->setFontItalic(italic);
    ui->textEdit->setFocus();
}

void RawJsonTextEdit::setUnderline(bool underline) {
    ui->textEdit->setFontUnderline(underline);
    ui->textEdit->setFocus();
}

void RawJsonTextEdit::setStrike(bool strike) {
    QTextCharFormat fmt;

    fmt.setFontStrikeOut(strike);
    mergeCurrentFormat(fmt);
    ui->textEdit->setFocus();
}

void RawJsonTextEdit::setColor(QColor color) {
    ui->textEdit->setTextColor(color);
    QPixmap pixmap(16, 16);
    pixmap.fill(color);
    ui->colorBtn->setIcon(pixmap);
}

void RawJsonTextEdit::colorBtnToggled(bool checked) {
    if (checked) {
        setColor(currTextColor);
    } else {
        auto fmt = ui->textEdit->currentCharFormat();
        fmt.clearForeground();
        ui->textEdit->setCurrentCharFormat(fmt);
    }
    ui->textEdit->setFocus();
}

void RawJsonTextEdit::mergeCurrentFormat(const QTextCharFormat &format) {
    QTextCursor cursor = ui->textEdit->textCursor();

    cursor.mergeCharFormat(format);
    ui->textEdit->mergeCurrentCharFormat(format);
}

bool RawJsonTextEdit::eventFilter(QObject *obj, QEvent *event) {
    if (isOneLine) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *key = static_cast<QKeyEvent*>(event);
            if ((key->key() == Qt::Key_Enter) ||
                (key->key() == Qt::Key_Return)) {
                /* */
            } else {
                return QObject::eventFilter(obj, event);
            }
            return true;
        } else {
            return QObject::eventFilter(obj, event);
        }
    } else {
        return QObject::eventFilter(obj, event);
    }
    return false;
}

void RawJsonTextEdit::updateFormatButtons() {
    auto cursor = ui->textEdit->textCursor();
    auto fmt    = cursor.charFormat();
    {
        const QSignalBlocker blocker1(ui->boldBtn);
        ui->boldBtn->setChecked(fmt.fontWeight() >= 75);
        const QSignalBlocker blocker2(ui->italicBtn);
        ui->italicBtn->setChecked(fmt.fontItalic());
        const QSignalBlocker blocker3(ui->underlineBtn);
        ui->underlineBtn->setChecked(fmt.fontUnderline());
        const QSignalBlocker blocker4(ui->strikeBtn);
        ui->strikeBtn->setChecked(fmt.fontStrikeOut());
    }
}

void RawJsonTextEdit::initColorMenu() {
    QString colorHexes[] = {
        "#000000", "#0000AA", "#00AA00", "#00AAAA", "#AA0000",
        "#AA00AA", "#FFAA00", "#AAAAAA", "#555555", "#5555FF",
        "#55FF55", "#55FFFF", "#FF5555", "#FF55FF", "#FFFF55", "#FFFFFF" };

    for (auto colorCode : colorHexes) {
        QPixmap pixmap(16, 16);
        pixmap.fill(colorCode);
        colorMenu.addAction(pixmap, "", this, [ = ]() {
            currTextColor = QColor(colorCode);
            if (ui->colorBtn->isChecked()) {
                colorBtnToggled(true);
            } else {
                ui->colorBtn->setChecked(true);
            }
        });
    }
    ui->colorBtn->setMenu(&colorMenu);
}
