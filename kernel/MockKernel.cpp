//
// Created by michiel on 24-1-2016.
//

#include "MockKernel.h"


void MockKernel::Configure(std::shared_ptr<PSTDFileConfiguration> config)
{
    _conf = config;
}

void MockKernel::run(KernelCallback *callback)
{
    if(!_conf)
        throw PSTDKernelNotConfiguredException();

    auto meta = GetSimulationMetadata();

    callback->Callback(CALLBACKSTATUS::STARTING, "Starting to mock", -1);

    for (int i = 0; i < 1000; ++i)
    {
        callback->Callback(CALLBACKSTATUS::RUNNING, "Starting to mock", i);
        //generate some frame
        //write
    }

    callback->Callback(CALLBACKSTATUS::FINISHED, "finished mocking", -1);
}

SimulationMetadata MockKernel::GetSimulationMetadata()
{
    if(!_conf)
        throw PSTDKernelNotConfiguredException();

    SimulationMetadata result;

    float grid = _conf->Settings.GetGridSpacing();
    for (int i = 0; i < _conf->Domains.size(); ++i)
    {
        std::vector<int> d;
        d.push_back(roundf(_conf->Domains[i].Size.x()/grid)*grid);
        d.push_back(roundf(_conf->Domains[i].Size.y()/grid)*grid);
        d.push_back(1);

        result.DomainMetadata.push_back(d);
    }

    result.Framecount = 1000;

    return result;
}