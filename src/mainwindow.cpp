#include "imgviewer.h"
#include "ui_mainwindow.h"

#include "statusbar.h"
#include "newdatapackdialog.h"
#include "settingsdialog.h"
#include "aboutdialog.h"
#include "disclaimerdialog.h"
#include "globalhelpers.h"
#include "tabbeddocumentinterface.h"
#include "parsers/command/minecraftparser.h"
#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QScreen>
#include <QGuiApplication>
#include <QFileInfo>
#include <QCloseEvent>
#include <QProcess>
#include <QShortcut>
#include <QClipboard>


QMap<QString, QVariantMap > MainWindow::MCRInfoMaps;
QVersionNumber              MainWindow::curGameVersion = QVersionNumber(1, 15);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    readSettings();
    if (curGameVersion >= QVersionNumber(1, 17)) {
        Command::MinecraftParser::setSchema(
            QStringLiteral(
                ":/minecraft/") + MainWindow::curGameVersion.toString() +
            QStringLiteral("/mcdata/processed/reports/commands/data.min.json"));
    } else {
        Command::MinecraftParser::setSchema(
            QStringLiteral(
                ":/minecraft/") + MainWindow::curGameVersion.toString() +
            QStringLiteral("/mcdata/generated/reports/commands.json"));
    }

    MainWindow::MCRInfoMaps.insert(QStringLiteral("block"),
                                   MainWindow::readMCRInfo(QStringLiteral(
                                                               "block")));
    MainWindow::MCRInfoMaps.insert(QStringLiteral("item"),
                                   MainWindow::readMCRInfo(QStringLiteral(
                                                               "item")));

    /*qDebug() << MainWindow::getMCRInfo("blockTag").count(); */

    m_statusBar = new StatusBar(this, ui->tabbedInterface);
    setStatusBar(m_statusBar);

    initMenu();
    connect(&fileWatcher, &QFileSystemWatcher::fileChanged,
            this, &MainWindow::onSystemWatcherFileChanged);
    connect(ui->datapackTreeView, &DatapackTreeView::fileRenamed,
            ui->tabbedInterface, &TabbedDocumentInterface::onFileRenamed);
    connect(ui->tabbedInterface,
            &TabbedDocumentInterface::curModificationChanged,
            this, &MainWindow::updateWindowTitle);
    connect(ui->datapackTreeView, &DatapackTreeView::datapackChanged,
            this, &MainWindow::onDatapackChanged);
    connect(ui->tabbedInterface, &TabbedDocumentInterface::curFileChanged,
            this, &MainWindow::onCurFileChanged);
    connect(ui->datapackTreeView, &DatapackTreeView::openFileRequested,
            ui->tabbedInterface, &TabbedDocumentInterface::onOpenFile);
    connect(this, &MainWindow::gameVersionChanged, ui->tabbedInterface,
            &TabbedDocumentInterface::onGameVersionChanged);
    connect(ui->tabbedInterface->getCodeEditor(),
            &CodeEditor::updateStatusBarRequest,
            m_statusBar, &StatusBar::updateCodeEditorStatus);
    connect(ui->tabbedInterface->getCodeEditor(),
            &CodeEditor::showMessageRequest,
            m_statusBar, &StatusBar::showMessage);
    connect(ui->tabbedInterface->getImgViewer(),
            &ImgViewer::updateStatusBarRequest,
            m_statusBar, &StatusBar::updateImgViewerStatus);

    #ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
    #endif

    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/icon");
    /*qDebug() << QIcon::fallbackSearchPaths(); */

    initDocks();
}

void MainWindow::initDocks() {
    visualRecipeEditorDock = new VisualRecipeEditorDock(this);
    addDockWidget(Qt::RightDockWidgetArea, visualRecipeEditorDock);
    visualRecipeEditorDock->hide();

    lootTableEditorDock = new LootTableEditorDock(this);
    addDockWidget(Qt::BottomDockWidgetArea, lootTableEditorDock);
    lootTableEditorDock->hide();

    predicateDock = new PredicateDock(this);
    addDockWidget(Qt::RightDockWidgetArea, predicateDock);
    predicateDock->hide();
}

void MainWindow::initMenu() {
    for (int i = 0; i < maxRecentFoldersActions; ++i) {
        auto *recentFolderAction = new QAction(this);
        recentFolderAction->setVisible(false);
        QObject::connect(recentFolderAction, &QAction::triggered,
                         this, &MainWindow::openRecentFolder);
        recentFoldersActions.append(recentFolderAction);
        ui->menuRecentDatapacks->addAction(recentFolderAction);
    }
    updateRecentFolders();

    /* File menu */
    connect(ui->actionNewDatapack, &QAction::triggered,
            this, &MainWindow::newDatapack);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::open);
    connect(ui->actionOpenFolder, &QAction::triggered,
            this, &MainWindow::openFolder);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionSaveAll, &QAction::triggered, this, &MainWindow::saveAll);
    connect(ui->actionRestart, &QAction::triggered, this, &MainWindow::restart);
    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);
    /* Edit menu */
    connect(ui->actionUndo, &QAction::triggered,
            ui->tabbedInterface, &TabbedDocumentInterface::undo);
    connect(ui->actionRedo, &QAction::triggered,
            ui->tabbedInterface, &TabbedDocumentInterface::redo);
    connect(ui->actionSelectAll, &QAction::triggered,
            ui->tabbedInterface, &TabbedDocumentInterface::selectAll);
    connect(ui->actionCut, &QAction::triggered,
            ui->tabbedInterface, &TabbedDocumentInterface::cut);
    connect(ui->actionCopy, &QAction::triggered,
            ui->tabbedInterface, &TabbedDocumentInterface::copy);
    connect(ui->actionPaste, &QAction::triggered,
            ui->tabbedInterface, &TabbedDocumentInterface::paste);
    /* Preferences menu */
    connect(ui->actionSettings, &QAction::triggered,
            this, &MainWindow::pref_settings);
    /* Help menu */
    connect(ui->actionAboutApp, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionAboutQt, &QAction::triggered, [this]() {
        QMessageBox::aboutQt(this);
    });
    connect(ui->actionDisclaimer, &QAction::triggered, this,
            &MainWindow::disclaimer);

    /* Menu items status update connections */
    connect(ui->tabbedInterface->getStackedWidget(),
            &QStackedWidget::currentChanged, this, &MainWindow::updateEditMenu);
    connect(ui->tabbedInterface->getCodeEditor(),
            &QPlainTextEdit::copyAvailable, this, &MainWindow::updateEditMenu);
    connect(ui->tabbedInterface->getCodeEditor(),
            &QPlainTextEdit::undoAvailable, this, &MainWindow::updateEditMenu);
    connect(ui->tabbedInterface->getCodeEditor(),
            &QPlainTextEdit::redoAvailable, this, &MainWindow::updateEditMenu);
    connect(qApp->clipboard(), &QClipboard::changed, this,
            &MainWindow::updateEditMenu);
    ui->actionRedo->setShortcutContext(Qt::ApplicationShortcut);
}

void MainWindow::open() {
    if (maybeSave()) {
        QString fileName
            = QFileDialog::getOpenFileName(this, tr("Open file"), "");
        if (!fileName.isEmpty())
            ui->tabbedInterface->openFile(fileName);
    }
}

bool MainWindow::save() {
    if (ui->tabbedInterface->hasNoFile())
        return false;

    if (auto *curFile = ui->tabbedInterface->getCurFile();
        curFile->fileInfo.fileName().isEmpty()) {
        QString filepath
            = QFileDialog::getSaveFileName(this, tr("Save File"), "");
        if (!filepath.isEmpty())
            return ui->tabbedInterface->saveCurFile(filepath);
        else
            return false;
    } else {
        return ui->tabbedInterface->saveCurFile();
    }
}

void MainWindow::saveAll() {
    ui->tabbedInterface->saveAllFile();
}

void MainWindow::pref_settings() {
    SettingsDialog dialog(this);

    dialog.exec();
}

void MainWindow::about() {
    auto *dialog = new AboutDialog(this);

    dialog->open();
}

void MainWindow::disclaimer() {
    auto *dialog = new DisclaimerDialog(this);

    dialog->open();
}

void MainWindow::onSystemWatcherFileChanged(const QString &filepath) {
    qDebug() << "onSystemWatcherFileChanged" << filepath;
    if ((filepath != ui->tabbedInterface->getCurFilePath())) return;

    auto reloadExternChanges = QSettings().value("general/reloadExternChanges",
                                                 0);
    if (reloadExternChanges == 1) {
        if (uniqueMessageBox != nullptr) return;

        uniqueMessageBox = new QMessageBox(this);
        uniqueMessageBox->setIcon(QMessageBox::Question);
        uniqueMessageBox->setWindowTitle(tr("Reload file"));
        uniqueMessageBox->setText(tr("The file %1 has been changed exernally.")
                                  .arg(QDir::toNativeSeparators(filepath)));
        uniqueMessageBox->setInformativeText(tr(
                                                 "Do you want to reload this file?"));
        uniqueMessageBox->setStandardButtons(QMessageBox::Yes |
                                             QMessageBox::No);
        uniqueMessageBox->setDefaultButton(QMessageBox::Yes);
        uniqueMessageBox->exec();
        auto userDecision = uniqueMessageBox->result();
        delete uniqueMessageBox;
        uniqueMessageBox    = nullptr;
        reloadExternChanges = (userDecision == QMessageBox::Yes) ? 0 : 2;
    }

    if (reloadExternChanges == 0) {
        /*qDebug() << filepath << fileWatcher.files().contains(filepath); */
    }
}

void MainWindow::onCurFileChanged(const QString &path) {
    if (!path.isEmpty()) {
        fileWatcher.removePath(path);
        fileWatcher.addPath(path);
    }

    auto curFileType = CodeFile::Text;
    if (auto *curFile = ui->tabbedInterface->getCurFile())
        curFileType = curFile->fileType;
    lootTableEditorDock->setVisible(curFileType == CodeFile::LootTable);
    visualRecipeEditorDock->setVisible(curFileType == CodeFile::Recipe);
    predicateDock->setVisible(curFileType == CodeFile::Predicate);
    m_statusBar->onCurFileChanged();
}

void MainWindow::onDatapackChanged() {
    ui->tabbedInterface->clear();
    updateWindowTitle(false);
    m_statusBar->onCurDirChanged();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    /*qDebug() << "closeEvent"; */
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::readSettings() {
    QSettings settings;

    settings.beginGroup("geometry");

    if (settings.value("isMaximized", true).toBool()) {
        showMaximized();
    } else {
        QSize geomertySize = settings.value("size", QSize(600, 600)).toSize();
        if (geomertySize.isEmpty())
            adjustSize();
        else
            resize(geomertySize);

        QRect availableGeometry =
            QGuiApplication::primaryScreen()->availableGeometry();
        move(settings.value("pos",
                            QPoint((availableGeometry.width() - width()) / 2,
                                   (availableGeometry.height() - height()) / 2))
             .toPoint());
    }
    settings.endGroup();

    readPrefSettings(settings);
}

void MainWindow::restart() {
    static const int restartExitCode = 2020;

    qApp->exit(restartExitCode);

    QProcess process;
    process.startDetached(qApp->arguments()[0], qApp->arguments());
}

void MainWindow::readPrefSettings(QSettings &settings, bool fromDialog) {
    settings.beginGroup("general");
    /*qDebug() << settings.value("locale", "").toString(); */
    loadLanguage(settings.value("locale", "").toString(), true);
    const QString gameVer = settings.value("gameVersion", "1.15").toString();

    if (gameVer != getCurGameVersion().toString()) {
        if (fromDialog) {
            QMessageBox msgBox(QMessageBox::Warning,
                               tr("Changing game version"),
                               tr(
                                   "The game version has been changed from %1 to %2\n"
                                   "The program need to restart to apply the changes.")
                               .arg(
                                   getCurGameVersion().toString(), gameVer));
            QPushButton* restartBtn = msgBox.addButton(tr("Restart"),
                                                       QMessageBox::YesRole);
            msgBox.setDefaultButton(restartBtn);
            msgBox.addButton(tr("Keep"), QMessageBox::NoRole);

            msgBox.exec();
            if (msgBox.clickedButton() == restartBtn) {
                restart();
            } else {
                settings.setValue("gameVersion",
                                  getCurGameVersion().toString());
            }
        } else {
            qDebug() << "Game version was changed to" << gameVer;
            MainWindow::MCRInfoMaps.insert(QStringLiteral("block"),
                                           MainWindow::readMCRInfo(
                                               QStringLiteral("block"),
                                               gameVer));
            MainWindow::MCRInfoMaps.insert(QStringLiteral("item"),
                                           MainWindow::readMCRInfo(
                                               QStringLiteral("item"),
                                               gameVer));
            MainWindow::curGameVersion = QVersionNumber::fromString(gameVer);
            emit gameVersionChanged(gameVer);
        }
    }
    settings.endGroup();
    ui->tabbedInterface->getCodeEditor()->readPrefSettings();
}

void MainWindow::writeSettings() {
    QSettings settings(QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());

    settings.beginGroup("geometry");
    settings.setValue("isMaximized", isMaximized());
    if (!isMaximized()) {
        settings.setValue("size", size());
        settings.setValue("pos", pos());
    }
    settings.endGroup();
}

bool MainWindow::maybeSave() {
    if (!ui->tabbedInterface->hasUnsavedChanges())
        return true;

    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this,
                               tr("Unsaved changes"),
                               tr("Some document(s) has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard |
                               QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return ui->tabbedInterface->saveAllFile();

    case QMessageBox::Cancel:
        return false;

    default:
        break;
    }
    return true;
}

void MainWindow::loadFolder(const QString &dirPath) {
    const QString &&pack_mcmeta = dirPath + QStringLiteral("/pack.mcmeta");

    if (QFile::exists(pack_mcmeta)) {
        if (pack_mcmeta.isEmpty()) {
            return;
        } else {
#ifndef QT_NO_CURSOR
            QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif

            QFile file(pack_mcmeta);
            if (!file.open(QIODevice::ReadOnly)) {
                QMessageBox::information(this, tr("Error"), file.errorString());
            } else {
                QTextStream     in(&file);
                const QString &&json_string = in.readAll();
                file.close();

                QJsonDocument &&json_doc = QJsonDocument::fromJson(
                    json_string.toUtf8());

                if (json_doc.isNull()) {
#ifndef QT_NO_CURSOR
                    QGuiApplication::restoreOverrideCursor();
#endif
                    QMessageBox::information(this,
                                             "pack.mcmeta error",
                                             tr(
                                                 "Failed to parse the pack.mcmeta file."));
                    return;
                }
                if (!json_doc.isObject()) {
#ifndef QT_NO_CURSOR
                    QGuiApplication::restoreOverrideCursor();
#endif
                    QMessageBox::information(this,
                                             "pack.mcmeta error",
                                             tr(
                                                 "The pack.mcmeta file is not a JSON object."));
                    return;
                }

                QJsonObject json_obj = json_doc.object();

                if (json_obj.isEmpty()) {
#ifndef QT_NO_CURSOR
                    QGuiApplication::restoreOverrideCursor();
#endif
                    QMessageBox::information(this,
                                             "pack.mcmeta error",
                                             tr(
                                                 "The pack.mcmeta file's contents is empty."));
                    return;
                }

                QVariantMap json_map = json_obj.toVariantMap();

                auto &&curDir = QDir::current();
                if (json_map.contains("pack")) {
                    QVariantMap &&pack = json_map["pack"].toMap();
                    if (pack.contains(QStringLiteral("description")) &&
                        pack.contains(QStringLiteral("pack_format"))) {
                        QDir dir(dirPath);
                        m_packInfo = PackMetaInfo{
                            pack["description"].toString(),
                            pack["pack_format"].toInt() };
                        QDir::setCurrent(dir.absolutePath());
                        ui->datapackTreeView->load(dir);

                        if (!curDir.path().isEmpty()) {
                            this->fileWatcher.removePath(curDir.path());
                            this->fileWatcher.addPath(curDir.path());
                        }
                        ui->tabbedInterface->clear();
                        emit curDirChanged(dirPath);
                        adjustForCurFolder(dirPath);
#ifndef QT_NO_CURSOR
                        QGuiApplication::restoreOverrideCursor();
#endif
                    }
                }
            }
        }
    } else {
        QMessageBox::information(this, tr("Error"),
                                 tr("Invaild datapack folder."));
        return;
    }
}

QString MainWindow::getCurLocale() {
    return this->curLocale.name();
}

void MainWindow::loadLanguage(const QString& rLanguage, bool atStartup) {
    if ((curLocale.name() != rLanguage) || atStartup) {
        curLocale = QLocale(rLanguage);
        QLocale::setDefault(curLocale);
        /*qDebug() << QLocale::system(); */
        QString transfile;
        if (rLanguage.isEmpty())
            transfile = QString("MCDatapacker_%1.qm").arg(
                QLocale::system().name());
        else
            transfile = QString("MCDatapacker_%1.qm").arg(rLanguage);
        switchTranslator(m_translator, transfile);
        switchTranslator(m_translatorQt, QString("qt_%1.qm").arg(rLanguage));
    }
}

void MainWindow::switchTranslator(QTranslator& translator,
                                  const QString& filename) {
    qApp->removeTranslator(&translator);

    QString path = QApplication::applicationDirPath() + QStringLiteral(
        "/translations/");
    if (translator.load(path + filename))
        qApp->installTranslator(&translator);
    else if (translator.load(QStringLiteral(":/i18n/") + filename))
        qApp->installTranslator(&translator);
}

void MainWindow::adjustForCurFolder(const QString &path) {
    QSettings   settings;
    QStringList recentPaths =
        settings.value(QStringLiteral("recentFolders")).toStringList();

    recentPaths.removeAll(path);
    recentPaths.prepend(path);
    while (recentPaths.size() > maxRecentFoldersActions)
        recentPaths.removeLast();
    settings.setValue(QStringLiteral("recentFolders"), recentPaths);

    updateRecentFolders();
}

void MainWindow::updateRecentFolders() {
    QSettings     settings;
    QStringList &&recentPaths =
        settings.value(QStringLiteral("recentFolders")).toStringList();

    auto itEnd = 0u;

    if (recentPaths.size() <= maxRecentFoldersActions)
        itEnd = recentPaths.size();
    else
        itEnd = maxRecentFoldersActions;

    for (auto i = 0u; i < itEnd; ++i) {
        auto *action = recentFoldersActions.at(i);
        action->setText(QString("&%1 | %2").arg(i + 1).arg(recentPaths.at(i)));
        action->setData(recentPaths.at(i));
        action->setVisible(true);
    }

    for (int i = itEnd; i < maxRecentFoldersActions; ++i)
        recentFoldersActions.at(i)->setVisible(false);
}

void MainWindow::updateEditMenu() {
    if (ui->tabbedInterface->getStackedWidget()->currentIndex() == 1) {
        ui->actionUndo->setEnabled(
            ui->tabbedInterface->getCodeEditor()->getCanUndo());
        ui->actionRedo->setEnabled(
            ui->tabbedInterface->getCodeEditor()->getCanRedo());
        ui->actionSelectAll->setEnabled(true);
        const bool hasSelection =
            ui->tabbedInterface->getCodeEditor()->textCursor().hasSelection();
        ui->actionCut->setEnabled(hasSelection);
        ui->actionCopy->setEnabled(hasSelection);
        ui->actionPaste->setEnabled(
            ui->tabbedInterface->getCodeEditor()->canPaste());
    } else {
        ui->actionUndo->setEnabled(false);
        ui->actionRedo->setEnabled(false);
        ui->actionSelectAll->setEnabled(false);
        ui->actionCut->setEnabled(false);
        ui->actionCopy->setEnabled(false);
        ui->actionPaste->setEnabled(false);
    }
}

void MainWindow::changeEvent(QEvent* event) {
    if (event != nullptr) {
        switch (event->type()) {
        /* this event is send if a translator is loaded */
        case QEvent::LanguageChange: {
            /*qDebug() << "QEvent::LanguageChange"; */
            ui->retranslateUi(this);
            break;
        }

        /* this event is send, if the system language changes */
        case QEvent::LocaleChange: {
            /*qDebug() << "QEvent::LocaleChange"; */
            QString locale = QLocale::system().name();
            loadLanguage(locale);
            break;
        }

        default:
            break;
        }
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::commitData(QSessionManager &) {
}

PackMetaInfo MainWindow::getPackInfo() const {
    return m_packInfo;
}

QVersionNumber MainWindow::getCurGameVersion() {
    return curGameVersion;
}

void MainWindow::updateWindowTitle(bool changed) {
    QStringList titleParts;

    if (!ui->tabbedInterface->hasNoFile()) {
        if (const auto &&curPath = ui->tabbedInterface->getCurFilePath();
            !curPath.isEmpty())
            titleParts << QFileInfo(curPath).fileName() + "[*]";
        else
            titleParts << QStringLiteral("Untitled") + "[*]";
    }
    auto curDir = QDir::current();
    if (curDir.exists())
        titleParts << "[" + curDir.dirName() + "]";
    titleParts << QCoreApplication::applicationName();
    setWindowTitle(titleParts.join(QStringLiteral(" - ")));
    this->setWindowModified(changed);
}

QVariantMap MainWindow::readMCRInfo(const QString &type, const QString &ver,
                                    [[maybe_unused]] const int depth) {
    QVariantMap retMap;

    QFileInfo finfo(":minecraft/" + ver + "/" + type + ".json");

    /*qDebug() << "readMCRInfo" << type << ver << finfo << finfo.exists(); */

    if (!finfo.exists())
        finfo.setFile(QStringLiteral(":minecraft/1.15/") + type + ".json");

    if (!(finfo.exists() && finfo.isFile())) {
        qWarning() << "File not exists:" << finfo.path() << "Return empty.";
        return retMap;
    }
    /*qDebug() << finfo; */
    QFile inFile(finfo.filePath());
    inFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray &&data = inFile.readAll();
    inFile.close();

    QJsonParseError errorPtr;
    QJsonDocument &&doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        qWarning() << "Parse failed" << errorPtr.error;
        return retMap;
    }
    QJsonObject root = doc.object();
    if (root.isEmpty()) {
        /*qDebug() << "Root is empty. Return empty"; */
        return retMap;
    }

    if (root.contains("base")) {
        const auto &tmpMap = MainWindow::readMCRInfo(type,
                                                     root["base"].toString(),
                                                     depth);
        retMap.insert(std::move(tmpMap));
        root.remove("base");
    }
    if (root.contains("removed")) {
        for (const auto &keyRef: root["removed"].toArray()) {
            const QString &key = keyRef.toString();
            if (retMap.contains(key))
                retMap.remove(key);
        }
        root.remove("removed");
    }
    if (root.contains("added"))
        retMap.insert(root["added"].toVariant().toMap());
    else
        retMap.insert(root.toVariantMap());
    return retMap;
}

QVariantMap &MainWindow::getMCRInfo(const QString &type) {
    return MainWindow::MCRInfoMaps[type];
}

void MainWindow::newDatapack() {
    auto *dialog     = new NewDatapackDialog(this);
    int   dialogCode = dialog->exec();

    if (dialogCode == 1) {
        if (maybeSave()) {
            ui->tabbedInterface->clear();
        } else {
            return;
        }
        const QString &&dirPath = dialog->getDirPath() + "/" +
                                  dialog->getName();
        QDir dir(dirPath);
        if (!dir.exists()) {
            dir.mkpath(dirPath);
        } else if (!dir.isEmpty()) {
            if (QMessageBox::question(this, tr("Folder not empty"),
                                      tr("The folder is not empty.\n"
                                         "Do you want to recreate this folder?"),
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::No)
                == QMessageBox::No) {
                return;
            } else {
                dir.removeRecursively();
                dir.mkpath(dirPath);
            }
        }

        QFile file(dirPath + QStringLiteral("/pack.mcmeta"));
        if (file.open(QIODevice::ReadWrite)) {
            QJsonObject root {
                { "pack", QJsonObject {
                      { "description", dialog->getDesc() },
                      { "pack_format", dialog->getFormat() }
                  }
                },
            };

            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            stream << QJsonDocument(root).toJson();
        }
        file.close();

        dir.mkpath(dirPath + QStringLiteral("/data"));
        QString namesp = dialog->getName()
                         .toLower()
                         .replace(" ", "_").replace("/", "_")
                         .replace(".", "_").replace(":", "_");
        dir.mkpath(dirPath + QStringLiteral("/data/") + namesp);

        loadFolder(dirPath);
        ui->tabbedInterface->openFile("pack.mcmeta");
    }
    delete dialog;
}

void MainWindow::openFolder() {
    if (maybeSave()) {
        const QString &&dirPath =
            QFileDialog::getExistingDirectory(this, tr("Open datapack folder"),
                                              QString(),
                                              QFileDialog::ShowDirsOnly |
                                              QFileDialog::DontResolveSymlinks);
        if (!dirPath.isEmpty())
            loadFolder(dirPath);
    }
}

void MainWindow::openRecentFolder() {
    if (maybeSave()) {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action)
            loadFolder(action->data().toString());
    }
}

void MainWindow::setCodeEditorText(const QString &text) {
    if (ui->tabbedInterface->hasNoFile())
        return;

    /*ui->codeEditor->setPlainText(text); */
    QTextCursor cursor = ui->tabbedInterface->getCodeEditor()->textCursor();

    cursor.beginEditBlock();
    cursor.select(QTextCursor::Document);
    cursor.insertText(text);
    cursor.endEditBlock();
    ui->tabbedInterface->getCodeEditor()->setTextCursor(cursor);
}

QString MainWindow::getCodeEditorText() {
    if (auto *doc = ui->tabbedInterface->getCurDoc())
        return ui->tabbedInterface->getCurDoc()->toPlainText();
    else
        return QString();
}

MainWindow::~MainWindow() {
    delete ui;
}
