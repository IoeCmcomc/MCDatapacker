#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"

#include "mainwindow.h"
#include "parsers/command/mcfunctionparser.h"
#include "parsers/command/nodes/macronode.h"
#include "globalhelpers.h"
#include "platforms/windows_specific.h"
#include "game.h"

#include <QDirIterator>
#include <QProgressDialog>
#include <QTableWidgetItem>
#include <QOperatingSystemVersion>
#include <QElapsedTimer>
#include <QTextDocument>
#include <QTextBlock>


StatisticsDialog::StatisticsDialog(MainWindow *parent) :
    QDialog(parent), ui(new Ui::StatisticsDialog) {
    m_mainWin = parent;
    ui->setupUi(this);

    Windows::extendFrame(this);

    ui->packNameLabel->setText(ui->packNameLabel->text().arg(
                                   QDir::current().dirName(),
                                   m_mainWin->getPackInfo().description));

    m_parser = new Command::McfunctionParser();

    QElapsedTimer timer;
    timer.start();
    collectAndSetupData();
    ui->timeLabel->setText(tr("Total processing time: %Ln second(s)",
                              nullptr, timer.elapsed() / 1000.0));
    connect(ui->syntaxErrorTable, &QTableWidget::cellDoubleClicked,
            this, &StatisticsDialog::onErrorTableDoubleClicked);
    setAttribute(Qt::WA_DeleteOnClose, true);
}

StatisticsDialog::~StatisticsDialog() {
    delete m_parser;
    delete ui;
}

void StatisticsDialog::onErrorTableDoubleClicked(int row,
                                                 [[maybe_unused]] int column) {
    const QString &&path = m_dirPath + '/' +
                           ui->syntaxErrorTable->item(row, 0)->data(
        Qt::DisplayRole).toString();

    const int lineNo =
        ui->syntaxErrorTable->item(row, 1)->data(Qt::DisplayRole).toInt() - 1;

    emit openFileWithLineRequested(path, lineNo);

    m_mainWin->activateWindow();
}

void StatisticsDialog::collectAndSetupData() {
    QDir          &&dir     = QDir::current();
    const QString &&dirPath = dir.path();

    m_dirPath = dirPath;

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

    int          entriesCount = 0;
    QDirIterator counterIt(dir, QDirIterator::Subdirectories);
    while (counterIt.hasNext()) {
        counterIt.next();
        ++entriesCount;
    }

    QDirIterator  it(dir, QDirIterator::Subdirectories);
    int           fileCount   = 0;
    int           folderCount = 0;
    QSet<QString> namespaces;

    QMap<CodeFile::FileType, uint> fileTypeCounts;

    hide();
    QProgressDialog progress(QString(), tr("Abort"), 0,
                             entriesCount, this);
    progress.setWindowTitle(tr("Scanning datapack..."));
    /*progress.setMinimumDuration(1000); */
    progress.setWindowModality(Qt::WindowModal);

    progress.resize(800, progress.height());
    const auto hostRect = geometry();
    progress.move(hostRect.center() - progress.rect().center());

    int i = 0;
    while (it.hasNext()) {
        const QString &&path = it.next();
        ++i;

        progress.setValue(i);
        progress.setLabelText(tr("Currently scanning: %1").arg(
                                  Glhp::relPath(dirPath, path)));
        if (progress.wasCanceled())
            break;

        const auto &&finfo = it.fileInfo();
        if (finfo.isFile()) {
            ++fileCount;

            const auto type = Glhp::pathToFileType(dirPath, path);
            if (type == CodeFile::Function)
                collectFunctionData(path);
            if (fileTypeCounts.contains(type))
                ++fileTypeCounts[type];
            else
                fileTypeCounts[type] = 1;
        } else if (finfo.isDir()) {
            ++folderCount;
            const QString &&nspace = Glhp::relNamespace(dirPath, path);
            if (!nspace.isEmpty())
                namespaces += nspace;
        }
    }
    progress.setValue(entriesCount);

    /* "General" tab */

    ui->leftFilesGeneralLabel->setText(ui->leftFilesGeneralLabel->text().arg(
                                           folderCount).arg(fileCount).arg(
                                           namespaces.size()));
    const QString hasTickJson = (QFileInfo::exists(dirPath +
                                                   "/data/minecraft/tags/functions/tick.json"))
        ? tr("Yes") : tr("No");
    const QString hasLoadJson = (QFileInfo::exists(dirPath +
                                                   "/data/minecraft/tags/functions/load.json"))
                                        ? tr("Yes") : tr("No");
    ui->rightFilesGeneralLabel->setText(
        ui->rightFilesGeneralLabel->text().arg(hasTickJson, hasLoadJson));

    ui->fileTypesTable->setRowCount(fileTypeCounts.size());
    int row = 0;
    for (auto it = fileTypeCounts.cbegin(); it != fileTypeCounts.cend();
         ++it) {
        ui->fileTypesTable->setItem(row, 0, new QTableWidgetItem(
                                        Glhp::fileTypeToIcon(it.key()),
                                        Glhp::fileTypeToName(it.key())));
        auto *countItem = new QTableWidgetItem();
        countItem->setData(Qt::DisplayRole, it.value());
        ui->fileTypesTable->setItem(row, 1, countItem);
        auto *percentItem = new QTableWidgetItem();
        if (fileCount > 0) {
            percentItem->setData(
                Qt::DisplayRole,
                qRound(it.value() * 10000.0 / fileCount) / 100.0);
        } else {
            percentItem->setData(Qt::DisplayRole, 0);
        }
        ui->fileTypesTable->setItem(row, 2, percentItem);
        ++row;
    }
    ui->fileTypesTable->resizeColumnsToContents();
    ui->fileTypesTable->setSortingEnabled(true);

    /* "Functions" tab */
    const QString &&macroStr = (Game::version() >= Game::v1_20_2)
                                   ? QString::number(m_macroLines)
                                   : tr("(not in this version)");
    ui->functionLabel->setText(
        ui->functionLabel->text().arg(m_commandLines).arg(m_commentLines).arg(
            macroStr).arg(m_syntaxErrors));

    const auto &&selectorCounts = m_nodeCounter.targetSelectorCounts();
    using At = Command::TargetSelectorNode::Variable;
    ui->selectorsLabel->setText(ui->selectorsLabel->text().arg(
                                    selectorCounts[At::A]).arg(
                                    selectorCounts[At::E]).arg(
                                    selectorCounts[At::P]).arg(
                                    selectorCounts[At::R]).arg(
                                    selectorCounts[At::S]));

    ui->otherFunctionLabel->setText(ui->otherFunctionLabel->text().arg(
                                        m_nodeCounter.nbtAccessCount()));

    row = 0;
    const auto &&commandCounts = m_nodeCounter.commandCounts();
    ui->commandTable->setRowCount(commandCounts.size());
    for (auto it = commandCounts.cbegin(); it != commandCounts.cend();
         ++it) {
        ui->commandTable->setItem(row, 0, new QTableWidgetItem(it.key()));
        auto *countItem = new QTableWidgetItem();
        countItem->setData(Qt::DisplayRole, it.value());
        ui->commandTable->setItem(row, 1, countItem);
        auto *percentItem = new QTableWidgetItem();
        if (m_commandLines > 0) {
            percentItem->setData(
                Qt::DisplayRole,
                qRound(it.value() * 10000.0 / m_commandLines) / 100.0);
        } else {
            percentItem->setData(Qt::DisplayRole, 0);
        }
        ui->commandTable->setItem(row, 2, percentItem);
        ++row;
    }
    ui->commandTable->resizeColumnsToContents();
    ui->commandTable->setSortingEnabled(true);

    ui->syntaxErrorTable->resizeColumnsToContents();

    show();
}

void StatisticsDialog::collectFunctionData(const QString &path) {
    QString text;

    if (QFile file(path); file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");

        text = in.readAll();
    }

    if (!text.isNull()) {
        Q_ASSERT(m_parser != nullptr);
        m_parser->parse(text);
        if (m_parser->errors().isEmpty()) {
            if (m_parser->syntaxTree()->isValid()) {
                const auto &lines = m_parser->syntaxTree()->lines();
                for (const auto &logiLine: lines) {
                    switch (logiLine->kind()) {
                        case Command::ParseNode::Kind::Span: {
                            const auto span =
                                static_cast<Command::SpanNode *>(logiLine.get());
                            const auto &trimmed = span->text().trimmed();
                            if (trimmed.isEmpty()) {
                                continue;
                            } else if (trimmed.front() == '#'_QL1) {
                                ++m_commentLines;
                                break;
                            }
                        }
                        case Command::ParseNode::Kind::Macro: {
                            const auto macro =
                                static_cast<Command::MacroNode *>(logiLine.get());
                            if (macro->isValid()) {
                                ++m_macroLines;
                            }
                            break;
                        }
                        case Command::ParseNode::Kind::Root: {
                            const auto root =
                                static_cast<Command::RootNode *>(logiLine.get());
                            if (root->isValid()) {
                                m_nodeCounter.startVisiting(root);
                                ++m_commandLines;
                            }
                        }
                        default: {
                        }
                    }
                }
            } else {
                const int row = ui->syntaxErrorTable->rowCount();
                ui->syntaxErrorTable->insertRow(row);
                ui->syntaxErrorTable->setItem(
                    row, 0,
                    new QTableWidgetItem(Glhp::relPath(m_dirPath, path)));
                ui->syntaxErrorTable->setItem(
                    row, 1, new QTableWidgetItem("1"));
                ui->syntaxErrorTable->setItem(0, 2, new QTableWidgetItem(
                                                  tr("Invalid command")));
                ++m_syntaxErrors;
            }
        } else {
            const auto  doc    = std::make_unique<QTextDocument>(text);
            const auto &errors = m_parser->errors();
            for (const auto &error: errors) {
                const int row = ui->syntaxErrorTable->rowCount();
                ui->syntaxErrorTable->insertRow(row);
                const int line = doc->findBlock(error.pos).blockNumber() + 1;
                ui->syntaxErrorTable->setItem(
                    row, 0,
                    new QTableWidgetItem(Glhp::relPath(m_dirPath, path)));
                ui->syntaxErrorTable->setItem(
                    row, 1, new QTableWidgetItem(QString::number(line)));
                ui->syntaxErrorTable->setItem(
                    row, 2, new QTableWidgetItem(
                        error.toLocalizedMessage()));
                ++m_syntaxErrors;
            }
        }
    }
}
