#ifndef POISSONMATTING_H
#define POISSONMATTING_H

#include <QImage>
#include "opencv/cv.hpp"

class PoissonMatting
{
public:
    PoissonMatting();
    void open(QString filename);
    QImage &getImage();
protected:
    cv::Mat img;
    QImage qimg;
};

#endif // POISSONMATTING_H
