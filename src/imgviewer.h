/****
 * Taken from "https://github.com/vitality82/QIV/blob/master/imgviewer.h"
 * (MIT license)
 ***/

#ifndef IMGVIEWER_H
#define IMGVIEWER_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QImage>

struct ImageFileData {
    QTransform transform;
    QImage     image;
    int        offsetX = -1;
    int        offsetY = -1;
};
Q_DECLARE_METATYPE(ImageFileData)

class ImgViewer : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImgViewer(QWidget *parent = 0);

    static ImageFileData fromFile(const QString &strFilePath,
                                  QString &strError);

    void resetView();
    void fitWindow();
    void originalSize();
    void rotateView(const int nVal);
    bool saveViewToDisk(QString &strError);
    inline bool isModified() {
        return m_rotateAngle != 0;
    }
    inline int getRotateAngle() {
        return m_rotateAngle;
    }

    bool setImage(const QImage &image);
    QImage getImage() const;

    void loadData(const ImageFileData &data);
    ImageFileData toData() const;

protected:
    void wheelEvent(QWheelEvent * event) override;
    void resizeEvent(QResizeEvent * event) override;
    void paintEvent(QPaintEvent *e) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

private:
    mutable QImage m_image;
    QPixmap m_pixmap;
    QPixmap m_bg;
    QGraphicsPixmapItem *m_pixmapItem = nullptr;
    QGraphicsScene *m_scene           = nullptr;
    int m_rotateAngle;
    bool m_IsFitWindow;
    bool m_IsViewInitialized;

    void redrawBg();

public slots:
    void reactToFitWindowToggle(bool);
};


#endif /* IMGVIEWER_H */
