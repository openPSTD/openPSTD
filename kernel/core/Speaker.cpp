//
// Created by omar on 17-9-15.
//

#include "Speaker.h"

namespace Kernel {
    Speaker::Speaker(std::vector<double> location) : x(location.at(0)), y(location.at(1)), z(location.at(2)) {
        this->location = location;
    }

    Eigen::ArrayXXd Speaker::computeDomainContribution(std::shared_ptr<Domain> domain) {
        Eigen::ArrayXXd pressure(domain->size.x, domain->size.y);
        std::vector<double> rel_location{
                this->x - domain->topleft.x, this->y - domain->topleft.y, this->z - domain->topleft.z};
        return pressure;
    }
}