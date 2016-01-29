//
// Created by michiel on 19-1-2016.
//

#ifndef OPENPSTD_MAIN_CLI_H_H
#define OPENPSTD_MAIN_CLI_H_H

#include <string>
#include <memory>
#include <export/Export.h>

class Command
{
public:
    virtual std::string GetName() = 0;
    virtual std::string GetDescription() = 0;
    virtual int execute(int argc, const char *argv[]) = 0;
};

class CreateCommand: public Command
{
public:
    std::string GetName() override;
    std::string GetDescription() override;
    int execute(int argc, const char *argv[]) override;
};

class ListCommand: public Command
{
private:
    void Print(const std::string& filename, bool debug);

public:
    std::string GetName() override;
    std::string GetDescription() override;
    int execute(int argc, const char *argv[]) override;
};

class EditCommand: public Command
{
public:
    std::string GetName() override;
    std::string GetDescription() override;
    int execute(int argc, const char *argv[]) override;
};

class RunCommand: public Command
{
public:
    std::string GetName() override;
    std::string GetDescription() override;
    int execute(int argc, const char *argv[]) override;
};

class ExportCommand: public Command
{
private:
    std::unique_ptr<ExportDomain> GetExport(std::string format);
public:
    std::string GetName() override;
    std::string GetDescription() override;
    int execute(int argc, const char *argv[]) override;
};

#endif //OPENPSTD_MAIN_CLI_H_H