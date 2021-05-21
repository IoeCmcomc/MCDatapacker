/****
 * Taken from "https://github.com/vitality82/QIV/blob/master/imgviewer.cpp"
 * (MIT license)
 ***/

#include "imgviewer.h"
#include <QDebug>
#include <QMessageBox>
#include <QWheelEvent>
#include <QtMath>
#include <QMatrix>

ImgViewer::ImgViewer(QWidget *parent) :
    QGraphicsView(parent), m_rotateAngle(0), m_IsFitWindow(false),
    m_IsViewInitialized(false) {
    m_scene = new QGraphicsScene(this);
    this->setScene(m_scene);
    /*this->setBackgroundBrush(QBrush(Qt::gray, Qt::CrossPattern)); */
    this->setDragMode(NoDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

bool ImgViewer::loadFile(const QString &strFilePath, QString &strError) {
    QImage image;

    image.load(strFilePath);
    if (image.isNull()) {
        strError = QObject::tr("Cannot load %1.").arg(strFilePath);
        return false;
    }
    m_fileName = strFilePath;
    return loadFile(image);
}

bool ImgViewer::loadFile(const QImage &image) {
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
    return true;
}

void ImgViewer::resetView() {
    if (m_image.isNull()) {
        return;
    }

    m_scene->clear();
    m_image = QImage();
    m_fileName.clear();
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

bool ImgViewer::saveViewToDisk(QString &strError) {
/*
      if (m_image.isNull()) {
          strError = QObject::tr("Save failed.");
          return false;
      }
 */

/*
      / * save a copy * /
      QImage imageCopy = m_image;
 */

/*
      / * Output file dialog * /
      QString fileFormat  = getImageFormat(m_fileName);
      QString strFilePath = QFileDialog::getSaveFileName(
          this,
          tr("Save File"),
          QDir::homePath(),
          fileFormat);
 */


/*
      / * If Cancel is pressed, getSaveFileName() returns a null string. * /
      if (strFilePath == "") {
          strError = QObject::tr("");
          return false;
      }
 */

/*
      / * ensure output path has proper extension * /
      if (!strFilePath.endsWith(fileFormat))
          strFilePath += "." + fileFormat;
 */

/*
      / * save image in modified state * /
      if (isModified()) {
          QTransform t;
          t.rotate(m_rotateAngle);
          imageCopy = imageCopy.transformed(t, Qt::SmoothTransformation);
      }
 */

/*
      / *
         quality factor (-1 default, 100 max)
         note: -1 is about 4 times smaller than original, 100 is larger than original
 * /
      if (!imageCopy.save(strFilePath, fileFormat.toLocal8Bit().constData(),
                          100)) {
          strError = QObject::tr("Save failed.");
          return false;
      }
 */
    return true;
}

QString ImgViewer::getImageFormat(QString strFileName) {
    QString str = strFileName.mid(strFileName.lastIndexOf(".") + 1, -1);

    if (str.toLower() == "tif")
        str = "tiff";
    return str;
}

QImage ImgViewer::getImage() const {
    return m_image;
}

void ImgViewer::loadData(const ImageFileData &data) {
    loadFile(data.image);

    auto viewRect = mapToScene(viewport()->geometry()).boundingRect();
}

/* preserve fitWindow state on window resize */
void ImgViewer::resizeEvent(QResizeEvent *event) {
    if (!m_IsViewInitialized)
        return;

    if (m_IsFitWindow)
        fitWindow();
    else {
        QGraphicsView::resizeEvent(event);  /* call base implementation */
        this->centerOn(m_pixmapItem);
    }
}

void ImgViewer::paintEvent(QPaintEvent *e) {
    { /* Draw the transparency checkerboard as the fixed background */
        QPainter painter(viewport());

        static QBrush whiteSmokeBrush(QColor(245, 245, 245));
        static QBrush whiteBrush(Qt::white);
        int           count = 0;

        painter.setPen(Qt::NoPen);
        const int cellSize = 31;
        for (int x = 0; x < viewport()->width(); x += cellSize) {
            for (int y = 0; y < viewport()->height(); y += cellSize) {
                painter.setBrush((count % 2 ==
                                  0) ? whiteSmokeBrush : whiteBrush);
                painter.drawRect(x, y, cellSize, cellSize);
                count++;
            }
            /* Offset rows by 1 */
            count++;
        }
    }

    QGraphicsView::paintEvent(e);

    QPainter painter(viewport());
    painter.setBrush(Qt::white);
    painter.setPen(QPen(QBrush(Qt::black), 2));
    painter.drawRect(16, 16, 384, 96);
    auto rectToStr = [](const QRectF &rect) -> QString {
                         return QString("(%1, %2, %3, %4)").arg(rect.x()).arg(
                             rect.y()).arg(rect.width()).arg(rect.height());
                     };
    QString viewRectStr = "View rect: " +
                          rectToStr(mapToScene(
                                        viewport()->geometry()).boundingRect());
    painter.drawText(32, 32, viewRectStr);
    painter.drawText(32, 64, "Scene rect: " + rectToStr(sceneRect()));
}

void ImgViewer::drawForeground(QPainter *painter, const QRectF &) {
    QPen pen(QBrush(Qt::black), 1);

    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->drawRect(sceneRect());
    painter->drawRect(m_pixmapItem->boundingRect());

    /*qDebug() << "drawBackground rect:" << rect; */
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

    /* scale  the View */
    this->scale(factor, factor);
    event->accept();
}
