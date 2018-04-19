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
/*
       void Speaker::addDomainContribution(std::shared_ptr<Domain> domain) {
            float dx = domain->settings->GetGridSpacing();
            float rel_x = this->x - domain->top_left.x;
            float rel_y = this->y - domain->top_left.y;
            for (int i = 0; i < domain->size.x; i++) {
                for (int j = 0; j < domain->size.y; j++) {
                    float squared_distance = SQR((rel_x - i) * dx) + SQR((rel_y - j) * dx);
                    float pressure = std::exp(-domain->settings->GetBandWidth() * squared_distance);
                    // Vectorized versions of above expressions exists
                    // but we need to get into a for loop anyway, because of atan2
                    float angle = std::atan2(rel_x - i,rel_y - j);
                    float horizontal_component = SQR(std::cos(angle)) * pressure;
                    float vertical_component = SQR(std::sin(angle)) * pressure;
                    domain->current_values.p0(j, i) += pressure;
                    domain->current_values.px0(j, i) += horizontal_component;
                    domain->current_values.py0(j, i) += vertical_component;
                    std::cout<<this->x<<"\n";
                    std::cout<<rel_y<<"\n";
                    std::cout<<squared_distance;
                    std::cin.get();
                }
            }
        }
  }
}*/


        void Speaker::addDomainContribution(std::shared_ptr<Domain> domain) {

            if (domain->is_pml!=1){
                if (this->x>domain->top_left.x && this->y>domain->top_left.y)
                {
                    float dx = domain->settings->GetGridSpacing();
                    float rel_x = (this->x - domain->top_left.x);
                    float rel_y = (this->y - domain->top_left.y);
                    for (int i = 0; i <= domain->size.x; i++) {
                        for (int j = 0; j <= domain->size.y; j++) {
                            float squared_distance = SQR((rel_x - i)*dx ) + SQR((rel_y - j)*dx );
                            float pressure = std::exp(-domain->settings->GetBandWidth() * squared_distance);
                            // Vectorized versions of above expressions exists
                             // but we need to get into a for loop anyway, because of atan2
                            float angle = std::atan2(rel_x - i,rel_y - j);
                            float horizontal_component = SQR(std::cos(angle)) * pressure;
                            float vertical_component = SQR(std::sin(angle)) * pressure;
                            domain->current_values.p0(i, j) += pressure;
                            domain->current_values.px0(i, j) += horizontal_component;
                            domain->current_values.py0(i, j) += vertical_component;
                           /* std::cout<<*domain<<std::endl;

                            std::cout<<"domain->bottom_right.x   "<<domain->bottom_right.x<<"\n";
                            std::cout<<"domain->bottom_right.y   "<<domain->bottom_right.y<<"\n";*/

                                std::cout<<rel_x<<"\n";
                           /* if (pressure!=0)
                            {
                                std::cout<<domain->settings->GetBandWidth( )<<"\n";
                                std::cout<<"Squared distance"<<squared_distance<<"\n";
                                std::cout<<"Pressure "<<pressure<<"\n";
                                std::cin.get();
                            }*/

                        }
                    }
                }
            }
        }

    }
}
