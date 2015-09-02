//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_FILEOPERATIONS_H
#define OPENPSTD_FILEOPERATIONS_H

#include "BaseOperation.h"
#include <string>

class OpenFileOperation: public BaseOperation
{
public:
    OpenFileOperation(std::string filename);

    virtual void Run(const Reciever &reciever) override;

private:
    std::string filename;
};

class NewFileOperation: public BaseOperation
{
public:
    NewFileOperation(std::string filename);

    virtual void Run(const Reciever &reciever) override;

private:
    std::string filename;
};

class SaveFileOperation: public BaseOperation
{
public:
    virtual void Run(const Reciever &reciever) override;
};


#endif //OPENPSTD_FILEOPERATIONS_H
