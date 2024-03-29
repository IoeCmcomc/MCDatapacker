/****
 * Based on "https://github.com/vitality82/QIV/blob/master/imgviewer.cpp"
 * (MIT license)
 ***/

#include "imgviewer.h"
#include <QDebug>
#include <QMessageBox>
#include <QWheelEvent>
#include <QtMath>
#include <QShortcut>
#include <QScrollBar>

const static int cellSize = 47;

ImgViewer::ImgViewer(QWidget *parent) :
    QGraphicsView(parent), m_rotateAngle(0), m_IsFitWindow(false),
    m_IsViewInitialized(false) {
    m_scene = new QGraphicsScene(this);
    this->setScene(m_scene);
    this->setDragMode(NoDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);

    redrawBg();

    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this),
            &QShortcut::activated, this, [this]() {
        setImage(m_image);
    });
    connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, [this]() {
        emit updateStatusBarRequest(this);
    });
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, [this]() {
        emit updateStatusBarRequest(this);
    });

    m_timer.setParent(this);
    m_timer.setInterval(100);
    connect(&m_timer, &QTimer::timeout, this, [this]() {
        updateScene({ m_scene->sceneRect() });
    });
    m_timer.start();
}

bool ImgViewer::setImage(const QImage &image) {
    resetView();
    m_image      = image;
    m_pixmap     = QPixmap::fromImage(m_image);
    m_pixmapItem = m_scene->addPixmap(m_pixmap); /* add pixmap to scene and return pointer to pixmapItem */
    m_scene->setSceneRect(m_pixmap.rect());      /* set scene rect to image */
    this->centerOn(m_pixmapItem);                /* ensure item is centered in the view. */

    /* preserve fitWindow if activated */
    if (m_IsFitWindow)
        fitWindow();
    else
        this->setDragMode(ScrollHandDrag);

    m_IsViewInitialized = true;
    emit updateStatusBarRequest(this);
    return true;
}

void ImgViewer::resetView() {
    if (m_image.isNull()) {
        return;
    }

    m_scene->clear();
    m_image       = QImage();
    m_rotateAngle = 0;
    this->setDragMode(NoDrag);
    this->resetTransform();
}

void ImgViewer::reactToFitWindowToggle(bool checked) {
    m_IsFitWindow = checked;
    if (checked)
        fitWindow();
    else
        originalSize();
}

void ImgViewer::fitWindow() {
    if (m_image.isNull())
        return;

    this->setDragMode(NoDrag);
    this->resetTransform();
    QPixmap px = m_pixmap; /* scale a copy to viewsize (scaling the original results in blurred image) */
    px = px.scaled(QSize(this->width(), this->height()),
                   Qt::KeepAspectRatio,
                   Qt::SmoothTransformation);
    m_pixmapItem->setPixmap(px);
    m_scene->setSceneRect(px.rect());
}

void ImgViewer::originalSize() {
    if (m_image.isNull())
        return;

    this->setDragMode(ScrollHandDrag);

    m_pixmap = m_pixmap.scaled(QSize(m_image.width(), m_image.height()),
                               Qt::KeepAspectRatio,
                               Qt::SmoothTransformation);
    m_pixmapItem->setPixmap(m_pixmap);
    m_scene->setSceneRect(m_pixmap.rect());

    /*setTransform(QTransform::fromScale(1, 1)); */
    this->centerOn(m_pixmapItem);
}

void ImgViewer::rotateView(const int nVal) {
    if (m_image.isNull()) {
        return;
    }

    /* rotate view */
    this->rotate(nVal);
    /*this->show(); // this necessary? */

    /* update angle */
    m_rotateAngle += nVal;  /* a=a+(-90)== -90 */

    /* reset angle */
    if (m_rotateAngle >= 360 || m_rotateAngle <= -360) {
        m_rotateAngle = 0;
    }
}

bool ImgViewer::setImage(const QString &path) {
    QImage image(path);

    if (image.isNull()) {
        return false;
    } else {
        return setImage(image);
    }
}

QImage ImgViewer::getImage() const {
    return m_image;
}

/* preserve fitWindow state on window resize */
void ImgViewer::resizeEvent(QResizeEvent *event) {
    if (!m_IsViewInitialized)
        return;

    redrawBg();
    if (m_IsFitWindow)
        fitWindow();
    else {
        QGraphicsView::resizeEvent(event);  /* call base implementation */
        this->centerOn(m_pixmapItem);
    }
}

void ImgViewer::paintEvent(QPaintEvent *e) {
    {
        QPainter painter(viewport());

        static int progress = 0;
        if (progress >= cellSize)
            progress = 0;
        else
            progress += 1;
        painter.translate(-progress, -progress);

        painter.drawPixmap(QPoint(0, 0), m_bg, m_bg.rect());
    }

    QGraphicsView::paintEvent(e);

/*    QPainter painter(viewport()); */

/*    painter.setPen(QPen(QBrush(Qt::black), 2)); */

/*
      painter.setBrush(Qt::white);
      QRect debugRect(16, 16, 400, 96);
      painter.drawRect(debugRect);
      auto rectToStr = [](const QRectF &rect) -> QString {
                           return QString("(%1, %2, %3, %4)").arg(rect.x()).arg(
                               rect.y()).arg(rect.width()).arg(rect.height());
                       };
      QString &&str =
          QString("Viewport rect: %1\n"
                  "Transformation scaling: %2, %3\n"
                  "Transformation translation: %4, %5\n"
                  "Scrollbar offsets: %6. %7"
                  ).arg(rectToStr(mapToScene(
                                      viewport()->geometry()).boundingRect()))
          .arg(transform().m11()).arg(transform().m22())
          .arg(transform().dx()).arg(transform().dy())
          .arg(horizontalScrollBar()->value()).arg(verticalScrollBar()->value());
      painter.drawText(debugRect, Qt::AlignLeft | Qt::AlignTop, str);
 */
}

void ImgViewer::drawForeground(QPainter *painter, const QRectF &) {
    QPen pen(palette().windowText(), 1);

    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->drawRect(sceneRect());
    painter->drawRect(m_pixmapItem->boundingRect());

    /*qDebug() << "drawBackground rect:" << rect; */
}

// Re:Draw the transparency checkerboard as the fixed background
void ImgViewer::redrawBg() {
    const QColor &base    = palette().base().color();
    const QBrush &altBase = palette().alternateBase();

    m_bg = QPixmap(size() + QSize(cellSize, cellSize));
    m_bg.fill(base);
    QPainter painter(&m_bg);

    painter.setBrush(altBase);
    painter.setPen(Qt::NoPen);
    bool xWhite = false;
    for (int x = 0; x - cellSize < width(); x += cellSize) {
        for (int y = 0; y - cellSize < height(); y += cellSize * 2) {
            painter.drawRect(x, y + cellSize * xWhite, cellSize, cellSize);
        }
        xWhite = !xWhite;
    }
}

/* scale image on wheelEvent */
void ImgViewer::wheelEvent(QWheelEvent *event) {
    /* prevent zooming when fitWindow is active */
    if (m_IsFitWindow)
        return;

    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    /* zoomIn factor */
    qreal factor = 1.15;

    /* zoomOut factor */
    if (event->angleDelta().y() < 0)
        factor = 1.0 / factor;

    /* scale the View */
    this->scale(factor, factor);
    event->accept();

    emit updateStatusBarRequest(this);
}
