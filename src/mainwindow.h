#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSessionManager>
#include <QMap>
#include <QTranslator>
#include <QSettings>
#include <QFileSystemWatcher>
#include <QMessageBox>
#include <QDir>
#include <QVersionNumber>
#include <QLocale>
#include <QJsonObject>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class VisualRecipeEditorDock;
class LootTableEditorDock;
class PredicateDock;
class ItemModifierDock;
class StatusBar;

struct PackMetaInfo {
    QString description;
    int     packFormat = 0;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getCurLocale();
    void setCodeEditorText(const QString &text);
    QString getCodeEditorText();
    static QVariantMap &getMCRInfo(const QString &type);
    void readPrefSettings(QSettings &settings, bool fromDialog = false);
    static QVariantMap readMCRInfo(const QString &type         = "block",
                                   const QString &ver          = curGameVersion.toString(),
                                   const int depth             = 0);

    static QVersionNumber getCurGameVersion();
    PackMetaInfo getPackInfo() const;

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent* event) override;

signals:
    void curFileChanged(const QString &filepath);
    void curDirChanged(const QDir &dir);
    void gameVersionChanged(const QString &ver);

private slots:
    /* File menu */
    void open();
    void newDatapack();
    void openFolder();
    void openRecentFolder();
    bool save();
    void saveAll();
    void restart();
    /* Edit menu */
    /* Tools menu */
    void statistics();
    /* Preferences menu */
    void pref_settings();
    /* Help menu */
    void about();
    void disclaimer();
    /* Internal slots */
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
    QFileSystemWatcher fileWatcher;
    QTranslator m_translator;
    QTranslator m_translatorQt;
    PackMetaInfo m_packInfo;
    QMessageBox *uniqueMessageBox                  = nullptr;
    StatusBar *m_statusBar                         = nullptr;
    VisualRecipeEditorDock *visualRecipeEditorDock = nullptr;
    LootTableEditorDock *lootTableEditorDock       = nullptr;
    PredicateDock *predicateDock                   = nullptr;
    ItemModifierDock *itemModifierDock             = nullptr;
    QLocale curLocale;
    QVector<QAction*> recentFoldersActions;
    static QVersionNumber curGameVersion;
    const int maxRecentFoldersActions = 10;

    void initDocks();
    void initMenu();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void openFolder(const QString &dirpath);
    void loadFolder(const QString &dirPath, const PackMetaInfo &packInfo);
    bool folderIsVaild(const QDir &dir, bool reportError = true);
    PackMetaInfo readPackMcmeta(const QString &filepath,
                                QString &errorMsg) const;

    bool isPathRelativeTo(const QString &path, const QString &catDir);
    void loadLanguage(const QString& rLanguage, bool atStartup = false);
    void switchTranslator(QTranslator& translator, const QString& filename);
    void adjustForCurFolder(const QString &path);
    void updateRecentFolders();
    void updateEditMenu();
};

#endif /* MAINWINDOW_H */
