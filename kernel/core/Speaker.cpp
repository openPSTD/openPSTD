//
// Created by omar on 17-9-15.
//

#include "Speaker.h"
#include <cassert>
namespace Kernel {
    Speaker::Speaker(std::vector<float> location) : x(location.at(0)), y(location.at(1)), z(location.at(2)) {
        this->location = location;
        //Round off coordinates so we store the cell centers as well as the offset.
    }

    void Speaker::addDomainContribution(std::shared_ptr<Domain> domain) {
        int domain_width = domain->top_left->x;
        int domain_height = domain->top_left->y;
        int domain_depth = domain->top_left->z;
        float dx = domain->settings->GetGridSpacing();
        //Only partially prepared for 3D.
        float rel_x = this->x - domain_width;
        float rel_y = this->y - domain_height;
        float rel_z = this->z - domain_depth;
        std::shared_ptr<field_values> values = domain->current_values;
        assert(values->p0.rows() == domain_width);
        assert(values->p0.cols() == domain_height);
        for (int i = 0; i < domain_width; i++) {
            for (int j = 0; j < domain_height; j++) {
                float distance = (float) sqrt(pow(rel_x - i * dx, 2) + pow(rel_y - j * dx, 2));
                float pressure = (float) exp(-domain->settings->getBandWidth() * pow(distance, 2));
                float horizontal_component = pressure;
                float vertical_component = 0; // Todo: why is the vertical component zero?
                values->p0(i,j) += pressure;
                values->px0(i, j) += horizontal_component;
            }
        }
    }
}