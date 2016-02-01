//
// Created by michiel on 24-1-2016.
//

#include "MockKernel.h"
#include <boost/lexical_cast.hpp>

namespace OpenPSTD
{
    namespace Kernel
    {
        void MockKernel::initialize_kernel(std::shared_ptr<PSTDConfiguration> config)
        {
            _conf = config;
        }

        void MockKernel::run(KernelCallback *callback)
        {
            if (!_conf)
                throw PSTDKernelNotConfiguredException();

            auto meta = get_metadata();

            callback->Callback(CALLBACKSTATUS::STARTING, "Starting to mock", -1);

            for (int i = 0; i < meta.Framecount; ++i)
            {
                callback->Callback(CALLBACKSTATUS::RUNNING, "At frame " + boost::lexical_cast<std::string>(i), i);
                for (int j = 0; j < _conf->Domains.size(); ++j)
                {
                    PSTD_FRAME_PTR frame;
                    int type = j % 6;
                    switch (type)
                    {
                        case 0:
                            frame = CreateRandomFrame(meta.DomainMetadata[j][0], meta.DomainMetadata[j][1]);
                            break;
                        case 1:
                            frame = CreateHorizontalLines(meta.DomainMetadata[j][0], meta.DomainMetadata[j][1]);
                            break;
                        case 2:
                            frame = CreateVerticalLines(meta.DomainMetadata[j][0], meta.DomainMetadata[j][1]);
                            break;
                        case 3:
                            frame = CreateHorizontalGradient(meta.DomainMetadata[j][0], meta.DomainMetadata[j][1]);
                            break;
                        case 4:
                            frame = CreateVerticalGradient(meta.DomainMetadata[j][0], meta.DomainMetadata[j][1]);
                            break;
                        case 5:
                            frame = CreateVerticalGradientNeg(meta.DomainMetadata[j][0], meta.DomainMetadata[j][1]);
                            break;
                    }
                    callback->WriteFrame(i, j, frame);
                }
            }

            callback->Callback(CALLBACKSTATUS::FINISHED, "finished mocking", -1);
        }

        SimulationMetadata MockKernel::get_metadata()
        {
            if (!_conf)
                throw PSTDKernelNotConfiguredException();

            SimulationMetadata result;

            float grid = _conf->Settings.GetGridSpacing();
            for (int i = 0; i < _conf->Domains.size(); ++i)
            {
                std::vector<int> d;
                d.push_back(roundf(_conf->Domains[i].Size.x() / grid));
                d.push_back(roundf(_conf->Domains[i].Size.y() / grid));
                d.push_back(1);

                result.DomainMetadata.push_back(d);

                std::vector<int> d2;
                d2.push_back(roundf(_conf->Domains[i].TopLeft.x() / grid));
                d2.push_back(roundf(_conf->Domains[i].TopLeft.y() / grid));
                d2.push_back(1);

                result.DomainPositions.push_back(d2);
            }

            result.Framecount = (int) (_conf->Settings.GetRenderTime() / _conf->Settings.GetTimeStep());

            return result;
        }

        PSTD_FRAME_PTR MockKernel::CreateRandomFrame(int x, int y)
        {
            PSTD_FRAME_PTR result = std::make_shared<PSTD_FRAME>();
            result->reserve((unsigned long) (x * y));
            for (int i = 0; i < x * y; ++i)
            {
                float v = rand() / ((float) RAND_MAX);
                result->push_back(v);
            }
            return result;
        }

        PSTD_FRAME_PTR MockKernel::CreateHorizontalLines(int x, int y)
        {
            PSTD_FRAME_PTR result = std::make_shared<PSTD_FRAME>();
            result->reserve((unsigned long) (x * y));
            for (int j = 0; j < y; ++j)
            {
                for (int i = 0; i < x; ++i)
                {
                    if (j % 2 == 0)
                    {
                        result->push_back(0);
                    }
                    else
                    {
                        result->push_back(1);
                    }
                }
            }
            return result;
        }

        PSTD_FRAME_PTR MockKernel::CreateVerticalLines(int x, int y)
        {
            PSTD_FRAME_PTR result = std::make_shared<PSTD_FRAME>();
            result->reserve((unsigned long) (x * y));
            for (int j = 0; j < y; ++j)
            {
                for (int i = 0; i < x; ++i)
                {
                    if (i % 2 == 0)
                    {
                        result->push_back(0);
                    }
                    else
                    {
                        result->push_back(1);
                    }
                }
            }
            return result;
        }

        PSTD_FRAME_PTR MockKernel::CreateHorizontalGradient(int x, int y)
        {
            PSTD_FRAME_PTR result = std::make_shared<PSTD_FRAME>();
            result->reserve((unsigned long) (x * y));
            for (int j = 0; j < y; ++j)
            {
                for (int i = 0; i < x; ++i)
                {
                    result->push_back(i / ((float) x));
                }
            }
            return result;
        }

        PSTD_FRAME_PTR MockKernel::CreateVerticalGradient(int x, int y)
        {
            PSTD_FRAME_PTR result = std::make_shared<PSTD_FRAME>();
            result->reserve((unsigned long) (x * y));
            for (int j = 0; j < y; ++j)
            {
                for (int i = 0; i < x; ++i)
                {
                    result->push_back(j / ((float) y));
                }
            }
            return result;
        }

        PSTD_FRAME_PTR MockKernel::CreateVerticalGradientNeg(int x, int y)
        {
            PSTD_FRAME_PTR result = std::make_shared<PSTD_FRAME>();
            result->reserve((unsigned long) (x * y));
            for (int j = 0; j < y; ++j)
            {
                for (int i = 0; i < x; ++i)
                {
                    result->push_back(j / ((float) y) * 2 - 1);
                }
            }
            return result;
        }

    }
}
