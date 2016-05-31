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
// Date: 30-5-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_JACOBI_H
#define OPENPSTD_JACOBI_H

#include <Eigen/Dense>

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            template <typename SimpleType>
            using VectorX = Eigen::Matrix<SimpleType, Eigen::Dynamic, 1>;

            template <typename SimpleType>
            using MatrixX = Eigen::Matrix<SimpleType, Eigen::Dynamic, Eigen::Dynamic>;

            template <typename SimpleType>
            struct JacobiGQResult
            {
            public:
                VectorX<SimpleType> X;
                VectorX<SimpleType> W;
            };

            template <typename SimpleType>
            inline SimpleType SQ(SimpleType x) { return x*x; }

            template <typename SimpleType>
            JacobiGQResult<SimpleType> JacobiGQ(SimpleType alpha, SimpleType beta, int N, bool sort)
            {
                //struct for sorting eigen values and eigen vectors
                struct EigenO;
                struct EigenO
                {
                public:
                    SimpleType value;
                    VectorX<SimpleType> vector;

                    bool operator<(EigenO const &other) const {
                        return value < other.value;
                    }
                };

                JacobiGQResult<SimpleType> result;

                if (0 == N)
                {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "IncompatibleTypes"
                    result.X = VectorX<SimpleType>(1);
                    result.W = VectorX<SimpleType>(1);
#pragma clang diagnostic pop

                    result.X << -(alpha-beta)/(alpha+beta+2.0);
                    result.W << 2.0;
                    return result;
                }

                SimpleType ab1 = alpha+beta+1.0;
                SimpleType a1=alpha+1.0;
                SimpleType b1=beta+1.0;

                MatrixX<SimpleType> J, Vr;

                VectorX<SimpleType> h1;
                h1 = (2.0 * VectorX<SimpleType>::LinSpaced(N + 1, 0, N)).array() + (alpha + beta);
                VectorX<SimpleType> d0(N+1);
                VectorX<SimpleType> d1(N);

                // main diagonal: diag(-1/2*(alpha^2-beta^2)./(h1+2)./h1)
                const SimpleType eps = 1e-8;
                SimpleType fac = -0.5*(SQ(alpha)-SQ(beta));
                for(int i=0; i < N+1; i++)
                {
                    if(alpha+beta < 10*eps && h1[i] == 0) //handle division by zero
                    {
                        d0[i] = 0;
                    }
                    else
                    {
                        d0[i] = fac / (h1[i]+2.0) / h1[i];
                    }

                }

                // 1st upper diagonal: diag(2./(h1(1:N)+2).*sqrt((1:N).*((1:N)+alpha+beta) .* ((1:N)+alpha).*((1:N)+beta)./(h1(1:N)+1)./(h1(1:N)+3)),1);
                for(int i=1; i<=N; ++i)
                {
                    int I = i-1;// i is from 1 to N (inclusive), I is from 0 to N (exclusive)
                    d1[I] = 2.0 / (h1[I] + 2.0) *
                            sqrt(i * (i + alpha + beta) * (i + alpha) * (i + beta) / (h1[I] + 1) / (h1[I] + 3.0));
                }

                // Form symmetric matrix from recurrence.
                J = d0.asDiagonal()*2;

                J.diagonal(1) = d1;
                J.diagonal(-1) = d1;

                // Compute quadrature by eigenvalue solve
                // [Vr,D] = eig(J); x = diag(D);
                Eigen::SelfAdjointEigenSolver<MatrixX<SimpleType>> es(J);
                result.X = es.eigenvalues();
                Vr = es.eigenvectors();

                if (sort) {

                    // Note: Matlab appears to sort results from eig()
                    // so that the eigenvalues are in ascending order.
                    // Here we sort the columns of eigenvector matrix
                    // with the same permutation required to sort the
                    // eigenvalues into ascending order. Target: make
                    // w=1st row of eigenvector matrix match Matlab's.

                    std::vector<EigenO> sortVector(result.X.size());
                    for(int i = 0; i < result.X.size(); i++)
                    {
                        sortVector[i].value = result.X[i];
                        sortVector[i].vector = Vr.col(i);
                    }
                    std::sort(sortVector.begin(), sortVector.end());
                    for(int i = 0; i < result.X.size(); i++)
                    {
                        result.X[i] = sortVector[i].value;
                        Vr.col(i) = sortVector[i].vector;
                    }
                }

                result.W = Vr.row(0);
                result.W = result.W.cwiseProduct(result.W);
                result.W *= pow(2.0,ab1) / (ab1)*tgamma(a1)*tgamma(b1)/tgamma(ab1);

                return result;
            }

            //VectorXd& JacobiP(const VectorXd& x, double alpha, double beta, int N);



        }
    }

}


#endif //OPENPSTD_JACOBI_H
