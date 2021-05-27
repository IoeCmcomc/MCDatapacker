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
#include <QJsonArray>
#include <QJsonObject>
#include <QTextBlock>
#include <QShortcut>

RawJsonTextEdit::RawJsonTextEdit(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::RawJsonTextEdit) {
    ui->setupUi(this);

    ui->textEdit->installEventFilter(this);

    initColorMenu();
    QPixmap pixmap(16, 16);
    pixmap.fill(currTextColor);
    ui->colorBtn->setIcon(pixmap);

    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_B), this, nullptr,
                          nullptr, Qt::WidgetWithChildrenShortcut),
            &QShortcut::activated, [this]() {
        ui->boldBtn->toggle();
    });
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_I), this, nullptr,
                          nullptr, Qt::WidgetWithChildrenShortcut),
            &QShortcut::activated, [this]() {
        ui->italicBtn->toggle();
    });
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_U), this, nullptr,
                          nullptr, Qt::WidgetWithChildrenShortcut),
            &QShortcut::activated, [this]() {
        ui->underlineBtn->toggle();
    });
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_K), this, nullptr,
                          nullptr, Qt::WidgetWithChildrenShortcut),
            &QShortcut::activated, [this]() {
        ui->strikeBtn->toggle();
    });

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
        auto *lineEdit = new QLineEdit();
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

QJsonValue RawJsonTextEdit::toJson() const {
    QJsonArray arr = { "" };

    QTextDocument *doc = ui->textEdit->document();

    for (QTextBlock currBlock = doc->begin(); currBlock != doc->end();
         currBlock = currBlock.next()) {
        if (currBlock != doc->begin())
            arr << QJsonObject({ { "text", "\n" } });

        QTextBlock::iterator it;
        for (it = currBlock.begin(); !(it.atEnd()); ++it) {
            QTextFragment currFragment = it.fragment();
            if (currFragment.isValid()) {
                QString txt = currFragment.text();
                auto    fmt = currFragment.charFormat();

                QJsonObject component;

                component.insert("text", txt);
                if (fmt.fontWeight() >= 75)
                    component.insert("bold", true);
                if (fmt.fontItalic())
                    component.insert("italic", true);
                if (fmt.fontUnderline())
                    component.insert("underlined", true);
                if (fmt.fontStrikeOut())
                    component.insert("strikethrough", true);
                if (fmt.foreground().style() == Qt::SolidPattern) {
                    auto key = colorHexes.key(
                        fmt.foreground().color().name());
                    if (!key.isEmpty())
                        component.insert("color", key);
                }

                arr << component;
            }
        }
    }

    return arr;
}

void RawJsonTextEdit::fromJson(const QJsonValue &root) {
    ui->textEdit->clear();
    appendJsonObject(JsonToComponent(root));
}

QJsonObject RawJsonTextEdit::JsonToComponent(const QJsonValue &root) {
    QJsonObject component;

    switch (root.type()) {
    case QJsonValue::Object: {
        return root.toObject();
    }

    case QJsonValue::Bool: {
        component.insert("text", (root.toBool()) ? "true" : "false");
        break;
    }

    case QJsonValue::Double: {
        component.insert("text", QString::number(root.toDouble()));
        break;
    }

    case QJsonValue::String: {
        component.insert("text", root.toString());
        break;
    }

    case QJsonValue::Array: {
        auto arr = root.toArray();
        if (arr.count() > 0) {
            component = JsonToComponent(arr[0]);
            if (arr.count() > 1) {
                arr.removeAt(0);
                component.insert("extra", arr);
            }
        }
        break;
    }

    default: {
        break;
    }
    }
    return component;
}

void RawJsonTextEdit::appendJsonObject(const QJsonObject &root,
                                       const QTextCharFormat &optFmt) {
    auto cursor = ui->textEdit->textCursor();
    auto fmt    = optFmt;

    if (root.contains("bold"))
        fmt.setFontWeight((root.value(
                               "bold").toBool()) ? QFont::Bold : QFont::Normal);
    if (root.contains("italic"))
        fmt.setFontItalic(root.value("italic").toBool());
    if (root.contains("underlined"))
        fmt.setFontUnderline(root.value("underlined").toBool());
    if (root.contains("strikethrough"))
        fmt.setFontStrikeOut(root.value("strikethrough").toBool());
    if (root.contains("color")) {
        QString color = colorHexes.value(root.value("color").toString());
        if (!color.isEmpty()) {
            fmt.setForeground(QBrush(QColor(color)));
        }
    }

    cursor.beginEditBlock();

    if (root.contains("text")) {
        cursor.setCharFormat(fmt);
        cursor.insertText(root.value("text").toString());
    }

    if (root.contains("extra")) {
        auto extra = root.value("extra").toArray();
        for (auto compRef: extra) {
            appendJsonObject(JsonToComponent(compRef), fmt);
        }
    }

    cursor.endEditBlock();
    ui->textEdit->setTextCursor(cursor);
}


void RawJsonTextEdit::setBold(bool bold) {
    ui->textEdit->setFontWeight((bold) ? QFont::Bold : QFont::Normal);
    ui->textEdit->setFocus();
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
            auto *key = dynamic_cast<QKeyEvent*>(event);
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
    for (const auto &colorCode : qAsConst(colorHexes)) {
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
