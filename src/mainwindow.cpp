#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "statusbar.h"
#include "newdatapackdialog.h"
#include "settingsdialog.h"
#include "aboutdialog.h"
#include "disclaimerdialog.h"
#include "tabbeddocumentinterface.h"
#include "parsers/command/minecraftparser.h"
#include "parsers/command/schema/schemaloader.h"
#include "visualrecipeeditordock.h"
#include "loottableeditordock.h"
#include "predicatedock.h"
#include "itemmodifierdock.h"
#include "advancementtabdock.h"
#include "statisticsdialog.h"
#include "rawjsontexteditor.h"
#include "darkfusionstyle.h"
#include "norwegianwoodstyle.h"

#include "game.h"
#include "platforms/windows_specific.h"

#include "QSimpleUpdater.h"
#include "zip.hpp"
#include "SystemThemeHelper.h"
#include <oclero/qlementine.hpp>

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
#include <QProgressDialog>
#include <QSaveFile>
#include <QDesktopServices>

static const QString updateDefUrl = QStringLiteral(
    "https://raw.githubusercontent.com/IoeCmcomc/MCDatapacker/master/updates.json");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
    m_systemThemeHelper{new libqdark::SystemThemeHelper(this)} {
    ui->setupUi(this);

    m_initialStyleId = style()->objectName();
    moveOldSettings();
    readSettings();
    loadLanguage(locale().name());

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
    connect(ui->tabbedInterface,
            &TabbedDocumentInterface::updateStatusBarRequest,
            m_statusBar, &StatusBar::updateStatusFrom);
    connect(ui->tabbedInterface,
            &TabbedDocumentInterface::showMessageRequest,
            m_statusBar, &StatusBar::showMessage);

    #ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
    #endif

    initDocks();

    auto *updater = QSimpleUpdater::getInstance();
#ifdef Q_OS_WIN64
    updater->setPlatformKey(updateDefUrl, "windows-x64");
#endif
    updater->setModuleVersion(updateDefUrl,
                              QCoreApplication::applicationVersion());
    updater->setUseCustomInstallProcedures(updateDefUrl, true);
    updater->setDownloaderEnabled(updateDefUrl, true);
    updater->setDownloadDir(updateDefUrl, qApp->applicationDirPath());
    connect(updater, &QSimpleUpdater::downloadFinished,
            this, &MainWindow::installUpdate);

    const QString &oldProgramFile = qApp->applicationDirPath() +
                                    QLatin1String("/MCDatapacker_old");
    if (QFile::exists(oldProgramFile))
        QFile::remove(oldProgramFile);

//    updater->setNotifyOnFinish(updateDefUrl, false);
//    updater->checkForUpdates(updateDefUrl);

#ifdef Q_OS_WIN
    QObject::connect(m_systemThemeHelper,
                     &libqdark::SystemThemeHelper::signalLightTheme,
                     this, [this]{
        onColorModeChanged(false);
    });
    QObject::connect(m_systemThemeHelper,
                     &libqdark::SystemThemeHelper::signalDarkTheme,
                     this, [this]{
        onColorModeChanged(true);
    });
    m_systemThemeHelper->setEnabled(true);
#endif
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

    advancementsDock = new AdvancementTabDock(this);
    addDockWidget(Qt::BottomDockWidgetArea, advancementsDock);
    connect(advancementsDock, &AdvancementTabDock::openFileRequested,
            ui->tabbedInterface, &TabbedDocumentInterface::onOpenFile);

    if (Game::version() >= Game::v1_17) {
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
    connect(ui->actionRawJsonTextEditor, &QAction::triggered,
            this, &MainWindow::rawJsonTextEditor);
    /* Preferences menu */
    connect(ui->actionSettings, &QAction::triggered,
            this, &MainWindow::pref_settings);
    /* Help menu */
    initResourcesMenu();
    connect(ui->actionAboutApp, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionCheckForUpdates, &QAction::triggered,
            this, &MainWindow::checkForUpdates);
    connect(ui->actionDisclaimer, &QAction::triggered, this,
            &MainWindow::disclaimer);
    connect(ui->actionAboutQt, &QAction::triggered, this, [this]() {
        QMessageBox::aboutQt(this);
    });

    /* Menu items status update connections */
    connect(ui->tabbedInterface,
            &TabbedDocumentInterface::updateEditMenuRequest, this,
            &MainWindow::updateEditMenu);
    connect(qApp->clipboard(), &QClipboard::changed, this,
            &MainWindow::updateEditMenu);
    ui->actionRedo->setShortcutContext(Qt::ApplicationShortcut);
}

void MainWindow::connectActionLink(QAction *action, const QString &&url) {
    connect(action, &QAction::triggered, this, [url](){
        QDesktopServices::openUrl(QUrl(url));
    });
}

void MainWindow::initResourcesMenu() {
    // Online tools
    connectActionLink(ui->actionUuidConverter, QStringLiteral(
                          R"(https://www.soltoder.com/mc-uuid-converter/)"));
    connectActionLink(ui->actionSnowcapped, QStringLiteral(
                          R"(https://snowcapped.jacobsjo.eu/)"));
    connectActionLink(ui->actionGenerators, QStringLiteral(
                          R"(https://misode.github.io/)"));
    connectActionLink(ui->actionUpgrader, QStringLiteral(
                          R"(https://misode.github.io/upgrader/)"));
    connectActionLink(ui->actionMcstacker, QStringLiteral(
                          R"(https://mcstacker.net/)"));
    connectActionLink(ui->actionMinecraft_Tools, QStringLiteral(
                          R"(https://minecraft.tools/en/)"));
    // Offline tools
    connectActionLink(ui->actionPackSquash, QStringLiteral(
                          R"(https://github.com/ComunidadAylas/PackSquash/)"));
    // Minecraft mods
    connectActionLink(ui->actionBetterCommandBlockUi, QStringLiteral(
                          R"(https://modrinth.com/mod/bettercommandblockui/)"));
    connectActionLink(ui->actionDataReload, QStringLiteral(
                          R"(https://modrinth.com/mod/data-reload/)"));
    connectActionLink(ui->actionNbtAutocomplete, QStringLiteral(
                          R"(https://modrinth.com/mod/nbt-autocomplete/)"));
    connectActionLink(ui->actionCommandHelper, QStringLiteral(
                          R"(https://github.com/SomeKitten/CommandHelper/)"));
    connectActionLink(ui->actionNbtTooltips, QStringLiteral(
                          R"(https://modrinth.com/mod/nbttooltips)"));
    connectActionLink(ui->actionCommand_Extractor, QStringLiteral(
                          R"(https://modrinth.com/mod/command_extractor)"));
    connectActionLink(ui->actionDatamancer, QStringLiteral(
                          R"(https://modrinth.com/mod/datamancer)"));
    // Datapack distribution platforms
    connectActionLink(ui->actionPlanetMinecraft, QStringLiteral(
                          R"(https://www.planetminecraft.com)"));
    connectActionLink(ui->actionModrinth, QStringLiteral(
                          R"(https://modrinth.com)"));
    connectActionLink(ui->actionSmithed, QStringLiteral(
                          R"(https://smithed.net)"));
    connectActionLink(ui->actionDatapack_Hub,
                      QStringLiteral(R"(https://datapackhub.net/)"));
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
        curFile->name().isEmpty()) {
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
    static const int restartExitCode = 2022;

    qApp->exit(restartExitCode);

    QProcess process;
    process.startDetached(qApp->arguments()[0], qApp->arguments());
}

void MainWindow::statistics() {
    auto *dialog = new StatisticsDialog(this);

    connect(dialog, &StatisticsDialog::openFileWithLineRequested,
            ui->tabbedInterface, &TabbedDocumentInterface::onOpenFileWithLine);
}

void MainWindow::rawJsonTextEditor() {
    auto *editor = new RawJsonTextEditor(this);

#ifdef Q_OS_LINUX
    editor->setWindowFlags(
        Qt::Window | Qt::FramelessWindowHint | Qt::CustomizeWindowHint | Qt::WindowTitleHint |
        Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
#else
    editor->setWindowFlags(
        Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint |
        Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
#endif
    editor->setWindowModality(Qt::NonModal);
    editor->setAttribute(Qt::WA_DeleteOnClose);
    editor->show();
    editor->activateWindow();
}

void MainWindow::pref_settings() {
    SettingsDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        QSettings settings;
        readPrefSettings(settings, true);
    }
}

void MainWindow::about() {
    auto *dialog = new AboutDialog(this);

    dialog->open();
}

void MainWindow::checkForUpdates() {
    auto *updater = QSimpleUpdater::getInstance();

    updater->setNotifyOnUpdate(updateDefUrl, false);
    updater->setNotifyOnFinish(updateDefUrl, true);

    updater->checkForUpdates(updateDefUrl);
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

    auto reloadExternChanges = QSettings().value(
        "general/reloadExternChanges", 0);
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
        uniqueMessageBox->deleteLater();
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
    updateEditMenu();
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
    const static int startupFontSize = qApp->font().pointSize();

    settings.beginGroup(QStringLiteral("interface"));

    Windows::setDarkFrame(this, Windows::isDarkMode());
    changeAppStyle(Windows::isDarkMode());

    qInfo() << "The application style has been set to" << qApp->style();

    const QString &localeCode =
        settings.value(QStringLiteral("locale"), QString()).toString();
    setLocale(localeCode.isEmpty() ? QLocale::system().name() : localeCode);

    const auto &&fontScale = settings.value(
        QStringLiteral("fontSizeScale"), QStringLiteral("100"));
    bool      ok    = false;
    const int scale = fontScale.toInt(&ok);
    if (ok && (fromDialog || scale != 100)) {
        auto &&appFont = qApp->font();
        appFont.setPointSize(startupFontSize / 100.0 * scale);
        qApp->setFont(appFont);
    }

    settings.endGroup();

    settings.beginGroup(QStringLiteral("game"));
    const QString &&gameVer = settings.value(
        QStringLiteral("version"), Game::defaultVersionString).toString();
    if (gameVer != tempGameVerStr) {
        if (fromDialog) {
            QMessageBox msgBox(QMessageBox::Warning,
                               tr("Changing game version"),
                               tr(
                                   "The game version has been changed from %1 to %2\n"
                                   "The program need to restart to apply the changes.")
                               .arg(tempGameVerStr, gameVer));
            QPushButton *restartBtn = msgBox.addButton(tr("Restart"),
                                                       QMessageBox::YesRole);
            msgBox.setDefaultButton(restartBtn);
            msgBox.addButton(tr("Keep"), QMessageBox::NoRole);

            msgBox.exec();
            if (msgBox.clickedButton() == restartBtn) {
                restart();
            } else {
                settings.setValue("version", tempGameVerStr); // Set value explicitly
            }
        } else {
            settings.setValue(QStringLiteral("version"), gameVer); // Set value explicitly
            tempGameVerStr = gameVer;
            qInfo() << "The game version has been set to" << gameVer;
            emit gameVersionChanged(gameVer);
        }
    }
    const auto &&syntaxPath =
        settings.value("customCommandSyntaxFilePath").toString();
    if (!syntaxPath.isEmpty()) {
        Command::Schema::SchemaLoader loader{ syntaxPath };
        if (loader.lastError().isEmpty()) {
            Command::MinecraftParser::setGameVer(Game::version(), false);
            Command::MinecraftParser::setSchema(loader.tree());
            qInfo() << "Command syntax tree has been overriden by" <<
                syntaxPath;
        } else {
            Command::MinecraftParser::setGameVer(Game::version());
        }
    } else {
        Command::MinecraftParser::setGameVer(Game::version());
    }
    settings.endGroup();

    emit ui->tabbedInterface->settingsChanged();
}

void MainWindow::writeSettings() {
    QSettings settings;

    settings.beginGroup("geometry");
    settings.setValue("isMaximized", isMaximized());
    if (!isMaximized()) {
        settings.setValue("size", size());
        settings.setValue("pos", pos());
    }
    settings.endGroup();
}

void MainWindow::moveSetting(QSettings &settings, const QString &oldKey,
                             const QString &newKey) {
    if (settings.contains(oldKey)) {
        settings.setValue(newKey, settings.value(oldKey));
        settings.remove(oldKey);
    }
}

void MainWindow::moveOldSettings() {
    QSettings settings;

    moveSetting(settings, QStringLiteral("theme"),
                QStringLiteral("interface/style"));
    moveSetting(settings, QStringLiteral("general/gameVersion"),
                QStringLiteral("game/version"));
    moveSetting(settings, QStringLiteral("general/locale"),
                QStringLiteral("interface/locale"));
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
                                  tr("Invalid datapack"),
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
    ui->tabbedInterface->clear();
    ui->tabbedInterface->setPackOpened(true);
    updateWindowTitle(false);
    m_packInfo = packInfo;
    m_statusBar->onCurDirChanged();
    adjustForCurFolder(dirPath);

    ui->actionStatistics->setEnabled(true);

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif

    emit curDirChanged(dirPath);

    advancementsDock->loadAdvancements();
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
        QTextStream in(&file);
        in.setCodec("UTF-8");
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

void MainWindow::loadLanguage(const QString &langCode) {
    QLocale::setDefault(locale());
    QString &&translationFile = QString("MCDatapacker_%1").arg(
        langCode);
    switchTranslator(m_translatorQt, QString("qt_%1").arg(langCode));
    switchTranslator(m_translator, translationFile);
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

    const int recentPathsSize = recentPaths.size();

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

    ui->tabbedInterface->updateRecentPacks(recentFoldersActions,
                                           recentPathsSize);
}

void MainWindow::updateEditMenu() {
    if (auto *editor = ui->tabbedInterface->getCodeEditor()) {
        ui->actionUndo->setEnabled(editor->getCanUndo());
        ui->actionRedo->setEnabled(editor->getCanRedo());
        ui->actionSelectAll->setEnabled(true);
        const bool hasSelection = editor->textCursor().hasSelection();
        ui->actionCut->setEnabled(hasSelection);
        ui->actionCopy->setEnabled(hasSelection);
        ui->actionPaste->setEnabled(editor->canPaste());
    } else {
        ui->actionUndo->setEnabled(false);
        ui->actionRedo->setEnabled(false);
        ui->actionSelectAll->setEnabled(false);
        ui->actionCut->setEnabled(false);
        ui->actionCopy->setEnabled(false);
        ui->actionPaste->setEnabled(false);
    }
}

void MainWindow::changeAppStyle(const bool darkMode) {
    QSettings      settings;
    const QString &styleId = settings.value(
        QStringLiteral("interface/style"),
        qApp->style()->objectName()).toString();

    if (!darkMode) {
        setAppStyle(styleId);
    } else {
        const QString &darkStyleId = settings.value(
            QStringLiteral("interface/darkStyle"), "DarkFusion").toString();
        setAppStyle(darkStyleId);
    }
}

void MainWindow::setAppStyle(const QString &name) {
    if (name.toCaseFolded() !=
        qApp->style()->objectName().toCaseFolded()) {
        if (name == QLatin1String("DarkFusion")) {
            qApp->setStyle(new DarkFusionStyle);
        } else if (name == QLatin1String("NorwegianWood")) {
            qApp->setStyle(new NorwegianWoodStyle);
        } else if (name == QLatin1String("Qlementine")) {
            auto *style = new oclero::qlementine::QlementineStyle;
            style->setAnimationsEnabled(false); // Prevent crash
            style->setAutoIconColor(
                oclero::qlementine::AutoIconColor::TextColor);
            qApp->setStyle(style);
        } else {
            qApp->setStyle(name);
        }
        qApp->setPalette(style()->standardPalette());
    }
}

void MainWindow::changeEvent(QEvent *event) {
    if (event != nullptr) {
        switch (event->type()) {
            /* this event is sent if a translator is loaded */
            case QEvent::LanguageChange: {
                /*qDebug() << "QEvent::LanguageChange"; */
                ui->retranslateUi(this);
                break;
            }

            /* this event is sent if the system locale changes (internal locale changes on Windows) */
            case QEvent::LocaleChange: {
                qDebug() << "QEvent::LocaleChange" << locale();
                const QString &locale = QSettings().value("interface/locale",
                                                          QString()).toString();
                loadLanguage(
                    locale.isEmpty() ? QLocale::system().name() : locale);
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

void MainWindow::installUpdate(const QString &url, const QString &filepath) {
    using namespace miniz_cpp;
    Q_UNUSED(url);

    const auto appDirPath = qApp->applicationDirPath();
    qDebug() << filepath << appDirPath << qApp->arguments()[0];
    QFile::rename(qApp->arguments()[0],
                  appDirPath + "/MCDatapacker_old");
    zip_file zipFile{ filepath.toStdString() };

/*  zipFile.printdir(); */

    QDir        dir(appDirPath);
    auto const &infoList = zipFile.infolist();

    QProgressDialog progress(QString(), QString(), 0,
                             infoList.size(), this);
    progress.setWindowTitle(tr("Extracting files"));
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);

    int i = 0;
    for (auto const &info: infoList) {
        const QString &filename = QString::fromStdString(info.filename);
        progress.setValue(i);
        progress.setLabelText(tr("Extracting: %1").arg(filename));
        if (filename.rightRef(1) == '/') {
            dir.mkpath(filename);
        } else {
            QSaveFile file(appDirPath + QLatin1Char('/') + filename);
            if (!file.open(QIODevice::WriteOnly))
                return;

            const auto &&fileData = zipFile.read(info);
            file.write(fileData.c_str(), fileData.size());
            file.commit();
        }
        i++;
    }

    /* QLatin1String("/QSU_Update.bin") crashes the program in release build */
    QFile::remove(appDirPath + QStringLiteral("/QSU_Update.bin"));

    progress.setValue(progress.maximum());

    QFile::rename(appDirPath + QLatin1String("/MCDatapacker.exe"),
                  qApp->arguments()[0].replace('\\', '/'));

    restart();
}

void MainWindow::onColorModeChanged(const bool isDark) {
    Windows::setDarkFrame(this, isDark);
    for (auto *child: children()) {
        auto *widget = qobject_cast<QWidget *>(child);
        if (widget) {
            Windows::setDarkFrame(widget, isDark);
        }
    }

    changeAppStyle(isDark);
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
                         .replace(' ', '_').replace('/', '_')
                         .replace('.', '_').replace(':', '_');
        dir.mkpath(dirPath + QStringLiteral("/data/") + namesp);

        loadFolder(dirPath,
                   PackMetaInfo{ dialog->getDesc(), dialog->getFormat() });
        ui->tabbedInterface->openFile(QStringLiteral("pack.mcmeta"));
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

    if (!ui->tabbedInterface->getCodeEditor())
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
        return doc->toPlainText();
    else
        return QString();
}

MainWindow::~MainWindow() {
    delete ui;
}
