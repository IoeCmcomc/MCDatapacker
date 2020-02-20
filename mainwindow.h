#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "visualrecipeeditordock.h"

#include <QMainWindow>
#include <QSessionManager>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum MCRFileType {
        Advancement,
        Function,
        LootTable,
        Predicate,
        Recipe,
        Structure,
        BlockTag, EntityTypeTag, FluidTag, FunctionTag, ItemTag,
        JsonText,
        Text
    };
    static MCRFileType curFileType;

    void openFile(const QString &filepath);
    QString getCurDir();
    void setCodeEditorText(const QString &text);
    QString getCodeEditorText();
    static QMap<QString, QVariant> *getMCRInfo(const QString &type);
    static bool isPathRelativeTo(const QString &path, const QString &dir,
                     const QString &catDir);

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

    QString curFile = QString();
    QString curDir = QString();
    static QMap<QString, QMap<QString, QVariant>* > *MCRInfoMaps;
    VisualRecipeEditorDock *visualRecipeEditorDock;

    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &filepath);
    void setCurrentFile(const QString &filepath);
    QString strippedName(const QString &fullFilepath);
    void updateWindowTitle();
    static QMap<QString, QVariant> *readMCRInfo(const QString &type = "block",
                                                const int depth = 0);
    bool isPathRelativeTo(const QString &path, const QString &catDir);
};

#endif // MAINWINDOW_H
