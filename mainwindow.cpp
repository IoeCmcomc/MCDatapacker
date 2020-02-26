#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "newdatapackdialog.h"
#include "settingsdialog.h"
#include "globalhelpers.h"

#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUiLoader>
#include <QSettings>
#include <QScreen>
#include <QGuiApplication>
#include <QSaveFile>
#include <QFileInfo>
#include <QAbstractButton>

MainWindow::MCRFileType MainWindow::curFileType = Text;
QMap<QString, QMap<QString, QVariant>* > *MainWindow::MCRInfoMaps = new QMap<QString, QMap<QString, QVariant>* >();

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    MainWindow::MCRInfoMaps->insert("block", MainWindow::readMCRInfo("blocks"));
    MainWindow::MCRInfoMaps->insert("item", MainWindow::readMCRInfo("items"));

    QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monoFont.setPointSize(11);

    ui->codeEditor->setFont(monoFont);
    ui->codeEditor->setFocus();

    connect(ui->actionNewDatapack, &QAction::triggered, this, &MainWindow::newDatapack);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::open);
    connect(ui->actionOpenFolder, &QAction::triggered, this, &MainWindow::openFolder);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->codeEditor->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::documentWasModified);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::pref_settings);
    connect(&fileWatcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::onSystemWatcherFileChanged);

    readSettings();

    #ifndef QT_NO_SESSIONMANAGER
        QGuiApplication::setFallbackSessionManagementEnabled(false);
        connect(qApp, &QGuiApplication::commitDataRequest,
                this, &MainWindow::commitData);
    #endif

    visualRecipeEditorDock = new VisualRecipeEditorDock(this);
    addDockWidget(Qt::RightDockWidgetArea, visualRecipeEditorDock);

    setCurrentFile(QString());
}

void MainWindow::open() {
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), "");
        if (!fileName.isEmpty())
            openFile(fileName);
    }
}

bool MainWindow::save() {
    if (curFile.isEmpty()) {
        QString filepath = QFileDialog::getSaveFileName(this, tr("Save File"), "");
        if(!filepath.isEmpty())
            return saveFile(filepath);
        else
            return false;
    } else {
        return saveFile(curFile);
    }
}

void MainWindow::pref_settings() {
    SettingsDialog dialog(this);
    dialog.exec();
}

void MainWindow::documentWasModified() {
    //qDebug() << "documentWasModified";
    setWindowModified(ui->codeEditor->document()->isModified());
}

void MainWindow::onSystemWatcherFileChanged(const QString &filepath) {
    //qDebug() << "onSystemWatcherFileChanged";
    if((filepath != curFile)) return;

    auto reloadExternChanges = QSettings().value("general/reloadExternChanges", 0);
    if(reloadExternChanges == 1) {
        if(uniqueMessageNox != nullptr) return;
        uniqueMessageNox = new QMessageBox(this);
        uniqueMessageNox->setIcon(QMessageBox::Question);
        uniqueMessageNox->setWindowTitle(tr("Reload file"));
        uniqueMessageNox->setText(tr("The file %1 has been changed exernally.")
                                      .arg(QDir::toNativeSeparators(filepath)));
        uniqueMessageNox->setInformativeText(tr("Do you want to reload this file?"));
        uniqueMessageNox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        uniqueMessageNox->setDefaultButton(QMessageBox::Yes);
        uniqueMessageNox->exec();
        auto userDecision = uniqueMessageNox->result();
        delete uniqueMessageNox;
        uniqueMessageNox = nullptr;
        reloadExternChanges = (userDecision == QMessageBox::Yes) ? 0 : 2;
    }

    if(reloadExternChanges == 0) {
        if(!ui->codeEditor->document()->isModified()) {
            openFile(filepath, true);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    //qDebug() << "closeEvent";
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
    QSize geomertySize = settings.value("size", QSize(600, 600)).toSize();
    if(geomertySize.isEmpty())
        adjustSize();
    else
        resize(geomertySize);

    QRect availableGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    move(settings.value("pos",
                        QPoint((availableGeometry.width() - width()) / 2,
                               (availableGeometry.height() - height()) / 2))
            .toPoint());

    if(settings.value("isMaximized", true).toBool())
        showMaximized();
    settings.endGroup();

    readPrefSettings(settings);
}

void MainWindow::readPrefSettings(QSettings &settings) {
    settings.beginGroup("general");
    qDebug() << settings.value("locale", "").toString();
    loadLanguage(settings.value("locale", "").toString(), true);

    settings.endGroup();
}

void MainWindow::writeSettings() {
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.beginGroup("geometry");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("isMaximized", isMaximized());
    settings.endGroup();
}

bool MainWindow::maybeSave() {
    if (!ui->codeEditor->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Unsaved changes"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}


QString MainWindow::getCurDir() {
    return this->curDir;
}

QString MainWindow::getCurLocale() {
    return this->curLocale.name();
}



void MainWindow::loadLanguage(const QString& rLanguage, bool atStartup) {
    if((curLocale.name() != rLanguage) || atStartup) {
    curLocale = QLocale(rLanguage);
    QLocale::setDefault(curLocale);
    qDebug() << QLocale::system();
    QString transfile;
    if(rLanguage.isEmpty())
        transfile = QString("MCDatapacker_%1.qm").arg(QLocale::system().name());
    else
        transfile = QString("MCDatapacker_%1.qm").arg(rLanguage);
    switchTranslator(m_translator, transfile);
    switchTranslator(m_translatorQt, QString("qt_%1.qm").arg(rLanguage));
    }
}

void MainWindow::switchTranslator(QTranslator& translator, const QString& filename) {
    qApp->removeTranslator(&translator);

    QString path = QApplication::applicationDirPath() + "/translations/";
    if(translator.load(path + filename))
        qApp->installTranslator(&translator);
}


void MainWindow::changeEvent(QEvent* event) {
    if(0 != event) {
        switch(event->type()) {
            // this event is send if a translator is loaded
        case QEvent::LanguageChange: {
            qDebug() << "QEvent::LanguageChange";
            ui->retranslateUi(this);
            if(visualRecipeEditorDock)
                visualRecipeEditorDock->retranslate();
        }
            break;
        // this event is send, if the system, language changes
        case QEvent::LocaleChange: {
            qDebug() << "QEvent::LocaleChange";
            QString locale = QLocale::system().name();
            loadLanguage(locale);
        }
        break;
        default:
            {}
        }
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::commitData(QSessionManager &) {
}

void MainWindow::setCurrentFile(const QString &filepath) {
    this->curFile = filepath;

    updateWindowTitle();
    ui->codeEditor->document()->setModified(false);
    setWindowModified(false);

    curFileType = GlobalHelpers::toMCRFileType(curDir, filepath);

    if(!curFile.isEmpty()) {
        fileWatcher.removePath(curFile);
        fileWatcher.addPath(curFile);
    }

    ui->codeEditor->setCurFile(filepath);

    visualRecipeEditorDock->setVisible(MainWindow::curFileType == Recipe);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::updateWindowTitle() {
    QStringList title;
    if(!curFile.isEmpty())
        title.push_back(strippedName(curFile) + "[*]");
    if(!curDir.isEmpty())
        title.push_back("[" + strippedName(curDir) + "]");
    title.push_back(QCoreApplication::applicationName());
    setWindowTitle(title.join(QStringLiteral(" - ")));
}

QMap<QString, QVariant> *MainWindow::readMCRInfo(const QString &type,
                                                const int depth) {
    QMap<QString, QVariant> *retMap = new QMap<QString, QVariant>();

    QFileInfo finfo = QFileInfo(":minecraft/info/"+type+".json");
    if(!(finfo.exists() && finfo.isFile())) {
        //qDebug() << "File not exists. Return empty.";
        return retMap;
    }
    QFile inFile(finfo.filePath());
    inFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = inFile.readAll();
    inFile.close();

    QJsonParseError errorPtr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        //qDebug() << "Parse failed" << errorPtr.error;
        return retMap;
    }
    QJsonObject root = doc.object();
    if(root.isEmpty()) {
        //qDebug() << "Root is empty. Return empty";
        return retMap;
    }

    QMap<QString, QVariant> *tmpMap2 = new QMap<QString, QVariant>(root["added"].toVariant().toMap());
    if(!tmpMap2->isEmpty())
        retMap->unite(*tmpMap2);
    return retMap;
}

QMap<QString, QVariant> *MainWindow::getMCRInfo(const QString &type) {
    return MainWindow::MCRInfoMaps->value(type, new QMap<QString, QVariant>());
}

void MainWindow::newDatapack() {
    NewDatapackDialog *dialog = new NewDatapackDialog(this);
    int dialogCode = dialog->exec();
    if(dialogCode == 1) {
        if (maybeSave()) {
            ui->codeEditor->clear();
            setCurrentFile(QString());
        } else {
            return;
        }
        QString dirPath = dialog->getDirPath() + "/" + dialog->getName();
        QDir dir(dirPath);
        if(!dir.exists()) {
            dir.mkpath(dirPath);
        } else if(!dir.isEmpty()) {
            if(QMessageBox::question(this, tr("Folder not empty"),
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

        QFile file(dirPath+"/pack.mcmeta");
        if(file.open(QIODevice::ReadWrite)) {
            QJsonObject root;
            QJsonObject pack;
            pack.insert("description", dialog->getDesc());
            pack.insert("pack_format", dialog->getFormat());
            root.insert("pack", pack);

            QTextStream stream(&file);
            stream << QJsonDocument(root).toJson();
        }
        file.close();

        dir.mkpath(dirPath+"/data");
        QString namesp = dialog->getName()
                .toLower()
                .replace(" ", "_").replace("/", "_")
                .replace(".", "_").replace(":", "_");
        dir.mkpath(dirPath+"/data/"+namesp);

        ui->datapackTreeView->load(dirPath);
        setCurrentFile("");
    }
    delete dialog;
}

void MainWindow::openFile(const QString &filepath, bool reload) {
    //qDebug() << "openFile";
    if (filepath.isEmpty() || (this->curFile == filepath && (!reload)))
        return;
    else {
        QFile file(filepath);
        if(!file.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::information(this, tr("Error"),
                                 tr("Cannot read file %1:\n%2.")
                                 .arg(QDir::toNativeSeparators(filepath), file.errorString()));
        } else {
            QTextStream in(&file);
            in.setCodec("UTF-8");

            #ifndef QT_NO_CURSOR
                QGuiApplication::setOverrideCursor(Qt::WaitCursor);
            #endif

            int c = 0;
            QString content;
            while(!in.atEnd()) {
                content += in.readLine();
                if(!in.atEnd()) content += '\n';
                ++c;
            }
            ui->codeEditor->setPlainText(content);
            setCurrentFile(filepath);

            #ifndef QT_NO_CURSOR
                QGuiApplication::restoreOverrideCursor();
            #endif

            emit fileOpened(QFileInfo(filepath).completeSuffix());
        }
    file.close();
    }
}

bool MainWindow::saveFile(const QString &filepath) {
    QString errorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(filepath);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << ui->codeEditor->toPlainText().toUtf8();
        if (!file.commit()) {
            errorMessage = tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(filepath), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                       .arg(QDir::toNativeSeparators(filepath), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty()) {
        QMessageBox::information(this, tr("Error"), errorMessage);
        return false;
    }

    setCurrentFile(filepath);
    return true;
}

void MainWindow::openFolder() {
    QString dir;
    if (maybeSave())
        dir = QFileDialog::getExistingDirectory(this, tr("Open datapack folder"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    else
        return;

    qDebug() << dir;
    if(dir.isEmpty()) return;

    QString pack_mcmeta = dir + "/pack.mcmeta";
    qDebug() << QFile::exists(pack_mcmeta);

    if(QFile::exists(pack_mcmeta)) {
        if(pack_mcmeta.isEmpty())
            return;
        else {
            QFile file(pack_mcmeta);
            if(!file.open(QIODevice::ReadOnly)) {
                QMessageBox::information(this, tr("Error"), file.errorString());
            } else {
                QTextStream in(&file);
                QString json_string;
                json_string = in.readAll();
                file.close();

                QJsonDocument json_doc = QJsonDocument::fromJson(json_string.toUtf8());

                //qDebug() << json_doc;

                if(json_doc.isNull()){
                    QMessageBox::information(this, "pack.mcmeta error", tr("Failed to parse the pack.mcmeta file."));
                    return;
                }
                if(!json_doc.isObject()){
                    QMessageBox::information(this, "pack.mcmeta error", tr("The pack.mcmeta file is not a JSON object."));
                    return;
                }

                QJsonObject json_obj = json_doc.object();

                if(json_obj.isEmpty()){
                    QMessageBox::information(this, "pack.mcmeta error", tr("The pack.mcmeta file's contents is empty."));
                    return;
                }

                QVariantMap json_map = json_obj.toVariantMap();

                if(json_map.contains("pack")) {
                    QVariantMap pack = json_map["pack"].toMap();
                    if(pack.contains("description") && pack.contains("pack_format")) {
                        ui->datapackTreeView->load(dir);
                        ui->codeEditor->clear();
                        this->curDir = dir;

                        if(!curDir.isEmpty()) {
                            this->fileWatcher.removePath(curDir);
                            this->fileWatcher.addPath(curDir);
                        }
                        setCurrentFile("");
                    }
                }
            }
        }
    } else {
        QMessageBox::information(this, tr("Error"), tr("Invaild datapack folder."));
        return;
    }
}

void MainWindow::setCodeEditorText(const QString &text) {
    ui->codeEditor->setPlainText(text);
}

QString MainWindow::getCodeEditorText() {
    return ui->codeEditor->toPlainText();
}

MainWindow::~MainWindow()
{
    delete ui;
}
