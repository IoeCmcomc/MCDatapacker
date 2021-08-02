#include "filenamedelegate.h"

#include <QLineEdit>
#include <QTimer>

FileNameDelegate::FileNameDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void FileNameDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const {
    QStyledItemDelegate::setEditorData(editor, index);
    if (auto *lineEdit = qobject_cast<QLineEdit*>(editor)) {
        QTimer::singleShot(0, this, [lineEdit]() {
            const int lastDotPos = lineEdit->text().lastIndexOf('.');
            if (lastDotPos != -1) {
                /* Select the name without the extension */
                lineEdit->setSelection(0, lastDotPos);
            } else if (lastDotPos == 0) {
                lineEdit->setCursorPosition(0);
            } else {
                lineEdit->selectAll();
            }
        });
    }
}
