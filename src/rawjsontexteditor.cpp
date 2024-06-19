#include "rawjsontexteditor.h"
#include "ui_rawjsontexteditor.h"

#include "rawjsontextobjectinterface.h"
#include "game.h"
#include "globalhelpers.h"

#include <QToolButton>
#include <QSignalBlocker>
#include <QPixmap>
#include <QPalette>
#include <QKeyEvent>
#include <QSizePolicy>
#include <QLineEdit>
#include <QJsonArray>
#include <QTextBlock>
#include <QShortcut>
#include <QColorDialog>
#include <QJsonDocument>


RawJsonTextEditor::RawJsonTextEditor(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::RawJsonTextEditor) {
    ui->setupUi(this);

    ui->textEdit->installEventFilter(this);

    initColorMenu();
    QPixmap pixmap(16, 16);
    pixmap.fill(currTextColor);
    ui->colorBtn->setIcon(pixmap);

    connect(ui->boldBtn, &QToolButton::clicked, this,
            &RawJsonTextEditor::setBold);
    connect(ui->italicBtn, &QToolButton::clicked,
            this, &RawJsonTextEditor::setItalic);
    connect(ui->underlineBtn, &QToolButton::clicked,
            this, &RawJsonTextEditor::setUnderline);
    connect(ui->strikeBtn, &QToolButton::clicked,
            this, &RawJsonTextEditor::setStrike);
    connect(ui->obfuscatedBtn, &QToolButton::clicked,
            this, &RawJsonTextEditor::setObfuscated);
    connect(ui->colorBtn, &QToolButton::clicked,
            this, &RawJsonTextEditor::colorBtnToggled);

    connect(ui->translatedTextBtn, &QToolButton::clicked, this, [this](){
        this->insertObjectBtnClicked(TextObject::Translate);
    });
    connect(ui->scoreboardBtn, &QToolButton::clicked, this, [this](){
        this->insertObjectBtnClicked(TextObject::Scoreboard);
    });
    connect(ui->entityNamesBtn, &QToolButton::clicked, this, [this](){
        this->insertObjectBtnClicked(TextObject::EntityNames);
    });
    connect(ui->keyBindBtn, &QToolButton::clicked, this, [this](){
        this->insertObjectBtnClicked(TextObject::Keybind);
    });
    connect(ui->snbtBtn, &QToolButton::clicked, this, [this](){
        this->insertObjectBtnClicked(TextObject::Nbt);
    });

    connect(ui->textEdit, &QTextEdit::cursorPositionChanged,
            this, &RawJsonTextEditor::updateFormatButtons);
    connect(ui->textEdit, &RawJsonTextEdit::textObjectSelected,
            this, &RawJsonTextEditor::updateFormatButtons);
    connect(ui->tabWidget, &QTabWidget::currentChanged,
            this, &RawJsonTextEditor::updateEditors);
    connect(ui->sourceFormatCombo,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this, &RawJsonTextEditor::writeSourceEditor);
}

RawJsonTextEditor::~RawJsonTextEditor() {
    delete ui;
}

QTextEdit * RawJsonTextEditor::getTextEdit() {
    return ui->textEdit;
}

bool RawJsonTextEditor::isEmpty() const {
    return ui->textEdit->document()->isEmpty();
}

void RawJsonTextEditor::setDarkMode(bool value) {
    QPalette textEditPatt = ui->textEdit->palette();

    textEditPatt.setColor(QPalette::Base, (value) ? Qt::darkGray : Qt::white);
    textEditPatt.setColor(QPalette::Text, (value) ? Qt::white : Qt::black);
    ui->textEdit->setPalette(textEditPatt);

    isDarkMode = value;
}

void RawJsonTextEditor::setOneLine(bool value) {
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

void RawJsonTextEditor::mergeObjectComponent(QJsonObject &component,
                                             const QTextCharFormat &fmt) const {
    using Property = RawJsonProperty;

    switch (fmt.objectType()) {
        case RawJsonTextEdit::Translate: {
            component.insert("translate",
                             fmt.stringProperty(Property::TranslateKey));
            insertNonEmptyProp<QJsonArray>(component, "with", fmt,
                                           TranslateArgs);
            if (Game::version() >= Game::v1_19_4) {
                insertNonEmptyProp<QString>(component, "fallback", fmt,
                                            Property::TranslateFallback);
            }
            if (Game::version() >= Game::v1_20_4) {
                component.insert(QStringLiteral("type"),
                                 QStringLiteral("translatable"));
            }
            break;
        }
        case RawJsonTextEdit::Scoreboard: {
            QJsonObject score{
                { "objective",
                  fmt.stringProperty(Property::ScoreboardObjective) },
                { "name", fmt.stringProperty(Property::ScoreboardName) },
            };
            insertNonEmptyProp<QString>(score, "value", fmt, ScoreboardValue);
            component.insert("score", score);
            if (Game::version() >= Game::v1_20_4) {
                component.insert(QStringLiteral("type"),
                                 QStringLiteral("score"));
            }
            break;
        }
        case RawJsonTextEdit::EntityNames: {
            component.insert("selector",
                             fmt.stringProperty(Property::Selector));
            if (Game::version() >= Game::v1_17) {
                insertNonEmptyProp<QJsonValue>(component, "separator",
                                               fmt, Property::Separator);
            }
            if (Game::version() >= Game::v1_20_4) {
                component.insert(QStringLiteral("type"),
                                 QStringLiteral("selector"));
            }
            break;
        }
        case RawJsonTextEdit::Keybind: {
            component.insert("keybind", fmt.stringProperty(Property::Keybind));
            if (Game::version() >= Game::v1_20_4) {
                component.insert(QStringLiteral("type"),
                                 QStringLiteral("keybind"));
            }
            break;
        }
        case RawJsonTextEdit::Nbt: {
            component.insert("nbt", fmt.stringProperty(Property::NbtPath));
            insertNonEmptyProp<QString>(component, "block", fmt, NbtBlock);
            insertNonEmptyProp<QString>(component, "entity", fmt, NbtEntity);
            insertNonEmptyProp<QString>(component, "storage", fmt, NbtStorage);
            insertNonEmptyProp<bool>(component, "interpret", fmt, NbtInterpret);
            if (Game::version() >= Game::v1_17) {
                insertNonEmptyProp<QJsonValue>(component, "separator",
                                               fmt, Property::Separator);
            }
            if (Game::version() >= Game::v1_20_4) {
                component.insert(QStringLiteral("type"), QStringLiteral("nbt"));
            }
            break;
        }
    }
}

void RawJsonTextEditor::appendBlockToArray(QJsonArray &arr,
                                           const QTextBlock &block) const {
    QTextBlock::iterator it;

    for (it = block.begin(); !(it.atEnd()); ++it) {
        QTextFragment currFragment = it.fragment();
        if (currFragment.isValid()) {
            QString txt = currFragment.text();
            auto    fmt = currFragment.charFormat();

            QJsonObject component;

            if (txt == QChar::ObjectReplacementCharacter) {
                mergeObjectComponent(component, fmt);
            } else {
                component.insert(QLatin1String("text"), txt);
                if (Game::version() >= Game::v1_20_4) {
                    component.insert(QStringLiteral("type"),
                                     QStringLiteral("text"));
                }
            }
            if (fmt.fontWeight() >= 75)
                component.insert(QLatin1String("bold"), true);
            if (fmt.fontItalic())
                component.insert(QLatin1String("italic"), true);
            if (fmt.fontUnderline())
                component.insert(QLatin1String("underlined"), true);
            if (fmt.boolProperty(RawJsonProperty::TextObfuscated))
                component.insert(QLatin1String("obfuscated"), true);
            if (fmt.fontStrikeOut())
                component.insert(QLatin1String("strikethrough"), true);
            if (fmt.foreground().style() == Qt::SolidPattern) {
                const QString &&key = Glhp::colorHexes.key(
                    fmt.foreground().color().name());
                if (!key.isEmpty()) {
                    component.insert(QLatin1String("color"), key);
                } else if (Game::version() >= Game::v1_16) {
                    component.insert(QLatin1String("color"),
                                     fmt.foreground().color().name());
                }
            }

            arr << component;
        }
    }
}

QJsonValue RawJsonTextEditor::toJson() const {
    const QTextDocument *doc = ui->textEdit->document();

    if (!doc->isModified()) {
        return m_json;
    }

    QJsonArray arr{ QString() };

    for (QTextBlock currBlock = doc->begin(); currBlock != doc->end();
         currBlock = currBlock.next()) {
        if (currBlock != doc->begin())
            arr << QJsonObject({ { "text", "\n" } });

        appendBlockToArray(arr, currBlock);
    }

    switch (arr.size()) {
        case 0:
        case 1: { // No text fragment
            return QJsonObject();
        }
        case 2: { // One text fragment
            return arr[1];
        }
        default: { // Multiple text fragments
            return arr;
        }
    }
}

void RawJsonTextEditor::fromJson(const QJsonValue &root) {
    m_json = root;
    ui->textEdit->clear();
    appendJsonObject(JsonToComponent(root));
}

QJsonArray RawJsonTextEditor::toJsonObjects() const {
    const QTextDocument *doc = ui->textEdit->document();

    if (!doc->isModified()) {
        return m_json.toArray();
    }

    QJsonArray arr{};

    for (QTextBlock currBlock = doc->begin(); currBlock != doc->end();
         currBlock = currBlock.next()) {
        QJsonArray line{ QString() };
        appendBlockToArray(line, currBlock);
        arr << line;
    }

    return arr;
}

void RawJsonTextEditor::fromJsonObjects(const QJsonArray &arr) {
    m_json = arr;
    ui->textEdit->clear();
    bool isFirst = true;
    for (const auto &&line: arr) {
        if (isFirst) {
            isFirst = !isFirst;
        } else {
            appendJsonObject({ { QStringLiteral("text"), QStringLiteral(
                                     "\n") } });
        }
        appendJsonObject(JsonToComponent(line));
    }
}

QString RawJsonTextEditor::toFormattingCodes(const QChar prefix) const {
    QString              codes;
    const QTextDocument *doc = ui->textEdit->document();

    for (QTextBlock block = doc->begin(); block != doc->end();
         block = block.next()) {
        if (block != doc->begin())
            codes += '\n';

        QTextBlock::iterator it;

        for (it = block.begin(); !(it.atEnd()); ++it) {
            QTextFragment currFragment = it.fragment();
            if (currFragment.isValid()) {
                const QString txt = currFragment.text();
                const auto    fmt = currFragment.charFormat();
                QString       formatCodes;
                QString       colorCode;

                if (fmt.fontWeight() >= 75)
                    formatCodes += prefix + 'l';
                if (fmt.fontItalic())
                    formatCodes += prefix + 'o';
                if (fmt.fontUnderline())
                    formatCodes += prefix + 'n';
                if (fmt.boolProperty(RawJsonProperty::TextObfuscated))
                    formatCodes += prefix + 'k';
                if (fmt.fontStrikeOut())
                    formatCodes += prefix + 'm';
                if (fmt.foreground().style() == Qt::SolidPattern) {
                    const char key = Glhp::colorCodes.key(
                        fmt.foreground().color().name(), '\0');
                    if (key != '\0') {
                        colorCode = prefix + key;
                    }
                } else {
                    colorCode = prefix + 'r';
                }

                if (txt != QChar::ObjectReplacementCharacter) {
                    codes += colorCode;
                    codes += formatCodes;
                    codes += txt;
                }
            }
        }
    }
    return codes;
}

void RawJsonTextEditor::fromFormattingCodes(const QString &codes,
                                            const QChar prefix) {
    enum class TokenType {
        Char,
        Sign,
        Code,
    };

    m_json = QJsonValue{};
    ui->textEdit->clear();
    bool            isFirst = true;
    QTextCharFormat prevFmt;
    const auto    &&lines = codes.splitRef('\n');

    auto cursor = ui->textEdit->textCursor();
    cursor.beginEditBlock();
    for (const auto &line: lines) {
        if (isFirst) {
            isFirst = !isFirst;
        } else {
            cursor.insertText(QStringLiteral("\n"));
        }

        QTextCharFormat fmt;

        TokenType tokenType = TokenType::Char;
        for (const auto &ch: line) {
            if (tokenType == TokenType::Sign) {
                switch (ch.toLower().toLatin1()) {
                    case 'l': {
                        fmt.setFontWeight(QFont::Bold);
                        tokenType = TokenType::Code;
                        break;
                    }
                    case 'o': {
                        fmt.setFontItalic(true);
                        tokenType = TokenType::Code;
                        break;
                    }
                    case 'n': {
                        fmt.setFontUnderline(true);
                        tokenType = TokenType::Code;
                        break;
                    }
                    case 'k': {
                        fmt.setProperty(RawJsonProperty::TextObfuscated, true);
                        tokenType = TokenType::Code;
                        break;
                    }
                    case 'm': {
                        fmt.setFontStrikeOut(true);
                        tokenType = TokenType::Code;
                        break;
                    }
                    case 'r': {
                        fmt       = QTextCharFormat{};
                        prevFmt   = QTextCharFormat{};
                        tokenType = TokenType::Code;
                        break;
                    }
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                    case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f': {
                        fmt     = QTextCharFormat{};
                        prevFmt = QTextCharFormat{};
                        fmt.setForeground(
                            QBrush(QColor(Glhp::colorCodes.value(
                                              ch.toLower().toLatin1()))));
                        tokenType = TokenType::Code;
                        break;
                    }
                }
            }

            switch (tokenType) {
                case TokenType::Sign: {
                    if (ch == prefix) {
                        cursor.setCharFormat(fmt);
                        cursor.insertText(prefix);
                        cursor.insertText(ch);
                        tokenType = TokenType::Char;
                    }
                    break;
                }
                case TokenType::Code: {
                    fmt.merge(prevFmt);
                    cursor.setCharFormat(fmt);
                    prevFmt   = fmt;
                    tokenType = TokenType::Char;
                    break;
                }
                case TokenType::Char: {
                    if (ch == prefix) {
                        tokenType = TokenType::Sign;
                    } else {
                        cursor.insertText(ch);
                        tokenType = TokenType::Char;
                    }
                    break;
                }
            }
        }
    }

    cursor.endEditBlock();
    ui->textEdit->setTextCursor(cursor);
}

QJsonObject RawJsonTextEditor::JsonToComponent(const QJsonValue &root) {
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

void RawJsonTextEditor::appendJsonObject(const QJsonObject &root,
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
    if (root.contains(QLatin1String("obfuscated")))
        fmt.setProperty(RawJsonProperty::TextObfuscated,
                        root.value(QLatin1String("obfuscated")).toBool());
    if (root.contains(QLatin1String("color"))) {
        const QString &&color = Glhp::colorHexes.value(root.value(
                                                           QLatin1String(
                                                               "color")).toString());
        if (!color.isEmpty()) {
            fmt.setForeground(QBrush(QColor(color)));
        } else if (Game::version() >= Game::v1_16) {
            static QRegularExpression regex(R"(#[0-9a-fA-F]{6})");
            const auto              &&match
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
    } else if (root.contains(QLatin1String("translate"))) {
        auto objFmt = fmt;
        objFmt.setObjectType(TextObject::Translate);
        objFmt.setProperty(RawJsonProperty::TranslateKey,
                           root.value(QLatin1String("translate")).toString());
        objFmt.setProperty(RawJsonProperty::TranslateArgs,
                           root.value(QLatin1String("with")).toArray());
        if (Game::version() >= Game::v1_19_4) {
            objFmt.setProperty(RawJsonProperty::TranslateFallback,
                               root.value(QLatin1String("fallback")).toString());
        }
        cursor.insertText(QString(QChar::ObjectReplacementCharacter), objFmt);
        cursor.setCharFormat(fmt);
    } else if (root.contains(QLatin1String("score"))) {
        const auto &score =
            root.value(QLatin1String("score")).toObject();
        auto objFmt = fmt;
        objFmt.setObjectType(TextObject::Scoreboard);
        if (score.contains("value")) {
            objFmt.setProperty(RawJsonProperty::ScoreboardValue,
                               score.value(QLatin1String("value")).toString());
        }
        objFmt.setProperty(RawJsonProperty::ScoreboardName,
                           score.value(QLatin1String("name")).toString());
        objFmt.setProperty(RawJsonProperty::ScoreboardObjective,
                           score.value(QLatin1String("objective")).toString());

        cursor.insertText(QString(QChar::ObjectReplacementCharacter), objFmt);
        cursor.setCharFormat(fmt);
    } else if (root.contains(QLatin1String("selector"))) {
        auto objFmt = fmt;
        objFmt.setObjectType(TextObject::EntityNames);
        objFmt.setProperty(RawJsonProperty::Selector,
                           root.value(QLatin1String("selector")).toString());
        if (Game::version() >= Game::v1_17) {
            objFmt.setProperty(RawJsonProperty::Separator,
                               root.value(QLatin1String("separator")));
        }
        cursor.insertText(QString(QChar::ObjectReplacementCharacter), objFmt);
        cursor.setCharFormat(fmt);
    } else if (root.contains(QLatin1String("keybind"))) {
        auto objFmt = fmt;
        objFmt.setObjectType(TextObject::Keybind);
        objFmt.setProperty(RawJsonProperty::Keybind,
                           root.value(QLatin1String("keybind")).toString());
        cursor.insertText(QString(QChar::ObjectReplacementCharacter), objFmt);
        cursor.setCharFormat(fmt);
    } else if (root.contains(QLatin1String("nbt"))) {
        auto objFmt = fmt;
        objFmt.setObjectType(TextObject::Nbt);
        objFmt.setProperty(RawJsonProperty::NbtPath,
                           root.value(QLatin1String("nbt")).toString());
        objFmt.setProperty(RawJsonProperty::NbtBlock,
                           root.value(QLatin1String("block")).toString());
        objFmt.setProperty(RawJsonProperty::NbtEntity,
                           root.value(QLatin1String("entity")).toString());
        objFmt.setProperty(RawJsonProperty::NbtStorage,
                           root.value(QLatin1String("storage")).toString());
        if (root.contains("interpret")) {
            objFmt.setProperty(RawJsonProperty::NbtInterpret,
                               root.value(QLatin1String("interpret")).toBool());
        }
        if (Game::version() >= Game::v1_17) {
            objFmt.setProperty(RawJsonProperty::Separator,
                               root.value(QLatin1String("separator")));
        }
        cursor.insertText(QString(QChar::ObjectReplacementCharacter), objFmt);
        cursor.setCharFormat(fmt);
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


void RawJsonTextEditor::setBold(bool bold) {
    ui->textEdit->setFontWeight((bold) ? QFont::Bold : QFont::Normal);
    ui->textEdit->setFocus();
}

void RawJsonTextEditor::setItalic(bool italic) {
    ui->textEdit->setFontItalic(italic);
    ui->textEdit->setFocus();
}

void RawJsonTextEditor::setUnderline(bool underline) {
    ui->textEdit->setFontUnderline(underline);
    ui->textEdit->setFocus();
}

void RawJsonTextEditor::setStrike(bool strike) {
    QTextCharFormat fmt;

    fmt.setFontStrikeOut(strike);
    mergeCurrentFormat(fmt);
    ui->textEdit->setFocus();
}

void RawJsonTextEditor::setObfuscated(bool obfuscated) {
    QTextCharFormat fmt;

    fmt.setProperty(RawJsonProperty::TextObfuscated, obfuscated);
    mergeCurrentFormat(fmt);
    ui->textEdit->setFocus();
}

void RawJsonTextEditor::setColorBtnIcon(const QColor &color) {
    QPixmap pixmap(16, 16);

    pixmap.fill(color);
    ui->colorBtn->setIcon(pixmap);
}

void RawJsonTextEditor::setColor(QColor color) {
    ui->textEdit->setTextColor(color);
    setColorBtnIcon(color);
}

void RawJsonTextEditor::colorBtnToggled(bool checked) {
    if (checked) {
        setColor(currTextColor);
    } else {
        auto &&fmt = ui->textEdit->currentCharFormat();
        fmt.clearForeground();
        ui->textEdit->setCurrentCharFormat(std::move(fmt));
    }
    ui->textEdit->setFocus();
}

void RawJsonTextEditor::insertObjectBtnClicked(
    RawJsonTextEdit::TextObject objectId) {
    QTextCursor &&cursor = ui->textEdit->textCursor();

    if (!cursor.hasSelection()) {
        auto &&fmt = ui->textEdit->currentCharFormat();
        fmt.setObjectType(objectId);
        if (ui->textEdit->editTextObject(fmt, cursor)) {
            cursor.insertText(QString(QChar::ObjectReplacementCharacter), fmt);
            ui->textEdit->setTextCursor(cursor);
        }
    } else if (ui->textEdit->isSelectingObject(cursor)) {
        auto &&fmt = cursor.charFormat();
        if (ui->textEdit->editTextObject(fmt, cursor)) {
            ui->textEdit->setCurrentCharFormat(fmt);
        }
    }
}

void RawJsonTextEditor::mergeCurrentFormat(const QTextCharFormat &format) {
    QTextCursor cursor = ui->textEdit->textCursor();

    cursor.mergeCharFormat(format);
    ui->textEdit->mergeCurrentCharFormat(format);
}

bool RawJsonTextEditor::eventFilter(QObject *obj, QEvent *event) {
    if (isOneLine) {
        if (event->type() == QEvent::KeyPress) {
            auto *key = dynamic_cast<QKeyEvent *>(event);
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

void RawJsonTextEditor::changeEvent(QEvent *event) {
    QFrame::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}

void RawJsonTextEditor::checkColorBtn() {
    if (ui->colorBtn->isChecked()) {
        colorBtnToggled(true);
    } else {
        ui->colorBtn->click();
    }
}

void RawJsonTextEditor::updateFormatButtons() {
    const auto  &cursor = ui->textEdit->textCursor();
    const auto &&fmt    = cursor.charFormat();

    ui->boldBtn->setChecked(fmt.fontWeight() >= 75);
    ui->italicBtn->setChecked(fmt.fontItalic());
    ui->underlineBtn->setChecked(fmt.fontUnderline());
    ui->strikeBtn->setChecked(fmt.fontStrikeOut());
    ui->obfuscatedBtn->setChecked(fmt.boolProperty(RawJsonProperty::
                                                   TextObfuscated));
    if (fmt.foreground().style() == Qt::SolidPattern) {
        ui->colorBtn->setChecked(true);
        setColorBtnIcon(fmt.foreground().color());
    } else {
        ui->colorBtn->setChecked(false);
    }

    const bool isntObject = !ui->textEdit->isSelectingObject(cursor);
    ui->translatedTextBtn->setEnabled(
        isntObject || fmt.objectType() == TextObject::Translate);
    ui->scoreboardBtn->setEnabled(
        isntObject || fmt.objectType() == TextObject::Scoreboard);
    ui->entityNamesBtn->setEnabled(
        isntObject || fmt.objectType() == TextObject::EntityNames);
    ui->keyBindBtn->setEnabled(
        isntObject || fmt.objectType() == TextObject::Keybind);
    ui->snbtBtn->setEnabled(
        isntObject || fmt.objectType() == TextObject::Nbt);
}

void RawJsonTextEditor::selectCustomColor() {
    const QColor &&color = QColorDialog::getColor(Qt::green, this);

    if (color.isValid()) {
        currTextColor = color;
        checkColorBtn();
    }
}

void RawJsonTextEditor::updateEditors(int tabIndex) {
    if (tabIndex == 0) { /* "Edit" tab */
        readSourceEditor(ui->sourceFormatCombo->currentIndex());
    } else {             /* "Source" tab */
        writeSourceEditor(ui->sourceFormatCombo->currentIndex());
    }
}

void RawJsonTextEditor::readSourceEditor(int format) {
    switch (format) {
        case JSON:
        case InlineJSON: {
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
        case FormattingCodes: {
            fromFormattingCodes(ui->sourceEdit->toPlainText());
            break;
        }
        case AmpFormattingCodes: {
            fromFormattingCodes(ui->sourceEdit->toPlainText(), u'&');
            break;
        }
    }
}

void RawJsonTextEditor::writeSourceEditor(int format) {
    switch (format) {
        case JSON:
        case InlineJSON: {
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
            if (format == JSON) {
                ui->sourceEdit->setPlainText(jsonDoc.toJson());
            } else {
                ui->sourceEdit->setPlainText(jsonDoc.toJson(
                                                 QJsonDocument::Compact));
            }
            break;
        }

        case HTML: {
            ui->sourceEdit->setPlainText(ui->textEdit->toHtml());
            break;
        }
        case FormattingCodes: {
            ui->sourceEdit->setPlainText(toFormattingCodes());
            break;
        }
        case AmpFormattingCodes: {
            ui->sourceEdit->setPlainText(toFormattingCodes(u'&'));
            break;
        }
    }
}

void RawJsonTextEditor::initColorMenu() {
    for (auto it = Glhp::colorHexes.cbegin(); it != Glhp::colorHexes.cend();
         ++it) {
        const auto &colorCode = it.value();
        QPixmap     pixmap(16, 16);
        pixmap.fill(colorCode);
        colorMenu.addAction(pixmap, it.key(), this, [ this, colorCode ]() {
            currTextColor = QColor(colorCode);
            checkColorBtn();
        });
    }
    if (Game::version() >= Game::v1_16) {
        colorMenu.addAction(tr("Select color..."), this,
                            &RawJsonTextEditor::selectCustomColor);
    }

    ui->colorBtn->setMenu(&colorMenu);
}
