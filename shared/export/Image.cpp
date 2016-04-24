//
// Created by michiel on 26-1-2016.
//

#include "Image.h"
#include <boost/lexical_cast.hpp>
#include <shared/Colors.h>
#include "kernel/MockKernel.h"

namespace OpenPSTD
{
    namespace Shared
    {

        OPENPSTD_SHARED_EXPORT std::vector<std::string> ExportImage::GetFormats()
        {
            auto result = std::vector<std::string>();
            result.push_back("image/png");
            result.push_back("image/bmp");
            result.push_back("image/jpg");
            return result;
        }

        OPENPSTD_SHARED_EXPORT bool ExportImage::GetFullView()
        {
            return _fullView;
        }
        OPENPSTD_SHARED_EXPORT void ExportImage::SetFullView(bool value)
        {
            _fullView = value;
        }

        OPENPSTD_SHARED_EXPORT void ExportImage::ExportData(std::string format, std::shared_ptr<PSTDFile> file, std::string directory,
                                     std::string name, std::vector<int> domains, int startFrame,
                                     int endFrame)
        {
            Kernel::MockKernel k;
            k.initialize_kernel(file->GetResultsSceneConf());
            auto metadata = k.get_metadata();

            if (domains.size() == 0)
            {
                int domainCount = file->GetResultsDomainCount();

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
                if (endFrame == -1) endFrame = file->GetResultsFrameCount(domains[d]) - 1;
                for (int f = startFrame; f <= endFrame; ++f)
                {
                    Kernel::PSTD_FRAME_PTR frame = file->GetResultsFrame(f, domains[d]);
                    for (int i = 0; i < frame->size(); ++i)
                    {
                        min = std::min((*frame)[i], min);
                        max = std::max((*frame)[i], max);
                    }
                }
            }

            if(this->_fullView)
            {
                if (startFrame == -1) startFrame = 0;
                if (endFrame == -1) endFrame = file->GetResultsFrameCount(domains[0]) - 1;
                for (int f = startFrame; f <= endFrame; ++f)
                {
                    this->saveFullImage(format, file, directory + "/" + name + "-" + boost::lexical_cast<std::string>(f),
                                        domains, f, metadata.DomainPositions, metadata.DomainMetadata, min, max);
                }
            }
            else
            {
                for(int d : domains)
                {
                    if (startFrame == -1) startFrame = 0;
                    if (endFrame == -1) endFrame = file->GetResultsFrameCount(d) - 1;
                    for (int f = startFrame; f <= endFrame; ++f)
                    {
                        this->saveImage(format, file,
                                        directory + "/" + name + "-" + boost::lexical_cast<std::string>(d) + "-" +
                                        boost::lexical_cast<std::string>(f),
                                        d, f, metadata.DomainMetadata[d], min, max);
                    }
                }
            }

        }

        OPENPSTD_SHARED_NO_EXPORT void ExportImage::drawData(std::shared_ptr<QImage> image, Kernel::PSTD_FRAME_PTR frame, float min, float max,
                                   int colormapSize, std::vector<int> position, std::vector<int> size)
        {
            //draw on image
            auto it = frame->begin();
            for (int i = 0; i < size[0]; ++i)
            {
                for (int j = 0; j < size[1]; ++j)
                {
                    int color = (int) roundf(((*it) - min) / (max - min) * (colormapSize - 1));
                    image->setPixel(position[0] + i, position[1] + j, color);
                    it++;
                }
            }
        }

        OPENPSTD_SHARED_NO_EXPORT void ExportImage::saveImage(std::string format, std::shared_ptr<PSTDFile> file, std::string output, int domain,
                                    int frame, std::vector<int> size, float min, float max)
        {
            //create image
            std::shared_ptr<QImage> result = std::make_shared<QImage>(size[0], size[1], QImage::Format_Indexed8);

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
                result->setColor(i, colorMap[i]);
            }

            //get data
            auto data = file->GetResultsFrame(frame, domain);

            //position
            std::vector<int> position = {0, 0};

            //draw on image
            drawData(result, data, min, max, colorMap.size(), position, size);

            //save image
            if (format == "image/png")
            {
                result->save(QString::fromStdString(output + ".png"));
            }
            else if (format == "image/bmp")
            {
                result->save(QString::fromStdString(output + ".bmp"));
            }
            else if (format == "image/jpg")
            {
                result->save(QString::fromStdString(output + ".jpg"));
            }
            else
            {
                throw ExportFormatNotSupported(format, this->GetFormats());
            }
        }

        OPENPSTD_SHARED_NO_EXPORT void ExportImage::saveFullImage(std::string format, std::shared_ptr<PSTDFile> file, std::string output,
                                        std::vector<int> domains, int frame, std::vector<std::vector<int>> positions,
                                        std::vector<std::vector<int>> sizes, float min, float max)
        {
            //gets the maximal values of the frame
            int minX = std::numeric_limits<int>::max(), minY = std::numeric_limits<int>::max();
            int maxX = std::numeric_limits<int>::min(), maxY = std::numeric_limits<int>::min();

            for (int d : domains)
            {
                int x = positions[d][0];
                minX = std::min(x, minX);
                maxX = std::max(positions[d][0] + sizes[d][0], maxX);

                minY = std::min(positions[d][1], minY);
                maxY = std::max(positions[d][1] + sizes[d][1], maxY);
            }

            maxX = maxX - minX;
            maxY = maxY - minY;

            for (int i = 0; i < positions.size(); ++i)
            {
                positions[i][0] = positions[i][0] - minX;
                positions[i][1] = positions[i][1] - minY;
            }

            //create image
            std::shared_ptr<QImage> result = std::make_shared<QImage>(maxX, maxY, QImage::Format_Indexed8);

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

            std::vector<QRgb> colorMap = *colorScheme.CreateColorRGBMap(min, max, 255);
            for (int i = 0; i < colorMap.size(); ++i)
            {
                result->setColor(i, colorMap[i]);
            }
            //back color
            result->setColor(255, QColor(127, 127, 127).rgb());

            for (int i = 0; i < maxX; ++i)
            {
                for (int j = 0; j < maxY; ++j)
                {
                    result->setPixel(i, j, 255);
                }
            }

            for (int d : domains)
            {
                //get data
                auto data = file->GetResultsFrame(frame, d);

                //draw on image
                drawData(result, data, min, max, colorMap.size(), positions[d], sizes[d]);
            }

            //save image
            if (format == "image/png")
            {
                result->save(QString::fromStdString(output + ".png"));
            }
            else if (format == "image/bmp")
            {
                result->save(QString::fromStdString(output + ".bmp"));
            }
            else if (format == "image/jpg")
            {
                result->save(QString::fromStdString(output + ".jpg"));
            }
            else
            {
                throw ExportFormatNotSupported(format, this->GetFormats());
            }
        }
    }
}
