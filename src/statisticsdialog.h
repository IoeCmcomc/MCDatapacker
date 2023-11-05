#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>

#include "parsers/command/visitors/nodecounter.h"


namespace Ui {
    class StatisticsDialog;
}

namespace Command {
    class McfunctionParser;
};

class MainWindow;

class StatisticsDialog : public QDialog
{
    Q_OBJECT

    struct SyntaxErrorInfo {
        QString path;
        QString msg;
        int     line = -1;
    };

public:
    explicit StatisticsDialog(MainWindow *parent = nullptr);
    ~StatisticsDialog();

signals:
    void openFileWithLineRequested(const QString &path, const int lineNo);

private /*slots*/ :
    void onErrorTableDoubleClicked(int row, int column);

private:
    QVector<SyntaxErrorInfo> m_syntaxErrorsInfo;
    QString m_dirPath;
    Ui::StatisticsDialog *ui;
    MainWindow *m_mainWin               = nullptr;
    Command::McfunctionParser *m_parser = nullptr;
    Command::NodeCounter m_nodeCounter;
    uint m_commandLines = 0;
    uint m_commentLines = 0;
    uint m_macroLines   = 0;
    uint m_syntaxErrors = 0;

    void collectAndSetupData();
    void collectFunctionData(const QString &path);
};

#endif /* STATISTICSDIALOG_H */
