#ifndef CODEFILE_H
#define CODEFILE_H

#include "highlighter.h"

#include <QFileInfo>
#include <QDebug>

struct CodeFile {
    enum FileType : char {
        Unknown = -1,
        Binary,
        Structure,
        Image,
        Text,
        Function,
        JsonText,
        Advancement,
        LootTable,
        Meta,
        Predicate,
        Recipe,
        BlockTag, EntityTypeTag, FluidTag, FunctionTag, ItemTag,
    };

    QVariant  data;
    QFileInfo fileInfo;
    QString   title;
    FileType  fileType   = Unknown;
    bool      isModified = false;

    CodeFile(const QString &path);

    inline bool isVaild() const {
        return (!fileInfo.filePath().isEmpty()) && data.isValid();
    };
    void changePath(const QString &path);
};

QDebug operator<<(QDebug debug, const CodeFile &file);

#endif /* CODEFILE_H */
