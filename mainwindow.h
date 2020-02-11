#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSessionManager>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void openFile(const QString &filepath);
    QString getCurDir();

    void setCodeEditorText(const QString &text);
    QString getCodeEditorText();

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void fileOpened(QString fileExt);;

private slots:
    void open();
    void newDatapack();
    void openFolder();
    bool save();
    void documentWasModified();

#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif

private:
    Ui::MainWindow *ui;

    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &filepath);
    void setCurrentFile(const QString &filepath);
    QString strippedName(const QString &fullFilepath);
    void updateWindowTitle();

    QString curFile = QString();
    QString curDir = QString();
};

#endif // MAINWINDOW_H
