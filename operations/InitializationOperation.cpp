//
// Created by michiel on 17-8-2015.
//

#include "InitializationOperation.h"
#include "MouseOperations.h"

void InitializationOperation::Run(const Reciever &reciever)
{
    reciever.model->d = std::unique_ptr<PSTDFile>(PSTDFile::New("test.jps"));
    reciever.model->view->value = QMatrix4x4();
    reciever.model->view->value.scale(0.5f);
    reciever.model->view->Change();

    reciever.model->colorScheme->value = std::unique_ptr<StandardColorScheme>(new StandardColorScheme());
    reciever.model->colorScheme->Change();

    //initialize Mouse handler
    std::shared_ptr<ChangeMouseHandlerOperations> op(new ChangeMouseHandlerOperations(std::unique_ptr<MouseMoveSceneStrategy>(new MouseMoveSceneStrategy())));
    reciever.operationRunner->RunOperation(op);
}