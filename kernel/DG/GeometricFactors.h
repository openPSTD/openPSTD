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
// Date: 1-6-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_GEOMETRICFACTORS_H
#define OPENPSTD_GEOMETRICFACTORS_H

#include <Eigen/Dense>
#include "GeneralTypes.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            /**
             * Compute Jacobian for the metric elements for the local mappings of the 1D elements.
             */
            template<typename SimpleType>
            MatrixX<SimpleType> GeometricFactors1D_J(const MatrixX<SimpleType>& x, const MatrixX<SimpleType>& Dr)
            {
                return Dr*x;
            }

            /**
             * Compute the metric elements for the local mappings of the 1D elements.
             */
            template<typename SimpleType>
            MatrixX<SimpleType> GeometricFactors1D_rx(const MatrixX<SimpleType>& J)
            {
                return J.unaryExpr([](SimpleType x){return 1/x;});
            }


            /**
             * Compute the metric elements for the local mappings of the 1D elements.
             */
            template<typename SimpleType>
            MatrixX<SimpleType> GeometricFactors1D_rx(const MatrixX<SimpleType>& x, const MatrixX<SimpleType>& Dr)
            {
                return GeometricFactors1D_rx(GeometricFactors1D_J(x, Dr));
            }


            template<typename SimpleType>
            struct GeometricFactors2DResult {
                MatrixX<SimpleType> rx;
                MatrixX<SimpleType> sx;
                MatrixX<SimpleType> ry;
                MatrixX<SimpleType> sy;
                MatrixX<SimpleType> J;
            };

            /**
             * Compute the metric elements for the local mappings of the elements
             * @param x
             * @param y
             * @param Dr
             * @param Ds
             * @return
             */
            template<typename SimpleType>
            GeometricFactors2DResult<SimpleType> GeometricFactors2D(const MatrixX<SimpleType>& x, const MatrixX<SimpleType>& y, const MatrixX<SimpleType>& Dr, const MatrixX<SimpleType>& Ds)
            {
                GeometricFactors2DResult<SimpleType> result;
                // xr = Dr*x;
                // xs = Ds*x;
                // yr = Dr*y;
                // ys = Ds*y;
                ArrayXX<SimpleType> xr = Dr*x;
                ArrayXX<SimpleType> xs = Ds*x;
                ArrayXX<SimpleType> yr = Dr*y;
                ArrayXX<SimpleType> ys = Ds*y;

                // J = -xs.*yr + xr.*ys;
                ArrayXX<SimpleType> J = -xs*yr + xr*ys; //this is done so that J is a array in place of a matrix
                result.J = J;

                // rx = ys./J;
                // sx =-yr./J;
                // ry =-xs./J;
                // sy = xr./J;
                result.rx =  ys/J;
                result.sx = -yr/J;
                result.ry = -xs/J;
                result.sy =  xr/J;

                return result;
            }

            extern template MatrixX<float> GeometricFactors1D_J(const MatrixX<float>& x, const MatrixX<float>& Dr);
            extern template MatrixX<double> GeometricFactors1D_J(const MatrixX<double>& x, const MatrixX<double>& Dr);

            extern template MatrixX<float> GeometricFactors1D_rx(const MatrixX<float>& J);
            extern template MatrixX<double> GeometricFactors1D_rx(const MatrixX<double>& J);

            extern template MatrixX<float> GeometricFactors1D_rx(const MatrixX<float>& x, const MatrixX<float>& Dr);
            extern template MatrixX<double> GeometricFactors1D_rx(const MatrixX<double>& x, const MatrixX<double>& Dr);

            extern template struct GeometricFactors2DResult<float>;
            extern template struct GeometricFactors2DResult<double>;

            extern template GeometricFactors2DResult<float> GeometricFactors2D(const MatrixX<float>& x, const MatrixX<float>& y, const MatrixX<float>& Dr, const MatrixX<float>& Ds);
            extern template GeometricFactors2DResult<double> GeometricFactors2D(const MatrixX<double>& x, const MatrixX<double>& y, const MatrixX<double>& Dr, const MatrixX<double>& Ds);


        }
    }
}
#endif //OPENPSTD_GEOMETRICFACTORS_H
