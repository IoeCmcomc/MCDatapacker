#include "tabbedcodeeditorinterface.h"
#include "ui_tabbedcodeeditorinterface.h"

#include "globalhelpers.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QTextStream>
#include <QDir>
#include <QSaveFile>


TabbedCodeEditorInterface::TabbedCodeEditorInterface(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TabbedCodeEditorInterface) {
    ui->setupUi(this);

    ui->tabBar->setTabsClosable(true);
    ui->tabBar->setDocumentMode(true);
    ui->tabBar->setExpanding(false);
    ui->tabBar->adjustSize();

    connect(ui->tabBar, &QTabBar::currentChanged,
            this, &TabbedCodeEditorInterface::onTabChanged);
    connect(ui->tabBar, &QTabBar::tabCloseRequested,
            this, &TabbedCodeEditorInterface::onCloseFile);
    connect(ui->codeEditor->document(), &QTextDocument::modificationChanged,
            this, &TabbedCodeEditorInterface::onModificationChanged);
    connect(this, &TabbedCodeEditorInterface::curFileChanged,
            ui->codeEditor, &CodeEditor::setFilePath);
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
    qDebug() << "saveFile" << index << filepath << count();
    QString errorMessage;
    bool    ok = true;

    /*onTabChanged(getCurIndex()); */

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

int TabbedCodeEditorInterface::getCurIndex() const {
    return ui->tabBar->currentIndex();
}

void TabbedCodeEditorInterface::setCurIndex(int i) {
    ui->tabBar->setCurrentIndex(i);
}

void TabbedCodeEditorInterface::onModificationChanged(bool changed) {
    /*qDebug() << "onModificationChanged" << changed; */
    Q_ASSERT(!isNoFile());

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
        newFile.fileType = Glhp::pathToFileType(MainWindow::getCurDir(), path);

#ifndef QT_NO_CURSOR
        QGuiApplication::restoreOverrideCursor();
#endif
    }
    file.close();
    return newFile;
}

CodeFile *TabbedCodeEditorInterface::getCurFile() {
    /*qDebug() << "getCurFile" << count() << getCurIndex(); */
    if (isNoFile() || (getCurIndex() == -1))
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

CodeEditor *TabbedCodeEditorInterface::getEditor() const {
    return ui->codeEditor;
}

void TabbedCodeEditorInterface::clear() {
    if (!isNoFile()) {
        for (int i = 0; i < count(); i++) {
            ui->tabBar->removeTab(i);
        }
    }
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

    if (!isNoFile()) {
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
    qDebug() << "TabbedCodeEditorInterface::onFileRenamed" << oldName <<
        newName << count();
    QString oldpath = path + '/' + oldName;
    QString newpath = path + '/' + newName;

    for (int i = 0; i < count(); i++) {
        auto *file = &files[i];
        if (file->fileInfo.absoluteFilePath() == oldpath) {
            file->changePath(newpath);
            updateTabTitle(i, file->doc->isModified());
            onModificationChanged(false);
            setCurIndex(getCurIndex());
            break;
        }
    }
}

void TabbedCodeEditorInterface::onTabChanged(int index) {
    qDebug() << "onTabChanged" << getCurIndex() << index << count();
    if (index > -1) {
        auto *curDoc = files[index].doc;
        ui->codeEditor->setDocument(curDoc);
        if (ui->stackedWidget->currentIndex() == 0)
            ui->stackedWidget->setCurrentIndex(1);

        emit curFileChanged(files.at(index).fileInfo.absoluteFilePath());
    } else {
        if (ui->stackedWidget->currentIndex() == 1)
            ui->stackedWidget->setCurrentIndex(0);

        emit curFileChanged("");
    }
    if (isNoFile())
        emit curModificationChanged(false);
    else
        emit curModificationChanged(getCurDoc()->isModified());
}

void TabbedCodeEditorInterface::onCloseFile(int index) {
    /*qDebug() << "onCloseFile" << index; */
    disconnect(getCurDoc(), &QTextDocument::modificationChanged,
               this, &TabbedCodeEditorInterface::onModificationChanged);
    files.remove(index);
    ui->tabBar->removeTab(index);
}