#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QtWin>
#include <QOperatingSystemVersion>
#include <QGraphicsOpacityEffect>
#include <QTimer>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->mainInfoLabel->setText(ui->mainInfoLabel->text().arg(qApp->
                                                             applicationVersion()));

    if (QOperatingSystemVersion::current() <
        QOperatingSystemVersion::Windows8) {
        if (QtWin::isCompositionEnabled()) {
            QtWin::extendFrameIntoClientArea(this, -1, -1, -1, -1);
            setAttribute(Qt::WA_TranslucentBackground, true);
            setAttribute(Qt::WA_NoSystemBackground, false);
            setStyleSheet(QStringLiteral(
                              "AboutDialog, QLabel { background: transparent; }"));
        } else {
            QtWin::resetExtendedFrame(this);
            setAttribute(Qt::WA_TranslucentBackground, false);
        }
    }

    auto *effect = new QGraphicsOpacityEffect(ui->logoLabel);
    ui->logoLabel->setGraphicsEffect(effect);
    auto *anime = new QPropertyAnimation(effect, "opacity");
    anime->setDuration(1500);
    anime->setStartValue(0);
    anime->setEndValue(1);
    anime->setEasingCurve(QEasingCurve::InQuint);
    anime->start(QPropertyAnimation::DeleteWhenStopped);
    m_animes << anime;

    effect = new QGraphicsOpacityEffect(ui->nameLabel);
    effect->setOpacity(0);
    ui->nameLabel->setGraphicsEffect(effect);
    anime = new QPropertyAnimation(effect, "opacity");
    anime->setDuration(400);
    anime->setStartValue(0);
    anime->setEndValue(1);
    anime->setEasingCurve(QEasingCurve::InCurve);
    QTimer::singleShot(1100, anime, [anime]() {
        anime->start(QPropertyAnimation::DeleteWhenStopped);
    });
    m_animes << anime;

    effect = new QGraphicsOpacityEffect(ui->mainInfoLabel);
    effect->setOpacity(0);
    ui->mainInfoLabel->setGraphicsEffect(effect);
    anime = new QPropertyAnimation(effect, "opacity");
    anime->setDuration(300);
    anime->setStartValue(0);
    anime->setEndValue(1);
    anime->setEasingCurve(QEasingCurve::InCurve);
    QTimer::singleShot(1500, anime, [anime]() {
        anime->start(QPropertyAnimation::DeleteWhenStopped);
    });
    m_animes << anime;

    effect = new QGraphicsOpacityEffect(ui->buttonBox);
    effect->setOpacity(0);
    ui->buttonBox->setGraphicsEffect(effect);
    anime = new QPropertyAnimation(effect, "opacity");
    anime->setDuration(250);
    anime->setStartValue(0);
    anime->setEndValue(1);
    anime->setEasingCurve(QEasingCurve::InCurve);
    QTimer::singleShot(1750, anime, [anime]() {
        anime->start(QPropertyAnimation::DeleteWhenStopped);
    });
    m_animes << anime;

    m_winAnime = new QPropertyAnimation(this, "windowOpacity");
    m_winAnime->setDuration(1000);
    m_winAnime->setStartValue(0);
    m_winAnime->setEndValue(1);
    m_winAnime->setEasingCurve(QEasingCurve::Linear);
    m_winAnime->start();
}

AboutDialog::~AboutDialog() {
    delete ui;
    for (auto *anime: m_animes)
        anime->deleteLater();
}

void AboutDialog::done(int r) {
    if (!m_isClosing) {
        m_winAnime->setDirection(QPropertyAnimation::Backward);
        connect(m_winAnime, &QPropertyAnimation::finished, this,
                &QDialog::hide);
        m_winAnime->start(QPropertyAnimation::DeleteWhenStopped);
        m_isClosing = true;
        if (r == QDialog::Accepted)
            emit accepted();
        else if (r == QDialog::Rejected)
            emit rejected();
    }
}
