#include "tabbeddocumentinterface.h"
#include "ui_tabbeddocumentinterface.h"

#include "globalhelpers.h"
#include "fileswitcher.h"
#include "mcfunctionhighlighter.h"
#include "jmchighlighter.h"
#include "mcbuildhighlighter.h"
#include "jsonhighlighter.h"
#include "mainwindow.h"
#include "codeeditor.h"
#include "imgviewer.h"
#include "parsers/command/mcfunctionparser.h"
#include "parsers/jsonparser.h"

#include <QMessageBox>
#include <QTextStream>
#include <QDir>
#include <QSaveFile>
#include <QShortcut>
#include <QJsonDocument>
#include <QDirIterator>
#include <QAction>

void openAllFiles(TabbedDocumentInterface *widget,
                  CodeFile::FileType minType, CodeFile::FileType maxType) {
    QDir          &&dir     = QDir::current();
    const QString &&dirPath = dir.path();

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

    QDirIterator it(dir, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        const QString &&path  = it.next();
        const auto    &&finfo = it.fileInfo();
        if (finfo.isFile()) {
            const auto type = CodeFile::pathToFileType(dirPath, path);
            if ((type >= minType) && (type <= maxType)) {
                widget->onOpenFile(path);
            }
        }
    }
}

TabbedDocumentInterface::TabbedDocumentInterface(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TabbedDocumentInterface) {
    ui->setupUi(this);

    getTabBar()->setDrawBase(false);

    connect(ui->tabWidget, &QTabWidget::currentChanged,
            this, &TabbedDocumentInterface::onTabChanged);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested,
            this, [this](int index) {
        this->onCloseFile(index, false);
    });
    connect(getTabBar(), &QTabBar::tabMoved,
            this, &TabbedDocumentInterface::onTabMoved);

    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_W), this),
            &QShortcut::activated, this, [this]() {
        onCloseFile(getCurIndex());
    });
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab), this),
            &QShortcut::activated, this,
            &TabbedDocumentInterface::onSwitchNextFile);
    connect(new QShortcut(QKeySequence(
                              Qt::CTRL + Qt::SHIFT + Qt::Key_Tab), this),
            &QShortcut::activated, this,
            &TabbedDocumentInterface::onSwitchPrevFile);

    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_O,
                                       Qt::Key_F), this),
            &QShortcut::activated, this, [this]() {
        openAllFiles(this, CodeFile::Function, CodeFile::Function);
    });
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_O,
                                       Qt::Key_J), this),
            &QShortcut::activated, this, [this]() {
        openAllFiles(this, CodeFile::JsonText, CodeFile::JsonText_end);
    });
    auto *mainWin = qobject_cast<MainWindow *>(window());
    Q_ASSERT(mainWin != nullptr);
    connect(ui->newDatapackBtn, &QPushButton::clicked,
            mainWin, &MainWindow::newDatapack);
    connect(ui->openDatapackBtn, &QPushButton::clicked,
            mainWin, qOverload<>(&MainWindow::openFolder));
    connect(ui->optionsBtn, &QPushButton::clicked,
            mainWin, &MainWindow::pref_settings);
    connect(ui->recentLabel, &QLabel::linkActivated, mainWin,
            qOverload<const QString &>(&MainWindow::openFolder));
}

TabbedDocumentInterface::~TabbedDocumentInterface() {
    delete ui;
}

void TabbedDocumentInterface::openFile(const QString &filepath,
                                       const QString &realPath,
                                       bool reload) {
    if (filepath.isEmpty()
        || ((getCurFilePath() == filepath) && (!reload)))
        return;
    else {
        addFile(filepath, realPath);
    }
}

bool TabbedDocumentInterface::saveFile(int index, const QString &filepath) {
    Q_ASSERT(index < files.count());
    auto &curFile = files[index];
    /*qDebug() << "saveFile" << index << filepath << count(); */
    QString errorMessage;
    bool    ok = true;

    QSaveFile file(filepath);
    if (curFile.fileType >= CodeFile::Text) {
        auto *doc =
            qobject_cast<CodeEditor *>(ui->tabWidget->widget(index))->document();
        Q_ASSERT(doc != nullptr);

#ifndef QT_NO_CURSOR
        QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif

        if (file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&file);
            out.setCodec("UTF-8");

            out << doc->toPlainText().toUtf8();
            if (!file.commit()) {
                errorMessage = tr("Cannot write file %1:\n%2.")
                               .arg(QDir::toNativeSeparators(filepath),
                                    file.errorString());
                ok = false;
            }
        } else {
            errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                           .arg(QDir::toNativeSeparators(filepath),
                                file.errorString());
            ok = false;
        }

#ifndef QT_NO_CURSOR
        QGuiApplication::restoreOverrideCursor();
#endif

        if (!errorMessage.isEmpty()) {
            QMessageBox::information(this, tr("Save file error"), errorMessage);
            ok = false;
        }

        if (ok) {
            curFile.changePath(filepath);
            doc->setModified(false);
            updateTabTitle(index, false);
            files[index].isModified = false;
        }

        return ok;
    }
    return false;
}

void TabbedDocumentInterface::updateTabTitle(int index, bool changed) {
    auto &&newTitle = files[index].name();

    if (changed)
        newTitle += '*';
    ui->tabWidget->setTabText(index, newTitle);
}

bool TabbedDocumentInterface::maybeSave(int index) {
    if (!(&files[index])->isModified)
        return true;

    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this,
                               tr("Unsaved file"),
                               tr("This document has been modified.\n"
                                  "Do you want to save it?"),
                               QMessageBox::Save | QMessageBox::Discard |
                               QMessageBox::Cancel);
    switch (ret) {
        case QMessageBox::Save:
            return saveFile(index, files[index].info.absoluteFilePath());

        case QMessageBox::Cancel:
            return false;

        default:
            break;
    }
    return true;
}

void TabbedDocumentInterface::retranslate() {
    ui->retranslateUi(this);
}

int TabbedDocumentInterface::getCurIndex() const {
    return ui->tabWidget->currentIndex();
}

void TabbedDocumentInterface::setCurIndex(int i) {
    ui->tabWidget->setCurrentIndex(i);
}

void TabbedDocumentInterface::onModificationChanged(bool changed) {
    /*qDebug() << "onModificationChanged" << changed; */
    Q_ASSERT(!hasNoFile());

    getCurFile()->isModified = changed;
    updateTabTitle(getCurIndex(), changed);
    emit curModificationChanged(changed);
}

void TabbedDocumentInterface::addFile(const QString &path,
                                      const QString &realPath) {
    CodeFile newFile(path);
    QWidget *widget = nullptr;

    if (newFile.fileType >= CodeFile::Text) {
        bool        ok;
        const auto &text =
            readTextFile(!realPath.isNull() ? realPath : path, ok);
        if (!ok)
            return;

        auto *codeEditor = new CodeEditor(this);
        codeEditor->setPlainText(text);
        if (!newFile.info.isWritable()) {
            codeEditor->setReadOnly(true);
            codeEditor->setTextInteractionFlags(
                codeEditor->textInteractionFlags() |
                Qt::TextSelectableByKeyboard);
        }

        if (newFile.fileType >= CodeFile::JsonText
            && newFile.fileType < CodeFile::JsonText_end) {
            codeEditor->setHighlighter(new JsonHighlighter(codeEditor->
                                                           document()));
            codeEditor->setParser(std::make_unique<JsonParser>());
        } else {
            switch (newFile.fileType) {
                case CodeFile::Function: {
                    auto &&parser =
                        std::make_unique<Command::McfunctionParser>();
                    codeEditor->setHighlighter(
                        new McfunctionHighlighter(codeEditor->document(),
                                                  parser.get()));
                    codeEditor->setParser(std::move(parser));
                    break;
                }
                /*
                 * TODO: Change the CodeFile, highlighter and parser of a file
                         everytime its file extension changed
                 */
                case CodeFile::Jmc: {
                    codeEditor->setHighlighter(
                        new JmcHighlighter(codeEditor->document(), false));
                    break;
                }
                case CodeFile::JmcHeader: {
                    codeEditor->setHighlighter(
                        new JmcHighlighter(codeEditor->document(), true));
                    break;
                }
                case CodeFile::McBuild: {
                    codeEditor->setHighlighter(
                        new McbuildHighlighter(codeEditor->document(), false));
                    break;
                }
                case CodeFile::McBuildMacro: {
                    codeEditor->setHighlighter(
                        new McbuildHighlighter(codeEditor->document(), true));
                    break;
                }
                default: {
                }
            }
        }

        codeEditor->setFileType(newFile.fileType);

        connect(codeEditor->document(), &QTextDocument::modificationChanged,
                this, &TabbedDocumentInterface::onModificationChanged);
        connect(codeEditor, &CodeEditor::openFileRequest,
                this, &TabbedDocumentInterface::onOpenFile);

        connect(codeEditor, &QPlainTextEdit::copyAvailable,
                this, &TabbedDocumentInterface::updateEditMenuRequest);
        connect(codeEditor, &QPlainTextEdit::undoAvailable,
                this, &TabbedDocumentInterface::updateEditMenuRequest);
        connect(codeEditor, &QPlainTextEdit::redoAvailable,
                this, &TabbedDocumentInterface::updateEditMenuRequest);

        connect(codeEditor, &CodeEditor::updateStatusBarRequest,
                this, &TabbedDocumentInterface::updateStatusBarRequest);
        connect(codeEditor, &CodeEditor::showMessageRequest,
                this, &TabbedDocumentInterface::showMessageRequest);

        connect(this, &TabbedDocumentInterface::settingsChanged,
                codeEditor, &CodeEditor::readPrefSettings);

        widget = codeEditor;
    } else if (newFile.fileType == CodeFile::Image) {
        auto *viewer = new ImgViewer(this);

        connect(viewer,
                &ImgViewer::updateStatusBarRequest,
                this, &TabbedDocumentInterface::updateStatusBarRequest);

        if (!viewer->setImage(path)) {
            viewer->deleteLater();
            return;
        }

        widget = viewer;
    }
    if (widget) {
        files << newFile;
        QIcon icon;
        if (newFile.info.isWritable()) {
            icon = CodeFile::fileTypeToIcon(newFile.fileType);
        } else {
            icon.addFile(QStringLiteral(":/icon/file-readonly.png"));
        }
        const int index =
            ui->tabWidget->addTab(widget, icon, newFile.name());
        ui->tabWidget->setCurrentIndex(index);
    }
}

CodeFile * TabbedDocumentInterface::getCurFile() {
    /*qDebug() << "getCurFile" << count() << getCurIndex(); */
    if (hasNoFile() || (getCurIndex() == -1))
        return nullptr;
    else
        return &files[getCurIndex()];
}

QString TabbedDocumentInterface::getCurFilePath() {
    /*qDebug() << "getCurFilePath"; */
    if (auto *curFile = getCurFile())
        return curFile->path();
    else
        return "";
}

QTextDocument * TabbedDocumentInterface::getCurDoc() {
    if (getCodeEditor()) {
        return getCodeEditor()->document();
    } else {
        return nullptr;
    }
}

QVector<CodeFile> &TabbedDocumentInterface::getFiles() {
    return files;
}

CodeEditor * TabbedDocumentInterface::getCodeEditor() const {
    return qobject_cast<CodeEditor *>(ui->tabWidget->currentWidget());
}

ImgViewer * TabbedDocumentInterface::getImgViewer() const {
    return qobject_cast<ImgViewer *>(ui->tabWidget->currentWidget());
}

QTabBar * TabbedDocumentInterface::getTabBar() const {
    return ui->tabWidget->tabBar();
}

int TabbedDocumentInterface::count() const {
    return ui->tabWidget->count();
}

bool TabbedDocumentInterface::hasNoFile() const {
    return count() == 0;
}

void TabbedDocumentInterface::clear() {
    if (!hasNoFile()) {
        for (int i = count() - 1; i >= 0; --i) {
            onCloseFile(i, true);
        }
    }

    Q_ASSERT(hasNoFile());
}

bool TabbedDocumentInterface::hasUnsavedChanges() const {
    return std::any_of(files.cbegin(), files.cend(), [](const CodeFile &file){
        return file.isModified;
    });
}

void TabbedDocumentInterface::onOpenFile(const QString &filepath) {
    for (int i = 0; i < count(); i++) {
        if (files[i].path() == filepath) {
            setCurIndex(i);
            return;
        }
    }
    openFile(filepath);
}

void TabbedDocumentInterface::onOpenAliasedFile(const QString &filepath,
                                                const QString &realPath) {
    for (int i = 0; i < count(); i++) {
        if (files[i].path() == filepath) {
            setCurIndex(i);
            return;
        }
    }
    openFile(filepath, realPath);
}

void TabbedDocumentInterface::onOpenFileWithLine(const QString &filepath,
                                                 const int lineNo) {
    onOpenFile(filepath);
    if (auto *editor = getCodeEditor()) {
        editor->goToLine(lineNo);
    }
}

bool TabbedDocumentInterface::saveCurFile(const QString &path) {
    return saveFile(getCurIndex(), path);
}

bool TabbedDocumentInterface::saveCurFile() {
    /*qDebug() << "saveCurFile" << getCurIndex(); */
    if (auto *curFile = getCurFile()) {
        return saveFile(getCurIndex(), curFile->path());
    }
    return false;
}

bool TabbedDocumentInterface::saveAllFile() {
    bool r = true;

    if (!hasNoFile()) {
        for (int i = 0; i < count(); i++) {
            const auto &codeFile = files.at(i);
            if (codeFile.isModified) {
                /* AND operation */
                r &= saveFile(i, codeFile.path());
            }
        }
    }
    return r;
}

void TabbedDocumentInterface::onFileRenamed(const QString &path,
                                            const QString &oldName,
                                            const QString &newName) {
/*
      qDebug() << "TabbedDocumentInterface::onFileRenamed" << oldName <<
          newName << count();
 */
    const QString &&oldpath = path + '/' + oldName;
    const QString &&newpath = path + '/' + newName;

    for (int i = 0; i < count(); i++) {
        auto *file = &files[i];
        if (file->info.absoluteFilePath() == oldpath) {
            file->changePath(newpath);
            updateTabTitle(i, file->isModified);
            ui->tabWidget->setTabIcon(
                i, CodeFile::fileTypeToIcon(CodeFile::pathToFileType(
                                                QDir::currentPath(),
                                                file->path())));

            onModificationChanged(false);
            setCurIndex(getCurIndex());
            break;
        }
    }
}

void TabbedDocumentInterface::invokeActionType(ActionType act) {
    constexpr qreal zoomInFactor  = 1.15;
    constexpr qreal zoomOutFactor = 1. / zoomInFactor;

    switch (act) {
        case ActionType::ZoomIn: {
            if (auto *editor = getCodeEditor()) {
                editor->zoomIn();
            } else if (auto *viewer = getImgViewer()) {
                viewer->scale(zoomInFactor, zoomInFactor);
            }
            break;
        }
        case ActionType::ZoomOut: {
            if (auto *editor = getCodeEditor()) {
                editor->zoomOut();
            } else if (auto *viewer = getImgViewer()) {
                viewer->scale(zoomOutFactor, zoomOutFactor);
            }
            break;
        }
        default:
            break;
    }
}

void TabbedDocumentInterface::setPackOpened(const bool value) {
    m_packOpened = value;
    ui->stackedWidget->setCurrentIndex(value ? 1 : 0);
}

void TabbedDocumentInterface::updateRecentPacks(
    const QVector<QAction *> &actions,
    const int size) {
    const static QString itemTemplate{
        R"(<li><a href="%1">%2</a><br><em>%1</em></li>)" };

    if (size == 0) {
        ui->recentLabel->clear();
        ui->recentDatapacksGroup->hide();
        return;
    } else {
        ui->recentDatapacksGroup->show();
    }
    QString &&text = QStringLiteral("<ol>");
    for (const auto *action: actions) {
        if (action->isEnabled()) {
            const QString &&link = action->data().toString();
            text += itemTemplate.arg(link, link.section('/', -1));
        }
    }
    text += "</ol>";
    ui->recentLabel->setText(text);
}

void TabbedDocumentInterface::changeEvent(QEvent *event) {
    QWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange)
        retranslate();
}

QTextDocument * TabbedDocumentInterface::getDocAt(int index) const {
    if (auto *editor =
            qobject_cast<CodeEditor *>(ui->tabWidget->widget(index))) {
        return editor->document();
    } else {
        return nullptr;
    }
}

void TabbedDocumentInterface::onTabChanged(int index) {
    /*qDebug() << "Change to tab" << index << '/' << count(); */

    if (index > -1) {
        auto *curFile = getCurFile();
        Q_ASSERT(curFile != nullptr);
        ui->stackedWidget->setCurrentIndex(2);
        emit curFileChanged(curFile->path());
    } else {
        ui->stackedWidget->setCurrentIndex(m_packOpened ? 1 : 0);

        emit curFileChanged(QString());
    }
    if (hasNoFile())
        emit curModificationChanged(false);
    else if (getCurDoc())
        emit curModificationChanged(getCurDoc()->isModified());
}

void TabbedDocumentInterface::onTabMoved(int from, int to) {
    /*qDebug() << "Move from tab" << from << "to tab" << to; */
    std::swap(files[from], files[to]);
}

void TabbedDocumentInterface::onCloseFile(int index, const bool force) {
    if (index == -1)
        return;

    Q_ASSERT(count() > 0);
    if (force || maybeSave(index)) {
        files.remove(index);
        /*qDebug() << "Close tab" << index; */
        ui->tabWidget->removeTab(index);
    }
}

void TabbedDocumentInterface::onSwitchNextFile() {
    if (!hasNoFile())
        new FileSwitcher(this, false);
}

void TabbedDocumentInterface::onSwitchPrevFile() {
    if (!hasNoFile())
        new FileSwitcher(this, true);
}

QString TabbedDocumentInterface::readTextFile(const QString &path, bool &ok) {
    QFile   file(path);
    QString content;

    // Removing QFile::Text enum allows '\r' to be recognized as a line sepatator
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::information(this, tr("Loading text file error"),
                                 tr("Cannot read file %1:\n%2.")
                                 .arg(QDir::toNativeSeparators(path),
                                      file.errorString()));
        ok = false;
    } else {
        QTextStream in(&file);
        in.setCodec("UTF-8");

#ifndef QT_NO_CURSOR
        QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif

        while (!in.atEnd()) {
            content += in.readLine();
            if (!in.atEnd())
                content += '\n';
        }
        ok = true;

#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif
    }
    file.close();
    return content;
}
