#include "darkfusionstyle.h"

#include "private/qfusionstyle_p.h"

DarkFusionStyle::DarkFusionStyle() {
    setObjectName(QStringLiteral("darkfusion"));
    setBaseStyle(new QFusionStyle);
    qApp->setPalette(DarkFusionStyle::standardPalette());
}

// The dark palette is based on:
// https://gist.github.com/QuantumCD/6245215?permalink_comment_id=3216848#gistcomment-3216848
QPalette DarkFusionStyle::standardPalette() const {
    QPalette palette = qt_fusionPalette();

    const QColor darkGray(53, 53, 53);
    const QColor gray(128, 128, 128);
    const QColor black(25, 25, 25);
    const QColor blue(42, 130, 218);

    palette.setColor(QPalette::Window, darkGray);
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, black);
    palette.setColor(QPalette::AlternateBase, darkGray);
    palette.setColor(QPalette::ToolTipBase, blue);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, darkGray);
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::Link, Qt::cyan);
    palette.setColor(QPalette::Highlight, blue);
    palette.setColor(QPalette::HighlightedText, Qt::black);

    palette.setColor(QPalette::Midlight, darkGray);
    palette.setColor(QPalette::Mid, gray);
    palette.setColor(QPalette::Shadow, QColor(200, 200, 53));

    palette.setColor(QPalette::Active, QPalette::Button, gray.darker());
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, gray);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, gray);
    palette.setColor(QPalette::Disabled, QPalette::Text, gray);
    palette.setColor(QPalette::Disabled, QPalette::Light, darkGray);

    return palette;
}
