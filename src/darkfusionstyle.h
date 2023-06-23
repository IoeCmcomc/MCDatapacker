#ifndef DARKFUSIONSTYLE_H
#define DARKFUSIONSTYLE_H

#include <QApplication>
#include <QProxyStyle>

extern QPalette qt_fusionPalette();

class DarkFusionStyle : public QProxyStyle {
    Q_OBJECT;
public:
    DarkFusionStyle();
    QPalette standardPalette() const override;
    QIcon standardIcon(QStyle::StandardPixmap standardIcon,
                       const QStyleOption *option = nullptr,
                       const QWidget *widget      = nullptr) const override;
};

#endif // DARKFUSIONSTYLE_H
