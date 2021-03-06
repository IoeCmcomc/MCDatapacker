#include "tabbedcodeeditorinterface.h"
#include "ui_tabbedcodeeditorinterface.h"

#include "globalhelpers.h"
#include "mainwindow.h"
#include "fileswitcher.h"
#include "mcfunctionhighlighter.h"
#include "jsonhighlighter.h"

#include <QMessageBox>
#include <QTextStream>
#include <QDir>
#include <QSaveFile>
#include <QShortcut>
#include <QJsonDocument>


TabbedCodeEditorInterface::TabbedCodeEditorInterface(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TabbedCodeEditorInterface) {
    ui->setupUi(this);

    ui->tabBar->setTabsClosable(true);
    ui->tabBar->setMovable(true);
    ui->tabBar->setDocumentMode(true);
    ui->tabBar->setExpanding(false);
    ui->tabBar->adjustSize();

    connect(ui->tabBar, &QTabBar::currentChanged,
            this, &TabbedCodeEditorInterface::onTabChanged);
    connect(ui->tabBar, &QTabBar::tabCloseRequested,
            this, &TabbedCodeEditorInterface::onCloseFile);
    connect(ui->tabBar, &QTabBar::tabMoved,
            this, &TabbedCodeEditorInterface::onTabMoved);

    connect(ui->codeEditor->document(), &QTextDocument::modificationChanged,
            this, &TabbedCodeEditorInterface::onModificationChanged);
    connect(this, &TabbedCodeEditorInterface::curFileChanged,
            this, &TabbedCodeEditorInterface::onCurFileChanged);
    connect(ui->codeEditor, &CodeEditor::openFile,
            this, &TabbedCodeEditorInterface::onOpenFile);
    connect(ui->codeEditor, &CodeEditor::textChanged,
            this, &TabbedCodeEditorInterface::onCurTextChanged);

    textChangedTimer = new QTimer(this);
    textChangedTimer->setSingleShot(true);
    connect(textChangedTimer, &QTimer::timeout,
            this, &TabbedCodeEditorInterface::onCurTextChangingDone); /* connect it to your slot */


    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_W), this),
            &QShortcut::activated, [this]() {
        onCloseFile(getCurIndex());
    });
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab), this),
            &QShortcut::activated, this,
            &TabbedCodeEditorInterface::onSwitchFile);
}

TabbedCodeEditorInterface::~TabbedCodeEditorInterface() {
    delete ui;
}

void TabbedCodeEditorInterface::addCodeFile(const CodeFile &file) {
    /*qDebug() << "addCodeFile"; */
    file.doc->setModified(false);
    connect(file.doc, &QTextDocument::modificationChanged,
            this, &TabbedCodeEditorInterface::onModificationChanged);
    files << file;
    ui->tabBar->addTab(file.title);
    const int lastIndex = count() - 1;
    ui->tabBar->setTabToolTip(lastIndex, file.fileInfo.filePath());
    ui->tabBar->setTabIcon(lastIndex, Glhp::fileTypeToIcon(
                               Glhp::pathToFileType(QDir::currentPath(),
                                                    file.fileInfo.
                                                    filePath())));
    setCurIndex(count() - 1);
}

void TabbedCodeEditorInterface::openFile(const QString &filepath, bool reload) {
    if (filepath.isEmpty()
        || ((getCurFilePath() == filepath) && (!reload)))
        return;
    else {
        auto newFile = readFile(filepath);
        if (newFile.isVaild())
            addCodeFile(newFile);
    }
}

bool TabbedCodeEditorInterface::saveFile(int index, const QString &filepath) {
    Q_ASSERT(index < files.count());
    /*qDebug() << "saveFile" << index << filepath << count(); */
    QString errorMessage;
    bool    ok = true;

#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    QSaveFile file(filepath);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << files.at(index).doc->toPlainText().toUtf8();
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
        QMessageBox::information(this, tr("Error"), errorMessage);
        ok = false;
    }

    if (ok) {
        auto file = files[index];
        file.changePath(filepath);
        file.doc->setModified(false);
    }

    return ok;
}

void TabbedCodeEditorInterface::updateTabTitle(int index, bool changed) {
    auto newTitle = getCurFile()->title;

    if (changed)
        newTitle += '*';
    ui->tabBar->setTabText(index, newTitle);
}

bool TabbedCodeEditorInterface::maybeSave(int index) {
    if (!files[index].doc->isModified())
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
        return saveFile(index, files[index].fileInfo.absoluteFilePath());

    case QMessageBox::Cancel:
        return false;

    default:
        break;
    }
    return true;
}

void TabbedCodeEditorInterface::retranslate() {
    ui->retranslateUi(this);
}

int TabbedCodeEditorInterface::getCurIndex() const {
    return ui->tabBar->currentIndex();
}

void TabbedCodeEditorInterface::setCurIndex(int i) {
    ui->tabBar->setCurrentIndex(i);
}

void TabbedCodeEditorInterface::onModificationChanged(bool changed) {
    /*qDebug() << "onModificationChanged" << changed; */
    Q_ASSERT(!hasNoFile());

    updateTabTitle(getCurIndex(), changed);
    emit curModificationChanged(changed);
}

CodeFile TabbedCodeEditorInterface::readFile(const QString &path) {
    auto newFile = CodeFile(path);

    QFile file(path);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Cannot read file %1:\n%2.")
                                 .arg(QDir::toNativeSeparators(path),
                                      file.errorString()));
    } else {
        QTextStream in(&file);
        in.setCodec("UTF-8");

#ifndef QT_NO_CURSOR
        QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif

        int     c = 0;
        QString content;
        while (!in.atEnd()) {
            content += in.readLine();
            if (!in.atEnd()) content += '\n';
            ++c;
        }

        newFile.doc->setParent(this);
        newFile.doc->setPlainText(content);
        newFile.textCursor.setPosition(0);
        newFile.fileType = Glhp::pathToFileType(QDir::currentPath(), path);
        if (newFile.fileType == CodeFile::Function) {
            newFile.highlighter = new McfunctionHighlighter(newFile.doc);
        } else if (newFile.fileType >= CodeFile::JsonText) {
            newFile.highlighter = new JsonHighlighter(newFile.doc);
        }

#ifndef QT_NO_CURSOR
        QGuiApplication::restoreOverrideCursor();
#endif
    }
    file.close();
    return newFile;
}

CodeFile *TabbedCodeEditorInterface::getCurFile() {
    /*qDebug() << "getCurFile" << count() << getCurIndex(); */
    if (hasNoFile() || (getCurIndex() == -1))
        return nullptr;
    else
        return &files[getCurIndex()];
}

QString TabbedCodeEditorInterface::getCurFilePath() {
    /*qDebug() << "getCurFilePath"; */
    if (auto *curFile = getCurFile())
        return curFile->fileInfo.filePath();
    else
        return "";
}

QTextDocument *TabbedCodeEditorInterface::getCurDoc() {
    if (auto *curFile = getCurFile())
        return curFile->doc;
    else
        return nullptr;
}

QVector<CodeFile> *TabbedCodeEditorInterface::getFiles() {
    return &files;
}

CodeEditor *TabbedCodeEditorInterface::getEditor() const {
    return ui->codeEditor;
}

void TabbedCodeEditorInterface::clear() {
    if (!hasNoFile()) {
        for (int i = count() - 1; i >= 0; --i) {
            onCloseFile(i);
        }
    }

    Q_ASSERT(hasNoFile());
}

bool TabbedCodeEditorInterface::hasUnsavedChanges() const {
    for (const auto &file: files) {
        if (file.doc->isModified())
            return true;
    }
    return false;
}

void TabbedCodeEditorInterface::onOpenFile(const QString &filepath) {
    for (int i = 0; i < count(); i++) {
        if (files[i].fileInfo.filePath() == filepath) {
            setCurIndex(i);
            return;
        }
    }
    openFile(filepath);
}

bool TabbedCodeEditorInterface::saveCurFile(const QString path) {
    return saveFile(getCurIndex(), path);
}

bool TabbedCodeEditorInterface::saveCurFile() {
    /*qDebug() << "saveCurFile" << getCurIndex(); */
    if (auto *curFile = getCurFile()) {
        return saveFile(getCurIndex(), curFile->fileInfo.filePath());
    }
    return false;
}

bool TabbedCodeEditorInterface::saveAllFile() {
    bool r = true;

    if (!hasNoFile()) {
        for (int i = 0; i < count(); i++) {
            /* AND operation */
            r &= saveFile(i, files[i].fileInfo.filePath());
        }
    }
    return r;
}

void TabbedCodeEditorInterface::onFileRenamed(const QString &path,
                                              const QString &oldName,
                                              const QString &newName) {
/*
      qDebug() << "TabbedCodeEditorInterface::onFileRenamed" << oldName <<
          newName << count();
 */
    QString oldpath = path + '/' + oldName;
    QString newpath = path + '/' + newName;

    for (int i = 0; i < count(); i++) {
        auto *file = &files[i];
        if (file->fileInfo.absoluteFilePath() == oldpath) {
            file->changePath(newpath);
            updateTabTitle(i, file->doc->isModified());
            ui->tabBar->setTabIcon(
                i, Glhp::fileTypeToIcon(Glhp::pathToFileType(
                                            QDir::currentPath(),
                                            file->fileInfo.filePath())));

            onModificationChanged(false);
            setCurIndex(getCurIndex());
            break;
        }
    }
}

void TabbedCodeEditorInterface::changeEvent(QEvent *event) {
    QWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange)
        retranslate();
}

void TabbedCodeEditorInterface::onTabChanged(int index) {
    qDebug() << "onTabChanged" << prevIndex << getCurIndex() << index <<
        count();
    if (index > -1) {
        if (prevIndex > -1)
            if ((prevIndex < count()) && (prevIndex != index) && !tabMoved)
                files[prevIndex].textCursor = ui->codeEditor->textCursor();

        auto *curFile = getCurFile();
        ui->codeEditor->setDocument(curFile->doc);
        ui->codeEditor->setTextCursor(curFile->textCursor);

        if (ui->stackedWidget->currentIndex() == 0)
            ui->stackedWidget->setCurrentIndex(1);

        emit curFileChanged(curFile->fileInfo.filePath());
    } else {
        if (ui->stackedWidget->currentIndex() == 1)
            ui->stackedWidget->setCurrentIndex(0);

        emit curFileChanged("");
    }
    if (hasNoFile())
        emit curModificationChanged(false);
    else
        emit curModificationChanged(getCurDoc()->isModified());

    prevIndex = index;
    tabMoved  = false;
}

void TabbedCodeEditorInterface::onTabMoved(int from, int to) {
    /*qDebug() << "onTabMoved" << from << to; */
    tabMoved = true;
    qSwap(files[from], files[to]);
}

void TabbedCodeEditorInterface::onCloseFile(int index) {
    if (index == -1)
        return;

    Q_ASSERT(count() > 0);
    /*qDebug() << "onCloseFile" << index << count(); */
    if (maybeSave(index)) {
        disconnect(getCurDoc(), &QTextDocument::modificationChanged,
                   this, &TabbedCodeEditorInterface::onModificationChanged);
        files.remove(index);
        ui->tabBar->removeTab(index);
    }
}

void TabbedCodeEditorInterface::onSwitchFile() {
    if (!hasNoFile()) {
        new FileSwitcher(this);
    }
}

void TabbedCodeEditorInterface::onCurTextChanged() {
    if (!hasNoFile())
        textChangedTimer->start(260);
    else
        textChangedTimer->stop();
}

void TabbedCodeEditorInterface::onCurTextChangingDone() {
    qDebug() << "TabbedCodeEditorInterface::onCurTextChangingDone";
    auto *curFile = getCurFile();
    if (curFile && curFile->highlighter) {
        curFile->highlighter->checkProblems();
        ui->codeEditor->updateErrorSelections();
    }
}

void TabbedCodeEditorInterface::onCurFileChanged(const QString &path) {
    ui->codeEditor->setFilePath(path);
    ui->codeEditor->setCurHighlighter(
        (!hasNoFile()) ? getCurFile()->highlighter : nullptr);
}
