#include "advancementtabdock.h"
#include "ui_advancementtabdock.h"

#include "advancementtab.h"
#include "inventoryitem.h"

#include "globalhelpers.h"

#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>

AdvancementTabDock::AdvancementTabDock(QWidget *parent) :
    QDockWidget(parent), ui(new Ui::AdvancementTabDock) {
    ui->setupUi(this);
}

AdvancementTabDock::~AdvancementTabDock() {
    delete ui;
}

void AdvancementTabDock::loadAdvancements() {
    const static QMap<QString,
                      AdvancemDisplayInfo::FrameType> stringToFrameType = {
        { "task",      AdvancemDisplayInfo::FrameType::Task      },
        { "goal",      AdvancemDisplayInfo::FrameType::Goal      },
        { "challenge", AdvancemDisplayInfo::FrameType::Challenge },
    };

    for (int i = ui->tabWidget->count(); i >= 0; --i) {
        ui->tabWidget->widget(i)->deleteLater();
        ui->tabWidget->removeTab(i);
    }

    // Read all showable advancements

    QDir          &&dir     = QDir::current();
    const QString &&dirPath = dir.path();

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    dir.setNameFilters({ "*.json"_QL1 });

    QDirIterator it(dir, QDirIterator::Subdirectories);

    std::map<QString, AdvancemDisplayInfo> advancements;
    QVector<QString>                       rootAdvancements;
    while (it.hasNext()) {
        const QString &&path = it.next();

        const auto &&finfo = it.fileInfo();
        if (finfo.isFile()) {
            if (Glhp::pathToFileType(dirPath, path) == CodeFile::Advancement) {
                QJsonObject obj;
                QFile       file(path);
                if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    const QByteArray &&data = file.readAll();
                    const auto       &&doc  = QJsonDocument::fromJson(data);
                    if (!doc.isNull() && doc.isObject()) {
                        obj = doc.object();
                    }
                    file.close();
                }
                if (!obj.isEmpty()) {
                    AdvancemDisplayInfo advancemInfo;
                    if (obj.contains("display")) {
                        const auto &&display = obj["display"].toObject();
                        if (display.contains("icon")) {
                            const auto &&icon = display["icon"].toObject();
                            if (icon.contains("item")) {
                                const auto &&item = icon["item"].toString();
                                if (!item.isEmpty()) {
                                    advancemInfo.displayIcon =
                                        InventoryItem{ item }.getPixmap();
                                } else {
                                    qWarning() << "No item in" <<
                                        Glhp::toNamespacedID(dirPath, path);
                                    continue;
                                }
                            }
                        }
                        if (display.contains("title")) {
                            advancemInfo.title = display["title"];
                        } else {
                            qWarning() << "No title in" << Glhp::toNamespacedID(
                                dirPath, path);
                            continue;
                        }
                        if (display.contains("description")) {
                            advancemInfo.description = display["description"];
                        } else {
                            qWarning() << "No description in" <<
                                Glhp::toNamespacedID(dirPath, path);
                            continue;
                        }
                        advancemInfo.backgroundPath =
                            display.value("background").toString();
                        advancemInfo.frameType =
                            stringToFrameType.value(
                                display.value("frame").toString("task"));
                        advancemInfo.hidden = display.value("hidden").toBool();
                    } else {
                        continue;
                    }
                    advancemInfo.parent = obj.value("parent").toString();
                    if (!obj.contains("criteria")) {
                        qWarning() << "No criteria in" << Glhp::toNamespacedID(
                            dirPath, path);
                        continue;
                    }
                    const auto &&id = Glhp::toNamespacedID(dirPath, path);
                    advancements[id] = std::move(advancemInfo);
                }
            }
        }
    }

    // Build advancement trees from the list

    QMap<QString, AdvancemDisplayInfo *> advancementRefs;
    for (auto &[key, value]: advancements) {
        advancementRefs[key] = &value;
    }

    for (auto it = advancementRefs.cbegin(); it != advancementRefs.cend();
         ++it) {
        const auto &parent = it.value()->parent;
        if (parent.isEmpty()) {
            rootAdvancements += it.key();
        } else if (advancementRefs.contains(parent)) {
            advancementRefs.value(parent)->children.insert(advancements.extract(
                                                               it.key()));
        }
    }

    // Populate advancement tabs

    for (const auto &id: qAsConst(rootAdvancements)) {
        auto *tab = new AdvancementTab();
        tab->setRootAdvancement(std::move(advancements.at(id)), id);
        connect(tab, &AdvancementTab::openFileRequested,
                this, &AdvancementTabDock::openFileRequested);
        ui->tabWidget->addTab(tab, id);
    }
}

void AdvancementTabDock::changeEvent(QEvent *e) {
    QDockWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}
