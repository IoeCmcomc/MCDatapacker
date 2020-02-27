#include "globalhelpers.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

using namespace GlobalHelpers;

void GlobalHelpers::someTest() {
    qDebug() << "GlobalHelpers::someTest()";
}

MainWindow::MCRFileType GlobalHelpers::toMCRFileType(const QString &dirpath,
                                                     const QString &filepath) {
    QFileInfo     info     = QFileInfo(filepath);
    const QString jsonExts = "json mcmeta";

    if (info.completeSuffix() == "mcfunction") {
        return MainWindow::Function;
    } else if (info.completeSuffix() == "nbt") {
        return MainWindow::Structure;
    } else if (jsonExts.contains(info.completeSuffix())) {
        if (isPathRelativeTo(dirpath, filepath, "advancements")) {
            return MainWindow::Advancement;
        } else if (isPathRelativeTo(dirpath, filepath, "loot_tables")) {
            return MainWindow::LootTable;
        } else if (isPathRelativeTo(dirpath, filepath, "predicates")) {
            return MainWindow::Predicate;
        } else if (isPathRelativeTo(dirpath, filepath, "recipes")) {
            return MainWindow::Recipe;
        } else if (isPathRelativeTo(dirpath, filepath, "tags/blocks")) {
            return MainWindow::BlockTag;
        } else if (isPathRelativeTo(dirpath, filepath, "tags/entity_types")) {
            return MainWindow::EntityTypeTag;
        } else if (isPathRelativeTo(dirpath, filepath, "tags/fluids")) {
            return MainWindow::FluidTag;
        } else if (isPathRelativeTo(dirpath, filepath, "tags/functions")) {
            return MainWindow::FunctionTag;
        } else if (isPathRelativeTo(dirpath, filepath, "tags/items")) {
            return MainWindow::ItemTag;
        } else {
            return MainWindow::JsonText;
        }
    } else {
        return MainWindow::Text;
    }
}

QString GlobalHelpers::relPath(const QString &dirpath, QString path) {
    QString dataDir = dirpath + "/";

    if (path.startsWith(dataDir)) {
        path = path.remove(0, dataDir.length());
    } else {
        return "";
    }
    return path;
}

QString GlobalHelpers::relNamespace(const QString &dirpath, QString path) {
    QString rp = relPath(dirpath, path);

    if (rp.startsWith("data/")) {
        rp.remove(0, 5);
        rp = rp.section('/', 0, 0);
    } else {
        rp = "";
    }
    return rp;
}

QString GlobalHelpers::randStr(int length) {
    const QString charset("abcdefghijklmnopqrstuvwxyz0123456789");
    QString       r;

    for (int i = 0; i < (length + 1); ++i) {
        int index = qrand() % charset.length();
        r.append(charset.at(index));
    }
    return r;
}

bool GlobalHelpers::isPathRelativeTo(const QString &dirpath,
                                     const QString &path,
                                     const QString &catDir) {
    auto tmpDir = dirpath + "/data/";

    if (!path.startsWith(tmpDir)) return false;

    return path.mid(tmpDir.length()).section('/', 1).startsWith(catDir);
}

QString GlobalHelpers::toNamespacedID(const QString &dirpath,
                                      QString filepath) {
    auto    datapath = dirpath + "/data/";
    QString r        = "";

    if (filepath.startsWith(datapath)) {
        auto finfo = QFileInfo(filepath);
        filepath = finfo.dir().path() + '/' + finfo.completeBaseName();
        filepath.remove(0, datapath.length());
        if (isPathRelativeTo(dirpath, filepath, "tags")) {
            if (filepath.split('/').count() >= 4)
                r = "#" + filepath.section('/', 0, 0)
                    + ':' + filepath.section('/', 3);
        } else {
            if (filepath.split('/').count() >= 3)
                r = filepath.section('/', 0, 0)
                    + ':' + filepath.section('/', 2);
        }
    }
    return r;
}
