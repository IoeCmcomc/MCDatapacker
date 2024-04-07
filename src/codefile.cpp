#include "codefile.h"

#include "globalhelpers.h"

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
    QString newPath = path;
    if (Glhp::removeInternalPrefix(newPath)) {
        fileType =
            Glhp::pathToFileType(QStringLiteral("data-json"), newPath);
    } else if (info.isFile()) {
        fileType = Glhp::pathToFileType(QDir::currentPath(), path);
    } else {
        qWarning() << "Non-file path:" << path;
    }
}

QDebug operator<<(QDebug debug, const CodeFile &file) {
    QDebugStateSaver saver(debug);

    debug.nospace() << "CodeFile(" << file.info << ", "
                    << file.fileType << ')';

    return debug;
}
