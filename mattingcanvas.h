#ifndef MATTINGCANVAS_H
#define MATTINGCANVAS_H

#include <QWidget>
#include <QMouseEvent>
#include <QPen>
#include <QImage>
#include "opencv/cv.hpp"

class MattingCanvas : public QWidget
{
    Q_OBJECT
public:
    static QImage cvMatToQImage(const cv::Mat &inMat);
    static cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData = true);
    explicit MattingCanvas(QWidget *parent = 0);
    QSize sizeHint() const { return img.size(); }
    QSize minimumSizeHint() const { return img.size(); }
    void setImage(cv::Mat &imgMat);
    void setDrawing(bool drawing);
    void setFilling(bool filling);
    void clearFilling();
    void clearAll();
    ~MattingCanvas();
protected:
    cv::Mat *imgMat = nullptr;
    QImage img;
    QImage trimap;
    QImage fillMap;
    QPen pen = QPen(QColor(100, 100, 100));
    bool isDrawing = false;
    bool isFilling = false;
    bool isPressed = false;
    int x, y;
signals:

public slots:

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // MATTINGCANVAS_H
