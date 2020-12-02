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
    files << file;
    curIndex += 1;
    ui->tabBar->addTab(file.title);
    setCurIndex(curIndex);
}

void TabbedCodeEditorInterface::openFile(const QString &filepath, bool reload) {
    if (filepath.isEmpty()
        || ((getCurFilePath() == filepath) && (!reload)))
        return;
    else {
        QFile file(filepath);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::information(this, tr("Error"),
                                     tr("Cannot read file %1:\n%2.")
                                     .arg(QDir::toNativeSeparators(filepath),
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

            auto newFile = CodeFile(filepath);
            newFile.doc->setParent(this);
            newFile.doc->setPlainText(content);
            addCodeFile(newFile);

#ifndef QT_NO_CURSOR
            QGuiApplication::restoreOverrideCursor();
#endif
        }
        file.close();
    }
}

bool TabbedCodeEditorInterface::saveFile(int index, const QString &filepath) {
    Q_ASSERT(index < files.count());
    qDebug() << "saveFile" << index << filepath << count();
    QString errorMessage;

    /*onTabChanged(curIndex); */

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
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                       .arg(QDir::toNativeSeparators(filepath),
                            file.errorString());
    }

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif

    if (!errorMessage.isEmpty()) {
        QMessageBox::information(this, tr("Error"), errorMessage);
        return false;
    }

    files[index].changePath(filepath);

    return true;
}

int TabbedCodeEditorInterface::getCurIndex() const {
    return curIndex;
}

void TabbedCodeEditorInterface::setCurIndex(int i) {
    ui->tabBar->setCurrentIndex(i);
}

void TabbedCodeEditorInterface::onModificationChanged(bool changed) {
    emit curModificationChanged(changed);
}

CodeFile *TabbedCodeEditorInterface::getCurFile() {
    /*qDebug() << "getCurFile" << count() << curIndex; */
    if (isNoFile() || (curIndex == -1))
        return nullptr;
    else
        return &files[curIndex];
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
    for (auto file: files) {
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
    return saveFile(curIndex, path);
}

bool TabbedCodeEditorInterface::saveCurFile() {
    /*qDebug() << "saveCurFile" << curIndex; */
    if (auto *curFile = getCurFile()) {
        return saveFile(curIndex, curFile->fileInfo.filePath());
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

void TabbedCodeEditorInterface::onTabChanged(int index) {
    qDebug() << "onTabChanged" << curIndex << index << count();
    curIndex = index;
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
}

void TabbedCodeEditorInterface::onCloseFile(int index) {
    qDebug() << "onCloseFile" << index;
    files.remove(index);
    ui->tabBar->removeTab(index);
}
