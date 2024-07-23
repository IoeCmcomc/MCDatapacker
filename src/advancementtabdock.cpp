#include "advancementtabdock.h"
#include "ui_advancementtabdock.h"

#include "inventoryitem.h"

#include "globalhelpers.h"
#include "game.h"
#include "codefile.h"
#include "platforms/windows_specific.h"

#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>

AdvancementTabDock::AdvancementTabDock(QWidget *parent) :
    QDockWidget(parent), ui(new Ui::AdvancementTabDock) {
    ui->setupUi(this);

    connect(this, &QDockWidget::visibilityChanged, this,
            [this](const bool visible) {
        if (visible && m_pendingLoad) {
            m_pendingLoad = false;
            loadAdvancements();
        }
    });
    connect(ui->reloadBtn, &QToolButton::clicked, this,
            &AdvancementTabDock::loadAdvancements);
    connect(this, &QDockWidget::topLevelChanged, [ = ](bool floating) {
        if (floating) {
            Windows::setDarkFrameIfDarkMode(this);
        }
    });
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

    if (isHidden()) {
        m_pendingLoad = true;
        return;
    }

#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    const QString &&itemIdField =
        (Game::version() >= Game::v1_20_6) ? "id"_QL1 : "item"_QL1;

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
            if (CodeFile::pathToFileType(dirPath, path) ==
                CodeFile::Advancement) {
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
                            if (icon.contains(itemIdField)) {
                                const auto &&item =
                                    icon[itemIdField].toString();
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
        if (m_tabs.contains(id)) {
            m_tabs.value(id)->setRootAdvancement(std::move(advancements.at(id)),
                                                 id);
        } else {
            auto *tab = new AdvancementTab();
            tab->setRootAdvancement(std::move(advancements.at(id)), id);
            connect(tab, &AdvancementTab::openFileRequested,
                    this, &AdvancementTabDock::openFileRequested);
            ui->tabWidget->addTab(tab, id);
            m_tabs[id] = tab;
        }
    }
    for (auto it = m_tabs.begin(); it != m_tabs.end();) {
        if (!rootAdvancements.contains(it.key())) {
            ui->tabWidget->removeTab(ui->tabWidget->indexOf(it.value()));
            it.value()->deleteLater();
            it = m_tabs.erase(it);
        } else {
            ++it;
        }
    }

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
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
