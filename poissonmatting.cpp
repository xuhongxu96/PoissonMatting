#include "poissonmatting.h"
#include <QDebug>

PoissonMatting::PoissonMatting()
{

}

void PoissonMatting::open(QString filename)
{
    img = cv::imread(filename.toStdString());
    switch (img.type()) {
    case CV_8UC1:
        cvtColor(img, img, CV_GRAY2RGB);
        break;
    case CV_8UC3:
        cvtColor(img, img, CV_BGR2RGB);
        break;
    }
    assert(img.isContinuous());
    qimg = QImage((uchar *) img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
}

QImage &PoissonMatting::getImage()
{
    return qimg;
}
