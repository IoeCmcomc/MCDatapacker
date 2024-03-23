/****
 * Based on "https://github.com/vitality82/QIV/blob/master/imgviewer.h"
 * (MIT license)
 ***/

#ifndef IMGVIEWER_H
#define IMGVIEWER_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QTimer>


//Q_DECLARE_METATYPE(QImage)

class ImgViewer : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImgViewer(QWidget *parent = 0);

    void resetView();
    void fitWindow();
    void originalSize();
    void rotateView(const int nVal);
    inline bool isModified() {
        return m_rotateAngle != 0;
    }
    inline int getRotateAngle() {
        return m_rotateAngle;
    }

    bool setImage(const QString &path);
    bool setImage(const QImage &image);
    QImage getImage() const;

signals:
    void updateStatusBarRequest(ImgViewer *viewer);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *e) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

private:
    QTimer m_timer;
    QImage m_image;
    QPixmap m_pixmap;
    QPixmap m_bg;
    QGraphicsPixmapItem *m_pixmapItem = nullptr;
    QGraphicsScene *m_scene           = nullptr;
    int m_rotateAngle;
    bool m_IsFitWindow;
    bool m_IsViewInitialized;
    bool m_isBgAnimated = false;

    void redrawBg();

public slots:
    void reactToFitWindowToggle(bool);
};


#endif /* IMGVIEWER_H */
