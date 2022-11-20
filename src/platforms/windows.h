#ifndef WINDOWS_H
#define WINDOWS_H

#include <QtGlobal>

#ifdef Q_OS_WIN
#include <QtWin>
#endif

class QWidget;

void extendFrameOnWindows(QWidget *w, const QString &className);

#endif // WINDOWS_H
