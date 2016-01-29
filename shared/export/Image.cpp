//
// Created by michiel on 26-1-2016.
//

#include "Image.h"
#include <QImage>
#include <boost/lexical_cast.hpp>
#include <shared/Colors.h>
#include "kernel/MockKernel.h"
#include <limits>

namespace OpenPSTD
{
    namespace Shared
    {

        std::vector<std::string> ExportImage::GetFormats()
        {
            auto result = std::vector<std::string>();
            result.push_back("image/png");
            result.push_back("image/bmp");
            result.push_back("image/jpg");
            return result;
        }

        bool ExportImage::FullViewSupported()
        {
            return false;
        }

        void ExportImage::ExportData(std::string format, std::shared_ptr<PSTDFile> file, std::string directory,
                                     std::string name, bool fullview, std::vector<int> domains, int startFrame,
                                     int endFrame)
        {
            MockKernel k;
            k.start_kernel(file->GetSceneConf());
            auto metadata = k.get_metadata();

            if (domains.size() == 0)
            {
                int domainCount = file->GetDomainCount();

                for (int d = 0; d < domainCount; ++d)
                {
                    domains.push_back(d);
                }
            }
            float min = std::numeric_limits<float>::infinity();
            float max = -std::numeric_limits<float>::infinity();

            for (int d = 0; d < domains.size(); ++d)
            {
                if (startFrame == -1) startFrame = 0;
                if (endFrame == -1) endFrame = file->GetFrameCount(domains[d]) - 1;
                for (int f = startFrame; f <= endFrame; ++f)
                {
                    PSTD_FRAME_PTR frame = file->GetFrame(f, domains[d]);
                    for (int i = 0; i < frame->size(); ++i)
                    {
                        min = std::min((*frame)[i], min);
                        max = std::max((*frame)[i], max);
                    }
                }
            }

            for (int d = 0; d < domains.size(); ++d)
            {
                if (startFrame == -1) startFrame = 0;
                if (endFrame == -1) endFrame = file->GetFrameCount(domains[d]) - 1;
                for (int f = startFrame; f <= endFrame; ++f)
                {
                    this->saveImage(format, file,
                                    directory + "/" + name + "-" + boost::lexical_cast<std::string>(domains[d]) + "-" +
                                    boost::lexical_cast<std::string>(f),
                                    domains[d], f, metadata.DomainMetadata[domains[d]], min, max);
                }
            }
        }

        void ExportImage::saveImage(std::string format, std::shared_ptr<PSTDFile> file, std::string output, int domain,
                                    int frame, std::vector<int> size, float min, float max)
        {
            //create image
            QImage result(size[0], size[0], QImage::Format_Indexed8);

            //create colormap
            MultiColorGradient colorScheme;
            if (min < 0)
            {
                colorScheme.AddColor(min, QColor(0, 0, 255, 255));
                colorScheme.AddColor(0, QColor(0, 0, 0, 255));
                colorScheme.AddColor(max, QColor(255, 0, 0, 255));
            }
            else
            {
                colorScheme.AddColor(min, QColor(0, 0, 0, 255));
                colorScheme.AddColor(max, QColor(255, 0, 0, 255));
            }

            std::vector<QRgb> colorMap = *colorScheme.CreateColorRGBMap(min, max, 256);
            for (int i = 0; i < colorMap.size(); ++i)
            {
                result.setColor(i, colorMap[i]);
            }

            //get data
            auto data = *file->GetFrame(frame, domain);

            //draw on image
            auto it = data.begin();
            for (int i = 0; i < size[0]; ++i)
            {
                for (int j = 0; j < size[1]; ++j)
                {
                    //int color = (int)roundf((*it)/0.10f*(colorMap.size()-1));
                    int color = (int) roundf(((*it) - min) / (max - min) * (colorMap.size() - 1));
                    result.setPixel(i, j, color);
                    it++;
                }
            }

            //save image
            if (format == "image/png")
            {
                result.save(QString::fromStdString(output + ".png"));
            }
            else if (format == "image/bmp")
            {
                result.save(QString::fromStdString(output + ".bmp"));
            }
            else if (format == "image/jpg")
            {
                result.save(QString::fromStdString(output + ".jpg"));
            }
            else
            {
                throw ExportFormatNotSupported(format, this->GetFormats());
            }
        }
    }
}
