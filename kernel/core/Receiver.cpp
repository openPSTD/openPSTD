//////////////////////////////////////////////////////////////////////////
// This file is part of openPSTD.                                       //
//                                                                      //
// openPSTD is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// openPSTD is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Date: 17-9-15
//
//
// Authors: Omar Richardson
//
//////////////////////////////////////////////////////////////////////////

#include "Receiver.h"

using namespace Eigen;
using namespace std;
namespace Kernel {
    Receiver::Receiver(vector<float> location, shared_ptr<PSTDFileSettings> config, int id,
                       shared_ptr<Domain> container) : x(location.at(0)), y(location.at(1)), z(location.at(2)) {
        this->config = config;
        this->location = location;
        this->container_domain = container;
        this->grid_location = Point((int) this->x, (int) this->y, (int) this->z);
        for (unsigned long i = 0; i < this->location.size(); i++) {
            this->grid_offset.push_back(this->location.at(i) = this->grid_location.array.at(i));
        }
        this->id = id;
    }

    float Receiver::compute_local_pressure() {
        float pressure;
        if (config->GetSpectralInterpolation()) {
            pressure = compute_with_si();
        } else {
            pressure = compute_with_nn();
        }
        received_values.push_back(pressure);
        return pressure;
    }

    ArrayXcf Receiver::get_fft_factors(Point size, CalcDirection bt) {
        int primary_dimension = 0;
        if (bt == CalcDirection::X) {
            primary_dimension = size.x;
        } else {
            primary_dimension = size.y;
        }
        float dx = config->GetGridSpacing();
        int wave_length_number = (int) (2 * config->GetWaveLength() + primary_dimension + 1);
        //Pressure grid is staggered, hence + 1
        WisdomCache::Discretization discr = container_domain->wnd->get_discretization(dx, wave_length_number);
        float offset = grid_offset.at(static_cast<unsigned long>(bt));
        ArrayXcf fft_factors(discr.wave_numbers.rows());
        for (int i = 0; i < discr.wave_numbers.rows(); i++) {
            float wave_number = discr.wave_numbers(i);
            complex<float> complex_factor = discr.complex_factors(i);
            fft_factors(i) = exp(offset * dx * wave_number * complex_factor);
        }
        return fft_factors;
    }

    float Receiver::compute_with_nn() {
        Point rel_location = grid_location - container_domain->top_left;
        float nn_value = container_domain->current_values.p0(rel_location.x, rel_location.y);
        return nn_value;
    }

    float Receiver::compute_with_si() {
        shared_ptr<Domain> top_domain = container_domain->get_neighbour_at(Direction::TOP, location);
        shared_ptr<Domain> bottom_domain = container_domain->get_neighbour_at(Direction::BOTTOM, location);
        ArrayXXf p0dx = calc_domain_fields(container_domain, CalcDirection::X);
        int rel_x_point = grid_location.x - container_domain->top_left.x;
        ArrayXXf p0dx_slice = p0dx.middleCols(rel_x_point, 1);

        ArrayXXf p0dx_top = calc_domain_fields(top_domain, CalcDirection::X);
        int top_rel_x_point = grid_location.x - top_domain->top_left.x;
        ArrayXXf p0dx_top_slice = p0dx_top.middleCols(top_rel_x_point, 1);

        ArrayXXf p0dx_bottom = calc_domain_fields(bottom_domain, CalcDirection::X);
        int bottom_rel_x_point = grid_location.x - bottom_domain->top_left.x;
        ArrayXXf p0dx_bottom_slice = p0dx_bottom.middleCols(bottom_rel_x_point, 1);

        ArrayXcf z_fact = get_fft_factors(Point(1, container_domain->size.y), CalcDirection::Y);
        float wave_number = 2 * config->GetWaveLength() + container_domain->size.y + 1;
        int opt_wave_number = next_2_power(wave_number);

        RhoArray rho_array = container_domain->rho_arrays[top_domain->id + bottom_domain->id];

        ArrayXXf p0shift = spatderp3(p0dx_bottom_slice, p0dx_slice, p0dx_top_slice,
                                     z_fact, rho_array, config->GetWindow(), config->GetWindowSize(),
                                     CalculationType::PRESSURE , CalcDirection::Y);

        int rel_y_point = grid_location.y - container_domain->top_left.y;
        float si_value = p0shift(rel_y_point, 0);
        return si_value;
    }

    // Todo: Different name;
    // Todo: Can we improve memory management here?
    ArrayXXf Receiver::calc_domain_fields(shared_ptr<Domain> domain, CalcDirection bt) {
        return domain->calc(bt, CalculationType::PRESSURE, get_fft_factors(domain->size, bt));
    }
}