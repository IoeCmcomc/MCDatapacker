#ifndef CODEFILE_H
#define CODEFILE_H

#include <QTextDocument>
#include <QFileInfo>
#include <QDebug>
#include <QTextCursor>

struct CodeFile {
    enum FileType {
        Text,
        Function,
        Structure,
        JsonText,
        Advancement,
        LootTable,
        Meta,
        Predicate,
        Recipe,
        BlockTag, EntityTypeTag, FluidTag, FunctionTag, ItemTag
    };

    QFileInfo      fileInfo;
    FileType       fileType = Text;
    QString        title;
    QTextDocument *doc        = new QTextDocument();
    QTextCursor    textCursor = QTextCursor(doc);

    CodeFile(const QString path);

    CodeFile();

    inline bool isVaild() const {
        return !fileInfo.filePath().isEmpty();
    };
    void changePath(const QString path);
    void initLayout();
};

QDebug operator<<(QDebug debug, const CodeFile &file);

#endif /* CODEFILE_H */
