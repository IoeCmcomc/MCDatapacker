#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSessionManager>
#include <QTranslator>
#include <QSettings>
#include <QFileSystemWatcher>
#include <QMessageBox>
#include <QDir>
#include <QVersionNumber>
#include <QLocale>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class VisualRecipeEditorDock;
class LootTableEditorDock;
class PredicateDock;
class ItemModifierDock;
class AdvancementTabDock;
class StatusBar;

namespace libqdark {
    class SystemThemeHelper;
}

struct PackMetaInfo {
    QString description;
    int     packFormat = 0;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setCodeEditorText(const QString &text);
    QString getCodeEditorText();
    void readPrefSettings(QSettings &settings, bool fromDialog = false);

    PackMetaInfo getPackInfo() const;

public /*slots*/ :
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
    void rawJsonTextEditor();
    /* Preferences menu */
    void pref_settings();
    /* Help menu */
    void about();
    void checkForUpdates();
    void disclaimer();

    void openFolder(const QString &dirpath);

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;

signals:
    void curFileChanged(const QString &filepath);
    void curDirChanged(const QDir &dir);
    void gameVersionChanged(const QString &ver);

private /*slots*/ :
    void onSystemWatcherFileChanged(const QString &filepath);
    void onCurFileChanged(const QString &path);
    void onDatapackChanged();
    void updateWindowTitle(bool changed);
    void installUpdate(const QString &url, const QString &filepath);
    void onColorModeChanged(const bool isDark);

#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif

private:
    Ui::MainWindow *ui;

    QFileSystemWatcher fileWatcher;
    QTranslator m_translator;
    QTranslator m_translatorQt;
    PackMetaInfo m_packInfo;
    QMessageBox *uniqueMessageBox                    = nullptr;
    StatusBar *m_statusBar                           = nullptr;
    VisualRecipeEditorDock *visualRecipeEditorDock   = nullptr;
    LootTableEditorDock *lootTableEditorDock         = nullptr;
    PredicateDock *predicateDock                     = nullptr;
    ItemModifierDock *itemModifierDock               = nullptr;
    AdvancementTabDock *advancementsDock             = nullptr;
    libqdark::SystemThemeHelper *m_systemThemeHelper = nullptr;
    QVector<QAction *> recentFoldersActions;
    QString tempGameVerStr;
    QString m_initialStyleId;
    const static int maxRecentFoldersActions = 10;

    void initDocks();
    void initMenu();
    void connectActionLink(QAction *action, const QString &&url);
    void initResourcesMenu();
    void readSettings();
    void writeSettings();
    void moveOldSettings();
    bool maybeSave();
    void loadFolder(const QString &dirPath, const PackMetaInfo &packInfo);
    bool folderIsVaild(const QDir &dir, bool reportError = true);
    PackMetaInfo readPackMcmeta(const QString &filepath,
                                QString &errorMsg) const;

    bool isPathRelativeTo(const QString &path, const QString &catDir);
    void loadLanguage(const QString& langCode);
    void switchTranslator(QTranslator& translator, const QString& filename);
    void moveSetting(QSettings &settings, const QString &oldKey,
                     const QString &newKey);
    void adjustForCurFolder(const QString &path);
    void updateRecentFolders();
    void updateEditMenu();
    void changeAppStyle(const bool darkMode);
    void setAppStyle(const QString &name);
};

#endif /* MAINWINDOW_H */
