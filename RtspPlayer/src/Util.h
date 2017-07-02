#ifndef Util_h__
#define Util_h__

#include <opencv2/core/core.hpp>
#include <QImage>
#include <QVector>
#include <QColor>

namespace CVHelper
{
    cv::Mat3b QImage2Mat(const QImage &src);
    QImage Mat2QImage(const cv::Mat4b &src);
    QImage Mat2QImage(const cv::Mat_<double> &src);

    inline cv::Vec3b QColor2Vec(const QColor &src)
    {
        return cv::Vec3b(src.blue(), src.green(), src.red());
    }

    inline std::vector<cv::Vec3b> QColor2Vec(const QVector<QColor> & src)
    {
        std::vector<cv::Vec3b> dest(src.size());

        for (int i = 0; i < src.size(); ++i)
        {
            dest[i] = QColor2Vec(src[i]);
        }

        return dest;
    }

    inline QColor Vec2QColor(const cv::Vec3b &src)
    {
        return QColor(src[2], src[1], src[0]);
    }

    inline QVector<QColor> Vec2QColor(const std::vector<cv::Vec3b> & src)
    {
        QVector<QColor> dest(src.size());

        for (size_t i = 0; i < src.size(); ++i)
        {
            dest[i] = Vec2QColor(src[i]);
        }

        return dest;
    }
}
#endif // Util_h__
