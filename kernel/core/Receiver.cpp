//
// Created by omar on 17-9-15.
//

#include "Receiver.h"

namespace Kernel {
    Receiver::Receiver(std::vector<double> location, std::shared_ptr<PSTDFileSettings> config, std::string id,
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

    Eigen::ArrayXcf Receiver::get_fft_factors(Point size, BoundaryType bt) {
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
        Eigen::ArrayXcf fft_factors(discr.wave_numbers->rows());
        for (int i = 0; i < discr.wave_numbers->rows(); i++) {
            std::complex<double> wave_number = (*discr.wave_numbers.get())(i);
            std::complex<double> complex_factor = (*discr.complex_factors.get())(i);
            fft_factors(i) = exp(offset * dx * wave_number * complex_factor);
        }
        return fft_factors;
    }

    double Receiver::compute_with_nn() {
        Point rel_location =
                *(this->grid_location) - *(this->container_domain->top_left); // Todo: Parentheses necessary?
        double nn_value = this->container_domain->current_values.p0(rel_location.x, rel_location.y);
        return nn_value;
    }

    double Receiver::compute_with_si() {
        std::shared_ptr<Domain> top_domain = this->container_domain->get_neighbour_at(Direction::TOP, this->location);
        std::shared_ptr<Domain> bottom_domain = this->container_domain->get_neighbour_at(Direction::BOTTOM,
                                                                                         this->location);
        Eigen::ArrayXXf p0dx = this->compute_domain_factors(this->container_domain, BoundaryType::HORIZONTAL);
        Eigen::ArrayXXf p0dx_top = this->compute_domain_factors(top_domain, BoundaryType::HORIZONTAL);
        Eigen::ArrayXXf p0dx_bottom = this->compute_domain_factors(bottom_domain, BoundaryType::HORIZONTAL);
        int rel_x_distance = this->grid_location->x - this->container_domain->top_left->x;
        int top_rel_x_distance = this->grid_location->x - top_domain->top_left->x;
        int bottom_rel_x_distance = this->grid_location->x - bottom_domain->top_left->x;
        Eigen::ArrayXcf z_fact = this->get_fft_factors(Point(1, this->container_domain->size->y),
                                                       BoundaryType::VERTICAL);
        float wave_number = 2 * this->config->getWaveLength() + this->container_domain->size->y + 1;
        int opt_wave_number = next2Power(wave_number);
        //Keys? What to do with that?



    }

    Eigen::ArrayXXf Receiver::compute_domain_factors(std::shared_ptr<Domain> domain, BoundaryType bt) {
        Eigen::ArrayXXf domain_result = domain->calc(bt, CalculationType::PRESSURE,
                                                     this->get_fft_factors(*(domain->size), bt));
        return domain_result;
    }
}