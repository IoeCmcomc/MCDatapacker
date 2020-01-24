#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString fileName = QStringLiteral("None");
    QString dirName = QStringLiteral("None");
    void openFile(QString filename);
    void setCodeEditorText(QString text);

private slots:
    void onActionOpen();
    void openFolder();
    void saveFile();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
