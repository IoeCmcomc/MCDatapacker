#ifndef DARKFUSIONSTYLE_H
#define DARKFUSIONSTYLE_H

#include <QApplication>
#include <QProxyStyle>

extern QPalette qt_fusionPalette();

class DarkFusionStyle : public QProxyStyle {
    Q_OBJECT;
public:
    DarkFusionStyle();
    QPalette standardPalette() const override;;
};

#endif // DARKFUSIONSTYLE_H
