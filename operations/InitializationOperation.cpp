//
// Created by michiel on 17-8-2015.
//

#include "InitializationOperation.h"


void InitializationOperation::Run(const Reciever &reciever)
{
    reciever.model->d = std::unique_ptr<PSTDFile>(PSTDFile::New("test.jps"));
    reciever.model->view = QMatrix4x4();
    reciever.model->view.scale(0.5f);
}