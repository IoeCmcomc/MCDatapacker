#include "findandreplacedock.h"
#include "ui_findandreplacedock.h"

#include "platforms/windows_specific.h"
#include "globalhelpers.h"

#include <QAction>
#include <QTextStream>
#include <QDirIterator>
#include <QRegularExpression>
#include <QDebug>
#include <QDirIterator>
#include <QProgressDialog>
#include <QTextBlock>
#include <QSettings>
#include <QPlainTextDocumentLayout>

constexpr int ResultLineRole = Qt::UserRole + 10;
constexpr int ResultColRole  = Qt::UserRole + 11;


FindAndReplaceDock::FindAndReplaceDock(QWidget *parent)
    : QDockWidget(parent), ui(new Ui::FindAndReplaceDock) {
    ui->setupUi(this);

    QAction *replaceAction = new QAction(tr("Replace only"), this);
    ui->replaceBtn->addAction(replaceAction);

    m_textDoc = new QTextDocument(this);
    QPlainTextDocumentLayout *layout = new QPlainTextDocumentLayout(m_textDoc);
    m_textDoc->setDocumentLayout(layout);
    m_textDoc->setUndoRedoEnabled(false);

    connect(ui->foldBtn, &QToolButton::clicked,
            this, &FindAndReplaceDock::onFoldBtnClicked);
    connect(ui->findEdit, &QLineEdit::textChanged,
            this, &FindAndReplaceDock::onQueryChanged);
    connect(ui->findEdit, &QLineEdit::returnPressed,
            this, &FindAndReplaceDock::onFindBtnClicked);
    connect(ui->findBtn, &QPushButton::clicked,
            this, &FindAndReplaceDock::onFindBtnClicked);
    connect(ui->replaceEdit, &QLineEdit::returnPressed,
            this, &FindAndReplaceDock::onReplaceBtnClicked);
    connect(ui->replaceBtn, &QPushButton::clicked,
            this, &FindAndReplaceDock::onReplaceBtnClicked);
    connect(replaceAction, &QAction::triggered,
            this, &FindAndReplaceDock::onReplaceOnly);
    connect(ui->replaceAllBtn, &QPushButton::clicked,
            this, &FindAndReplaceDock::onReplaceAll);
    connect(ui->findAllBtn, &QPushButton::clicked,
            this, &FindAndReplaceDock::onFindAllClicked);
    connect(ui->clearResultsBtn, &QPushButton::clicked,
            this, &FindAndReplaceDock::clearResults);
    connect(ui->packRadio, &QRadioButton::clicked,
            this, &FindAndReplaceDock::updateButtonStates);
    connect(ui->fileRadio, &QRadioButton::clicked,
            this, &FindAndReplaceDock::updateButtonStates);
    connect(ui->findReplaceCheck, &QCheckBox::toggled,
            ui->replaceFrame, &QFrame::setVisible);
    connect(ui->findReplaceCheck, &QCheckBox::toggled,
            this, &FindAndReplaceDock::updateButtonStates);
    connect(ui->resultsTree, &QTreeWidget::itemDoubleClicked,
            this, &FindAndReplaceDock::onResultsTreeDoubleClicked);

    connect(this, &QDockWidget::topLevelChanged, this, [this](bool) {
        Windows::setDarkFrameIfDarkMode(this);
        widget()->adjustSize();
        adjustSize();
    });

    setOptions({});
    ui->replaceFrame->setVisible(ui->findReplaceCheck->isChecked());
    updateButtonStates();
    clearResults();

    setFloating(true);
    widget()->adjustSize();
    adjustSize();
}

FindAndReplaceDock::~FindAndReplaceDock() {
    if (m_dirIter) {
        delete m_dirIter;
    }
    delete ui;
}

FindAndReplaceDock::Options FindAndReplaceDock::options() const {
    Options options;

    if (ui->matchCaseCheck->isChecked()) {
        options |= Option::MatchCase;
    }
    if (ui->matchWholeCheck->isChecked()) {
        options |= Option::FindWholeWord;
    }
    if (ui->regexCheck->isChecked()) {
        options |= Option::UseRegex;
    }
    if (ui->warpCheck->isChecked()) {
        options |= Option::WarpAround;
    }
    if (ui->findReplaceCheck->isChecked()) {
        options |= Option::FindAndReplace;
    }
    if (ui->packRadio->isChecked()) {
        options |= Option::FindInDatapack;
    }

    return options;
}

void FindAndReplaceDock::setOptions(Options options) {
    ui->matchCaseCheck->setChecked(options & Option::MatchCase);
    ui->matchWholeCheck->setChecked(options & Option::FindWholeWord);
    ui->regexCheck->setChecked(options & Option::UseRegex);
    ui->warpCheck->setChecked(options & Option::WarpAround);
    ui->findReplaceCheck->setChecked(options & Option::FindAndReplace);
    if (options & Option::FindInDatapack) {
        ui->packRadio->setChecked(true);
    } else {
        ui->fileRadio->setChecked(true);
    }
}

void FindAndReplaceDock::setQuery(const QString &text) {
    ui->findEdit->setText(text);
}

void FindAndReplaceDock::focusToInput() {
    activateWindow();
    raise();
    ui->findEdit->setFocus(Qt::ActiveWindowFocusReason);
}

bool FindAndReplaceDock::findCursor(const QTextDocument *doc,
                                    QTextCursor &cursor,
                                    const QString &text,
                                    FindAndReplaceDock::Options options) {
    Q_ASSERT(doc != nullptr);

    QTextDocument::FindFlags flags;

    const bool matchCase = options &
                           FindAndReplaceDock::Option::MatchCase;
    const bool useRegex = options &
                          FindAndReplaceDock::Option::UseRegex;
    const bool warpAround = options &
                            FindAndReplaceDock::Option::WarpAround;

    if (options & FindAndReplaceDock::Option::FindWholeWord) {
        flags |= QTextDocument::FindWholeWords;
    }

    QRegularExpression regex{ text };
    QTextCursor        matchCursor;
    if (!useRegex) {
        if (matchCase) {
            flags |= QTextDocument::FindCaseSensitively;
        }
        matchCursor = doc->find(text, cursor, flags);
    } else {
        if (matchCase) {
            regex.setPatternOptions(
                regex.patternOptions() | QRegularExpression::
                CaseInsensitiveOption);
        }
        matchCursor = doc->find(regex, cursor, flags);
    }

    if (matchCursor.isNull()) {
        if (warpAround) {
            cursor.movePosition(QTextCursor::Start);
            matchCursor = useRegex ? doc->find(regex, cursor, flags)
                                   : doc->find(text, cursor, flags);
        }
    }
    if (matchCursor.isNull()) {
        return false;
    } else {
        cursor.setPosition(matchCursor.anchor());
        cursor.setPosition(matchCursor.position(), QTextCursor::KeepAnchor);
        return true;
    }
}

void FindAndReplaceDock::onFindCurFileCompleted(const bool found) {
    m_continueFile = found;
}

void FindAndReplaceDock::changeEvent(QEvent *e) {
    QDockWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange: {
            ui->retranslateUi(this);
            ui->resultsBox->setTitle(tr("Search results (%1)").arg(m_matchCount));
            break;
        }
        default:
            break;
    }
}

void FindAndReplaceDock::closeEvent(QCloseEvent *e) {
    clearResults();
    QDockWidget::closeEvent(e);
}

void FindAndReplaceDock::onQueryChanged(const QString &text) {
    m_continue     = false;
    m_continueFile = false;
    updateButtonStates();
}

void FindAndReplaceDock::onFoldBtnClicked(const bool checked) {
    if (checked) {
        ui->moreFrame->show();
        ui->foldBtn->setArrowType(Qt::UpArrow);
    } else {
        ui->moreFrame->hide();
        ui->foldBtn->setArrowType(Qt::DownArrow);
    }
    widget()->adjustSize();
    adjustSize();
}

void FindAndReplaceDock::onFindBtnClicked() {
    if (ui->findEdit->text().isEmpty()) {
        return;
    }
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    if (ui->fileRadio->isChecked()) {
        emit findCurFileRequested(ui->findEdit->text(), options());
    } else {
        if (!m_continue) {
            if (m_dirIter) {
                delete m_dirIter;
            }
            m_dirIter = new QDirIterator(QDir::currentPath(),
                                         QDir::Files | QDir::NoDotAndDotDot,
                                         QDirIterator::Subdirectories);
        }
        if (m_continueFile) {
            emit findCurFileRequested(ui->findEdit->text(), options());
        }
        if (!m_continueFile) {
            while (m_dirIter->hasNext()) {
                const QString &path = m_dirIter->next();
                if (findInFile(path)) {
                    m_continue = true;
#ifndef QT_NO_CURSOR
                    QGuiApplication::restoreOverrideCursor();
#endif
                    return;
                }
            }
            m_continueFile = false;
        }
    }
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
}

void FindAndReplaceDock::onFindAllClicked() {
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    clearResults();

    const QString &&dirPath = QDir::currentPath();
    QDirIterator    it(dirPath, QDir::Files | QDir::NoDotAndDotDot,
                       QDirIterator::Subdirectories);
    Options         opts        = options();
    const QString &&query       = ui->findEdit->text();
    const int       previewSize = 10;

    QSettings settings;
    QFont     monoFont =
        qvariant_cast<QFont>(settings.value("editor/textFont",
                                            QFontDatabase::systemFont(
                                                QFontDatabase::FixedFont)));
    // monoFont.setStyleHint(QFont::Monospace);
    // monoFont.setFixedPitch(true);
    QFont boldMonoFont = monoFont;
    boldMonoFont.setBold(true);

    m_matchCount = 0;
    while (it.hasNext()) {
        const QString &&path    = it.next();
        const QString &&relPath = Glhp::relPath(dirPath, path);

        const auto &&finfo = it.fileInfo();
        if (finfo.isFile()) {
            QString text;

            if (QFile file(path); file.open(QFile::ReadOnly)) {
                QTextStream in(&file);
                in.setCodec("UTF-8");

                while (!in.atEnd()) {
                    text += in.readLine();
                    if (!in.atEnd())
                        text += '\n';
                }
            }

            if (!text.isNull()) {
                m_textDoc->setPlainText(text);
                QTreeWidgetItem *fileItem = nullptr;

                QTextCursor cursor(m_textDoc);

                while (findCursor(m_textDoc, cursor, query, opts)) {
                    if (!fileItem) {
                        fileItem =
                            new QTreeWidgetItem(ui->resultsTree, { relPath });
                        fileItem->setFirstColumnSpanned(true);
                    }
                    auto *matchItem = new QTreeWidgetItem(fileItem);
                    matchItem->setFont(0, monoFont);
                    matchItem->setFont(1, monoFont);
                    matchItem->setFont(2, boldMonoFont);
                    matchItem->setFont(3, monoFont);

                    // matchItem->setText(1, cursor.block().text());
                    const QString &matchStr = cursor.selectedText();
                    const QString &line     = cursor.block().text();
                    const int      length   = matchStr.length();
                    const int      lineNo   = cursor.blockNumber();
                    const int      pos      = cursor.positionInBlock() - length;


                    matchItem->setText(0, QString::number(lineNo));
                    matchItem->setData(0, ResultLineRole, lineNo);
                    matchItem->setData(0, ResultColRole, pos);

                    const int leftPreviewStart =
                        std::max(0, pos - previewSize);
                    const int leftPreviewLength = std::min(previewSize, pos);

                    matchItem->setText(1, line.mid(leftPreviewStart,
                                                   leftPreviewLength));
                    matchItem->setTextAlignment(1, Qt::AlignRight);
                    matchItem->setText(2, line.mid(pos, length));
                    matchItem->setTextAlignment(2, Qt::AlignCenter);
                    matchItem->setText(3, line.mid(pos + length, previewSize));

                    ++m_matchCount;
                }
            }
        }
    }
    ui->resultsBox->setTitle(tr("Search results (%1)").arg(m_matchCount));
    ui->resultsTree->expandAll();
    for (int i = 0; i <= 3; ++i) {
        ui->resultsTree->resizeColumnToContents(i);
    }
    focusToInput();

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
}

void FindAndReplaceDock::onReplaceBtnClicked() {
    emit replaceCurFileRequested(ui->replaceEdit->text());

    onFindBtnClicked();
}

void FindAndReplaceDock::onReplaceOnly() {
    emit replaceCurFileRequested(ui->replaceEdit->text());
}

void FindAndReplaceDock::onReplaceAll() {
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    emit replaceAllCurFileRequested(ui->findEdit->text(),
                                    ui->replaceEdit->text(),
                                    options());
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
}

void FindAndReplaceDock::clearResults() {
    ui->resultsTree->clear();
    m_matchCount = 0;
    ui->resultsBox->setTitle(tr("Search results (%1)").arg(m_matchCount));
}

void FindAndReplaceDock::updateButtonStates() {
    const QString &&query = ui->findEdit->text();

    ui->findBtn->setDisabled(query.isEmpty());
    ui->replaceBtn->setDisabled(query.isEmpty());
    ui->findAllBtn->setEnabled(ui->packRadio->isChecked() && !query.isEmpty());
    ui->resultsBox->setEnabled(ui->packRadio->isChecked());
    ui->clearResultsBtn->setEnabled(ui->packRadio->isChecked());
    ui->replaceAllBtn->setEnabled(ui->findReplaceCheck->isChecked()
                                  && ui->fileRadio->isChecked()
                                  && !query.isEmpty());
}

void FindAndReplaceDock::onResultsTreeDoubleClicked(QTreeWidgetItem *item) {
    if (item->parent()) {
        const QString &&filePath  = item->parent()->text(0);
        const int       lineNo    = item->data(0, ResultLineRole).toInt();
        const int       colNo     = item->data(0, ResultColRole).toInt();
        const int       selLength = item->text(2).length();
        emit            openFileRequested(filePath, lineNo, colNo, selLength);
    }
}

bool FindAndReplaceDock::findInFile(const QString &path) {
    QString text;

    if (QFile file(path); file.open(QFile::ReadOnly)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");

        while (!in.atEnd()) {
            text += in.readLine();
            if (!in.atEnd())
                text += '\n';
        }
    }

    if (!text.isEmpty()) {
        Options         opts  = options();
        const QString &&query = ui->findEdit->text();

        if (mayHasMatchInText(text, query, opts)) {
            if (opts & Option::FindWholeWord) {
                m_textDoc->setPlainText(text);
                QTextCursor cursor{ m_textDoc };
                if (!findCursor(m_textDoc, cursor, query, opts)) {
                    return false;
                }
            }
            showMatchInEditor(path, query);
            return true;
        }
    }
    return false;
}

void FindAndReplaceDock::showMatchInEditor(const QString &path,
                                           const QString &query) {
    emit openFileRequested(path);
    emit resetCursorRequested();
    emit findCurFileRequested(query, options());
}

/*!
 * Quickly determine whether there may be a match of query in the text or not.
 * Compared to QTextDocument::find, it doesn't support "whole word" mode,
 * but it searchs faster.
 */
bool FindAndReplaceDock::mayHasMatchInText(const QString &text,
                                           const QString &query,
                                           Options options) {
    if (!text.isEmpty() && !query.isEmpty()) {
        int index = 0;
        if (options & Option::UseRegex) {
            QRegularExpression regex{ text };
            if (!(options & Option::MatchCase)) {
                regex.setPatternOptions(
                    regex.patternOptions() |
                    QRegularExpression::CaseInsensitiveOption);
            }
            index = text.indexOf(regex);
        } else {
            const auto caseSense =
                (options & Option::MatchCase) ? Qt::CaseSensitive
                                              : Qt::CaseInsensitive;
            index = text.indexOf(query, caseSense);
        }
        return index != -1;
    }
    return false;
}
