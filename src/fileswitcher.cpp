#include "fileswitcher.h"

#include "globalhelpers.h"

#include <QShortcut>

FileSwitcher::FileSwitcher(TabbedCodeEditorInterface *parent)
    : QListWidget(parent) {
    this->parent = parent;
    setWindowFlag(Qt::Popup, true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSizeAdjustPolicy(QListWidget::AdjustToContents);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    initFileList();

    connect(this, &QListWidget::itemClicked,
            this, &QObject::deleteLater);
    connect(this, &QListWidget::itemActivated,
            this, &QObject::deleteLater);
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab), this),
            &QShortcut::activated, this, &FileSwitcher::onSelectNextItem);
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab),
                          this),
            &QShortcut::activated, this, &FileSwitcher::onSelectPrevItem);

    QRect parentRect(parentWidget()->mapToGlobal(QPoint(0, 0)),
                     parentWidget()->size());
    move(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(),
                             parentRect).topLeft());

    setCurrentRow(parent->getCurIndex());
    onSelectNextItem();
    setMinimumWidth(sizeHintForColumn(0) + 2 * frameWidth());
    setMinimumHeight(sizeHintForRow(0) * count() + 2 * frameWidth());
    setMaximumWidth(700);
    setMaximumHeight(500);
    adjustSize();
    show();
    grabMouse();
    setFocus();
}

FileSwitcher::~FileSwitcher() {
    if (parent->getCurIndex() != currentRow()) {
        parent->setCurIndex(currentRow());
    }
}

void FileSwitcher::focusOutEvent(QFocusEvent *event) {
    deleteLater();
}

void FileSwitcher::keyReleaseEvent(QKeyEvent *event) {
    QListWidget::keyPressEvent(event);
    if (!event->modifiers().testFlag(Qt::ControlModifier)) {
        deleteLater();
    }
}

void FileSwitcher::onSelectNextItem() {
    if (count() > 0) {
        if (currentRow() < count() - 1) {
            setCurrentRow(currentRow() + 1);
        } else {
            setCurrentRow(0);
        }
    }
}

void FileSwitcher::onSelectPrevItem() {
    if (count() > 0) {
        if (currentRow() > 0) {
            setCurrentRow(currentRow() - 1);
        } else {
            setCurrentRow(count() - 1);
        }
    }
}

void FileSwitcher::initFileList() {
    auto *files = parent->getFiles();

    if (!files) {
        return;
    }

    for (const auto &file: *files) {
        QFileInfo        finfo(file.fileInfo);
        QListWidgetItem *fileItem = new QListWidgetItem(this);
        fileItem->setText
            (QDir::current().relativeFilePath(finfo.filePath()));
        fileItem->setIcon(Glhp::fileTypeToIcon(file.fileType));
        fileItem->setToolTip(finfo.filePath());
        addItem(fileItem);
    }
}
