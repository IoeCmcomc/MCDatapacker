#include "darkfusionstyle.h"

#include "private/qfusionstyle_p.h"

#include <QStyleOption>
#include <QDebug>

DarkFusionStyle::DarkFusionStyle() {
    setObjectName(QStringLiteral("DarkFusion"));
    setBaseStyle(new QFusionStyle);
}

// The dark palette is based on:
// https://gist.github.com/QuantumCD/6245215?permalink_comment_id=3216848#gistcomment-3216848
QPalette DarkFusionStyle::standardPalette() const {
    QPalette palette = qt_fusionPalette();

    const QColor darkGray(53, 53, 53);
    const QColor gray(127, 127, 127);
    const QColor black(25, 25, 25);
    const QColor altBlack(39, 39, 39);
    const QColor blue(24, 165, 229);
    const QColor button(62, 62, 62);
    const QColor yellowGreen(200, 200, 53);

    palette.setColor(QPalette::Window, darkGray);
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, black);
    palette.setColor(QPalette::AlternateBase, altBlack);
    palette.setColor(QPalette::ToolTipBase, blue);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, button);
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::Link, Qt::cyan);
    palette.setColor(QPalette::Highlight, blue);
    palette.setColor(QPalette::HighlightedText, Qt::black);

    palette.setColor(QPalette::Midlight, darkGray);
    palette.setColor(QPalette::Mid, gray);
    palette.setColor(QPalette::Shadow, yellowGreen);

    palette.setColor(QPalette::Active, QPalette::Button, button.darker());
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, gray);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, gray);
    palette.setColor(QPalette::Disabled, QPalette::Text, gray);
    palette.setColor(QPalette::Disabled, QPalette::Light, darkGray);

    return palette;
}

QIcon DarkFusionStyle::standardIcon(StandardPixmap standardIcon,
                                    const QStyleOption *option,
                                    const QWidget *widget) const {
//    qDebug() << standardIcon << option << widget;
    if (standardIcon == QStyle::SP_DialogCloseButton) {
        auto &&icon   = QProxyStyle::standardIcon(standardIcon, option, widget);
        auto &&pixmap = icon.pixmap(option->rect.size());
        auto &&image  = pixmap.toImage();

        QRgb         *st         = reinterpret_cast<QRgb *>(image.bits()); // Detach the image
        const quint64 pixelCount = image.width() * image.height();

        for (quint64 p = 0; p < pixelCount; ++p) {
            const auto pixel = st[p];
            int        red   = 255 - qRed(pixel);
            int        green = 255 - qGreen(pixel);
            int        blue  = 255 - qBlue(pixel);
            int        alpha = qAlpha(pixel);
            st[p] = qRgba(red, green, blue, alpha);
        }

        return QPixmap::fromImage(image);
    }
    return QProxyStyle::standardIcon(standardIcon, option, widget);
}
