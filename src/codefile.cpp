#include "codefile.h"

#include "globalhelpers.h"

CodeFile::CodeFile(const QString &path) {
    changePath(path);
}

void CodeFile::changePath(const QString &path) {
    fileInfo = QFileInfo(path);
    title    = fileInfo.fileName();
    fileType = Glhp::pathToFileType(QDir::currentPath(), path);
}

QDebug operator<<(QDebug debug, const CodeFile &file) {
    QDebugStateSaver saver(debug);

    debug.nospace() << "CodeFile(" << file.fileInfo << ", "
                    << file.title << ", "
                    << file.fileType << ", "
                    << file.data << ')';

    return debug;
}
