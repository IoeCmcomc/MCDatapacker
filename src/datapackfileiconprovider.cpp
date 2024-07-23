#include "datapackfileiconprovider.h"

#include "globalhelpers.h"
#include "codefile.h"

DatapackFileIconProvider::DatapackFileIconProvider() = default;

QIcon DatapackFileIconProvider::icon(const QFileInfo &info) const {
    if (info.isFile()) {
        const auto &&fileIcon = CodeFile::fileTypeToIcon(CodeFile::pathToFileType(
                                                         QDir::currentPath(),
                                                         info.filePath()));
        if (!fileIcon.isNull())
            return fileIcon;
    }
    return QFileIconProvider::icon(info);
}
