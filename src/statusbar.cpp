#include "statusbar.h"

#include "mainwindow.h"
#include "globalhelpers.h"
#include "tabbeddocumentinterface.h"
#include "imgviewer.h"

#include "game.h"

#include <QScrollBar>
#include <QLabel>

#include <cmath>

StatusBar::StatusBar(MainWindow *parent,
                     TabbedDocumentInterface *tabInterface) : QStatusBar(
        parent) {
    m_mainWin         = parent;
    m_tabbedInterface = tabInterface;

    m_gameVerLabel = new QLabel(this);
    m_gameVerLabel->setText(tr("Minecraft %1",
                               "Example: \"Minecraft 1.16\"")
                            .arg(Game::versionString()));
    addWidget(m_gameVerLabel, 1);

    m_packFmtLabel = new QLabel(this);
    addWidget(m_packFmtLabel, 1);

    m_tabsLabel = new QLabel(this);
    addWidget(m_tabsLabel, 1);

    m_fileLabel = new QLabel(this);
    addWidget(m_fileLabel, 1);

    for (int i = 0; i < 3; ++i) {
        m_editorLabels << new QLabel(this);
        addWidget(m_editorLabels.last(), 1);
    }
    Q_ASSERT(m_editorLabels.size() == 3);
}

StatusBar::~StatusBar() {
}

void StatusBar::onCurDirChanged() {
    const auto &&info = m_mainWin->getPackInfo();

    if (info.packFormat > 0)
        m_packFmtLabel->setText(tr("Pack format: %1").arg(info.packFormat));
    else
        m_packFmtLabel->clear();
}

void StatusBar::onCurFileChanged() {
    if (!m_tabbedInterface->hasNoFile()) {
        m_tabsLabel->setText(tr("Tab %1 / %2")
                             .arg(m_tabbedInterface->getCurIndex() + 1)
                             .arg(m_tabbedInterface->count()));
        m_fileLabel->setText(Glhp::fileTypeToName(
                                 m_tabbedInterface->getCurFile()->fileType));
        for (auto *label: qAsConst(m_editorLabels))
            label->clear();
        updateCodeEditorStatus(m_tabbedInterface->getCodeEditor());
        updateImgViewerStatus(m_tabbedInterface->getImgViewer());
    } else {
        m_tabsLabel->clear();
        m_fileLabel->clear();
        for (auto *label: qAsConst(m_editorLabels))
            label->clear();
    }
}

void StatusBar::updateStatusFrom(QWidget *widget) {
    if (auto *editor = qobject_cast<CodeEditor *>(widget)) {
        updateCodeEditorStatus(editor);
    } else if (auto *viewer = qobject_cast<ImgViewer *>(widget)) {
        updateImgViewerStatus(viewer);
    }
}

void StatusBar::updateCodeEditorStatus(CodeEditor *editor) {
    if (!editor)
        return;

    Q_ASSERT(m_editorLabels.size() == 3);
    if (m_tabbedInterface->getCurFile() &&
        m_tabbedInterface->getCurFile()->fileType >= CodeFile::Text) {
        const int line = editor->textCursor().blockNumber() + 1;
        const int row  = editor->textCursor().positionInBlock();

        m_editorLabels[0]->setText(tr("Line %1, row %2").arg(line).arg(row));
        m_editorLabels[1]->setText(tr("%Ln problem(s)", nullptr,
                                      editor->problemCount()));
        m_editorLabels[2]->setText((editor->overwriteMode()) ? "OVR" : "INS");
    }
}

void StatusBar::updateImgViewerStatus(ImgViewer *viewer) {
    if (!viewer)
        return;

    if (m_tabbedInterface->getCurFile() &&
        m_tabbedInterface->getCurFile()->fileType == CodeFile::Image) {
        const auto &&img = viewer->getImage();

        m_editorLabels[0]->setText(QString("%1 x %2 px")
                                   .arg(img.width()).arg(img.height()));
        m_editorLabels[1]->setText(QString("%1, %2")
                                   .arg(viewer->horizontalScrollBar()->value())
                                   .arg(viewer->verticalScrollBar()->value()));
        m_editorLabels[2]->setText(tr("Zoom: %1%")
                                   .arg(std::round(viewer->transform().m11() *
                                                   10000) / 100));
    }
}

void StatusBar::changeEvent(QEvent *e) {
    QStatusBar::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        onCurDirChanged();
        onCurFileChanged();
    }
}
