#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "newdatapackdialog.h"
#include "settingsdialog.h"
#include "aboutdialog.h"
#include "disclaimerdialog.h"
#include "globalhelpers.h"
#include "tabbedcodeeditorinterface.h"

#include <QDebug>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QScreen>
#include <QGuiApplication>
#include <QFileInfo>
#include <QCloseEvent>


QMap<QString, QVariantMap > MainWindow::MCRInfoMaps;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    MainWindow::MCRInfoMaps.insert(QStringLiteral("block"),
                                   MainWindow::readMCRInfo(QStringLiteral(
                                                               "block")));
    MainWindow::MCRInfoMaps.insert(QStringLiteral("item"),
                                   MainWindow::readMCRInfo(QStringLiteral(
                                                               "item")));

    /*qDebug() << MainWindow::getMCRInfo("blockTag").count(); */

    connect(ui->actionNewDatapack, &QAction::triggered,
            this, &MainWindow::newDatapack);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::open);
    connect(ui->actionOpenFolder, &QAction::triggered,
            this, &MainWindow::openFolder);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionSaveAll, &QAction::triggered,
            this, &MainWindow::saveAll);
    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionSettings, &QAction::triggered,
            this, &MainWindow::pref_settings);
    connect(ui->actionAboutApp, &QAction::triggered,
            this, &MainWindow::about);
    connect(ui->actionAboutQt, &QAction::triggered, [this]() {
        QMessageBox::aboutQt(this);
    });
    connect(ui->actionDisclaimer, &QAction::triggered,
            this, &MainWindow::disclaimer);
    connect(&fileWatcher, &QFileSystemWatcher::fileChanged,
            this, &MainWindow::onSystemWatcherFileChanged);
    connect(ui->datapackTreeView, &DatapackTreeView::fileRenamed,
            ui->codeEditorInterface, &TabbedCodeEditorInterface::onFileRenamed);
    connect(ui->codeEditorInterface,
            &TabbedCodeEditorInterface::curModificationChanged,
            this, &MainWindow::updateWindowTitle);
    connect(ui->datapackTreeView, &DatapackTreeView::datapackChanged,
            this, &MainWindow::onDatapackChanged);
    connect(ui->codeEditorInterface, &TabbedCodeEditorInterface::curFileChanged,
            this, &MainWindow::onCurFileChanged);
    connect(ui->datapackTreeView, &DatapackTreeView::openFileRequested,
            ui->codeEditorInterface, &TabbedCodeEditorInterface::onOpenFile);

    readSettings();

    #ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
    #endif

    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/icon");
    /*qDebug() << QIcon::fallbackSearchPaths(); */

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

void MainWindow::open() {
    if (maybeSave()) {
        QString fileName
            = QFileDialog::getOpenFileName(this, tr("Open file"), "");
        if (!fileName.isEmpty())
            ui->codeEditorInterface->openFile(fileName);
    }
}

bool MainWindow::save() {
    if (ui->codeEditorInterface->isNoFile())
        return false;

    if (auto *curFile = ui->codeEditorInterface->getCurFile();
        curFile->fileInfo.fileName().isEmpty()) {
        QString filepath
            = QFileDialog::getSaveFileName(this, tr("Save File"), "");
        if (!filepath.isEmpty())
            return ui->codeEditorInterface->saveCurFile(filepath);
        else
            return false;
    } else {
        return ui->codeEditorInterface->saveCurFile();
    }
}

void MainWindow::saveAll() {
    ui->codeEditorInterface->saveAllFile();
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
    /*qDebug() << "onSystemWatcherFileChanged"; */
    if ((filepath != ui->codeEditorInterface->getCurFilePath())) return;

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
    if (auto *curFile = ui->codeEditorInterface->getCurFile())
        curFileType = curFile->fileType;
    lootTableEditorDock->setVisible(curFileType == CodeFile::LootTable);
    visualRecipeEditorDock->setVisible(curFileType == CodeFile::Recipe);
    predicateDock->setVisible(curFileType == CodeFile::Predicate);
}

void MainWindow::onDatapackChanged() {
    ui->codeEditorInterface->clear();
    updateWindowTitle(false);
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

void MainWindow::readPrefSettings(QSettings &settings) {
    settings.beginGroup("general");
    /*qDebug() << settings.value("locale", "").toString(); */
    loadLanguage(settings.value("locale", "").toString(), true);

    settings.endGroup();
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
    if (!ui->codeEditorInterface->hasUnsavedChanges())
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
        return ui->codeEditorInterface->saveAllFile();

    case QMessageBox::Cancel:
        return false;

    default:
        break;
    }
    return true;
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

    QString path = QApplication::applicationDirPath() + "/translations/";
    if (translator.load(path + filename))
        qApp->installTranslator(&translator);
    else if (translator.load(":/i18n/" + filename))
        qApp->installTranslator(&translator);
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

        /* this event is send, if the system, language changes */
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

void MainWindow::updateWindowTitle(bool changed) {
    QStringList titleParts;

    if (!ui->codeEditorInterface->isNoFile()) {
        if (auto curPath = ui->codeEditorInterface->getCurFilePath();
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

QMap<QString, QVariant> MainWindow::readMCRInfo(const QString &type,
                                                [[maybe_unused]] const int depth)
{
    QMap<QString, QVariant> retMap;

    QFileInfo finfo = QFileInfo(":minecraft/info/" + type + ".json");

    if (!(finfo.exists() && finfo.isFile())) {
        qWarning() << "File not exists:" << finfo.path() << "Return empty.";
        return retMap;
    }
    QFile inFile(finfo.filePath());
    inFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = inFile.readAll();
    inFile.close();

    QJsonParseError errorPtr{};
    QJsonDocument   doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        qWarning() << "Parse failed" << errorPtr.error;
        return retMap;
    }
    QJsonObject root = doc.object();
    if (root.isEmpty()) {
        /*qDebug() << "Root is empty. Return empty"; */
        return retMap;
    }

    QMap<QString, QVariant> tmpMap2;
    if (root.contains("added"))
        tmpMap2 = root["added"].toVariant().toMap();
    else
        tmpMap2 = root.toVariantMap();
    if (!tmpMap2.isEmpty())
        retMap.insert(tmpMap2);
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
            ui->codeEditorInterface->clear();
        } else {
            return;
        }
        QString dirPath = dialog->getDirPath() + "/" + dialog->getName();
        QDir    dir(dirPath);
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

        QFile file(dirPath + "/pack.mcmeta");
        if (file.open(QIODevice::ReadWrite)) {
            QJsonObject root;
            QJsonObject pack;
            pack.insert("description", dialog->getDesc());
            pack.insert("pack_format", dialog->getFormat());
            root.insert("pack", pack);

            QTextStream stream(&file);
            stream << QJsonDocument(root).toJson();
        }
        file.close();

        dir.mkpath(dirPath + "/data");
        QString namesp = dialog->getName()
                         .toLower()
                         .replace(" ", "_").replace("/", "_")
                         .replace(".", "_").replace(":", "_");
        dir.mkpath(dirPath + "/data/" + namesp);

        ui->datapackTreeView->load(dir);
    }
    delete dialog;
}

void MainWindow::openFolder() {
    QString dirPath;

    if (maybeSave())
        dirPath = QFileDialog::getExistingDirectory(this,
                                                    tr(
                                                        "Open datapack folder"),
                                                    "",
                                                    QFileDialog::ShowDirsOnly |
                                                    QFileDialog::DontResolveSymlinks);
    else
        return;

    if (dirPath.isEmpty()) return;

    QString pack_mcmeta = dirPath + "/pack.mcmeta";

    if (QFile::exists(pack_mcmeta)) {
        if (pack_mcmeta.isEmpty())
            return;
        else {
            QFile file(pack_mcmeta);
            if (!file.open(QIODevice::ReadOnly)) {
                QMessageBox::information(this, tr("Error"), file.errorString());
            } else {
                QTextStream in(&file);
                QString     json_string;
                json_string = in.readAll();
                file.close();

                QJsonDocument json_doc = QJsonDocument::fromJson(
                    json_string.toUtf8());

                /*qDebug() << json_doc; */

                if (json_doc.isNull()) {
                    QMessageBox::information(this,
                                             "pack.mcmeta error",
                                             tr(
                                                 "Failed to parse the pack.mcmeta file."));
                    return;
                }
                if (!json_doc.isObject()) {
                    QMessageBox::information(this,
                                             "pack.mcmeta error",
                                             tr(
                                                 "The pack.mcmeta file is not a JSON object."));
                    return;
                }

                QJsonObject json_obj = json_doc.object();

                if (json_obj.isEmpty()) {
                    QMessageBox::information(this,
                                             "pack.mcmeta error",
                                             tr(
                                                 "The pack.mcmeta file's contents is empty."));
                    return;
                }

                QVariantMap json_map = json_obj.toVariantMap();

                auto curDir = QDir::current();
                if (json_map.contains("pack")) {
                    QVariantMap pack = json_map["pack"].toMap();
                    if (pack.contains("description") &&
                        pack.contains("pack_format")) {
                        QDir dir(dirPath);
                        QDir::setCurrent(dir.absolutePath());
                        ui->datapackTreeView->load(dir);

                        if (!curDir.path().isEmpty()) {
                            this->fileWatcher.removePath(curDir.path());
                            this->fileWatcher.addPath(curDir.path());
                        }
                        ui->codeEditorInterface->clear();
                        emit curDirChanged(dirPath);
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

void MainWindow::setCodeEditorText(const QString &text) {
    if (ui->codeEditorInterface->isNoFile())
        return;

    /*ui->codeEditor->setPlainText(text); */
    QTextCursor cursor = ui->codeEditorInterface->getEditor()->textCursor();

    cursor.beginEditBlock();
    cursor.select(QTextCursor::Document);
    cursor.insertText(text);
    cursor.endEditBlock();
    ui->codeEditorInterface->getEditor()->setTextCursor(cursor);
}

QString MainWindow::getCodeEditorText() {
    if (auto *doc = ui->codeEditorInterface->getCurDoc())
        return ui->codeEditorInterface->getCurDoc()->toPlainText();
    else
        return QString();
}

MainWindow::~MainWindow() {
    delete ui;
}
