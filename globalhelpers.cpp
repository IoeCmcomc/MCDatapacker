#include "globalhelpers.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

using namespace Glhp;

void Glhp::someTest() {
    qDebug() << "GlobalHelpers::someTest()";
}

MainWindow::MCRFileType Glhp::toMCRFileType(const QString &dirpath,
                                            const QString &filepath) {
    if (filepath.isEmpty())
        return MainWindow::Text;

    QFileInfo     info     = QFileInfo(filepath);
    const QString jsonExts = "json mcmeta";

    if (info.suffix() == "mcfunction") {
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

QString Glhp::relPath(const QString &dirpath, QString path) {
    QString dataDir = dirpath + "/";

    if (path.startsWith(dataDir)) {
        path = path.remove(0, dataDir.length());
    } else {
        return "";
    }
    return path;
}

QString Glhp::relNamespace(const QString &dirpath, QString path) {
    QString rp = relPath(dirpath, path);

    if (rp.startsWith("data/")) {
        rp.remove(0, 5);
        rp = rp.section('/', 0, 0);
    } else {
        rp = "";
    }
    return rp;
}

QString Glhp::randStr(int length) {
    const QString charset("abcdefghijklmnopqrstuvwxyz0123456789");
    QString       r;

    for (int i = 0; i < (length + 1); ++i) {
        int index = qrand() % charset.length();
        r.append(charset.at(index));
    }
    return r;
}

bool Glhp::isPathRelativeTo(const QString &dirpath,
                            const QString &path,
                            const QString &catDir) {
    auto tmpDir = dirpath + "/data/";

    if (!path.startsWith(tmpDir)) return false;

    return path.mid(tmpDir.length()).section('/', 1).startsWith(catDir);
}

QString Glhp::toNamespacedID(const QString &dirpath,
                             QString filepath) {
    auto    datapath = dirpath + "/data/";
    QString r;

    if (filepath.startsWith(datapath)) {
        auto finfo = QFileInfo(filepath);
        filepath = finfo.dir().path() + '/' + finfo.completeBaseName();
        filepath.remove(0, datapath.length());
        if (isPathRelativeTo(dirpath, finfo.filePath(), "tags")) {
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

QVariant Glhp::strToVariant(const QString &str) {
    bool isInt;
    auto intValue = str.toInt(&isInt);

    if (isInt) {
        return intValue;
    } else {
        if (str == QStringLiteral("true") || str == QStringLiteral("false")) {
            return str == QStringLiteral("true");
        } else {
            return str;
        }
    }
}

QString Glhp::variantToStr(const QVariant &vari) {
    if (vari.type() == QVariant::Bool)
        return vari.toBool() ? QStringLiteral("true") : QStringLiteral("false");
    else if (vari.type() == QVariant::Int)
        return vari.toString();
    else
        return vari.toString();
}

QVector<QString> Glhp::fileIDList(const QString &dirpath,
                                  const QString &catDir,
                                  const QString &nspace) {
    QVector<QString> IDList;
    QString          dataPath = dirpath + QStringLiteral("/data/");

    auto appendIDToList =
        [&](const QString &nspace)->void {
            QString tagPath = dataPath + nspace + '/' + catDir;
            QDir    IDDir(tagPath);

            if (IDDir.exists() && (!IDDir.isEmpty())) {
                auto names =
                    IDDir.entryList(
                        QDir::Files | QDir::NoDotAndDotDot);
                for (auto name : names) {
                    name = name.section('.', 0, 0);
                    IDList.push_back(nspace + ":" + name);
                }
            }
        };

    if (nspace.isEmpty()) {
        QDir dir(dataPath);
        auto nspaceDirs = dir.entryList(
            QDir::Dirs | QDir::NoDotAndDotDot);
        for (auto nspaceDir : nspaceDirs) {
            auto nspace = nspaceDir.section('.', 0, 0);
            appendIDToList(nspace);
        }
    } else {
        appendIDToList(nspace);
    }
    return IDList;
}

void Glhp::deleteChildrenIn(QWidget *widget) {
    /*widget->setUpdatesEnabled(false); */
    auto children = widget->findChildren<QWidget*>
                        (QString(), Qt::FindDirectChildrenOnly);

    for (auto child : children)
        child->deleteLater();
    /*widget->setUpdatesEnabled(true); */
}

void Glhp::removePrefix(QString &str, const QString &prefix) {
    if (str.startsWith(prefix))
        str.remove(0, prefix.length());
}
