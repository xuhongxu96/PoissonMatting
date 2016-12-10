#include "poissonmatting.h"
#include <QDebug>
#include <algorithm>
#include <vector>

/****************************************************
 *
 * Some useful function
 *
 */

template <class T>
T sqr(T t) {
    return t * t;
}

double dist_sqr(cv::Point p1, cv::Point p2) {
    return sqr(p1.x - p2.x) + sqr(p1.y - p2.y);
}
int color_dis(cv::Vec3b p1, cv::Vec3b p2) {
    int t1 = fmax(fmax(p1[0], p1[1]), p1[2]);
    int t2 = fmax(fmax(p2[0], p2[1]), p2[2]);
    return t1 - t2;
}

template <class T>
int inX(const T &image, int x) {
    if (x < 0) x = 0;
    if (x >= image.cols) x = image.cols - 1;
    return x;
}

template <class T>
int inY(const T &image, int y) {
    if (y < 0) y = 0;
    if (y >= image.rows) y = image.rows - 1;
    return y;
}

double intensity(cv::Vec3b v) {
    return fmax(fmax(v[0], v[1]), v[2]);
}

/***************************************************/

PoissonMatting::PoissonMatting()
{

}

void PoissonMatting::open(QString filename)
{
    img = cv::imread(filename.toStdString());
}

cv::Mat &PoissonMatting::getImageMat()
{
    return img;
}

void PoissonMatting::matting(cv::InputArray _trimap, cv::OutputArray _foreground, cv::OutputArray _alpha)
{
    cv::Mat image = img;
    cv::Mat trimap = _trimap.getMat();
    
    if (image.empty()) 
        CV_Error(CV_StsBadArg, "Image is empty");
    if (image.type() != CV_8UC3) 
        CV_Error(CV_StsBadArg, "Image must have CV_8UC3 type");
    
    if (trimap.empty()) 
        CV_Error(CV_StsBadArg, "Trimap is empty");
    if (trimap.type() != CV_8UC1) 
        CV_Error(CV_StsBadArg, "Trimap must have CV_8UC1 type");
    
    if (image.size() != trimap.size()) 
        CV_Error(CV_StsBadArg, "Trimap and image must have the same size.");
        
    cv::Mat &foreground = _foreground.getMatRef();
    cv::Mat &alpha = _alpha.getMatRef();
    
    _matting(image, trimap, foreground, alpha);
}

std::vector<cv::Point> PoissonMatting::findBoundaryPixels(const cv::Mat_<uchar> &trimap, int a, int b)
{
    std::vector<cv::Point> result;

    for (int x = 1; x < trimap.cols - 1; ++x) {
        for (int y = 1; y < trimap.rows - 1; ++y) {
            if (trimap(y, x) == a) {
                if (trimap(y - 1, x) == b ||
                        trimap(y + 1, x) == b ||
                        trimap(y, x - 1) == b ||
                        trimap(y, x + 1) == b) {
                    result.push_back(cv::Point(x, y));
                }
            }
        }
    }

    return result;
}


void PoissonMatting::_matting(cv::Mat _image, cv::Mat _trimap, cv::Mat &_foreground, cv::Mat &_alpha)
{
    const cv::Mat_<cv::Vec3b> &image = static_cast<const cv::Mat_<cv::Vec3b> &>(_image);
    cv::Mat_<uchar> &trimap = static_cast<cv::Mat_<uchar> &>(_trimap);

    _foreground.create(image.size(), CV_8UC3);
    _alpha.create(image.size(), CV_8UC1);

    cv::Mat_<cv::Vec3b> &foreground = static_cast<cv::Mat_<cv::Vec3b>&>(_foreground);
    cv::Mat_<uchar> &alpha = static_cast<cv::Mat_<uchar>&>(_alpha);

    cv::Mat_<double> FminusB = cv::Mat_<double>::zeros(trimap.rows, trimap.cols);

    for (int times = 0; times < 5; ++times) {

        // Output the Progress
        qDebug() << times;

        std::vector<cv::Point> foregroundBoundary = findBoundaryPixels(trimap, 255, 128);
        std::vector<cv::Point> backgroundBoundary = findBoundaryPixels(trimap, 0, 128);

        cv::Mat_<uchar> trimap_blur;

        // Smooth Trimap by gaussian filter to denoise
        cv::GaussianBlur(trimap, trimap_blur, cv::Size(9, 9), 0);

        // Build the F-B Map
        for (int x = 0; x < trimap.cols; ++x) {
            for (int y = 0; y < trimap.rows; ++y) {
                cv::Point current;
                current.x = x;
                current.y = y;
                if (trimap_blur(y, x) == 255) {
                    FminusB(y, x) = color_dis(image(y, x), cv::Vec3b(0, 0, 0));
                } else if (trimap_blur(y, x) == 0) {
                    FminusB(y, x) = color_dis(cv::Vec3b(0, 0, 0), image(y, x));
                } else {
                    // is in Unknown Area
                    // Find Nearest Foreground and Background Point
                    cv::Point nearestForegroundPoint, nearestBackgroundPoint;
                    double nearestForegroundDistance = 1e9, nearestBackgroundDistance = 1e9;
                    for(cv::Point &p : foregroundBoundary) {
                        double t = dist_sqr(p, current);
                        if (t < nearestForegroundDistance) {
                            nearestForegroundDistance = t;
                            nearestForegroundPoint = p;
                        }
                    }
                    for(cv::Point &p : backgroundBoundary) {
                        double t = dist_sqr(p, current);
                        if (t < nearestBackgroundDistance) {
                            nearestBackgroundDistance = t;
                            nearestBackgroundPoint = p;
                        }
                    }
                    // Calculate F - B
                    FminusB(y, x) = color_dis(image(nearestForegroundPoint.y, nearestForegroundPoint.x),
                                              image(nearestBackgroundPoint.y, nearestBackgroundPoint.x));
                    if (FminusB(y, x) == 0)
                        FminusB(y, x) = 1e-9;
                }
            }
        }

        // Smooth (F - B) image by Gaussian filter
        cv::GaussianBlur(FminusB, FminusB, cv::Size(9, 9), 0);

        // Solve the Poisson Equation By The Gauss-Seidel Method (Iterative Method)
        for (int times2 = 0; times2 < 300; ++times2) {
            for (int x = 0; x < trimap.cols; ++x) {
                for (int y = 0; y < trimap.rows; ++y) {
                    if (trimap(y, x) == 128) {
                        // is in Unknown Area
#define I(x, y) (intensity(image(inY(image, y), inX(image, x))))
#define FmB(y, x) (FminusB(inY(FminusB, y), inX(FminusB, x)))
                        // Calculate the divergence
                        double dvgX = ((I(x + 1, y) + I(x - 1, y) - 2 * I(x, y)) * FmB(y, x)

                                - (I(x + 1, y) - I(x, y)) * (FmB(y, x + 1) - FmB(y, x)))
                                / (FmB(y, x) * FmB(y, x));
                        double dvgY = ((I(x, y + 1) + I(x, y - 1) - 2 * I(x, y)) * FmB(y, x)
                                - (I(x, y + 1) - I(x, y)) * (FmB(y + 1, x) - FmB(y, x)))
                                / (FmB(y, x) * FmB(y, x));
                        double dvg = dvgX + dvgY;
#undef FmB
#undef I
                        // Calculate the New Alpha (Gauss-Seidel Method)
                        double newAlpha = (((double)alpha(y, x + 1)
                                        + alpha(y, x - 1)
                                        + alpha(y + 1, x)
                                        + alpha(y - 1, x)
                                        - dvg * 255.0) / 4.0);
                        // Update the Trimap
                        if (newAlpha > 253) {
                            // fore
                            trimap(y, x) = 255;
                        } else if (newAlpha < 3) {
                            // back
                            trimap(y, x) = 0;
                        }
                        // Avoid overflow
                        if (newAlpha < 0) {
                            newAlpha = 0;
                        }
                        if (newAlpha > 255) {
                            newAlpha = 255;
                        }
                        // Assign new alpha
                        alpha(y, x) = newAlpha;
                    } else if (trimap(y, x) == 255) {
                        // is Foreground
                        alpha(y, x) = 255;
                    } else if (trimap(y, x) == 0) {
                        // is Background
                        alpha(y, x) = 0;
                    }
                }
            }
        }

        /*
         * Show In-process Trimap, Alpha, and F-B Images For Debug
         *
         *
        QString ttl = "tri" + QString::number(times);
        cv::imshow(ttl.toStdString().c_str(), trimap);
        ttl = "alp" + QString::number(times);
        cv::imshow(ttl.toStdString().c_str(), alpha);
        FminusB = (FminusB + 255) / 2;
        cv::Mat_<uchar> pic = cv::Mat_<uchar>::zeros(trimap.rows, trimap.cols);
        for (int x = 0; x < trimap.cols; ++x) {
            for (int y = 0; y < trimap.rows; ++y) {
                pic(y, x) = (uchar) FminusB(y, x);
            }
        }
        ttl = "f-b" + QString::number(times);
        cv::imshow(ttl.toStdString().c_str(), pic);
         *
         *
         */
    }

    // Generate Foreground Image (Red Background)
    for (int x = 0; x < alpha.cols; ++x) {
        for (int y = 0; y < alpha.rows; ++y) {
            foreground(y, x) = ((double) alpha(y, x) / 255) * image(y, x) + ((255.0 - alpha(y, x)) / 255 * cv::Vec3b(0, 0, 255));
        }
    }
}
