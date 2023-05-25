#ifndef FILENAMEDELEGATE_H
#define FILENAMEDELEGATE_H

#include <QStyledItemDelegate>

class FileNameDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit FileNameDelegate(QObject *parent = nullptr);

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const override;
};

#endif /* FILENAMEDELEGATE_H */
