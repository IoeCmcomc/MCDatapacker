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
    QImage image;
    QRect  viewRect;
};

class ImgViewer : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImgViewer(QWidget *parent = 0);

    bool loadFile(const QString &strFilePath, QString &strError);
    bool loadFile(const QImage &image);

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
    QString getImageFormat(QString strFileName);

    QImage getImage() const;

    void loadData(const ImageFileData &data);
    ImageFileData toData() const;

private:
    mutable QImage m_image;
    QPixmap m_pixmap;
    QGraphicsPixmapItem *m_pixmapItem;
    QGraphicsScene *m_scene;
    int m_rotateAngle;
    bool m_IsFitWindow;
    bool m_IsViewInitialized;
    QString m_fileName;

protected:
    virtual void wheelEvent(QWheelEvent * event) override;
    virtual void resizeEvent(QResizeEvent * event) override;
    void paintEvent(QPaintEvent *e) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

public slots:
    void reactToFitWindowToggle(bool);
};


#endif /* IMGVIEWER_H */
