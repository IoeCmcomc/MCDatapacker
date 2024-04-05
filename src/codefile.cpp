#include "codefile.h"

#include "globalhelpers.h"
#ifndef NO_GAME_H
#include "game.h"
#endif

CodeFile::CodeFile(const QString &path) {
    changePath(path);
}

QString CodeFile::name() const {
    return info.fileName();
}

QString CodeFile::path() const {
    return info.filePath();
}

void CodeFile::changePath(const QString &path) {
    info = QFileInfo(path);
#ifndef NO_GAME_H
    if (path.startsWith(QStringLiteral(":/minecraft/"))) {
        QString newPath = path;
        Glhp::removePrefix(newPath, ":/minecraft/"_QL1);
        const int index = newPath.indexOf('/');
        newPath.remove(0, index + 1);
        fileType = Glhp::pathToFileType(QStringLiteral("data-json"), newPath);
    } else {
        fileType = Glhp::pathToFileType(QDir::currentPath(), path);
    }
#else
    fileType = Glhp::pathToFileType(QDir::currentPath(), path);
#endif
}

QDebug operator<<(QDebug debug, const CodeFile &file) {
    QDebugStateSaver saver(debug);

    debug.nospace() << "CodeFile(" << file.info << ", "
                    << file.fileType << ')';

    return debug;
}
