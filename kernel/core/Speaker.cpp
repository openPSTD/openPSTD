//
// Created by omar on 17-9-15.
//

#include "Speaker.h"

#define SQR(x) x*x

#include <cassert>

namespace Kernel {
    Speaker::Speaker(std::vector<float> location) : x(location.at(0)), y(location.at(1)), z(location.at(2)) {
        this->location = location;
        //this->grid_point=Point((int)location.at(0),(int)location.at(1),(int)location.at(2));
        grid_offset = std::vector<float>{this->location.at(0) - grid_point.x,
                                         this->location.at(1) - grid_point.y,
                                         this->location.at(2) - grid_point.z};
    }

    void Speaker::addDomainContribution(std::shared_ptr<Domain> domain) {
        int domain_width = domain->size.x;
        int domain_height = domain->size.y;
        int domain_depth = domain->size.z;
        float dx = domain->settings->GetGridSpacing();
        //Only partially prepared for 3D.
        float rel_x = this->x - domain_width;
        float rel_y = this->y - domain_height;
        float rel_z = this->z - domain_depth;
        FieldValues values = domain->current_values;
        assert(values.p0.rows() == domain_width);
        assert(values.p0.cols() == domain_height);
        for (int i = 0; i < domain_width; i++) {
            for (int j = 0; j < domain_height; j++) {
                float distance = (float) sqrt(pow(rel_x - i * dx, 2) + pow(rel_y - j * dx, 2));
                float pressure = (float) exp(-domain->settings->GetBandWidth() * pow(distance, 2));
                // Vectorized versions of above expressions exists//
                // but we need to get into a for loop anyway, because of atan2
                float angle = std::atan2(rel_y - j * dx, rel_x - i * dx);
                float horizontal_component = SQR(std::cos(angle)) * pressure;
                float vertical_component = SQR(std::sin(angle)) * pressure;
                values.p0(i, j) += pressure;
                values.px0(i, j) += horizontal_component;
                values.py0(i, j) += vertical_component;
            }
        }
    }
}