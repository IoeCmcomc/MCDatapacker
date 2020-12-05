#include "datapackfileiconprovider.h"

#include "globalhelpers.h"
#include "codefile.h"
#include "mainwindow.h"

DatapackFileIconProvider::DatapackFileIconProvider() {
}

QIcon DatapackFileIconProvider::icon(const QFileInfo &info) const {
    if (info.isFile()) {
        auto fileType = Glhp::pathToFileType(MainWindow::getCurDir(),
                                             info.absoluteFilePath());
        switch (fileType) {
        case CodeFile::Function:
            return QIcon(":/file-mcfunction");

        case CodeFile::Structure:
            return QIcon(":/file-nbt");

        case CodeFile::Meta:
            return QIcon(":/file-mcmeta");

        default: {
            if (fileType >= CodeFile::JsonText)
                return QIcon(":/file-json");

            break;
        }
        }
    }
    return QFileIconProvider::icon(info);
}
