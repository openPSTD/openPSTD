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
            /**
             * short hand for a Vector
             */
            template <typename SimpleType>
            using VectorX = Eigen::Matrix<SimpleType, Eigen::Dynamic, 1>;

            /**
             * short hand for a Matrix
             */
            template <typename SimpleType>
            using MatrixX = Eigen::Matrix<SimpleType, Eigen::Dynamic, Eigen::Dynamic>;

            /**
             * Result of the JacobiGQ function
             */
            template <typename SimpleType>
            struct JacobiGQResult
            {
            public:
                VectorX<SimpleType> Points;
                VectorX<SimpleType> Weights;
            };

            /**
             * short hand for the power of two
             */
            template <typename SimpleType>
            inline SimpleType SQ(SimpleType x) { return x*x; }

            /**
             * Compute the N'th order Gauss quadrature points and weights, associated with the Jacobi
             * polynomial, of type (alpha,beta) > -1 ( <> -0.5).
             */
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
                    result.Points = VectorX<SimpleType>(1);
                    result.Weights = VectorX<SimpleType>(1);
#pragma clang diagnostic pop

                    result.Points << -(alpha-beta)/(alpha+beta+2.0);
                    result.Weights << 2.0;
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
                result.Points = es.eigenvalues();
                Vr = es.eigenvectors();

                if (sort) {

                    // Note: Matlab appears to sort results from eig()
                    // so that the eigenvalues are in ascending order.
                    // Here we sort the columns of eigenvector matrix
                    // with the same permutation required to sort the
                    // eigenvalues into ascending order. Target: make
                    // w=1st row of eigenvector matrix match Matlab's.

                    std::vector<EigenO> sortVector(result.Points.size());
                    for(int i = 0; i < result.Points.size(); i++)
                    {
                        sortVector[i].value = result.Points[i];
                        sortVector[i].vector = Vr.col(i);
                    }
                    std::sort(sortVector.begin(), sortVector.end());
                    for(int i = 0; i < result.Points.size(); i++)
                    {
                        result.Points[i] = sortVector[i].value;
                        Vr.col(i) = sortVector[i].vector;
                    }
                }

                result.Weights = Vr.row(0);
                result.Weights = result.Weights.cwiseProduct(result.Weights);
                result.Weights *= pow(2.0,ab1) / (ab1)*tgamma(a1)*tgamma(b1)/tgamma(ab1);

                return result;
            }

            /*
             * Compute the N'th order Gauss Lobatto quadrature
             * points, associated with the Jacobi polynomial,
             * of type (alpha,beta) > -1 ( <> -0.5).
             */
            template <typename SimpleType>
            VectorX<SimpleType> JacobiGL(SimpleType alpha, SimpleType beta, int N)
            {
                VectorX<SimpleType> result(N+1);

                if (N==1)
                {
                    result << -1, 1;
                }
                else
                {
                    // calculate points
                    auto GQ = JacobiGQ(alpha+1, beta+1, N-2, true);

                    // assemble result: sandwich eigenvalues between [-1,1]
                    result << -1, GQ.Points, 1;
                }

                return result;
            }

            /**
             * Evaluate Jacobi Polynomial of type (alpha,beta) > -1
             * (alpha+beta <> -1) at points x for order N and
             * returns P[1:length(x)]
             * Note: They are normalized to be orthonormal.
             */
            template <typename SimpleType>
            VectorX<SimpleType> JacobiP(VectorX<SimpleType> x, SimpleType alpha, SimpleType beta, int N)
            {
                SimpleType aold=0.0, anew=0.0, bnew=0.0, h1=0.0;
                SimpleType gamma0=0.0, gamma1=0.0;
                SimpleType ab=alpha+beta, ab1=alpha+beta+1.0, a1=alpha+1.0, b1=beta+1.0;

                int Nc = x.size();
                VectorX<SimpleType> P(x.size());
                VectorX<SimpleType> prow, x_bnew;
                VectorX<SimpleType> a(x.size()), b(x.size());

                // Initial values P_0(x) and P_1(x)
                gamma0 = pow(2.0,ab1)/(ab1)*tgamma(a1)*tgamma(b1)/tgamma(ab1);

                a = a.unaryExpr([gamma0](SimpleType x) { return 1.0/sqrt(gamma0); });

                if (N>0)
                {
                    gamma1 = (a1)*(b1)/(ab+3.0)*gamma0;
                    b = (((ab+2.0)*x/2.0).array() + (alpha-beta)/2.0) / sqrt(gamma1);

                    if (N>1)
                    {
                        // Repeat value in recurrence.
                        aold = 2.0/(2.0+ab)*sqrt((a1)*(b1)/(ab+3.0));

                        // Forward recurrence using the symmetry of the recurrence.
                        for (int i=1; i<=(N-1); ++i)
                        {
                            int I = i-1;

                            h1 = 2.0*i+ab;
                            anew = 2.0/(h1+2.0)*sqrt((i+1)*(i+ab1)*(i+a1)*(i+b1)/(h1+1.0)/(h1+3.0));
                            bnew = - (SQ(alpha)-SQ(beta))/h1/(h1+2.0);
                            x_bnew = x.array()-bnew;
                            VectorX<SimpleType> tmp = 1.0/anew*( -aold*a + x_bnew.cwiseProduct(b));
                            a = b;
                            b = tmp;
                            aold =anew;
                        }
                    }
                }
                return N==0?a:b;
            }

            /**
             * Evaluate the derivative of the orthonormal Jacobi
             * polynomial of type (alpha,beta)>-1, at points x
             * for order N and returns dP[1:length(xp))]
             */
            template <typename SimpleType>
            VectorX<SimpleType> GradJacobiP(const VectorX<SimpleType> z, double alpha, double beta, int N)
            {
                if (0 == N) {
                    return VectorX<SimpleType>::Zero(z.size());
                } else {
                    return sqrt(N*(N+alpha+beta+1))*JacobiP<SimpleType>(z,alpha+1,beta+1, N-1);
                }
            }
        }
    }

}


#endif //OPENPSTD_JACOBI_H
