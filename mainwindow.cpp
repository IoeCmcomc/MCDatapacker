#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "newdatapackdialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUiLoader>
#include <QSettings>
#include <QScreen>
#include <QGuiApplication>
#include <QSaveFile>
#include <QFileInfo>

MainWindow::MCRFileType MainWindow::curFileType = Text;
QMap<QString, QMap<QString, QVariant>* > *MainWindow::MCRInfoMaps = new QMap<QString, QMap<QString, QVariant>* >();

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    MainWindow::MCRInfoMaps->insert("block", MainWindow::readMCRInfo(2225, "block"));
    MainWindow::MCRInfoMaps->insert("item", MainWindow::readMCRInfo(2225, "item"));

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

    readSettings();

    #ifndef QT_NO_SESSIONMANAGER
        QGuiApplication::setFallbackSessionManagementEnabled(false);
        connect(qApp, &QGuiApplication::commitDataRequest,
                this, &MainWindow::commitData);
    #endif

    visualRecipeEditorDock =  new VisualRecipeEditorDock(this);
    addDockWidget(Qt::RightDockWidgetArea, visualRecipeEditorDock);

    setCurrentFile(QString());

    /*
    int c = 0;
    for(auto k : MainWindow::getMCRInfo("item")->keys()) {
        qDebug() << k;
        if(c > 10) break;
        ++c;
    }
    qDebug() << MainWindow::getMCRInfo("item")->contains("acacia_boat");
    qDebug() << MainWindow::getMCRInfo("item")->value("acacia_boat");
    */
}

void MainWindow::open() {
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "");
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

void MainWindow::documentWasModified() {
    //qDebug() << "documentWasModified";
    setWindowModified(ui->codeEditor->document()->isModified());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    qDebug() << "closeEvent";
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::readSettings() {
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    //restoreGeometry(geometry);
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
}

void MainWindow::writeSettings() {
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    //settings.setValue("geometry", saveGeometry());
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

bool MainWindow::isPathRelativeTo(const QString &path, const QString &catDir) {
    auto tmpDir = curDir + "/data/";
    if(!path.startsWith(tmpDir)) return false;
    return path.mid(tmpDir.length()).section('/', 1).startsWith(catDir);
}

bool MainWindow::isPathRelativeTo(const QString &path, const QString &dir,
                                  const QString &catDir) {
    auto tmpDir = dir + "/data/";
    if(!path.startsWith(tmpDir)) return false;
    return path.mid(tmpDir.length()).section('/', 1).startsWith(catDir);
}

void MainWindow::commitData(QSessionManager &) {
}

void MainWindow::setCurrentFile(const QString &filepath) {
    this->curFile = filepath;

    updateWindowTitle();
    ui->codeEditor->document()->setModified(false);
    setWindowModified(false);

    QFileInfo info = QFileInfo(filepath);
    const QString jsonExts = "json mcmeta";

    if(info.completeSuffix() == "mcfunction") {
        MainWindow::curFileType = Function;
    } else if(jsonExts.contains(info.completeSuffix())) {
        if(isPathRelativeTo(curFile, "recipes")) {
            MainWindow::curFileType = Recipe;
        } else {
            MainWindow::curFileType = JsonText;
        }
    } else {
        MainWindow::curFileType = Text;
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

QMap<QString, QVariant> *MainWindow::readMCRInfo(const int &dataVersion,
                                                const QString &type,
                                                const int depth) {
    QMap<QString, QVariant> *retMap = new QMap<QString, QVariant>();

    QFileInfo finfo = QFileInfo(":minecraft/info/"+type+"/"+QString::number(dataVersion)+".json");
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

    if(root["version_id"].toInt() != dataVersion) return retMap;
    if(root.contains("base_version")) {
        auto baseVer = root["base_version"].toInt();
        //qDebug() << "Go to base version" << baseVer;
        QMap<QString, QVariant> *tmpMap = readMCRInfo(baseVer, type, depth+1);
        if(!tmpMap->isEmpty())
            retMap->unite(*tmpMap);
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
            if(QMessageBox::warning
                    (this,
                     "Directory not empty",
                     "The directory is not empty. Do you want to delete this directory?",
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
    }
    qDebug() << dialogCode;
    //qDebug() << dialog->getFormat() << dialog->getDesc() << dialog->getDirPath();
}

void MainWindow::openFile(const QString &filepath) {
    if (filepath.isEmpty() || this->curFile == filepath)
        return;
    else {
        QFile file(filepath);
        if(!file.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("Application"),
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
        out << ui->codeEditor->toPlainText();
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
        QMessageBox::warning(this, tr("Application"), errorMessage);
        return false;
    }

    setCurrentFile(filepath);
    return true;
}

void MainWindow::openFolder() {
    QString dir;
    if (maybeSave())
        dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
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
                QMessageBox::information(0, "error", file.errorString());
            } else {
                QTextStream in(&file);
                QString json_string;
                json_string = in.readAll();
                file.close();

                QJsonDocument json_doc = QJsonDocument::fromJson(json_string.toUtf8());

                //qDebug() << json_doc;

                if(json_doc.isNull()){
                    QMessageBox::information(0, "error", "Failed to create JSON doc.");
                    return;
                }
                if(!json_doc.isObject()){
                    QMessageBox::information(0, "error", "JSON is not an object.");
                    return;
                }

                QJsonObject json_obj = json_doc.object();

                if(json_obj.isEmpty()){
                    QMessageBox::information(0, "error", "JSON object is empty.");
                    return;
                }

                QVariantMap json_map = json_obj.toVariantMap();

                if(json_map.contains("pack")) {
                    QVariantMap pack = json_map["pack"].toMap();
                    if(pack.contains("description") && pack.contains("pack_format")) {
                        qDebug()<< pack["description"].toString();
                        qDebug()<< pack["pack_format"].toDouble();

                        ui->datapackTreeView->load(dir);
                        ui->codeEditor->clear();
                        this->curDir = dir;
                        setCurrentFile("");
                    }
                }
            }
        }
    } else {
        QMessageBox::information(0, "error", "Invaild datapack folder");
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
