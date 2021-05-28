#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QPropertyAnimation>

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

public slots:
    void done(int r) override;

private:
    Ui::AboutDialog *ui;
    QVector<QPropertyAnimation*> m_animes;
    QPropertyAnimation *m_winAnime = nullptr;
    bool m_isClosing               = false;
};

#endif /* ABOUTDIALOG_H */
