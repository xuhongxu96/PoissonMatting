#include "poissonmatting.h"
#include <QDebug>

PoissonMatting::PoissonMatting()
{

}

void PoissonMatting::open(QString filename)
{
    img = cv::imread(filename.toStdString());
}

QImage &PoissonMatting::getImage()
{
    return qimg;
}

cv::Mat &PoissonMatting::getImageMat()
{
    return img;
}
