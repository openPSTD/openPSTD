//
// Created by michiel on 24-1-2016.
//

#ifndef OPENPSTD_EXPORT_H
#define OPENPSTD_EXPORT_H

#include <shared/PSTDFile.h>
#include <string>
#include <vector>

enum ExportType
{
    EXPORT_TYPE_DOMAIN,
    EXPORT_TYPE_RECEIVER
};

struct ExportFormat
{
public:
    std::string format;
    ExportType type;
};

/**
 * Possible export implementations:
 *  Domain:
 *   Images (image/png, image/bmp, image/jpg)
 *   HDF5 (application/x-hdf)
 *   CSV (text/csv)
 *  Receiver:
 *   Wav (audio/wav)
 *   CSV (text/csv)
 */

/**
 * An interface to export domain information
 */
class ExportDomain
{
public:
    /**
     * Creates a vector with strings that describes the formats that are supported. Every element should be in the
     * mime type format, for example the png format uses image/png.
     */
    virtual std::vector<std::string> GetFormats() = 0;
    /**
     * true if full view is supported. Full view is when all the domains are shown in the same view.
     */
    virtual bool FullViewSupported() = 0;
    /**
     * Exports a number of frames.
     * @param format: The format that has to be exported, this has to be one of the formats that is returned by GetFormats.
     * @param file: The file from where the export must be done.
     * @param directory: The directory where the output must be written too.
     * @param name: The name that must be used for the output file(s). This should be without extension, the extension is
     *              added automatically. This is that a postfix can be used when multiple frames/domains can't be
     *              contained in a single file.
     * @param fullview: A function named fullview is enabled. This is where every domain is written to a single frame.
     *              The result is that in a single image all the domains are shown.
     * @param domains: A list of domains that has to be exported. With an empty list, all the domains are exported.
     * @param startFrame: The first frame that must be exported. -1 when it should start with the first frame.
     * @param endFrame: The last frame that must be exported. -1 when it should should finish with the last frame.
     */
    virtual void ExportData(std::string format, std::shared_ptr<PSTDFile> file, std::string directory, std::string name,
                        bool fullview, std::vector<int> domains, int startFrame, int endFrame) = 0;
};

/**
 * An interface to export receiver information
 */
class ExportReceiver
{
public:
    /**
     * Creates a vector with strings that describes the formats that are supported. Every element should be in the
     * mime type format, for example the png format uses image/png.
     */
    virtual std::vector<std::string> GetFormats() = 0;
    /**
     * Exports a number of frames.
     * @param format: The format that has to be exported, this has to be one of the formats that is returned by GetFormats.
     * @param file: The file from where the export must be done.
     * @param directory: The directory where the output must be written too.
     * @param name: The name that must be used for the output file(s). This should be without extension, the extension is
     *              added automatically. This is that a postfix can be used when multiple frames/domains can't be
     *              contained in a single file.
     * @param receivers: A list of receivers that has to be exported. With an empty list, all the domains are exported.
     * @param startFrame: The first frame that must be exported. -1 when it should start with the first frame.
     * @param endFrame: The last frame that must be exported. -1 when it should should finish with the last frame.
     */
    virtual void ExportData(std::string format, std::shared_ptr<PSTDFile> file, std::string directory, std::string name,
                            std::vector<int> receivers, int startFrame, int endFrame) = 0;
};

class ExportFormatNotSupported : public std::exception
{
private:
    std::string _format;
    std::vector<std::string> _supportedFormats;
public:
    /**
     * Built exception for format errors
     */
    ExportFormatNotSupported(std::string format, std::vector<std::string> supportedFormats);
    const char* what() const noexcept override;
};

#endif //OPENPSTD_EXPORT_H
