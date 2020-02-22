#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "visualrecipeeditordock.h"

#include <QMainWindow>
#include <QSessionManager>
#include <QMap>
#include <QTranslator>
#include <QSettings>

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
    QString getCurLocale();
    void setCodeEditorText(const QString &text);
    QString getCodeEditorText();
    static QMap<QString, QVariant> *getMCRInfo(const QString &type);
    static bool isPathRelativeTo(const QString &path, const QString &dir,
                     const QString &catDir);
    void readPrefSettings(QSettings &settings);

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent* event) override;

signals:
    void fileOpened(QString fileExt);;

private slots:
    void open();
    void newDatapack();
    void openFolder();
    bool save();
    void pref_settings();
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
    QLocale curLocale;

    QTranslator m_translator; // contains the translations for this application
    QTranslator m_translatorQt; // contains the translations for qt

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
    void loadLanguage(const QString& rLanguage, bool atStartup = false);
    void switchTranslator(QTranslator& translator, const QString& filename);

};

#endif // MAINWINDOW_H
