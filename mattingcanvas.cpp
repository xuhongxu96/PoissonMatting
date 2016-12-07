#include "mattingcanvas.h"
#include <QPainter>
#include <QDebug>

MattingCanvas::MattingCanvas(QWidget *parent) : QWidget(parent)
{

}

void MattingCanvas::setImage(QImage &img)
{
    this->img = &img;
    this->setFixedSize(img.size());
    repaint();
}

void MattingCanvas::paintEvent(QPaintEvent *)
{
    if (!img) return;
    QPainter painter(this);
    painter.drawImage(QPoint(0,0), *img);
    painter.end();
}
