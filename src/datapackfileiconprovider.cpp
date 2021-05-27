#include "datapackfileiconprovider.h"

#include "globalhelpers.h"
#include "codefile.h"

DatapackFileIconProvider::DatapackFileIconProvider() = default;

QIcon DatapackFileIconProvider::icon(const QFileInfo &info) const {
    if (info.isFile()) {
        const auto &&icon = Glhp::fileTypeToIcon(Glhp::pathToFileType(
                                                     QDir::currentPath(),
                                                     info.filePath()));
        if (!icon.isNull())
            return icon;
    }
    return QFileIconProvider::icon(info);
}
