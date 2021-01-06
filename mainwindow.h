#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "visualrecipeeditordock.h"
#include "loottableeditordock.h"
#include "predicatedock.h"

#include <QMainWindow>
#include <QSessionManager>
#include <QMap>
#include <QTranslator>
#include <QSettings>
#include <QFileSystemWatcher>
#include <QMessageBox>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getCurLocale();
    void setCodeEditorText(const QString &text);
    QString getCodeEditorText();
    static QMap<QString, QVariant> &getMCRInfo(const QString &type);
    void readPrefSettings(QSettings &settings);
    static QVariantMap readMCRInfo(const QString &type = "block",
                                   const int depth     = 0);

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent* event) override;

signals:
    void curFileChanged(const QString filepath);
    void curDirChanged(const QDir dir);

private slots:
    void open();
    void newDatapack();
    void openFolder();
    bool save();
    void saveAll();
    void pref_settings();
    void about();
    void onSystemWatcherFileChanged(const QString &filepath);
    void onCurFileChanged(const QString &path);
    void onDatapackChanged();
    void updateWindowTitle(bool changed);

#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif

private:
    Ui::MainWindow *ui;

    static QMap<QString, QVariantMap> MCRInfoMaps;
    VisualRecipeEditorDock *visualRecipeEditorDock;
    QLocale curLocale;
    QFileSystemWatcher fileWatcher;
    QTranslator m_translator;
    QTranslator m_translatorQt;
    QMessageBox *uniqueMessageBox            = nullptr;
    LootTableEditorDock *lootTableEditorDock = nullptr;
    PredicateDock *predicateDock             = nullptr;

    void readSettings();
    void writeSettings();
    bool maybeSave();

    bool isPathRelativeTo(const QString &path, const QString &catDir);
    void loadLanguage(const QString& rLanguage, bool atStartup = false);
    void switchTranslator(QTranslator& translator, const QString& filename);
};

#endif /* MAINWINDOW_H */
