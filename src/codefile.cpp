#include "codefile.h"

#include <QPlainTextDocumentLayout>

CodeFile::CodeFile(const QString &path) {
    initLayout();
    changePath(path);
}

CodeFile::CodeFile() {
    initLayout();
}

void CodeFile::changePath(const QString &path) {
    fileInfo = QFileInfo(path);
    title    = fileInfo.fileName();
    doc->setMetaInformation(QTextDocument::DocumentTitle, title);
}

void CodeFile::initLayout() {
    auto *layout = new QPlainTextDocumentLayout(doc);

    doc->setDocumentLayout(layout);
}

QDebug operator<<(QDebug debug, const CodeFile &file) {
    QDebugStateSaver saver(debug);

    debug.nospace() << "CodeFile(" << file.fileInfo << ", "
                    << file.title << ", "
                    << file.fileType << ", "
                    << &file.textCursor << ", "
                    << file.doc << ')';

    return debug;
}
