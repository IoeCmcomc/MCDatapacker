#include "codefile.h"

#include "globalhelpers.h"

CodeFile::CodeFile(const QString &path) {
    changePath(path);
}

void CodeFile::changePath(const QString &path) {
    info     = QFileInfo(path);
    fileType = Glhp::pathToFileType(QDir::currentPath(), path);
}

QDebug operator<<(QDebug debug, const CodeFile &file) {
    QDebugStateSaver saver(debug);

    debug.nospace() << "CodeFile(" << file.info << ", "
                    << file.fileType << ", "
                    << file.data << ')';

    return debug;
}
