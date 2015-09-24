//
// Created by omar on 17-9-15.
//

#include "Receiver.h"

namespace Kernel {
    Receiver::Receiver(std::vector<double> location,std::shared_ptr<PSTDFile> config, std::string id,
                       std::shared_ptr<Domain> container) : x(location.at(0)), y(location.at(1)), z(location.at(2)) {
        this->config = config;
        this->location = location;
        this->grid_location = std::make_shared<Point>(Point(this->x, this->y, this->z));
    }
}