#include "fileswitcher.h"

#include "globalhelpers.h"

#include <QShortcut>
#include <QApplication>
#include <QScreen>

FileSwitcher::FileSwitcher(TabbedDocumentInterface *parent, bool backward)
    : QListWidget(parent) {
    this->parent = parent;
    setWindowFlag(Qt::Popup, true);
    setSelectionMode(QAbstractItemView::SingleSelection);

    connect(this, &QListWidget::itemClicked, this, &QObject::deleteLater);
    connect(this, &QListWidget::itemActivated, this, &QObject::deleteLater);
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab), this),
            &QShortcut::activated, this, &FileSwitcher::onSelectNextItem);
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab),
                          this),
            &QShortcut::activated, this, &FileSwitcher::onSelectPrevItem);

    initFileList();

    const QRect parentRect(parent->mapToGlobal({ 0, 0 }),
                           parent->size());
    move(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(),
                             parentRect).topLeft());

    setCurrentRow(parent->getCurIndex());
    if (!backward)
        onSelectNextItem();
    else
        onSelectPrevItem();
    show();
    grabMouse();
    setFocus();
}

FileSwitcher::~FileSwitcher() {
    if (parent->getCurIndex() != currentRow()) {
        parent->setCurIndex(currentRow());
    }
}

QSize FileSwitcher::sizeHint() const {
    const int width  = sizeHintForColumn(0) + 2 * frameWidth();
    const int height = sizeHintForRow(0) * count() + 2 * frameWidth();

    return QSize(width, height).boundedTo(qApp->screens().at(0)->size());
}

void FileSwitcher::show() {
    adjustSize();

    const auto scrSize  = qApp->screens().at(0)->size();
    QPoint     newPoint = pos();

    if ((newPoint.x() + width()) > scrSize.width())
        newPoint.rx() = scrSize.width() - width();
    if ((newPoint.y() + height()) > scrSize.height())
        newPoint.ry() = scrSize.height() - height();

    newPoint.rx() = qMax(newPoint.x(), 0);
    newPoint.ry() = qMax(newPoint.y(), 0);

    move(newPoint);
    adjustSize();

    QListWidget::show();
}

void FileSwitcher::focusOutEvent(QFocusEvent *) {
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
    const auto *files = parent->getFiles();

    if (!files) {
        return;
    }

    for (const auto &file: *files) {
        const QFileInfo  finfo(file.info);
        QListWidgetItem *fileItem = new QListWidgetItem(this);
        fileItem->setText
            (QDir::current().relativeFilePath(finfo.filePath()));
        fileItem->setIcon(Glhp::fileTypeToIcon(file.fileType));
        fileItem->setToolTip(finfo.filePath());
        addItem(fileItem);
    }
}
