#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "visualrecipeeditordock.h"
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


MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monoFont.setPointSize(11);

    ui->codeEditor->setFont(monoFont);
    ui->codeEditor->setFocus();

    connect(ui->actionNewDatapack, &QAction::triggered, this, &MainWindow::onActionNewDatapack);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onActionOpen);
    connect(ui->actionOpenFolder, &QAction::triggered, this, &MainWindow::openFolder);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionExit, &QAction::triggered, this, &QApplication::quit);

    VisualRecipeEditorDock *visualRecipeEditorDock =  new VisualRecipeEditorDock(this);
    addDockWidget(Qt::RightDockWidgetArea, visualRecipeEditorDock);
}

void MainWindow::onActionNewDatapack() {
    NewDatapackDialog *dialog = new NewDatapackDialog(this);
    int dialogCode = dialog->exec();
    if(dialogCode == 1) {
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
                .replace(" ", "_")
                .replace("/", "_")
                .replace(".", "_")
                .replace(":", "_");
        dir.mkpath(dirPath+"/data/"+namesp);

        ui->datapackTreeView->load(dirPath);
    }
    qDebug() << dialogCode;
    //qDebug() << dialog->getFormat() << dialog->getDesc() << dialog->getDirPath();
}

void MainWindow::onActionOpen() {
    this->openFile(QFileDialog::getOpenFileName(this, tr("Open File"), ""));
}

void MainWindow::openFile(QString filename) {
    if (filename.isEmpty() || this->fileName == filename)
        return;
    else {
        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(0, "error", file.errorString());
        } else {
            QTextStream in(&file);

            in.setCodec("UTF-8");

            int c = 0;
            QString content;
            while(!in.atEnd()) {
                content += in.readLine();
                if(!in.atEnd()) content += '\n';
                ++c;
            }
            ui->codeEditor->setPlainText(content);
            this->fileName = filename;
            ui->codeEditor->setFileName(filename);
        }
    file.close();
    emit fileOpened(QFileInfo(fileName).completeSuffix());
    }
}

void MainWindow::saveFile() {
    if(this->fileName.isEmpty()) {
        this->fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "");
    }

    QFile file(this->fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << ui->codeEditor->toPlainText();
    file.close();
}

void MainWindow::openFolder() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

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
                QByteArray json_bytes = json_string.toLocal8Bit();

                auto json_doc = QJsonDocument::fromJson(json_bytes);

                qDebug() << json_doc;

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
                        this->dirName = dir;
                    }
                }
            }
        }
    } else {
        QMessageBox::information(0, "error", "Invaild datapack folder");
        return;
    }
}

void MainWindow::setCodeEditorText(QString text) {
    ui->codeEditor->setPlainText(text);
}

MainWindow::~MainWindow()
{
    delete ui;
}
