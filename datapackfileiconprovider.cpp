#include "datapackfileiconprovider.h"

#include "globalhelpers.h"
#include "codefile.h"
#include "mainwindow.h"

DatapackFileIconProvider::DatapackFileIconProvider() {
}

QIcon DatapackFileIconProvider::icon(const QFileInfo &info) const {
    if (info.isFile()) {
        auto icon = Glhp::fileTypeToIcon(Glhp::pathToFileType(
                                             MainWindow::getCurDir(),
                                             info.absoluteFilePath()));
        if (!icon.isNull())
            return icon;
    }
    return QFileIconProvider::icon(info);
}
