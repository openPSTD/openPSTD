//
// Created by omar on 17-9-15.
//

#include "Receiver.h"

namespace Kernel {
    Receiver::Receiver(std::vector<double> location,std::shared_ptr<PSTDFileSettings> config, std::string id,
                       std::shared_ptr<Domain> container) : x(location.at(0)), y(location.at(1)), z(location.at(2)) {
        this->config = config;
        this->location = location;
        this->grid_location = std::make_shared<Point>(Point(this->x, this->y, this->z));
    }

    double Receiver::compute_local_pressure() {
        double pressure;
        if (this->config->getSpectralInterpolation()) {
            pressure = this->compute_with_si();
        } else {
            pressure = this->compute_with_nn();
        }
        this->received_values.push_back(pressure);
    }

    double Receiver::compute_fft_factor(Point size, BoundaryType bt) {
        int primary_dimension = 0;
        if (bt == HORIZONTAL) {
            primary_dimension = size.x;
        } else {
            primary_dimension = size.y;
        }
        //Todo: Is this the wave length number?
        double wave_length_number = 2 * this->config->getWaveLength() + primary_dimension + 1;
        //Pressure grid is staggered, hence + 1
        Domain top = this->container_domain.get_neighbour_at(Direction.TOP);
        Domain bottom = this->container_domain.get_neighbour_at(Direction.BOTTOM);
        //Todo: Finish

        return 0;

    }

    double Receiver::compute_with_nn() {

    }

    double Receiver::compute_with_si() {

    }

    double Receiver::compute_factors(Point size, BoundaryType bt) {

    }

    double Receiver::compute_domain_factors(Domain domain, BoundaryType bt) {

    }
}