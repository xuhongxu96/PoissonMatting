#include "mattingcanvas.h"
#include <QPainter>
#include <QDebug>

QImage MattingCanvas::cvMatToQImage(const cv::Mat &inMat)
{
    switch ( inMat.type() )
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_ARGB32 );

        return image;
    }
    // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_RGB888 );

        return image.rgbSwapped();
    }
    // 8-bit, 1 channel
    case CV_8UC1:
    {
        static QVector<QRgb>  sColorTable( 256 );

        // only create our color table the first time
        if ( sColorTable.isEmpty() )
        {
            for ( int i = 0; i < 256; ++i )
            {
                sColorTable[i] = qRgb( i, i, i );
            }
        }

        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_Indexed8 );

        image.setColorTable( sColorTable );

        return image;
    }
    default:
        qWarning() << "cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
        break;
    }
    return QImage();
}

cv::Mat MattingCanvas::QImageToCvMat(const QImage &inImage, bool inCloneImageData )
{
    switch ( inImage.format() )
    {
    // 8-bit, 4 channel
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    {
        cv::Mat  mat( inImage.height(), inImage.width(),
                      CV_8UC4,
                      const_cast<uchar*>(inImage.bits()),
                      static_cast<size_t>(inImage.bytesPerLine())
                      );

        return (inCloneImageData ? mat.clone() : mat);
    }

    // 8-bit, 3 channel
    case QImage::Format_RGB32:
    case QImage::Format_RGB888:
    {
        if ( !inCloneImageData )
        {
            qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning because we use a temporary QImage";
        }

        QImage   swapped;

        if ( inImage.format() == QImage::Format_RGB32 )
            swapped = inImage.convertToFormat( QImage::Format_RGB888 );

        swapped = inImage.rgbSwapped();

        return cv::Mat( swapped.height(), swapped.width(),
                        CV_8UC3,
                        const_cast<uchar*>(swapped.bits()),
                        static_cast<size_t>(swapped.bytesPerLine())
                        ).clone();
    }
    // 8-bit, 1 channel
    case QImage::Format_Indexed8:
    {
        cv::Mat  mat( inImage.height(), inImage.width(),
                      CV_8UC1,
                      const_cast<uchar*>(inImage.bits()),
                      static_cast<size_t>(inImage.bytesPerLine())
                      );

        return (inCloneImageData ? mat.clone() : mat);
    }
    default:
        qWarning() << "QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
        break;
    }

    return cv::Mat();
}

MattingCanvas::MattingCanvas(QWidget *parent) : QWidget(parent)
{
    pen.setWidth(penSize);
    setMouseTracking(true);
}

cv::Mat MattingCanvas::getTrimapMat() const
{
    cv::Mat temp;
    cv::cvtColor(fillMat, temp, CV_RGB2GRAY);
    return temp;
}

void MattingCanvas::setImage(cv::Mat &imgMat)
{
    this->imgMat = &imgMat;
    this->img = cvMatToQImage(imgMat);
    this->setFixedSize(img.size());
    trimap = QImage(img.size(), QImage::Format_ARGB32);
    trimap.fill(Qt::transparent);
    refill = true;
    repaint();
}

void MattingCanvas::setDrawing(bool drawing)
{
    isDrawing = drawing;
}

void MattingCanvas::setFilling(bool filling)
{
    isFilling = filling;
}

void MattingCanvas::clearFilling()
{
    fillMap = QImage();
    refill = true;
    repaint();
}

void MattingCanvas::clearAll()
{
    trimap.fill(Qt::transparent);
    clearFilling();
}

void MattingCanvas::setPenSize(int s)
{
    pen.setWidth(s);
    penSize = s;
}

void MattingCanvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), img);
    painter.drawImage(QPoint(0, 0), trimap);
    painter.setCompositionMode(QPainter::CompositionMode_SoftLight);
    painter.drawImage(QPoint(0, 0), fillMap);
    painter.setCompositionMode(QPainter::CompositionMode_Overlay);
    painter.drawRect(QRect(mouseX - penSize / 2, mouseY - penSize / 2, penSize, penSize));
    painter.end();
}

void MattingCanvas::mousePressEvent(QMouseEvent *event)
{
    isPressed = true;
    x = event->x();
    y = event->y();
}

void MattingCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    isPressed = false;
    x = event->x();
    y = event->y();
    if (isFilling) {
        if (refill) {
            refill = false;
            fillMat = QImageToCvMat(trimap);
            cv::cvtColor(fillMat, fillMat, CV_RGBA2RGB);
        }
        cv::floodFill(fillMat, cv::Point(x, y), cv::Scalar(255, 255, 255));
        fillMap = cvMatToQImage(fillMat);
        repaint();
    }
}

void MattingCanvas::mouseMoveEvent(QMouseEvent *event)
{
    mouseX = event->x();
    mouseY = event->y();
    if (isDrawing && isPressed) {
        if (abs(x - event->x()) + abs(y - event->y()) > 5) {
            QPainter painter(&trimap);
            painter.setPen(pen);
            painter.drawLine(QPoint(x, y), QPoint(event->x(), event->y()));
            painter.end();
            refill = true;
            x = event->x();
            y = event->y();
        }
    }
    repaint();
}
