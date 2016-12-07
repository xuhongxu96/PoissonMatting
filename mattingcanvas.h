#ifndef MATTINGCANVAS_H
#define MATTINGCANVAS_H

#include <QWidget>

class MattingCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit MattingCanvas(QWidget *parent = 0);
    void setImage(QImage &img);
    QSize sizeHint() const { return img ? img->size() : QSize(); }
    QSize minimumSizeHint() const { return img ? img->size() : QSize(); }
protected:
    void paintEvent(QPaintEvent *event);

    QImage *img = nullptr;
signals:

public slots:
};

#endif // MATTINGCANVAS_H
