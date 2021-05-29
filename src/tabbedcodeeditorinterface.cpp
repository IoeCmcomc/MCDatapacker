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
    ui->tabBar->setDrawBase(false);
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
            this, &TabbedCodeEditorInterface::onCurTextChangingDone);


    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_W), this),
            &QShortcut::activated, [this]() {
        onCloseFile(getCurIndex());
    });
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab), this),
            &QShortcut::activated, this,
            &TabbedCodeEditorInterface::onSwitchNextFile);
    connect(new QShortcut(QKeySequence(
                              Qt::CTRL + Qt::SHIFT + Qt::Key_Tab), this),
            &QShortcut::activated, this,
            &TabbedCodeEditorInterface::onSwitchPrevFile);
}

TabbedCodeEditorInterface::~TabbedCodeEditorInterface() {
    delete ui;
}

void TabbedCodeEditorInterface::addCodeFile(const CodeFile &file) {
    /*qDebug() << "addCodeFile"; */
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
        if (newFile.isVaild()) {
            if (newFile.fileType >= CodeFile::Text) {
                auto &&fileData = qvariant_cast<TextFileData>(newFile.data);
                fileData.doc->setModified(false);
                connect(fileData.doc, &QTextDocument::modificationChanged,
                        this,
                        &TabbedCodeEditorInterface::onModificationChanged);
            }
            addCodeFile(newFile);
        }
    }
}

bool TabbedCodeEditorInterface::saveFile(int index, const QString &filepath) {
    Q_ASSERT(index < files.count());
    auto &curFile = files[index];
    /*qDebug() << "saveFile" << index << filepath << count(); */
    QString errorMessage;
    bool    ok = true;

    QSaveFile file(filepath);
    if (curFile.fileType >= CodeFile::Text) {
        auto data = qvariant_cast<TextFileData>(curFile.data);

#ifndef QT_NO_CURSOR
        QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif

        if (file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&file);
            out.setCodec("UTF-8");
            out << data.doc->toPlainText().toUtf8();
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
            curFile.changePath(filepath);
            data.doc->setModified(false);
        }

        return ok;
    }
    return false;
}

void TabbedCodeEditorInterface::updateTabTitle(int index, bool changed) {
    auto newTitle = getCurFile()->title;

    if (changed)
        newTitle += '*';
    ui->tabBar->setTabText(index, newTitle);
}

bool TabbedCodeEditorInterface::maybeSave(int index) {
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

    getCurFile()->isModified = changed;
    updateTabTitle(getCurIndex(), changed);
    emit curModificationChanged(changed);
}

CodeFile TabbedCodeEditorInterface::readFile(const QString &path) {
    CodeFile newFile(path);

    if (newFile.fileType >= CodeFile::Text) {
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

            TextFileData data(new QTextDocument(this));
            auto         doc = data.doc;
            doc->setPlainText(content);
            doc->setMetaInformation(QTextDocument::DocumentTitle,
                                    newFile.title);
            if (newFile.fileType == CodeFile::Function) {
                data.highlighter = new McfunctionHighlighter(data.doc);
            } else if (newFile.fileType >= CodeFile::JsonText) {
                data.highlighter = new JsonHighlighter(data.doc);
            }
            newFile.data.setValue(data);
        }
        file.close();
    } else if (newFile.fileType == CodeFile::Image) {
        QString errStr;
        if (auto &&data = ImgViewer::fromFile(path, errStr);
            !errStr.isEmpty()) {
            QMessageBox::information(this, tr("Error"),
                                     tr("Cannot read file %1:\n%2.")
                                     .arg(QDir::toNativeSeparators(path),
                                          errStr));
        } else {
            newFile.data.setValue(data);
        }
    }
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif

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
    return getDocAt(getCurIndex());
}

QVector<CodeFile> *TabbedCodeEditorInterface::getFiles() {
    return &files;
}

CodeEditor *TabbedCodeEditorInterface::getCodeEditor() const {
    return ui->codeEditor;
}

QStackedWidget *TabbedCodeEditorInterface::getStackedWidget() {
    return ui->stackedWidget;
}

int TabbedCodeEditorInterface::count() const {
    return files.count();
}

bool TabbedCodeEditorInterface::hasNoFile() const {
    return count() == 0;
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
        if (file.isModified)
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
    const QString &&oldpath = path + '/' + oldName;
    const QString &&newpath = path + '/' + newName;

    for (int i = 0; i < count(); i++) {
        auto *file = &files[i];
        if (file->fileInfo.absoluteFilePath() == oldpath) {
            file->changePath(newpath);
            updateTabTitle(i, file->isModified);
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

void TabbedCodeEditorInterface::onGameVersionChanged(const QString &ver) {
    Command::MinecraftParser::setSchema(
        QStringLiteral(":/minecraft/") + ver +
        QStringLiteral("/mcdata/generated/reports/commands.json"));

    for (const auto &file: qAsConst(files)) {
        if (file.fileType == CodeFile::Function) {
            auto &&data = qvariant_cast<TextFileData>(file.data);
            data.highlighter->checkProblems(true);
        }
    }
}

void TabbedCodeEditorInterface::undo() {
    if (ui->stackedWidget->currentIndex() == 1) {
        ui->codeEditor->undo();
    }
}

void TabbedCodeEditorInterface::redo() {
    if (ui->stackedWidget->currentIndex() == 1) {
        ui->codeEditor->redo();
    }
}

void TabbedCodeEditorInterface::selectAll() {
    if (ui->stackedWidget->currentIndex() == 1) {
        ui->codeEditor->selectAll();
    }
}

void TabbedCodeEditorInterface::cut() {
    if (ui->stackedWidget->currentIndex() == 1) {
        ui->codeEditor->cut();
    }
}

void TabbedCodeEditorInterface::copy() {
    if (ui->stackedWidget->currentIndex() == 1) {
        ui->codeEditor->copy();
    }
}

void TabbedCodeEditorInterface::paste() {
    if (ui->stackedWidget->currentIndex() == 1) {
        ui->codeEditor->paste();
    }
}

void TabbedCodeEditorInterface::changeEvent(QEvent *event) {
    QWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange)
        retranslate();
}

void TabbedCodeEditorInterface::printPanOffsets() {
    QStringList panOffsets;

    for (const auto &file: qAsConst(files)) {
        if (file.data.canConvert<ImageFileData>()) {
            const auto &data = qvariant_cast<ImageFileData>(file.data);
            panOffsets <<
                QString("(%1, %2)").arg(data.offsetX).arg(data.offsetY);
        } else {
            panOffsets << "<Not an image>";
        }
    }
    qDebug() << "Pan offsets: " << panOffsets.join(", ");
}

void TabbedCodeEditorInterface::saveFileData(int index) {
    auto &file = files[index];

    if (file.data.canConvert<TextFileData>()) {
        auto &&data = qvariant_cast<TextFileData>(file.data);
        data.textCursor = ui->codeEditor->textCursor();
        file.data.setValue(std::move(data));
    } else if (files[prevIndex].fileType == CodeFile::Image) {
        file.data.setValue(ui->imgViewer->toData());
    }
}

QTextDocument *TabbedCodeEditorInterface::getDocAt(int index) const {
    if (const auto &file = files[index];
        file.data.canConvert<TextFileData>()) {
        const auto &&data = qvariant_cast<TextFileData>(file.data);
        return data.doc;
    } else {
        return nullptr;
    }
}

void TabbedCodeEditorInterface::onTabChanged(int index) {
    /*qDebug() << "Change to tab" << index << '/' << count(); */
    /*printPanOffsets(); */

    /*QString &&ptrStrBefore = QString::number((int)lastRemovedDoc, 16); */

    if (index > -1) {
        if (prevIndex > -1)
            if ((prevIndex < count()) && (prevIndex != index) &&
                !tabMovedOrRemoved) {
                saveFileData(prevIndex);
            }

        auto *curFile = getCurFile();
        Q_ASSERT(curFile != nullptr);
        if (curFile->data.canConvert<TextFileData>()) {
            auto &&data = qvariant_cast<TextFileData>(curFile->data);

            Q_ASSERT(data.doc != nullptr);
            Q_ASSERT(data.doc == getDocAt(index));
            ui->codeEditor->setDocument(data.doc);
            if (lastRemovedDoc) {
                if (lastRemovedDoc != ui->codeEditor->document())
                    lastRemovedDoc->deleteLater();
                lastRemovedDoc = nullptr;
            }
            ui->codeEditor->setTextCursor(data.textCursor);

            if (ui->stackedWidget->currentIndex() != 1)
                ui->stackedWidget->setCurrentIndex(1);
        } else if (curFile->data.canConvert<ImageFileData>()) {
            ui->imgViewer->loadData(qvariant_cast<ImageFileData>(curFile->data));

            if (ui->stackedWidget->currentIndex() != 2)
                ui->stackedWidget->setCurrentIndex(2);
        }

        emit curFileChanged(curFile->fileInfo.filePath());
    } else {
        if (ui->stackedWidget->currentIndex() != 0)
            ui->stackedWidget->setCurrentIndex(0);

        emit curFileChanged(QString());
    }
    if (hasNoFile())
        emit curModificationChanged(false);
    else if (getCurDoc())
        emit curModificationChanged(getCurDoc()->isModified());

    prevIndex         = index;
    tabMovedOrRemoved = false;

/*
      for (int i = 0; i < ui->tabBar->count(); ++i) {
          const auto &file = files[i];
          if (file.data.canConvert<TextFileData>()) {
              const auto &data   = qvariant_cast<TextFileData>(file.data);
              QString   &&ptrStr = QString::number((int)data.doc.data(), 16);
              ui->tabBar->setTabText(i, ptrStr);
          }
      }
 */

/*
      QString &&ptrStrAfter = QString::number((int)lastRemovedDoc, 16);
      QTimer::singleShot(1, [this, ptrStrAfter, ptrStrBefore]() {
          window()->setWindowTitle(ptrStrBefore + " -> " + ptrStrAfter);
      });
 */


    /*printPanOffsets(); */
}

void TabbedCodeEditorInterface::onTabMoved(int from, int to) {
    /*qDebug() << "Move from tab" << from << "to tab" << to; */
    tabMovedOrRemoved = true;
    qSwap(files[from], files[to]);
}

void TabbedCodeEditorInterface::onCloseFile(int index) {
    if (index == -1)
        return;

    Q_ASSERT(count() > 0);
    if (maybeSave(index)) {
        if (auto *doc = getDocAt(index)) {
            disconnect(doc, &QTextDocument::modificationChanged,
                       this, &TabbedCodeEditorInterface::onModificationChanged);
            lastRemovedDoc = doc;
        }
        files.remove(index);
        if ((index < getCurIndex()) && (count() > 1)) {
            saveFileData(getCurIndex() - 1);
            tabMovedOrRemoved = true;
        }
        /*qDebug() << "Close tab" << index; */
        ui->tabBar->removeTab(index);
    }
}

void TabbedCodeEditorInterface::onSwitchNextFile() {
    if (!hasNoFile())
        new FileSwitcher(this, false);
}

void TabbedCodeEditorInterface::onSwitchPrevFile() {
    if (!hasNoFile())
        new FileSwitcher(this, true);
}

void TabbedCodeEditorInterface::onCurTextChanged() {
    /*qDebug() << "TabbedCodeEditorInterface::onCurTextChanged"; */
    if (!hasNoFile() && getCurFile()->data.canConvert<TextFileData>()) {
        auto data =
            qvariant_cast<TextFileData>(getCurFile()->data);
        const auto *highlighter = data.highlighter;
        textChangedTimer->start(
            (highlighter) ? highlighter->changedBlocks().length() : 0);
    } else {
        textChangedTimer->stop();
    }
}

void TabbedCodeEditorInterface::onCurTextChangingDone() {
    /*qDebug() << "TabbedCodeEditorInterface::onCurTextChangingDone"; */
    if (const auto *curFile = getCurFile(); curFile &&
        curFile->data.canConvert<TextFileData>()) {
        auto data = qvariant_cast<TextFileData>(getCurFile()->data);
        if (auto *highlighter = data.highlighter) {
            highlighter->checkProblems();
            highlighter->onDocChanged();
            ui->codeEditor->updateErrorSelections();
        }
    }
}

void TabbedCodeEditorInterface::onCurFileChanged(const QString &path) {
    ui->codeEditor->setFilePath(path);
    if (path.isEmpty())
        return;

    if (getCurFile()->data.canConvert<TextFileData>()) {
        auto data = qvariant_cast<TextFileData>(getCurFile()->data);
        ui->codeEditor->setCurHighlighter(
            (!hasNoFile()) ? data.highlighter : nullptr);
    }
}
