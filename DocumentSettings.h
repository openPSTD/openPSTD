//
// Created by michiel on 5-9-2015.
//

#ifndef OPENPSTD_DOCUMENTSETTINGS_H
#define OPENPSTD_DOCUMENTSETTINGS_H

#include <ui_DocumentSettings.h>
#include <memory>
#include <Model.h>

class DocumentSettings: public QDialog
{
    Q_OBJECT

public:
    explicit DocumentSettings(QWidget *parent = 0);
    void UpdateFromModel(std::shared_ptr<Model> const &model);
    PSTDFileSettings GetResult();

private:
    std::unique_ptr<Ui_DocumentSettings> ui;
};


#endif //OPENPSTD_DOCUMENTSETTINGS_H
