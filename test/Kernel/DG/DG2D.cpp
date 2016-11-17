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
// Date: 15-11-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif

#include <boost/test/unit_test.hpp>
#include <kernel/DG/DG2D.h>
#include <kernel/DG/RK.h>

#include <iostream>

using namespace OpenPSTD::Kernel::DG;
using namespace std;
using namespace Eigen;

BOOST_AUTO_TEST_SUITE(DG)
    BOOST_AUTO_TEST_SUITE(DG_General_2D)

        BOOST_AUTO_TEST_CASE(DTScale)
        {
            std::shared_ptr<Vertex2D<double>> a = std::make_shared<Vertex2D<double>>();
            a->Position = VectorX<double>(2);
            a->Position << -1.0000000000000000e+00,-7.5000000000000000e-01;
            std::shared_ptr<Vertex2D<double>> b = std::make_shared<Vertex2D<double>>();
            b->Position = VectorX<double>(2);
            b->Position << -1.0000000000000000e+00,-1.0000000000000000e+00;
            std::shared_ptr<Vertex2D<double>> c = std::make_shared<Vertex2D<double>>();
            c->Position = VectorX<double>(2);
            c->Position << -8.2531548167799995e-01,-8.2446481953499995e-01;

            std::shared_ptr<Element2D<double>> e = std::make_shared<Element2D<double>>();
            e->Vertices.push_back(a);
            e->Vertices.push_back(b);
            e->Vertices.push_back(c);

            double dtScaleExpected = +6.3518192568559464e-02;
            double dtscale = e->GetDTScale();

            BOOST_CHECK(dtscale - dtScaleExpected < 1e-10);
        }


        BOOST_AUTO_TEST_CASE(DTScale2)
        {
            std::shared_ptr<Vertex2D<double>> a = std::make_shared<Vertex2D<double>>();
            a->Position = VectorX<double>(2);
            a->Position << -8.2531548167799995e-01,-8.2446481953499995e-01;
            std::shared_ptr<Vertex2D<double>> b = std::make_shared<Vertex2D<double>>();
            b->Position = VectorX<double>(2);
            b->Position << -1.0000000000000000e+00,-1.0000000000000000e+00;
            std::shared_ptr<Vertex2D<double>> c = std::make_shared<Vertex2D<double>>();
            c->Position = VectorX<double>(2);
            c->Position << -7.5000000000000000e-01,-1.0000000000000000e+00;

            std::shared_ptr<Element2D<double>> e = std::make_shared<Element2D<double>>();
            e->Vertices.push_back(a);
            e->Vertices.push_back(b);
            e->Vertices.push_back(c);

            double dtScaleExpected = +6.3724012799132326e-02;
            double dtscale = e->GetDTScale();

            BOOST_CHECK(dtscale - dtScaleExpected < 1e-10);
        }

        BOOST_AUTO_TEST_CASE(Normal)
        {
            VectorX<double> nxExpected(18);
            VectorX<double> nyExpected(18);
            VectorX<double> sJExpected(18);
            nxExpected << -1.0000000000000000e+00,-1.0000000000000000e+00,-1.0000000000000000e+00,-1.0000000000000000e+00,-1.0000000000000000e+00,-1.0000000000000000e+00,+7.0882220920667693e-01,+7.0882220920668970e-01,+7.0882220920668726e-01,+7.0882220920668981e-01,+7.0882220920669214e-01,+7.0882220920668348e-01,+3.9213909766951061e-01,+3.9213909766953103e-01,+3.9213909766952243e-01,+3.9213909766952121e-01,+3.9213909766952654e-01,+3.9213909766952315e-01;
            nyExpected << -1.1114919695525362e-14,+1.0501130522289045e-15,+1.4474034688129570e-15,+1.4108530268604642e-15,-1.5339244074193899e-15,+1.3082242099858044e-14,-7.0538718143680923e-01,-7.0538718143679635e-01,-7.0538718143679890e-01,-7.0538718143679624e-01,-7.0538718143679369e-01,-7.0538718143680257e-01,+9.1990593436445545e-01,+9.1990593436444679e-01,+9.1990593436445045e-01,+9.1990593436445101e-01,+9.1990593436444879e-01,+9.1990593436445023e-01;
            sJExpected << +1.2499999999999953e-01,+1.2500000000000003e-01,+1.2499999999999986e-01,+1.2500000000000036e-01,+1.2499999999999985e-01,+1.2500000000000100e-01,+1.2382172721524916e-01,+1.2382172721524777e-01,+1.2382172721524774e-01,+1.2382172721524556e-01,+1.2382172721524719e-01,+1.2382172721524656e-01,+9.4946946093291448e-02,+9.4946946093290324e-02,+9.4946946093291212e-02,+9.4946946093291434e-02,+9.4946946093290990e-02,+9.4946946093291809e-02;

            int N = 5;
            auto xy = Nodes2D<double>(N);
            auto rs = xytors<double>(xy);

            auto V = Vandermonde2D<double>(N, rs);
            auto D = DMatrices2D<double>(N, rs, V);

            std::shared_ptr<Vertex2D<double>> a = std::make_shared<Vertex2D<double>>();
            a->Position = VectorX<double>(2);
            a->Position << -1.0000000000000000e+00,-7.5000000000000000e-01;
            std::shared_ptr<Vertex2D<double>> b = std::make_shared<Vertex2D<double>>();
            b->Position = VectorX<double>(2);
            b->Position << -1.0000000000000000e+00,-1.0000000000000000e+00;
            std::shared_ptr<Vertex2D<double>> c = std::make_shared<Vertex2D<double>>();
            c->Position = VectorX<double>(2);
            c->Position << -8.2531548167799995e-01,-8.2446481953499995e-01;

            std::shared_ptr<Edge2D<double>> AB = std::make_shared<Edge2D<double>>(a, b);
            std::shared_ptr<Edge2D<double>> AC = std::make_shared<Edge2D<double>>(a, c);
            std::shared_ptr<Edge2D<double>> BC = std::make_shared<Edge2D<double>>(b, c);
            AB->RegisterEdgeWithVertices();
            AC->RegisterEdgeWithVertices();
            BC->RegisterEdgeWithVertices();

            std::vector<std::shared_ptr<Vertex2D<double>>> v;
            v.push_back(a);
            v.push_back(b);
            v.push_back(c);

            std::shared_ptr<Element2D<double>> e = std::make_shared<Element2D<double>>();
            e->init(v, N, 0, rs, D.Dr, D.Ds);

            BOOST_CHECK(e->nx.isApprox(nxExpected));
            BOOST_CHECK(e->ny.isApprox(nyExpected));
            BOOST_CHECK(e->sJ.isApprox(sJExpected));
        }

        BOOST_AUTO_TEST_CASE(Normal2)
        {
            VectorX<double> nxExpected(18);
            VectorX<double> nyExpected(18);
            VectorX<double> sJExpected(18);
            nxExpected << -7.0882220920668548e-01,-7.0882220920668804e-01,-7.0882220920668748e-01,-7.0882220920668626e-01,-7.0882220920668881e-01,-7.0882220920668348e-01,-5.5511151231256634e-16,+7.7715611723761084e-15,-7.5495165674510708e-15,-2.3980817331903987e-14,+0.0000000000000000e+00,-3.5527136788004725e-15,+9.1898170941055235e-01,+9.1898170941055501e-01,+9.1898170941055368e-01,+9.1898170941055335e-01,+9.1898170941055624e-01,+9.1898170941055446e-01;
            nyExpected << +7.0538718143680068e-01,+7.0538718143679802e-01,+7.0538718143679857e-01,+7.0538718143679979e-01,+7.0538718143679724e-01,+7.0538718143680246e-01,-1.0000000000000000e+00,-1.0000000000000000e+00,-1.0000000000000000e+00,-1.0000000000000000e+00,-1.0000000000000000e+00,-1.0000000000000000e+00,+3.9430016201982349e-01,+3.9430016201981744e-01,+3.9430016201982038e-01,+3.9430016201982099e-01,+3.9430016201981460e-01,+3.9430016201981882e-01;
            sJExpected << +1.2382172721524647e-01,+1.2382172721524827e-01,+1.2382172721524853e-01,+1.2382172721524810e-01,+1.2382172721524790e-01,+1.2382172721525096e-01,+1.2500000000000269e-01,+1.2499999999999981e-01,+1.2499999999999989e-01,+1.2499999999999684e-01,+1.2499999999999972e-01,+1.2500000000000100e-01,+9.5505263416824748e-02,+9.5505263416825650e-02,+9.5505263416826414e-02,+9.5505263416826205e-02,+9.5505263416825914e-02,+9.5505263416826580e-02;

            int N = 5;
            auto xy = Nodes2D<double>(N);
            auto rs = xytors<double>(xy);

            auto V = Vandermonde2D<double>(N, rs);
            auto D = DMatrices2D<double>(N, rs, V);

            std::shared_ptr<Vertex2D<double>> a = std::make_shared<Vertex2D<double>>();
            a->Position = VectorX<double>(2);
            a->Position << -8.2531548167799995e-01,-8.2446481953499995e-01;
            std::shared_ptr<Vertex2D<double>> b = std::make_shared<Vertex2D<double>>();
            b->Position = VectorX<double>(2);
            b->Position << -1.0000000000000000e+00,-1.0000000000000000e+00;
            std::shared_ptr<Vertex2D<double>> c = std::make_shared<Vertex2D<double>>();
            c->Position = VectorX<double>(2);
            c->Position << -7.5000000000000000e-01,-1.0000000000000000e+00;

            std::shared_ptr<Edge2D<double>> AB = std::make_shared<Edge2D<double>>(a, b);
            std::shared_ptr<Edge2D<double>> AC = std::make_shared<Edge2D<double>>(a, c);
            std::shared_ptr<Edge2D<double>> BC = std::make_shared<Edge2D<double>>(b, c);
            AB->RegisterEdgeWithVertices();
            AC->RegisterEdgeWithVertices();
            BC->RegisterEdgeWithVertices();

            std::vector<std::shared_ptr<Vertex2D<double>>> v;
            v.push_back(a);
            v.push_back(b);
            v.push_back(c);

            std::shared_ptr<Element2D<double>> e = std::make_shared<Element2D<double>>();
            e->init(v, N, 0, rs, D.Dr, D.Ds);

            BOOST_CHECK(e->nx.isApprox(nxExpected));
            BOOST_CHECK(e->ny.isApprox(nyExpected));
            BOOST_CHECK(e->sJ.isApprox(sJExpected));
        }

    BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()