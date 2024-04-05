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
    info     = QFileInfo(path);
    fileType = Glhp::pathToFileType(QDir::currentPath(), path);
}

QDebug operator<<(QDebug debug, const CodeFile &file) {
    QDebugStateSaver saver(debug);

    debug.nospace() << "CodeFile(" << file.info << ", "
                    << file.fileType << ')';

    return debug;
}
