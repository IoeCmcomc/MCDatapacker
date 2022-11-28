#include "rawjsontextedit.h"
#include "ui_rawjsontextedit.h"

#include "globalhelpers.h"
#include "game.h"

#include <QToolButton>
#include <QSignalBlocker>
#include <QPixmap>
#include <QPalette>
#include <QPainter>
#include <QKeyEvent>
#include <QSizePolicy>
#include <QLineEdit>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextBlock>
#include <QShortcut>
#include <QColorDialog>
#include <QJsonDocument>


RawJsonTextEdit::RawJsonTextEdit(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::RawJsonTextEdit) {
    ui->setupUi(this);

    ui->textEdit->installEventFilter(this);
    QFont font;
    /*
       QTextMarkdownWriter doesn't output other formattings with monospaced fonts.
       Therefore, normal font will be used instead.
     */
    /* font.setFixedPitch(true); */
    /* font.setStyleHint(QFont::Monospace); */
    font.setPointSize(10);
    ui->textEdit->setFont(std::move(font));

    initColorMenu();
    QPixmap pixmap(16, 16);
    pixmap.fill(currTextColor);
    ui->colorBtn->setIcon(pixmap);

    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_B), this, nullptr,
                          nullptr, Qt::WidgetWithChildrenShortcut),
            &QShortcut::activated, this, [this]() {
        ui->boldBtn->toggle();
    });
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_I), this, nullptr,
                          nullptr, Qt::WidgetWithChildrenShortcut),
            &QShortcut::activated, this, [this]() {
        ui->italicBtn->toggle();
    });
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_U), this, nullptr,
                          nullptr, Qt::WidgetWithChildrenShortcut),
            &QShortcut::activated, this, [this]() {
        ui->underlineBtn->toggle();
    });
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_K), this, nullptr,
                          nullptr, Qt::WidgetWithChildrenShortcut),
            &QShortcut::activated, this, [this]() {
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
    connect(ui->tabWidget, &QTabWidget::currentChanged,
            this, &RawJsonTextEdit::updateEditors);
    connect(ui->sourceFormatCombo,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this, &RawJsonTextEdit::writeSourceEditor);
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
        QLineEdit lineEdit(this);
        ui->textEdit->setFixedHeight(lineEdit.height());
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
    if (!ui->textEdit->document()->isModified()) {
        return m_json;
    }

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
                    component.insert(QLatin1String("bold"), true);
                if (fmt.fontItalic())
                    component.insert(QLatin1String("italic"), true);
                if (fmt.fontUnderline())
                    component.insert(QLatin1String("underlined"), true);
                if (fmt.fontStrikeOut())
                    component.insert(QLatin1String("strikethrough"), true);
                if (fmt.foreground().style() == Qt::SolidPattern) {
                    const QString &&key = Glhp::colorHexes.key(
                        fmt.foreground().color().name());
                    if (!key.isEmpty()) {
                        component.insert(QLatin1String("color"), key);
                    } else if (Game::version() >=
                               Game::v1_16) {
                        component.insert(QLatin1String("color"),
                                         fmt.foreground().color().name());
                    }
                }

                arr << component;
            }
        }
    }

    return arr;
}

void RawJsonTextEdit::fromJson(const QJsonValue &root) {
    m_json = root;
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
        component.insert(QLatin1String("text"),
                         (root.toBool()) ? QStringLiteral(
                             "true") : QStringLiteral("false"));
        break;
    }

    case QJsonValue::Double: {
        component.insert(QLatin1String("text"),
                         QString::number(root.toDouble()));
        break;
    }

    case QJsonValue::String: {
        component.insert(QLatin1String("text"), root.toString());
        break;
    }

    case QJsonValue::Array: {
        auto arr = root.toArray();
        if (arr.count() > 0) {
            component = JsonToComponent(arr[0]);
            if (arr.count() > 1) {
                arr.removeAt(0);
                component.insert(QLatin1String("extra"), arr);
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

    if (root.contains(QLatin1String("bold")))
        fmt.setFontWeight((root.value(
                               QLatin1String(
                                   "bold")).toBool()) ? QFont::Bold : QFont::Normal);
    if (root.contains(QLatin1String("italic")))
        fmt.setFontItalic(root.value(QLatin1String("italic")).toBool());
    if (root.contains(QLatin1String("underlined")))
        fmt.setFontUnderline(root.value(QLatin1String("underlined")).toBool());
    if (root.contains(QLatin1String("strikethrough")))
        fmt.setFontStrikeOut(root.value(QLatin1String("strikethrough")).toBool());
    if (root.contains(QLatin1String("color"))) {
        const QString &&color = Glhp::colorHexes.value(root.value(
                                                           QLatin1String(
                                                               "color")).toString());
        if (!color.isEmpty()) {
            fmt.setForeground(QBrush(QColor(color)));
        } else if (Game::version() >= Game::v1_16) {
            static QRegularExpression regex(R"(#[0-9a-fA-F]{6})");
            const auto       &&match
                = regex.match(root.value(QLatin1String("color")).toString(), 0,
                              QRegularExpression::NormalMatch,
                              QRegularExpression::AnchoredMatchOption);
            if (match.hasMatch()) {
                fmt.setForeground(QBrush(QColor(match.captured())));
            }
        }
    }

    cursor.beginEditBlock();

    if (root.contains(QLatin1String("text"))) {
        cursor.setCharFormat(fmt);
        cursor.insertText(root.value(QLatin1String("text")).toString());
    }

    if (root.contains(QLatin1String("extra"))) {
        auto extra = root.value(QLatin1String("extra")).toArray();
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
        auto &&fmt = ui->textEdit->currentCharFormat();
        fmt.clearForeground();
        ui->textEdit->setCurrentCharFormat(std::move(fmt));
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
                /* Do nothing */
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

void RawJsonTextEdit::checkColorBtn() {
    if (ui->colorBtn->isChecked()) {
        colorBtnToggled(true);
    } else {
        ui->colorBtn->setChecked(true);
    }
}

void RawJsonTextEdit::updateFormatButtons() {
    auto   cursor = ui->textEdit->textCursor();
    auto &&fmt    = cursor.charFormat();
    {
        const QSignalBlocker blocker1(ui->boldBtn);
        ui->boldBtn->setChecked(fmt.fontWeight() >= 75);
        const QSignalBlocker blocker2(ui->italicBtn);
        ui->italicBtn->setChecked(fmt.fontItalic());
        const QSignalBlocker blocker3(ui->underlineBtn);
        ui->underlineBtn->setChecked(fmt.fontUnderline());
        const QSignalBlocker blocker4(ui->strikeBtn);
        ui->strikeBtn->setChecked(fmt.fontStrikeOut());
        const QSignalBlocker blocker5(ui->colorBtn);
        if (fmt.foreground().style() == Qt::SolidPattern) {
            currTextColor = fmt.foreground().color();
            checkColorBtn();
        } else {
            colorBtnToggled(false);
            ui->colorBtn->setChecked(false);
        }
    }
}

void RawJsonTextEdit::selectCustomColor() {
    QColor &&color = QColorDialog::getColor(Qt::green, this);

    if (color.isValid()) {
        currTextColor = color;
        checkColorBtn();
    }
}

void RawJsonTextEdit::updateEditors(int tabIndex) {
    if (tabIndex == 0) { /* "Edit" tab */
        readSourceEditor(ui->sourceFormatCombo->currentIndex());
    } else {             /* "Source" tab */
        writeSourceEditor(ui->sourceFormatCombo->currentIndex());
    }
}

void RawJsonTextEdit::readSourceEditor(int format) {
    switch (format) {
    case JSON: {
        const auto &doc = QJsonDocument::fromJson(
            ui->sourceEdit->toPlainText().toUtf8());
        if (doc.isObject()) {
            fromJson(doc.object());
        } else if (doc.isArray()) {
            fromJson(doc.array());
        } else {
            fromJson(QJsonValue());
        }

        break;
    }

    case HTML: {
        ui->textEdit->setHtml(ui->sourceEdit->toPlainText());
        break;
    }

    case Markdown: {
        ui->textEdit->setMarkdown(ui->sourceEdit->toPlainText());
        break;
    }
    }
}

void RawJsonTextEdit::writeSourceEditor(int format) {
    switch (format) {
    case JSON: {
        const auto  &&json = toJson();
        QJsonDocument jsonDoc;
        switch (json.type()) {
        case QJsonValue::Object: {
            jsonDoc = QJsonDocument(json.toObject());
            break;
        }

        case QJsonValue::Array: {
            jsonDoc = QJsonDocument(json.toArray());
            break;
        }

        default:
            break;
        }
        ui->sourceEdit->setPlainText(jsonDoc.toJson());
        break;
    }

    case HTML: {
        ui->sourceEdit->setPlainText(ui->textEdit->toHtml());
        break;
    }

    case Markdown: {
        ui->sourceEdit->setPlainText(ui->textEdit->toMarkdown(QTextDocument::
                                                              MarkdownDialectCommonMark));
        break;
    }
    }
}

void RawJsonTextEdit::initColorMenu() {
    for (const auto &colorCode : qAsConst(Glhp::colorHexes)) {
        QPixmap pixmap(16, 16);
        pixmap.fill(colorCode);
        colorMenu.addAction(pixmap, QString(), this, [ = ]() {
            currTextColor = QColor(colorCode);
            checkColorBtn();
        });
    }
    if (Game::version() >= Game::v1_16) {
        colorMenu.addAction(tr("Select color..."), this,
                            &RawJsonTextEdit::selectCustomColor);
    }

    ui->colorBtn->setMenu(&colorMenu);
}
