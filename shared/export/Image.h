//
// Created by michiel on 26-1-2016.
//

#ifndef OPENPSTD_IMAGE_H
#define OPENPSTD_IMAGE_H

#include "Export.h"
#include <shared/PSTDFile.h>
#include <string>
#include <vector>
#include <QImage>

namespace OpenPSTD
{
    namespace Shared
    {
        class ExportImage
        {
        private:
            bool _fullView;

            void saveImage(std::string format, std::shared_ptr<PSTDFile> file, std::string output,
                           int domain, int frame, std::vector<int> size, float min, float max);

            void saveFullImage(std::string format, std::shared_ptr<PSTDFile> file, std::string output,
                               std::vector<int> domains, int frame, std::vector<std::vector<int>> positions,
                               std::vector<std::vector<int>> sizes, float min, float max);

            void drawData(std::shared_ptr<QImage> image, Kernel::PSTD_FRAME_PTR frame, float min, float max,
                          int colormapSize, std::vector<int> position, std::vector<int> size);

        public:
            /**
             * Creates a vector with strings that describes the formats that are supported. Every element should be in the
             * mime type format, for example the png format uses image/png.
             */
            virtual std::vector<std::string> GetFormats();


            /**
             * This is where every domain is written to a single frame.
             * The result is that in a single image all the domains are shown
             */
            bool GetFullView();

            /**
             * This is where every domain is written to a single frame.
             * The result is that in a single image all the domains are shown
             */
            void SetFullView(bool value);

            /**
             * Exports a number of frames.
             * @param format: The format that has to be exported, this has to be one of the formats that is returned by GetFormats.
             * @param file: The file from where the export must be done.
             * @param directory: The directory where the output must be written too.
             * @param name: The name that must be used for the output file(s). This should be without extension, the extension is
             *              added automatically. This is that a postfix can be used when multiple frames/domains can't be
             *              contained in a single file.
             * @param domains: A list of domains that has to be exported. With an empty list, all the domains are exported.
             * @param startFrame: The first frame that must be exported (included). -1 when it should start with the first frame.
             * @param endFrame: The last frame that must be exported (included). -1 when it should should finish with the last frame.
             */
            virtual void ExportData(std::string format, std::shared_ptr<PSTDFile> file, std::string directory,
                                    std::string name, std::vector<int> domains, int startFrame, int endFrame);
        };

    }
}

#endif //OPENPSTD_IMAGE_H
