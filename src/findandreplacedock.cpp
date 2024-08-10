#include "findandreplacedock.h"
#include "ui_findandreplacedock.h"

#include "platforms/windows_specific.h"

#include <QAction>
#include <QTextStream>
#include <QDirIterator>
#include <QRegularExpression>
#include <QDebug>


FindAndReplaceDock::FindAndReplaceDock(QWidget *parent)
    : QDockWidget(parent), ui(new Ui::FindAndReplaceDock) {
    ui->setupUi(this);

    QAction *replaceAllAction = new QAction(tr("Replace all"), this);
    ui->replaceBtn->addAction(replaceAllAction);

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
    connect(replaceAllAction, &QAction::triggered,
            this, &FindAndReplaceDock::onReplaceAll);
    connect(ui->findReplaceRadio, &QRadioButton::toggled,
            ui->replaceFrame, &QFrame::setVisible);

    connect(this, &QDockWidget::topLevelChanged, this, [this](bool) {
        Windows::setDarkFrameIfDarkMode(this);
        widget()->adjustSize();
        adjustSize();
    });

    setOptions({});

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
    if (ui->findReplaceRadio->isChecked()) {
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
    if (options & Option::FindAndReplace) {
        ui->findReplaceRadio->setChecked(true);
    } else {
        ui->findRadio->setChecked(true);
    }
    if (options & Option::FindInDatapack) {
        ui->packRadio->setChecked(true);
    } else {
        ui->fileRadio->setChecked(true);
    }
}

void FindAndReplaceDock::setQuery(const QString &text) {
    ui->findEdit->setText(text);
}

void FindAndReplaceDock::onFindCurFileCompleted(const bool found) {
    m_continueFile = found;
}

void FindAndReplaceDock::changeEvent(QEvent *e) {
    QDockWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void FindAndReplaceDock::onQueryChanged(const QString &text) {
    m_continue     = false;
    m_continueFile = false;
    ui->findBtn->setDisabled(text.isEmpty());
    ui->replaceBtn->setDisabled(text.isEmpty());
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
                                         { "*.mcfunction", "*.json", "*.nbt" },
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

void FindAndReplaceDock::onReplaceBtnClicked() {
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
        const QString &&query = ui->findEdit->text();
        if (ui->regexCheck->isChecked()) {
            QRegularExpression regex{ text };
            if (!ui->matchCaseCheck->isChecked()) {
                regex.setPatternOptions(
                    regex.patternOptions() |
                    QRegularExpression::CaseInsensitiveOption);
            }
            if (text.indexOf(regex) != -1) {
                showMatchInEditor(path, query);
                return true;
            }
        } else {
            const auto caseSense =
                ui->matchCaseCheck->isChecked() ? Qt::CaseSensitive : Qt::
                CaseInsensitive;
            if (text.indexOf(query, caseSense) != -1) {
                showMatchInEditor(path, query);
                return true;
            }
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
