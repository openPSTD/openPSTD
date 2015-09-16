//
// Created by michiel on 18-7-2015.
//

#include "Model.h"

Model::Model() : interactive(new InteractiveModel()), view(new View()),
                 settings(new Settings())
{
    this->Register(interactive);
    this->Register(view);
    this->Register(settings);
}

