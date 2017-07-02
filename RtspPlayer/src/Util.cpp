#include "Util.h"

namespace CVHelper
{
    cv::Mat3b QImage2Mat(const QImage &src)
    {
        unsigned int height = src.height();
        unsigned int width = src.width();

        cv::Mat3b dest(height, width);
        for (unsigned int y = 0; y < height; ++y)
        {
            cv::Vec3b *destrow = dest[y];
            for (unsigned int x = 0; x < width; ++x)
            {
                QRgb pxl = src.pixel(x, y);
                destrow[x] = cv::Vec3b(qBlue(pxl), qGreen(pxl), qRed(pxl));
            }
        }
        return dest;
    }

    QImage Mat2QImage(const cv::Mat4b &src)
    {
        QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
        for (int y = 0; y < src.rows; ++y)
        {
            const cv::Vec4b *srcrow = src[y];
            QRgb *destrow = (QRgb*)dest.scanLine(y);
            for (int x = 0; x < src.cols; ++x)
            {
                destrow[x] = qRgba(srcrow[x][2], srcrow[x][1], srcrow[x][0], 255);
            }
        }
        return dest;
    }


    QImage Mat2QImage(const cv::Mat_<double> &src)
    {
        double scale = 255.0;
        QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
        for (int y = 0; y < src.rows; ++y)
        {
            const double *srcrow = src[y];
            QRgb *destrow = (QRgb*)dest.scanLine(y);
            for (int x = 0; x < src.cols; ++x)
            {
                unsigned int color = srcrow[x] * scale;
                destrow[x] = qRgba(color, color, color, 255);
            }
        }
        return dest;
    }
}