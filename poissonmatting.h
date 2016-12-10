#ifndef POISSONMATTING_H
#define POISSONMATTING_H

#include <QImage>
#include "opencv/cv.hpp"

class PoissonMatting
{
public:
    PoissonMatting();
    void open(QString filename);
    cv::Mat &getImageMat();
    void matting(cv::InputArray _trimap, cv::OutputArray _foreground, cv::OutputArray _alpha);
protected:
    cv::Mat img;
    static std::vector<cv::Point> findBoundaryPixels(const cv::Mat_<uchar> &trimap, int a, int b);
    void _matting(cv::Mat _image, cv::Mat _trimap, cv::Mat &_foreground, cv::Mat &_alpha);
};

#endif // POISSONMATTING_H
