#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "statusbar.h"
#include "newdatapackdialog.h"
#include "settingsdialog.h"
#include "aboutdialog.h"
#include "disclaimerdialog.h"
#include "globalhelpers.h"
#include "tabbeddocumentinterface.h"
#include "parsers/command/minecraftparser.h"
#include "imgviewer.h"
#include "visualrecipeeditordock.h"
#include "loottableeditordock.h"
#include "predicatedock.h"
#include "itemmodifierdock.h"
#include "statisticsdialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSettings>
#include <QScreen>
/*#include <QGuiApplication> */
#include <QFileInfo>
#include <QCloseEvent>
#include <QProcess>
#include <QShortcut>
#include <QClipboard>


QMap<QString, QVariantMap> MainWindow::MCRInfoMaps;
QVersionNumber             MainWindow::curGameVersion = QVersionNumber(1, 15);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    readSettings();

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

    if (MainWindow::curGameVersion >= QVersionNumber(1, 17)) {
        itemModifierDock = new ItemModifierDock(this);
        addDockWidget(Qt::RightDockWidgetArea, itemModifierDock);
        itemModifierDock->hide();
    }
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
            this, qOverload<>(&MainWindow::openFolder));
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
    /* Tools menu */
    connect(ui->actionStatistics, &QAction::triggered,
            this, &MainWindow::statistics);
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
            = QFileDialog::getOpenFileName(this, tr("Open file"), QString());
        if (!fileName.isEmpty()) {
            QFileInfo finfo(fileName);
            if (finfo.fileName() == QLatin1String("pack.mcmeta")) {
                QString      errMsg;
                const auto &&packInfo = readPackMcmeta(fileName, errMsg);
                const auto &&dir      = finfo.dir();
                if (folderIsVaild(dir, false) && (packInfo.packFormat > 0)
                    && (QDir::current() != dir)) {
                    loadFolder(dir.path(), packInfo);
                }
            }
            ui->tabbedInterface->openFile(fileName);
        }
    }
}

bool MainWindow::save() {
    if (ui->tabbedInterface->hasNoFile())
        return false;

    if (auto *curFile = ui->tabbedInterface->getCurFile();
        curFile->fileInfo.fileName().isEmpty()) {
        QString filepath
            = QFileDialog::getSaveFileName(this, tr("Save File"), QString());
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

void MainWindow::restart() {
    static const int restartExitCode = 2020;

    qApp->exit(restartExitCode);

    QProcess process;
    process.startDetached(qApp->arguments()[0], qApp->arguments());
}

void MainWindow::statistics() {
    auto *dialog = new StatisticsDialog(this);
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

    const QString &&packMcmetaPath = QDir::currentPath() + QStringLiteral(
        "/pack.mcmeta");
    if (filepath == packMcmetaPath) {
        QString      errMsg;
        const auto &&metaInfo = readPackMcmeta(packMcmetaPath, errMsg);
        if (metaInfo.packFormat > 0) {
            m_packInfo = metaInfo;
            m_statusBar->onCurDirChanged(); /* Update pack format */
        }
    }

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
    if (itemModifierDock)
        itemModifierDock->setVisible(curFileType == CodeFile::ItemModifier);
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

void MainWindow::readPrefSettings(QSettings &settings, bool fromDialog) {
    settings.beginGroup(QStringLiteral("general"));
    loadLanguage(settings.value(QStringLiteral("locale"), QString()).toString(),
                 true);
    const QString gameVer = settings.value(QStringLiteral("gameVersion"),
                                           QStringLiteral("1.15")).toString();

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
            MainWindow::MCRInfoMaps.insert(QStringLiteral("block"),
                                           MainWindow::readMCRInfo(
                                               QStringLiteral("block"),
                                               gameVer));
            MainWindow::MCRInfoMaps.insert(QStringLiteral("item"),
                                           MainWindow::readMCRInfo(
                                               QStringLiteral("item"),
                                               gameVer));
            MainWindow::curGameVersion = QVersionNumber::fromString(gameVer);
            if (curGameVersion >= QVersionNumber(1, 17)) {
                Command::MinecraftParser::setSchema(
                    QStringLiteral(
                        ":/minecraft/") + gameVer +
                    QStringLiteral(
                        "/mcdata/processed/reports/commands/data.min.json"));
            } else {
                Command::MinecraftParser::setSchema(
                    QStringLiteral(
                        ":/minecraft/") + gameVer +
                    QStringLiteral("/mcdata/generated/reports/commands.json"));
            }

            qInfo() << "The game version has been set to" << gameVer;
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

void MainWindow::openFolder(const QString &dirpath) {
    QDir dir(dirpath);

    if (folderIsVaild(dir)) {
        const QString &&packMcmetaPath = dirpath + QStringLiteral(
            "/pack.mcmeta");
        QString      errMsg;
        const auto &&metaInfo = readPackMcmeta(packMcmetaPath, errMsg);
        if (metaInfo.packFormat > 0) {
            loadFolder(dirpath, metaInfo);
        } else {
            QMessageBox::critical(this,
                                  tr("Invaild datapack"),
                                  tr(
                                      "The pack format in the pack.memeta file must be greater than zero."));
        }
    }
}

void MainWindow::loadFolder(const QString &dirPath,
                            const PackMetaInfo &packInfo) {
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    auto &&curDir = QDir::current();

    QDir dir(dirPath);
    QDir::setCurrent(dir.absolutePath());
    ui->datapackTreeView->load(dir);

    if (!curDir.path().isEmpty()) {
        this->fileWatcher.removePath(curDir.path());
        this->fileWatcher.addPath(curDir.path());
    }
    emit curDirChanged(dirPath);
    ui->tabbedInterface->clear();
    updateWindowTitle(false);
    m_packInfo = packInfo;
    m_statusBar->onCurDirChanged();
    adjustForCurFolder(dirPath);

    ui->menuTools->setEnabled(true);

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
}

bool MainWindow::folderIsVaild(const QDir &dir, bool reportError) {
    QDir datadir = dir;

    if (datadir.cd(QStringLiteral("data"))) {
        datadir.cdUp();
        if (datadir.exists(QStringLiteral("pack.mcmeta"))) {
            return true;
        } else if (reportError) {
            QMessageBox::critical(
                this, tr("Can't load datapack"),
                tr("The specified datapack must have a pack.mcmeta file."));
        }
    } else if (reportError) {
        QMessageBox::critical(
            this, tr("Can't load datapack"),
            tr("The specified datapack must contain a /data folder."));
    }
    return false;
}

PackMetaInfo MainWindow::readPackMcmeta(const QString &filepath,
                                        QString &errorMsg) const {
    QFile        file(filepath);
    PackMetaInfo ret;

    if (!file.open(QIODevice::ReadOnly)) {
        errorMsg = file.errorString();
        return ret;
    } else {
        QTextStream     in(&file);
        const QString &&json_string = in.readAll();
        file.close();

        QJsonDocument &&json_doc = QJsonDocument::fromJson(
            json_string.toUtf8());

        if (json_doc.isNull()) {
            errorMsg = QT_TR_NOOP("The file is not a vaild JSON file.");
            return ret;
        }
        if (!json_doc.isObject()) {
            errorMsg = QT_TR_NOOP("The file must contain a JSON object.");
            return ret;
        }

        const QJsonObject &&json_obj = json_doc.object();

        if (json_obj.isEmpty()) {
            errorMsg = QT_TR_NOOP("The file contents is empty.");
            return ret;
        }

        const QVariantMap &&json_map = json_obj.toVariantMap();

        if (json_map.contains(QLatin1String("pack"))) {
            const QVariantMap &&pack = json_map[QLatin1String("pack")].toMap();
            if (pack.contains(QLatin1String("description")) &&
                pack.contains(QLatin1String("pack_format"))) {
                const int packFormat =
                    pack[QLatin1String("pack_format")].toInt();
                if (packFormat > 0) {
                    ret.description =
                        pack[QLatin1String("description")].toString();
                    ret.packFormat = packFormat;
                } else {
                    errorMsg = QT_TR_NOOP(
                        "The pack format number must be greater than 0.");
                }
            } else {
                errorMsg = QT_TR_NOOP(
                    "The %1 object must have the %2 and %3 keys");
                errorMsg = errorMsg.arg(R"("pack")",
                                        R"("description")",
                                        R"("pack_format")");
            }
        } else {
            errorMsg = QT_TR_NOOP("The root object must have the %1 section.");
            errorMsg = errorMsg.arg(R"("pack")");
        }
    }
    return ret;
}

QString MainWindow::getCurLocale() {
    return this->curLocale.name();
}

void MainWindow::loadLanguage(const QString &rLanguage, bool atStartup) {
    if ((curLocale.bcp47Name() != rLanguage) || atStartup) {
        curLocale = (rLanguage.isEmpty()) ? QLocale::system() : QLocale(
            rLanguage);
        QLocale::setDefault(curLocale);
        const QString &&langCode        = curLocale.bcp47Name();
        QString       &&translationFile = QString("MCDatapacker_%1").arg(
            langCode);
        switchTranslator(m_translatorQt, QString("qt_%1").arg(langCode));
        switchTranslator(m_translator, translationFile);
    }
}

void MainWindow::switchTranslator(QTranslator &translator,
                                  const QString &filename) {
    qApp->removeTranslator(&translator);

    const QString &&filepath = QApplication::applicationDirPath() +
                               QStringLiteral("/translations/") + filename;
    if (translator.load(filepath)) {
        qApp->installTranslator(&translator);
        qInfo() << "External translation file has been loaded successfully:" <<
            filename;
    } else {
        qWarning() << "Cannot load external translation file:" << filename <<
            "Attempting to load the corresponding internal translation instead.";
        if (translator.load(filename, QStringLiteral(":/i18n/"))) {
            qApp->installTranslator(&translator);
            qInfo() <<
                "Internal translation file has been loaded successfully:" <<
                filename;
        } else {
            qWarning() << "Cannot load internal translation file:" << filename;
        }
    }
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

    const auto recentPathsSize = recentPaths.size();

    if (recentPathsSize <= maxRecentFoldersActions)
        itEnd = recentPathsSize;
    else
        itEnd = maxRecentFoldersActions;

    ui->menuRecentDatapacks->setEnabled(recentPathsSize > 0);
    for (auto i = 0u; i < itEnd; ++i) {
        auto *action = recentFoldersActions.at(i);
        action->setText(QString("&%1 | %2").arg(i + 1).arg(recentPaths.at(i)));
        action->setData(recentPaths.at(i));
        action->setVisible(true);
        QDir dir(recentPaths.at(i));
        action->setEnabled(dir.exists() && dir.isReadable());
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

        loadFolder(dirPath,
                   PackMetaInfo{ dialog->getDesc(), dialog->getFormat() });
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
        if (!dirPath.isEmpty()) {
            openFolder(dirPath);
        }
    }
}

void MainWindow::openRecentFolder() {
    if (maybeSave()) {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action)
            openFolder(action->data().toString());
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
