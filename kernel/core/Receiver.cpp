//
// Created by omar on 17-9-15.
//

#include "Receiver.h"

namespace Kernel {
    Receiver::Receiver(std::vector<double> location,std::shared_ptr<PSTDFileSettings> config, std::string id,
                       std::shared_ptr<Domain> container) : x(location.at(0)), y(location.at(1)), z(location.at(2)) {
        this->config = config;
        this->location = location;
        this->container_domain = container;
        this->grid_location = std::make_shared<Point>(Point(this->x, this->y, this->z));
        for (int i = 0; i < this->location.size(); i++) {
            this->grid_offset.push_back(this->location.at(i) = this->grid_location->array.at(i));
        }
    }

    double Receiver::compute_local_pressure() {
        double pressure;
        if (this->config->getSpectralInterpolation()) {
            pressure = this->compute_with_si();
        } else {
            pressure = this->compute_with_nn();
        }
        this->received_values.push_back(pressure);
        return pressure;
    }

    Eigen::ArrayXXcd Receiver::compute_fft_factors(Point size, BoundaryType bt) {
        int primary_dimension = 0;
        if (bt == BoundaryType::HORIZONTAL) {
            primary_dimension = size.x;
        } else {
            primary_dimension = size.y;
        }
        double dx = this->config->GetGridSpacing();
        int wave_length_number = (int) (2 * this->config->getWaveLength() + primary_dimension + 1);
        //Pressure grid is staggered, hence + 1
        WaveNumberDiscretizer::Discretization discr = this->container_domain->wnd->get_discretization(dx,
                                                                                                      wave_length_number);
        double offset;
        if (bt == BoundaryType::HORIZONTAL) { //Todo: If this happens more often, add indices to the enum
            offset = this->grid_offset.at(0);
        } else {
            offset = this->grid_offset.at(1);
        }
        Eigen::ArrayXXcd fft_factors(discr.wave_numbers->rows(), discr.wave_numbers->cols());
        for (int i = 0; i < discr.wave_numbers->rows(); i++) {
            for (int j = 0; j < discr.wave_numbers->cols(); j++) {
                std::complex<double> wave_number = (*discr.wave_numbers.get())(i, j);
                std::complex<double> complex_factor = (*discr.complex_factors.get())(i, j);
                fft_factors(i, j) = exp(offset * dx * wave_number * complex_factor);
            }
        }
        return fft_factors;
    }

    double Receiver::compute_with_nn() {
        Point rel_location = *(this->grid_location.get()) - *(this->container_domain->top_left.get());
        double nn_value = this->container_domain->current_values.p0(rel_location.x, rel_location.y);
        return nn_value;
    }

    double Receiver::compute_with_si() {
        std::shared_ptr<Domain> top_domain = this->container_domain->get_neighbour_at(Direction::TOP, this->location);
        std::shared_ptr<Domain> bottom_domain = this->container_domain->get_neighbour_at(Direction::BOTTOM,
                                                                                         this->location);
    }

    double Receiver::compute_factors(Point size, BoundaryType bt) {

    }

    double Receiver::compute_domain_factors(Domain domain, BoundaryType bt) {

    }
}