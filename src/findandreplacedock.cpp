#include "findandreplacedock.h"
#include "ui_findandreplacedock.h"

#include "platforms/windows_specific.h"

#include <QAction>


FindAndReplaceDock::FindAndReplaceDock(QWidget *parent)
    : QDockWidget(parent)
    , ui(new Ui::FindAndReplaceDock) {
    ui->setupUi(this);

    QAction *replaceAllAction = new QAction(tr("Replace all"), this);
    ui->replaceBtn->addAction(replaceAllAction);

    connect(ui->foldBtn, &QToolButton::clicked,
            this, &FindAndReplaceDock::onFoldBtnClicked);
    connect(ui->findEdit, &QLineEdit::textChanged, this,
            [ = ](const QString &text){
        ui->findBtn->setDisabled(text.isEmpty());
        ui->replaceBtn->setDisabled(text.isEmpty());
    });
    connect(ui->findBtn, &QPushButton::clicked,
            this, &FindAndReplaceDock::onFindBtnClicked);
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
    emit findCurFileRequested(ui->findEdit->text(), options());
}

void FindAndReplaceDock::onReplaceBtnClicked() {
    emit replaceCurFileRequested(ui->replaceEdit->text());
}

void FindAndReplaceDock::onReplaceAll() {
    emit replaceAllCurFileRequested(ui->findEdit->text(),
                                    ui->replaceEdit->text(),
                                    options());
}
