//
// Created by omar on 17-9-15.
//

#include "Speaker.h"

#define SQR(x) x*x

#include <cassert>

namespace OpenPSTD {
    namespace Kernel {
        Speaker::Speaker(std::vector<float> location) : x(location.at(0)), y(location.at(1)), z(location.at(2)) {
            this->location = location;
            //this->grid_point=Point((int)location.at(0),(int)location.at(1),(int)location.at(2));
            grid_offset = std::vector<float>{this->location.at(0) - grid_point.x,
                                             this->location.at(1) - grid_point.y,
                                             this->location.at(2) - grid_point.z};
        }

        void Speaker::addDomainContribution(std::shared_ptr<Domain> domain) {
            float dx = domain->settings->GetGridSpacing();
            float rel_x = this->x - domain->top_left.x;
            float rel_y = this->y - domain->top_left.y;
            FieldValues values = domain->current_values;
            for (int i = 0; i < domain->size.x; i++) {
                for (int j = 0; j < domain->size.y; j++) {
                    float distance = (float) sqrt(pow((rel_x - i) * dx, 2) + pow((rel_y - j) * dx, 2));
                    float pressure = (float) exp(-domain->settings->GetBandWidth() * distance * distance);
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
            std::cout << values.p0 << std::endl;
        }
    }
}
