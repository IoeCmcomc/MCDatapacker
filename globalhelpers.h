#ifndef GLOBALHELPERS_H
#define GLOBALHELPERS_H

#include "mainwindow.h"
#include "vieweventfilter.h"
#include "tabbedcodeeditorinterface.h"

#include <QString>
#include <QVariant>
#include <QLayout>
#include <QJsonArray>
#include <QJsonObject>

namespace Glhp {
    void someTest();
    bool isPathRelativeTo(const QString &dirpath, const QString &path,
                          const QString &catDir);
    QString randStr(int length = 5);
    QString relPath(const QString &dirpath, QString path);
    QString relNamespace(const QString &dirpath, QString path);
    CodeFile::FileType pathToFileType(const QString &dirpath,
                                      const QString &filepath);
    QString toNamespacedID(const QString &dirpath, QString filepath);
    QVariant strToVariant(const QString &str);
    QString variantToStr(const QVariant &vari);
    QVector<QString> fileIDList(const QString &dirpath, const QString &catDir,
                                const QString &nspace = "");
    void deleteChildrenIn(QWidget *widget);

    void removePrefix(QString &str, const QString &prefix);


    /* Template functions */
    template < class T >
    void loadJsonToObjectsToLayout(const QJsonArray &arr,
                                   QLayout &layout) {
        Q_ASSERT(layout.parentWidget() != nullptr);
        auto *parent = layout.parentWidget();
        /*parent->setUpdatesEnabled(false); */
        for (auto childRef : arr) {
            auto childData = childRef.toObject();
            T   *child     = new T(parent);
            child->setMinimumWidth(300);
            layout.addWidget(child);
            child->fromJson(childRef.toObject());
            child->adjustSize();
        }
        /*parent->setUpdatesEnabled(true); */
    }

    template < class T >
    QJsonArray getJsonFromObjectsFromParent(const QObject *parent) {
        int childCount = parent->children().count();

        if (childCount != 0) {
            QJsonArray arr;
            for (auto *child : parent->children()) {
                T *childT = qobject_cast < T* > (child);
                if (childT != nullptr)
                    arr.append(childT->toJson());
            }
            return arr;
        }
        return QJsonArray();
    }
}

#endif /* GLOBALHELPERS_H */
