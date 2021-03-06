/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2008 Ferdinando Ametrano
 Copyright (C) 2007, 2008 Laurent Hoffmann
 Copyright (C) 2015, 2016 Michael von den Driesch

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include "optionletstripper.hpp"
#include "BootStrapper.hpp"
#include <ql/instruments/makecapfloor.hpp>
#include <ql/time/all.hpp>
#include <ql/time/calendars/all.hpp>
#include <boost/assign/std/vector.hpp>
#include <ql/pricingengines/capfloor/all.hpp>
#include <algorithm>
#include <iterator>
#include <ql/termstructures/volatility/capfloor/all.hpp>

using namespace QuantLib;
using namespace boost::assign;
using namespace boost::unit_test_framework;
using boost::shared_ptr;


namespace BootStrapper {

struct CommonVars {
        // global data
        Calendar calendar;
        DayCounter dayCounter;

        RelinkableHandle<YieldTermStructure> yieldTermStructure;
        RelinkableHandle< YieldTermStructure > discountingYTS;
        RelinkableHandle< YieldTermStructure > forwardingYTS;

        std::vector<Rate> strikes;
        std::vector<Period> optionTenors;
        Matrix termV;
        std::vector<Rate> atmTermV;
        std::vector<Handle<Quote> > atmTermVolHandle;

        Handle<CapFloorTermVolCurve> capFloorVolCurve;
        Handle<CapFloorTermVolCurve> flatTermVolCurve;

        boost::shared_ptr<CapFloorTermVolSurface> capFloorVolSurface;
        boost::shared_ptr<CapFloorTermVolSurface> flatTermVolSurface;
        boost::shared_ptr< CapFloorTermVolSurface > capFloorVolRealSurface;

        Real accuracy;
        Real tolerance;

        // cleanup
        SavedSettings backup;

        CommonVars() {
            accuracy = 1.0e-6;
            tolerance = 2.5e-8;
        }

        void setTermStructure() {

            calendar = TARGET();
            dayCounter = Actual365Fixed();

            Rate flatFwdRate = 0.04;
            yieldTermStructure.linkTo(
                boost::shared_ptr<FlatForward>(new FlatForward(0,
                                                               calendar,
                                                               flatFwdRate,
                                                               dayCounter)));
        }

        void setRealTermStructure() {

            calendar = TARGET();
            dayCounter = Actual365Fixed();

            std::vector< int > datesTmp;
            std::vector< Date > dates;
            std::vector< Rate > rates;

            datesTmp += 42124, 42129, 42143, 42221, 42254, 42282, 42313, 42345,
                42374, 42405, 42465, 42495, 42587, 42681, 42772, 42860, 43227,
                43956, 44321, 44686, 45051, 45418, 45782, 46147, 46512, 47609,
                49436, 51263, 53087, 56739, 60392;

            for (std::vector< int >::iterator it = datesTmp.begin();
                 it != datesTmp.end(); ++it)
                dates.push_back(Date(*it));

            rates += -0.00292, -0.00292, -0.001441, -0.00117, -0.001204,
                -0.001212, -0.001223, -0.001236, -0.001221, -0.001238,
                -0.001262, -0.00125, -0.001256, -0.001233, -0.00118, -0.001108,
                -0.000619, 0.000833, 0.001617, 0.002414, 0.003183, 0.003883,
                0.004514, 0.005074, 0.005606, 0.006856, 0.00813, 0.008709,
                0.009136, 0.009601, 0.009384;

            discountingYTS.linkTo(
                boost::shared_ptr< InterpolatedZeroCurve< Linear > >(
                    new InterpolatedZeroCurve< Linear >(dates, rates,
                                                        dayCounter, calendar)));

            QL_REQUIRE(!discountingYTS.empty(),
                "Could not create discounting yieldcurve");

            datesTmp.clear();
            dates.clear();
            rates.clear();

            datesTmp += 42124, 42313, 42436, 42556, 42618, 42800, 42830, 42860,
                43227, 43591, 43956, 44321, 44686, 45051, 45418, 45782, 46147,
                46512, 46878, 47245, 47609, 47973, 48339, 48704, 49069, 49436,
                49800, 50165, 50530, 50895, 51263, 51627, 51991, 52356, 52722,
                53087, 54913, 56739, 60392, 64045;

            for (std::vector< int >::iterator it = datesTmp.begin();
                 it != datesTmp.end(); ++it)
                dates.push_back(Date(*it));

            rates += 0.000649, 0.000649, 0.000684, 0.000717, 0.000745, 0.000872,
                0.000905, 0.000954, 0.001532, 0.002319, 0.003147, 0.003949,
                0.004743, 0.00551, 0.006198, 0.006798, 0.007339, 0.007832,
                0.008242, 0.008614, 0.008935, 0.009205, 0.009443, 0.009651,
                0.009818, 0.009952, 0.010054, 0.010146, 0.010206, 0.010266,
                0.010315, 0.010365, 0.010416, 0.010468, 0.010519, 0.010571,
                0.010757, 0.010806, 0.010423, 0.010217;

            forwardingYTS.linkTo(
                boost::shared_ptr< InterpolatedZeroCurve< Linear > >(
                    new InterpolatedZeroCurve< Linear >(dates, rates,
                                                        dayCounter, calendar)));

            QL_REQUIRE(!forwardingYTS.empty(),
                       "Could not create forwarding yieldcurve");
        }

        void setFlatTermVolCurve() {

          setTermStructure();

          optionTenors.resize(10);
          for (Size i = 0; i < optionTenors.size(); ++i)
              optionTenors[i] = Period(i + 1, Years);

          Volatility flatVol = .18;

          std::vector<Handle<Quote> >  curveVHandle(optionTenors.size());
          for (Size i=0; i<optionTenors.size(); ++i)
              curveVHandle[i] = Handle<Quote>(boost::shared_ptr<Quote>(new
                                                        SimpleQuote(flatVol)));

          flatTermVolCurve = Handle<CapFloorTermVolCurve>(
              shared_ptr<CapFloorTermVolCurve>(new
                  CapFloorTermVolCurve(0, calendar, Following, optionTenors,
                                       curveVHandle, dayCounter)));

        }

        void setFlatTermVolSurface() {

            setTermStructure();

            optionTenors.resize(10);
            for (Size i = 0; i < optionTenors.size(); ++i)
                optionTenors[i] = Period(i + 1, Years);

            strikes.resize(10);
            for (Size j = 0; j < strikes.size(); ++j)
                strikes[j] = Real(j + 1) / 100.0;

            Volatility flatVol = .18;
            termV = Matrix(optionTenors.size(), strikes.size(), flatVol);
            flatTermVolSurface = boost::shared_ptr<CapFloorTermVolSurface>(new
                CapFloorTermVolSurface(0, calendar, Following,
                                       optionTenors, strikes,
                                       termV, dayCounter));
        }


        void setCapFloorTermVolCurve() {

          setTermStructure();

          //atm cap volatility curve
          optionTenors = std::vector<Period>();
          optionTenors.push_back(Period(1, Years));
          optionTenors.push_back(Period(18, Months));
          optionTenors.push_back(Period(2, Years));
          optionTenors.push_back(Period(3, Years));
          optionTenors.push_back(Period(4, Years));
          optionTenors.push_back(Period(5, Years));
          optionTenors.push_back(Period(6, Years));
          optionTenors.push_back(Period(7, Years));
          optionTenors.push_back(Period(8, Years));
          optionTenors.push_back(Period(9, Years));
          optionTenors.push_back(Period(10, Years));
          optionTenors.push_back(Period(12, Years));
          optionTenors.push_back(Period(15, Years));
          optionTenors.push_back(Period(20, Years));
          optionTenors.push_back(Period(25, Years));
          optionTenors.push_back(Period(30, Years));

          //atm capfloor vols from mkt vol matrix using flat yield curve
          atmTermV = std::vector<Volatility>();
          atmTermV.push_back(0.090304);
          atmTermV.push_back(0.12180);
          atmTermV.push_back(0.13077);
          atmTermV.push_back(0.14832);
          atmTermV.push_back(0.15570);
          atmTermV.push_back(0.15816);
          atmTermV.push_back(0.15932);
          atmTermV.push_back(0.16035);
          atmTermV.push_back(0.15951);
          atmTermV.push_back(0.15855);
          atmTermV.push_back(0.15754);
          atmTermV.push_back(0.15459);
          atmTermV.push_back(0.15163);
          atmTermV.push_back(0.14575);
          atmTermV.push_back(0.14175);
          atmTermV.push_back(0.13889);
          atmTermVolHandle.resize(optionTenors.size());
          for (Size i=0; i<optionTenors.size(); ++i) {
            atmTermVolHandle[i] = Handle<Quote>(boost::shared_ptr<Quote>(new
                            SimpleQuote(atmTermV[i])));
          }

          capFloorVolCurve = Handle<CapFloorTermVolCurve>(
            shared_ptr<CapFloorTermVolCurve>(new
                CapFloorTermVolCurve(0, calendar, Following,
                                     optionTenors, atmTermVolHandle,
                                     dayCounter)));

         }

        void setCapFloorTermVolSurface() {

            setTermStructure();

            //cap volatility smile matrix
            optionTenors = std::vector<Period>();
            optionTenors.push_back(Period(1, Years));
            optionTenors.push_back(Period(18, Months));
            optionTenors.push_back(Period(2, Years));
            optionTenors.push_back(Period(3, Years));
            optionTenors.push_back(Period(4, Years));
            optionTenors.push_back(Period(5, Years));
            optionTenors.push_back(Period(6, Years));
            optionTenors.push_back(Period(7, Years));
            optionTenors.push_back(Period(8, Years));
            optionTenors.push_back(Period(9, Years));
            optionTenors.push_back(Period(10, Years));
            optionTenors.push_back(Period(12, Years));
            optionTenors.push_back(Period(15, Years));
            optionTenors.push_back(Period(20, Years));
            optionTenors.push_back(Period(25, Years));
            optionTenors.push_back(Period(30, Years));

            strikes = std::vector<Rate>();
            strikes.push_back(0.015);
            strikes.push_back(0.0175);
            strikes.push_back(0.02);
            strikes.push_back(0.0225);
            strikes.push_back(0.025);
            strikes.push_back(0.03);
            strikes.push_back(0.035);
            strikes.push_back(0.04);
            strikes.push_back(0.05);
            strikes.push_back(0.06);
            strikes.push_back(0.07);
            strikes.push_back(0.08);
            strikes.push_back(0.1);

            termV = Matrix(optionTenors.size(), strikes.size());
            termV[0][0]=0.287;  termV[0][1]=0.274;  termV[0][2]=0.256;  termV[0][3]=0.245;  termV[0][4]=0.227;  termV[0][5]=0.148;  termV[0][6]=0.096;  termV[0][7]=0.09;   termV[0][8]=0.11;   termV[0][9]=0.139;  termV[0][10]=0.166;  termV[0][11]=0.19;   termV[0][12]=0.214;
            termV[1][0]=0.303;  termV[1][1]=0.258;  termV[1][2]=0.22;   termV[1][3]=0.203;  termV[1][4]=0.19;   termV[1][5]=0.153;  termV[1][6]=0.126;  termV[1][7]=0.118;  termV[1][8]=0.147;  termV[1][9]=0.165;  termV[1][10]=0.18;   termV[1][11]=0.192;  termV[1][12]=0.212;
            termV[2][0]=0.303;  termV[2][1]=0.257;  termV[2][2]=0.216;  termV[2][3]=0.196;  termV[2][4]=0.182;  termV[2][5]=0.154;  termV[2][6]=0.134;  termV[2][7]=0.127;  termV[2][8]=0.149;  termV[2][9]=0.166;  termV[2][10]=0.18;   termV[2][11]=0.192;  termV[2][12]=0.212;
            termV[3][0]=0.305;  termV[3][1]=0.266;  termV[3][2]=0.226;  termV[3][3]=0.203;  termV[3][4]=0.19;   termV[3][5]=0.167;  termV[3][6]=0.151;  termV[3][7]=0.144;  termV[3][8]=0.16;   termV[3][9]=0.172;  termV[3][10]=0.183;  termV[3][11]=0.193;  termV[3][12]=0.209;
            termV[4][0]=0.294;  termV[4][1]=0.261;  termV[4][2]=0.216;  termV[4][3]=0.201;  termV[4][4]=0.19;   termV[4][5]=0.171;  termV[4][6]=0.158;  termV[4][7]=0.151;  termV[4][8]=0.163;  termV[4][9]=0.172;  termV[4][10]=0.181;  termV[4][11]=0.188;  termV[4][12]=0.201;
            termV[5][0]=0.276;  termV[5][1]=0.248;  termV[5][2]=0.212;  termV[5][3]=0.199;  termV[5][4]=0.189;  termV[5][5]=0.172;  termV[5][6]=0.16;   termV[5][7]=0.155;  termV[5][8]=0.162;  termV[5][9]=0.17;   termV[5][10]=0.177;  termV[5][11]=0.183;  termV[5][12]=0.195;
            termV[6][0]=0.26;   termV[6][1]=0.237;  termV[6][2]=0.21;   termV[6][3]=0.198;  termV[6][4]=0.188;  termV[6][5]=0.172;  termV[6][6]=0.161;  termV[6][7]=0.156;  termV[6][8]=0.161;  termV[6][9]=0.167;  termV[6][10]=0.173;  termV[6][11]=0.179;  termV[6][12]=0.19;
            termV[7][0]=0.25;   termV[7][1]=0.231;  termV[7][2]=0.208;  termV[7][3]=0.196;  termV[7][4]=0.187;  termV[7][5]=0.172;  termV[7][6]=0.162;  termV[7][7]=0.156;  termV[7][8]=0.16;   termV[7][9]=0.165;  termV[7][10]=0.17;   termV[7][11]=0.175;  termV[7][12]=0.185;
            termV[8][0]=0.244;  termV[8][1]=0.226;  termV[8][2]=0.206;  termV[8][3]=0.195;  termV[8][4]=0.186;  termV[8][5]=0.171;  termV[8][6]=0.161;  termV[8][7]=0.156;  termV[8][8]=0.158;  termV[8][9]=0.162;  termV[8][10]=0.166;  termV[8][11]=0.171;  termV[8][12]=0.18;
            termV[9][0]=0.239;  termV[9][1]=0.222;  termV[9][2]=0.204;  termV[9][3]=0.193;  termV[9][4]=0.185;  termV[9][5]=0.17;   termV[9][6]=0.16;   termV[9][7]=0.155;  termV[9][8]=0.156;  termV[9][9]=0.159;  termV[9][10]=0.163;  termV[9][11]=0.168;  termV[9][12]=0.177;
            termV[10][0]=0.235; termV[10][1]=0.219; termV[10][2]=0.202; termV[10][3]=0.192; termV[10][4]=0.183; termV[10][5]=0.169; termV[10][6]=0.159; termV[10][7]=0.154; termV[10][8]=0.154; termV[10][9]=0.156; termV[10][10]=0.16;  termV[10][11]=0.164; termV[10][12]=0.173;
            termV[11][0]=0.227; termV[11][1]=0.212; termV[11][2]=0.197; termV[11][3]=0.187; termV[11][4]=0.179; termV[11][5]=0.166; termV[11][6]=0.156; termV[11][7]=0.151; termV[11][8]=0.149; termV[11][9]=0.15;  termV[11][10]=0.153; termV[11][11]=0.157; termV[11][12]=0.165;
            termV[12][0]=0.22;  termV[12][1]=0.206; termV[12][2]=0.192; termV[12][3]=0.183; termV[12][4]=0.175; termV[12][5]=0.162; termV[12][6]=0.153; termV[12][7]=0.147; termV[12][8]=0.144; termV[12][9]=0.144; termV[12][10]=0.147; termV[12][11]=0.151; termV[12][12]=0.158;
            termV[13][0]=0.211; termV[13][1]=0.197; termV[13][2]=0.185; termV[13][3]=0.176; termV[13][4]=0.168; termV[13][5]=0.156; termV[13][6]=0.147; termV[13][7]=0.142; termV[13][8]=0.138; termV[13][9]=0.138; termV[13][10]=0.14;  termV[13][11]=0.144; termV[13][12]=0.151;
            termV[14][0]=0.204; termV[14][1]=0.192; termV[14][2]=0.18;  termV[14][3]=0.171; termV[14][4]=0.164; termV[14][5]=0.152; termV[14][6]=0.143; termV[14][7]=0.138; termV[14][8]=0.134; termV[14][9]=0.134; termV[14][10]=0.137; termV[14][11]=0.14;  termV[14][12]=0.148;
            termV[15][0]=0.2;   termV[15][1]=0.187; termV[15][2]=0.176; termV[15][3]=0.167; termV[15][4]=0.16;  termV[15][5]=0.148; termV[15][6]=0.14;  termV[15][7]=0.135; termV[15][8]=0.131; termV[15][9]=0.132; termV[15][10]=0.135; termV[15][11]=0.139; termV[15][12]=0.146;

            capFloorVolSurface = boost::shared_ptr<CapFloorTermVolSurface>(new
                CapFloorTermVolSurface(0, calendar, Following,
                                       optionTenors, strikes,
                                       termV, dayCounter));
        }

        void setRealCapFloorTermVolSurface() {

            setRealTermStructure();

            // cap volatility smile matrix
            optionTenors = std::vector< Period >();
            optionTenors.push_back(Period(1, Years));
            optionTenors.push_back(Period(18, Months));
            optionTenors.push_back(Period(2, Years));
            optionTenors.push_back(Period(3, Years));
            optionTenors.push_back(Period(4, Years));
            optionTenors.push_back(Period(5, Years));
            optionTenors.push_back(Period(6, Years));
            optionTenors.push_back(Period(7, Years));
            optionTenors.push_back(Period(8, Years));
            optionTenors.push_back(Period(9, Years));
            optionTenors.push_back(Period(10, Years));
            optionTenors.push_back(Period(12, Years));
            optionTenors.push_back(Period(15, Years));
            optionTenors.push_back(Period(20, Years));
            optionTenors.push_back(Period(25, Years));
            optionTenors.push_back(Period(30, Years));
            // 16

            strikes = std::vector< Rate >();
            strikes.push_back(-0.005);
            strikes.push_back(-0.0025);
            strikes.push_back(-0.00125);
            strikes.push_back(0.0);
            strikes.push_back(0.00125);
            strikes.push_back(0.0025);
            strikes.push_back(0.005);
            strikes.push_back(0.01);
            strikes.push_back(0.015);
            strikes.push_back(0.02);
            strikes.push_back(0.03);
            strikes.push_back(0.05);
            strikes.push_back(0.1);
            // 13

            std::vector< Real > rawVols;
            rawVols += 0.49, 0.39, 0.34, 0.31, 0.34, 0.37, 0.50, 0.75, 0.99, 1.21, 1.64, 2.44, 4.29, 
                       0.44, 0.36, 0.33, 0.31, 0.33, 0.35,0.45, 0.65, 0.83, 1.00, 1.32, 1.93, 3.30, 
                       0.40, 0.35, 0.33,0.31, 0.33, 0.34, 0.41, 0.55, 0.69, 0.82, 1.08, 1.56, 2.68,
                       0.42, 0.39, 0.38, 0.37, 0.38, 0.39, 0.43, 0.54, 0.64, 0.74,0.94, 1.31, 2.18, 
                       0.46, 0.43, 0.42, 0.41, 0.42, 0.43, 0.47,0.56, 0.66, 0.75, 0.93, 1.28, 2.07, 
                       0.49, 0.47, 0.46, 0.45,0.46, 0.47, 0.51, 0.59, 0.68, 0.76, 0.93, 1.25, 1.99, 
                       0.51, 0.49, 0.49, 0.48, 0.49, 0.50, 0.54, 0.62, 0.70, 0.78, 0.94,1.24, 1.94, 
                       0.52, 0.51, 0.51, 0.51, 0.52, 0.53, 0.56, 0.63,0.71, 0.79, 0.94, 1.23, 1.89, 
                       0.53, 0.52, 0.52, 0.52, 0.53,0.54, 0.57, 0.65, 0.72, 0.79, 0.94, 1.21, 1.83, 
                       0.55, 0.54, 0.54, 0.54, 0.55, 0.56, 0.59, 0.66, 0.72, 0.79, 0.91, 1.15,1.71, 
                       0.56, 0.56, 0.56, 0.56, 0.57, 0.58, 0.61, 0.67, 0.72,0.78, 0.89, 1.09, 1.59, 
                       0.59, 0.58, 0.58, 0.59, 0.59, 0.60,0.63, 0.68, 0.73, 0.78, 0.86, 1.03, 1.45, 
                       0.61, 0.61, 0.61,0.61, 0.62, 0.62, 0.64, 0.69, 0.73, 0.77, 0.85, 1.02, 1.44,
                       0.62, 0.62, 0.63, 0.63, 0.64, 0.64, 0.65, 0.69, 0.72, 0.76,0.82, 0.96, 1.32, 
                       0.62, 0.63, 0.63, 0.63, 0.65, 0.66, 0.66,0.68, 0.72, 0.74, 0.80, 0.93, 1.25, 
                       0.62, 0.62, 0.62, 0.62,0.66, 0.67, 0.67, 0.67, 0.72, 0.72, 0.78, 0.90, 1.25;

            termV = Matrix(optionTenors.size(), strikes.size());
            std::copy(rawVols.begin(), rawVols.end(), termV.begin());
            termV /= 100;

            capFloorVolRealSurface =
                boost::shared_ptr< CapFloorTermVolSurface >(
                    new CapFloorTermVolSurface(0, calendar, Following,
                                               optionTenors, strikes, termV,
                                               dayCounter));
        }
};


void OptionletStripperTest::testFlatTermVolatilityStripping1() {

    BOOST_TEST_MESSAGE(
        "Testing forward/forward vol stripping from flat term vol "
        "surface using OptionletStripper1 class...");

    CommonVars vars;
    Settings::instance().evaluationDate() = Date(28, October, 2013);

    vars.setFlatTermVolSurface();

    shared_ptr<IborIndex> iborIndex(new Euribor6M(vars.yieldTermStructure));

    boost::shared_ptr<OptionletStripper> optionletStripper1(new
        OptionletStripper1(vars.flatTermVolSurface,
                           iborIndex,
                           Null<Rate>(),
                           vars.accuracy));

    boost::shared_ptr<StrippedOptionletAdapter> strippedOptionletAdapter(new
        StrippedOptionletAdapter(optionletStripper1));

    Handle<OptionletVolatilityStructure> vol(strippedOptionletAdapter);

    vol->enableExtrapolation();

    boost::shared_ptr<BlackCapFloorEngine> strippedVolEngine(new
        BlackCapFloorEngine(vars.yieldTermStructure,
                            vol));

    boost::shared_ptr<CapFloor> cap;
    for (Size tenorIndex=0; tenorIndex<vars.optionTenors.size(); ++tenorIndex) {
        for (Size strikeIndex=0; strikeIndex<vars.strikes.size(); ++strikeIndex) {
            cap = MakeCapFloor(CapFloor::Cap,
                               vars.optionTenors[tenorIndex],
                               iborIndex,
                               vars.strikes[strikeIndex],
                               0*Days)
                  .withPricingEngine(strippedVolEngine);

            Real priceFromStrippedVolatility = cap->NPV();

            boost::shared_ptr<PricingEngine> blackCapFloorEngineConstantVolatility(new
                BlackCapFloorEngine(vars.yieldTermStructure,
                                    vars.termV[tenorIndex][strikeIndex]));

            cap->setPricingEngine(blackCapFloorEngineConstantVolatility);
            Real priceFromConstantVolatility = cap->NPV();

            Real error = std::fabs(priceFromStrippedVolatility - priceFromConstantVolatility);
            if (error>vars.tolerance)
                BOOST_FAIL("\noption tenor:       " << vars.optionTenors[tenorIndex] <<
                           "\nstrike:             " << io::rate(vars.strikes[strikeIndex]) <<
                           "\nstripped vol price: " << io::rate(priceFromStrippedVolatility) <<
                           "\nconstant vol price: " << io::rate(priceFromConstantVolatility) <<
                           "\nerror:              " << io::rate(error) <<
                           "\ntolerance:          " << io::rate(vars.tolerance));
            }
    }
}



void OptionletStripperTest::testTermVolatilityStripping1(MarketData md, MarketConventions mc, Date asOfDate) {



    //CommonVars vars;
    //Date asOfDate = Settings::instance().evaluationDate(MarketData md, Date asOfDate);
    //BootStrapper::MarketConventions mc; BootStrapper::MarketData md;

    /*std::map<std::string, boost::shared_ptr<PiecewiseFlatForward> > rateTermStructures = Bootstrap(asOfDate, md, mc);
    RelinkableHandle<YieldTermStructure> eoniaYields;
    eoniaYields.linkTo(rateTermStructures["OIS"]);
    boost::shared_ptr<Eonia> eoniaIndex = boost::shared_ptr<Eonia>(new Eonia(eoniaYields));

    RelinkableHandle<YieldTermStructure> libor3MYields;
    libor3MYields.linkTo(rateTermStructures["Libor3M"]);
    shared_ptr<IborIndex> iborIndex(new Euribor6M(libor3MYields));*/

    RelinkableHandle<YieldTermStructure> yieldTermStructure;
    Rate flatFwdRate = 0.04;
    yieldTermStructure.linkTo(
        boost::shared_ptr<FlatForward>(new FlatForward(0,
        TARGET(),
        flatFwdRate,
        Actual365Fixed())));

    Handle<YieldTermStructure> eoniaYields(yieldTermStructure);
    Handle<YieldTermStructure> libor3MYields(yieldTermStructure);
    boost::shared_ptr<Eonia> eoniaIndex = boost::shared_ptr<Eonia>(new Eonia(eoniaYields));
    shared_ptr<IborIndex> iborIndex(new Euribor6M(libor3MYields)); 

    //Matrix vols =  md.termV;

    //boost::shared_ptr<CapFloorTermVolSurface> capFloorVolSurface = boost::shared_ptr<CapFloorTermVolSurface>(new
    //    CapFloorTermVolSurface(0, mc.calendar, Following,
    //    md.optionTenors, md.strikes,
    //    vols, mc.floatSwapDayCount));
	boost::shared_ptr<CapFloorTermVolSurface> capFloorVolSurface = md.mdContainer["EURCapFlatVolaNormal"];

    boost::shared_ptr<OptionletStripper> optionletStripper1(new
        OptionletStripper1(capFloorVolSurface,iborIndex,
                           Null<Rate>(),
                           0.00001));

    boost::shared_ptr<StrippedOptionletAdapter> strippedOptionletAdapter =
        boost::shared_ptr<StrippedOptionletAdapter>(new
            StrippedOptionletAdapter(optionletStripper1));

    Handle<OptionletVolatilityStructure> vol(strippedOptionletAdapter);

    vol->enableExtrapolation();

    boost::shared_ptr<BlackCapFloorEngine> strippedVolEngine(new
        BlackCapFloorEngine(libor3MYields,vol));

    boost::shared_ptr<CapFloor> cap;
    for (Size tenorIndex=0; tenorIndex<md.optionTenors.size(); ++tenorIndex) {
        for (Size strikeIndex = 0; strikeIndex < md.strikes.size(); ++strikeIndex) {
            cap = MakeCapFloor(CapFloor::Cap,
                md.optionTenors[tenorIndex],
                iborIndex,
                md.strikes[strikeIndex],
                0 * Days)
                .withPricingEngine(strippedVolEngine);

            Real priceFromStrippedVolatility = cap->NPV();

            boost::shared_ptr<PricingEngine> blackCapFloorEngineConstantVolatility(new
                BlackCapFloorEngine(libor3MYields,
                md.termV[tenorIndex][strikeIndex]));

            cap->setPricingEngine(blackCapFloorEngineConstantVolatility);
            Real priceFromConstantVolatility = cap->NPV();

            Real error = std::fabs(priceFromStrippedVolatility - priceFromConstantVolatility);
            if (error > 0.00001)
            {
                std::cout << "\noption tenor:       " << md.optionTenors[tenorIndex]
                          << "\nstrike:             " << io::rate(md.strikes[strikeIndex]) 
                          << "\nstripped vol price: " << io::rate(priceFromStrippedVolatility) 
                          << "\nconstant vol price: " << io::rate(priceFromConstantVolatility) 
                          << "\nerror:              " << io::rate(error) 
                          << "\ntolerance:          " << io::rate(0.00001);
            }
        }
    }
}

void OptionletStripperTest::testTermVolatilityStrippingNormalVol(MarketData md, MarketConventions mc, Date asOfDate,
	boost::shared_ptr<BootStrapper::PiecewiseFlatForward> discCurve, boost::shared_ptr<BootStrapper::PiecewiseFlatForward> fwdCurve, 
	boost::shared_ptr<OptionletVolatilityStructure> capletVols) {

	Handle<YieldTermStructure> fwdCurveHandle(fwdCurve);
	Handle<YieldTermStructure> discCurveHandle(discCurve);

    shared_ptr< IborIndex > iborIndex(new Euribor3M(fwdCurveHandle));

	boost::shared_ptr<CapFloorTermVolSurface> flatVols = md.mdContainer["EURCapFlatVolaNormal"];

	
    /*boost::shared_ptr< OptionletStripper > optionletStripper1(
        new OptionletStripper1(capFloorVolSurface, iborIndex,
                               Null< Rate >(), md.tolerance, md.maxIter,
			discCurveHandle, Normal));*/

   /* QL_REQUIRE(optionletStripper1 != NULL,
               "Could not create optionletStripper");*/

    //boost::shared_ptr< StrippedOptionletAdapter > strippedOptionletAdapter =
    //    boost::shared_ptr< StrippedOptionletAdapter >(
    //        new StrippedOptionletAdapter(optionletStripper1));

    //QL_REQUIRE(optionletStripper1 != NULL,
    //           "Could not create StrippedOptionletAdapter");


    Handle< OptionletVolatilityStructure > vol(capletVols);

    vol->enableExtrapolation();

    boost::shared_ptr< BachelierCapFloorEngine > strippedVolEngine(
        new BachelierCapFloorEngine(discCurveHandle, vol));
    QL_REQUIRE(strippedVolEngine != NULL, "Could not create strippedVolEngine");

    boost::shared_ptr< CapFloor > cap;
    for (Size tenorIndex = 0; tenorIndex < flatVols->optionTenors().size();
         ++tenorIndex) {
        for (Size strikeIndex = 0; strikeIndex < flatVols->strikes().size();
             ++strikeIndex) {

			Period period = flatVols->optionTenors()[tenorIndex];
			Rate strike = flatVols->strikes()[strikeIndex];
            cap = MakeCapFloor(CapFloor::Cap, period, iborIndex, strike, 0 * Days).withPricingEngine(strippedVolEngine);
            Real priceFromStrippedVolatility = cap->NPV();
			
			Real flatVol = flatVols->volatility(period, strike);
            boost::shared_ptr< PricingEngine >
                bachelierCapFloorEngineConstantVolatility(
                    new BachelierCapFloorEngine(
                        discCurveHandle,
                        flatVol));

            cap->setPricingEngine(bachelierCapFloorEngineConstantVolatility);
            Real priceFromConstantVolatility = cap->NPV();

            Real error = std::fabs(priceFromStrippedVolatility -
                                   priceFromConstantVolatility);
			if (error > md.tolerance)
			{
				std::cout <<
					"\noption tenor:       "
					<< period << "\nstrike:             "
					<< io::rate(strike)
					<< "\nstripped vol price: "
					<< io::rate(priceFromStrippedVolatility)
					<< "\nconstant vol price: "
					<< io::rate(priceFromConstantVolatility)
					<< "\nerror:              " << io::rate(error)
					<< "\ntolerance:          " << io::rate(md.tolerance) << std::endl;
			}
			else
			{
				std::cout
					<< "Libor Cap(" << period << "," << strike << ") premium target: " << io::rate(priceFromConstantVolatility) << "    " << "actual: " << io::rate(priceFromStrippedVolatility) << std::endl;
			}
        }
    }
}

void OptionletStripperTest::testTermVolatilityStrippingShiftedLogNormalVol() {

    BOOST_TEST_MESSAGE(
        "Testing forward/forward vol stripping from non-flat normal vol term "
        "vol surface for normal vol setup using OptionletStripper1 class...");

    CommonVars vars;
    Real shift = 0.03;
    Settings::instance().evaluationDate() = Date(30, April, 2015);

    vars.setRealCapFloorTermVolSurface();

    shared_ptr< IborIndex > iborIndex(new Euribor6M(vars.forwardingYTS));

    boost::shared_ptr< OptionletStripper > optionletStripper1(
        new OptionletStripper1(vars.capFloorVolRealSurface, iborIndex,
                               Null< Rate >(), vars.accuracy, 100,
                               vars.discountingYTS, ShiftedLognormal, shift,
                               true));

    QL_REQUIRE(optionletStripper1 != NULL,
               "Could not create optionletStripper");

    boost::shared_ptr< StrippedOptionletAdapter > strippedOptionletAdapter =
        boost::shared_ptr< StrippedOptionletAdapter >(
            new StrippedOptionletAdapter(optionletStripper1));

    QL_REQUIRE(optionletStripper1 != NULL,
               "Could not create StrippedOptionletAdapter");

    Handle< OptionletVolatilityStructure > vol(strippedOptionletAdapter);

    vol->enableExtrapolation();

    boost::shared_ptr< BlackCapFloorEngine > strippedVolEngine(
        new BlackCapFloorEngine(vars.discountingYTS, vol));
    QL_REQUIRE(strippedVolEngine != NULL, "Could not create strippedVolEngine");

    boost::shared_ptr< CapFloor > cap;
    for (Size strikeIndex = 0; strikeIndex < vars.strikes.size();
         ++strikeIndex) {
        for (Size tenorIndex = 0; tenorIndex < vars.optionTenors.size();
             ++tenorIndex) {
            cap = MakeCapFloor(CapFloor::Cap, vars.optionTenors[tenorIndex],
                               iborIndex, vars.strikes[strikeIndex],
                               0 * Days).withPricingEngine(strippedVolEngine);

            Real priceFromStrippedVolatility = cap->NPV();

            boost::shared_ptr< PricingEngine >
                blackCapFloorEngineConstantVolatility(new BlackCapFloorEngine(
                    vars.discountingYTS, vars.termV[tenorIndex][strikeIndex],
                    vars.capFloorVolRealSurface->dayCounter(), shift));

            cap->setPricingEngine(blackCapFloorEngineConstantVolatility);
            Real priceFromConstantVolatility = cap->NPV();

            Real error = std::fabs(priceFromStrippedVolatility -
                                   priceFromConstantVolatility);
            if (error > vars.tolerance)
                BOOST_FAIL(
                    "\noption tenor:       "
                    << vars.optionTenors[tenorIndex] << "\nstrike:             "
                    << io::rate(vars.strikes[strikeIndex])
                    << "\nstripped vol price: "
                    << io::rate(priceFromStrippedVolatility)
                    << "\nconstant vol price: "
                    << io::rate(priceFromConstantVolatility)
                    << "\nerror:              " << io::rate(error)
                    << "\ntolerance:          " << io::rate(vars.tolerance));
        }
    }
}

void OptionletStripperTest::testFlatTermVolatilityStripping2() {

  BOOST_TEST_MESSAGE(
        "Testing forward/forward vol stripping from flat term vol "
        "surface using OptionletStripper2 class...");

  CommonVars vars;
  Settings::instance().evaluationDate() = Date::todaysDate();

  vars.setFlatTermVolCurve();
  vars.setFlatTermVolSurface();

  shared_ptr<IborIndex> iborIndex(new Euribor6M(vars.yieldTermStructure));

  // optionletstripper1
  shared_ptr<OptionletStripper1> optionletStripper1(new
        OptionletStripper1(vars.flatTermVolSurface,
                           iborIndex,
                           Null<Rate>(),
                           vars.accuracy));

  boost::shared_ptr<StrippedOptionletAdapter> strippedOptionletAdapter1(new
        StrippedOptionletAdapter(optionletStripper1));

  Handle<OptionletVolatilityStructure> vol1(strippedOptionletAdapter1);

  vol1->enableExtrapolation();

  // optionletstripper2
  shared_ptr<OptionletStripper> optionletStripper2(new
        OptionletStripper2(optionletStripper1, vars.flatTermVolCurve));

  shared_ptr<StrippedOptionletAdapter> strippedOptionletAdapter2(new
        StrippedOptionletAdapter(optionletStripper2));

  Handle<OptionletVolatilityStructure> vol2(strippedOptionletAdapter2);

  vol2->enableExtrapolation();

  // consistency check: diff(stripped vol1-stripped vol2)
  for (Size strikeIndex=0; strikeIndex<vars.strikes.size(); ++strikeIndex) {
    for (Size tenorIndex=0; tenorIndex<vars.optionTenors.size(); ++tenorIndex) {

      Volatility strippedVol1 = vol1->volatility(vars.optionTenors[tenorIndex],
                                                 vars.strikes[strikeIndex], true);

      Volatility strippedVol2 = vol2->volatility(vars.optionTenors[tenorIndex],
                                                 vars.strikes[strikeIndex], true);

      // vol from flat vol surface (for comparison only)
      Volatility flatVol = vars.flatTermVolSurface->volatility(vars.optionTenors[tenorIndex],
                                                               vars.strikes[strikeIndex], true);

    Real error = std::fabs(strippedVol1-strippedVol2);
      if (error>vars.tolerance)
      BOOST_FAIL("\noption tenor:  " << vars.optionTenors[tenorIndex] <<
                 "\nstrike:        " << io::rate(vars.strikes[strikeIndex]) <<
                 "\nstripped vol1: " << io::rate(strippedVol1) <<
                 "\nstripped vol2: " << io::rate(strippedVol2) <<
                 "\nflat vol:      " << io::rate(flatVol) <<
                 "\nerror:         " << io::rate(error) <<
                 "\ntolerance:     " << io::rate(vars.tolerance));
    }
  }

}

void OptionletStripperTest::testTermVolatilityStripping2(MarketData md, MarketConventions mc, Date asOfDate,
	boost::shared_ptr<BootStrapper::PiecewiseFlatForward> discCurve, boost::shared_ptr<BootStrapper::PiecewiseFlatForward> fwdCurve, 
	boost::shared_ptr<OptionletVolatilityStructure> capFloorVolSurface) {

	Handle<YieldTermStructure> fwdCurveHandle(fwdCurve);
	Handle<YieldTermStructure> discCurveHandle(discCurve);

	shared_ptr< IborIndex > iborIndex(new Euribor3M(fwdCurveHandle));

	boost::shared_ptr<CapFloorTermVolSurface> flatVols = md.mdContainer["EURCapFlatVolaNormal"];



  // optionletstripper1

	bool dontThrow = false; Real shift = 0;	Rate switchStrike = Null< Rate >();
	boost::shared_ptr<OptionletStripper1> optionletStripper1(new
		OptionletStripper1(flatVols,
			iborIndex,switchStrike, md.tolerance, md.maxIter,
			Handle<YieldTermStructure>(discCurve), Normal, shift, dontThrow));


  boost::shared_ptr<StrippedOptionletAdapter> strippedOptionletAdapter1 =
        boost::shared_ptr<StrippedOptionletAdapter>(new
            StrippedOptionletAdapter(optionletStripper1));


  Handle<OptionletVolatilityStructure> vol1(strippedOptionletAdapter1);
  vol1->enableExtrapolation();


  // optionletstripper2
  boost::shared_ptr<OptionletStripper> optionletStripper2(new
                OptionletStripper2(optionletStripper1,
                                   md.atmVols));

  boost::shared_ptr<StrippedOptionletAdapter> strippedOptionletAdapter2(new
        StrippedOptionletAdapter(optionletStripper2));

  Handle<OptionletVolatilityStructure> vol2(strippedOptionletAdapter2);
  vol2->enableExtrapolation();

  // consistency check: diff(stripped vol1-stripped vol2)
  for (Size strikeIndex=0; strikeIndex<flatVols->strikes().size(); ++strikeIndex) {
    for (Size tenorIndex=0; tenorIndex<flatVols->optionTenors().size(); ++tenorIndex) {

      Volatility strippedVol1 = vol1->volatility(flatVols->optionTenors()[tenorIndex],
		  flatVols->strikes()[strikeIndex], true);

      Volatility strippedVol2 = vol2->volatility(flatVols->optionTenors()[tenorIndex],
		  flatVols->strikes()[strikeIndex], true);

      // vol from flat vol surface (for comparison only)
      //Volatility flatVol = capFloorVolSurface->volatility(vars.optionTenors[tenorIndex],
      //                                                         vars.strikes[strikeIndex], true);

      Real error = std::fabs(strippedVol1-strippedVol2);
	  if (error > md.tolerance)
	  {
		 /* std::cout << "\noption tenor:  " << capFloorVolSurface.optionTenors()[tenorIndex] <<
			  "\nstrike:        " << io::rate(capFloorVolSurface->strikes()[strikeIndex]) <<
			  "\nstripped vol1: " << io::rate(strippedVol1) <<
			  "\nstripped vol2: " << io::rate(strippedVol2) <<
			  "\nflat vol:      " << io::rate(flatVol) <<
			  "\nerror:         " << io::rate(error) <<
			  "\ntolerance:     " << io::rate(md.tolerance));*/
	  }
    }
  }
}

void OptionletStripperTest::testSwitchStrike() {
    BOOST_TEST_MESSAGE("Testing switch strike level and recalibration of level "
                       "in case of curve relinking...");

    CommonVars vars;
    Settings::instance().evaluationDate() = Date(28, October, 2013);
    vars.setCapFloorTermVolSurface();

    RelinkableHandle< YieldTermStructure > yieldTermStructure;
    yieldTermStructure.linkTo(boost::shared_ptr< FlatForward >(
        new FlatForward(0, vars.calendar, 0.03, vars.dayCounter)));

    shared_ptr< IborIndex > iborIndex(new Euribor6M(yieldTermStructure));

    boost::shared_ptr< OptionletStripper1 > optionletStripper1(
        new OptionletStripper1(vars.capFloorVolSurface, iborIndex,
                               Null< Rate >(), vars.accuracy));

    Real error = std::fabs(optionletStripper1->switchStrike() - 0.02981223);
    if (error > vars.tolerance)
        BOOST_FAIL("\nSwitchstrike not correctly computed:  "
                   << "\nexpected switch strike: " << io::rate(0.02981223)
                   << "\ncomputed switch strike: "
                   << io::rate(optionletStripper1->switchStrike())
                   << "\nerror:         " << io::rate(error)
                   << "\ntolerance:     " << io::rate(vars.tolerance));

    yieldTermStructure.linkTo(boost::shared_ptr< FlatForward >(
        new FlatForward(0, vars.calendar, 0.05, vars.dayCounter)));

    error = std::fabs(optionletStripper1->switchStrike() - 0.0499371);
    if (error > vars.tolerance)
        BOOST_FAIL("\nSwitchstrike not correctly computed:  "
                   << "\nexpected switch strike: " << io::rate(0.0499371)
                   << "\ncomputed switch strike: "
                   << io::rate(optionletStripper1->switchStrike())
                   << "\nerror:         " << io::rate(error)
                   << "\ntolerance:     " << io::rate(vars.tolerance));
}


}

