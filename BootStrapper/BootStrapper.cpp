#include "BootStrapper.hpp"
//#include <boost/shared_ptr.hpp>
#include <ql/termstructures/volatility/volatilitytype.hpp>
#include <string>
#include <ql/indexes/ibor/sofr.hpp>
#include <ql/termstructures/yield/piecewiseyieldcurve.hpp>
#include <ql/time/date.hpp>
#include <ql/time/timeunit.hpp>
#include <ql/time/imm.hpp>





namespace BootStrapper {
    using namespace QuantLib;

    void printSwapRateHelper(LiborSwapObs sd, MarketConventions mc)
    {
        Period tenor = sd.nIndexUnits * sd.indexUnit;
        Period term = sd.nTermUnits * sd.termUnit;

        std::cout << "SwapRateHelper created:" << std::endl
            << "\t rate:\t" << 0.01 * sd.rate << std::endl
            << "\t term:\t" << term << std::endl
            << "\t tenor:\t" << tenor << std::endl
            << "\t calendar:\t" << mc.calendar << std::endl
            << "\t settlemend days:\t" << mc.settlementDays << std::endl;

    }
    void printDepoHelper(DepoSwapObs sd, MarketConventions mc)
    {

        Period term = sd.n * sd.unit;

        std::cout << "DepoRateHelper created:" << std::endl
            << "\t rate:\t" << 0.01 * sd.rate << std::endl
            << "\t term:\t" << term << std::endl
            << "\t calendar:\t" << mc.calendar << std::endl
            << "\t settlemend days:\t" << mc.settlementDays << std::endl;

    }

    void PrintCurve(ext::shared_ptr<PiecewiseYieldCurve<Discount, Linear>> curve)
    {
        std::vector<std::pair<Date, Real> > data = curve.get()->nodes();
        for (std::vector<std::pair<Date, Real> >::iterator it = data.begin(); it != data.end(); ++it) {
            std::cout << io::iso_date(it->first) << ": " << io::percent(it->second) << std::endl;
        }
    }

    ext::shared_ptr<PiecewiseYieldCurve<Discount, Linear>>  BootstrapLIBOR(Date asOfDate, MarketData& md, MarketConventions& mc, boost::shared_ptr<PiecewiseYieldCurve<Discount, Linear>>& discountCurve)
    {

        Handle<YieldTermStructure> oisCurveHandle(discountCurve);

        std::vector<ext::shared_ptr<RateHelper> > swap3mHelpers;
        ext::shared_ptr<IborIndex> euribor3m(new Euribor3M);


        for (Size i = 0; i < md.depoEUR.size(); i++) {
            Real rate = md.depoEUR[i].rate;
            ext::shared_ptr<SimpleQuote> simple = boost::shared_ptr<SimpleQuote>(new SimpleQuote(rate));
            ext::shared_ptr<Quote> quote(simple);
            Period term = md.depoEUR[i].n * md.depoEUR[i].unit;
            ext::shared_ptr<RateHelper> helper(new DepositRateHelper(Handle<Quote>(quote), term, md.depoEUR[i].settlementDays, euribor3m->fixingCalendar(), euribor3m->businessDayConvention(), euribor3m->endOfMonth(), euribor3m->dayCounter()));

        }

        for (Size i = 0; i < md.liborSwap6mEUR.size(); i++) {
            SimpleQuote quote(md.liborSwap6mEUR[i].rate);
            Handle<Quote> quoteHandle(ext::make_shared<SimpleQuote>(quote));

            SimpleQuote spread(0);
            Handle<Quote> spreadHandle(ext::make_shared<SimpleQuote>(spread));

            Period tenor = md.liborSwap6mEUR[i].nIndexUnits * md.liborSwap6mEUR[i].indexUnit;
            Period term = md.liborSwap6mEUR[i].nTermUnits * md.liborSwap6mEUR[i].termUnit;

            Period  fwdStart = 0 * Days;
            Natural settlementDays = 0;
            bool useExternalDiscount = true;
            boost::shared_ptr<RateHelper> helper;
            if (useExternalDiscount)
            {
                ext::shared_ptr<RateHelper> helperWith(new SwapRateHelper(quoteHandle, term, mc.calendar, mc.fixedSwapFrequency, mc.fixedSwapConvention, mc.fixedSwapDayCount, euribor3m, spreadHandle, fwdStart, oisCurveHandle, md.liborSwap6mEUR[i].settlementDays));
                helper = helperWith;
            }
            else
            {
                ext::shared_ptr<RateHelper> helperWithout(new SwapRateHelper(quoteHandle, term, mc.calendar, mc.fixedSwapFrequency, mc.fixedSwapConvention, mc.fixedSwapDayCount, euribor3m, spreadHandle, fwdStart));
                helper = helperWithout;

            }


            if (tenor == 3 * Months)
            {
                swap3mHelpers.push_back(helper);
                //printSwapRateHelper(md.swapData[i], mc);

            }
        }


        return ext::shared_ptr<PiecewiseYieldCurve<Discount, Linear>>(new PiecewiseYieldCurve<Discount, Linear>(asOfDate, swap3mHelpers, Actual365Fixed()));
    }

    boost::shared_ptr<PiecewiseYieldCurve<Discount, Linear>> BootstrapSOFRFutures(Date asOfDate, std::vector<SofrFutureObs>& sofrFutureUSD, MarketConventions& mc)
    {
        ext::shared_ptr<OvernightIndex> index = ext::make_shared<Sofr>();
        /*index->addFixing(Date(1, October, 2018), 0.0222);
        index->addFixing(Date(2, October, 2018), 0.022);
        index->addFixing(Date(3, October, 2018), 0.022sofrFutureUSD
        index->addFixing(Date(4, October, 2018), 0.0218);
        index->addFixing(Date(5, October, 2018), 0.0216);
        index->addFixing(Date(9, October, 2018), 0.0215);
        index->addFixing(Date(10, October, 2018), 0.0215);
        index->addFixing(Date(11, October, 2018), 0.0217);
        index->addFixing(Date(12, October, 2018), 0.0218);
        index->addFixing(Date(15, October, 2018), 0.0221);
        index->addFixing(Date(16, October, 2018), 0.0218);
        index->addFixing(Date(17, October, 2018), 0.0218);
        index->addFixing(Date(18, October, 2018), 0.0219);
        index->addFixing(Date(19, October, 2018), 0.0219);
        index->addFixing(Date(22, October, 2018), 0.0218);
        index->addFixing(Date(23, October, 2018), 0.0217);
        index->addFixing(Date(24, October, 2018), 0.0218);
        index->addFixing(Date(25, October, 2018), 0.0219);*/
        std::vector<ext::shared_ptr<RateHelper> > helpers;
        for (const auto& sofrQuote : sofrFutureUSD) {
            helpers.push_back(ext::make_shared<SofrFutureRateHelper>(
                sofrQuote.price, sofrQuote.month, sofrQuote.year, sofrQuote.freq,
                0.0));
        }

        ext::shared_ptr<PiecewiseYieldCurve<Discount, Linear> > curve =
            ext::make_shared<PiecewiseYieldCurve<Discount, Linear> >(asOfDate, helpers, Actual365Fixed());

        return curve;
    }

    boost::shared_ptr<PiecewiseYieldCurve<Discount, Linear>> BootstrapOIS(Date asOfDate, std::vector<DepoSwapObs>& depo, std::vector<DepoSwapObs>& oiSwap, MarketConventions& mc)
    {

        std::vector<ext::shared_ptr<RateHelper> > eoniaHelpers;

        ext::shared_ptr<IborIndex> euribor3m(new Euribor3M);
        ext::shared_ptr<Eonia> eonia(new Eonia);

        for (Size i = 0; i < depo.size(); i++) {
            Real rate = depo[i].rate;
            ext::shared_ptr<SimpleQuote> simple = ext::shared_ptr<SimpleQuote>(new SimpleQuote(rate));
            ext::shared_ptr<Quote> quote(simple);
            Period term = depo[i].n * depo[i].unit;
            ext::shared_ptr<RateHelper> helper(new DepositRateHelper(Handle<Quote>(quote), term, depo[i].settlementDays, euribor3m->fixingCalendar(), euribor3m->businessDayConvention(), euribor3m->endOfMonth(), euribor3m->dayCounter()));

            if (term <= 2 * Days)
                eoniaHelpers.push_back(helper);
            // if (term <= 3 * Months)
            // 	swap3mHelpers.push_back(helper);
        }


        for (Size i = 0; i < oiSwap.size(); i++) {
            Real rate = oiSwap[i].rate;
            ext::shared_ptr<SimpleQuote> simple = ext::shared_ptr<SimpleQuote>(new SimpleQuote(rate));
            ext::shared_ptr<Quote> quote(simple);
            Period term = oiSwap[i].n * oiSwap[i].unit;
            ext::shared_ptr<RateHelper> helper(new OISRateHelper(oiSwap[i].settlementDays, term, Handle<Quote>(quote), eonia));
            eoniaHelpers.push_back(helper);
        }

        return ext::shared_ptr<PiecewiseYieldCurve<Discount, Linear>>(new PiecewiseYieldCurve<Discount, Linear>(asOfDate, eoniaHelpers, Actual365Fixed()));

    }



    std::map<std::string, ext::shared_ptr<OptionletVolatilityStructure> > BootstrapCaplets(Date asOfDate,
        MarketData& md, MarketConventions& mc,
        VolatilityType volaType,
        ext::shared_ptr<YieldTermStructure>  discCurve,
        ext::shared_ptr<YieldTermStructure>  liborCurve)
    {

        ext::shared_ptr< IborIndex > iborIndex(new Euribor3M(Handle<YieldTermStructure>(liborCurve)));
        ext::shared_ptr<CapFloorTermVolSurface> capFloorVolRealSurface = md.mdContainer["EURCapFlatVolaNormal"];

        bool dontThrow = false;
        double accuracy = 0.000000001; Natural maxIter = 10000; Real shift = 0;
        Rate switchStrike = Null< Rate >();
        ext::shared_ptr< OptionletStripper1 > optionletStripper1(
            new OptionletStripper1(capFloorVolRealSurface, iborIndex, switchStrike, accuracy, maxIter,
                Handle<YieldTermStructure>(discCurve), volaType, shift, dontThrow));

        ext::shared_ptr< StrippedOptionletAdapter > strippedOptionletAdapter =
            boost::shared_ptr< StrippedOptionletAdapter >(
                new StrippedOptionletAdapter(optionletStripper1));

        Handle<OptionletVolatilityStructure> vol(strippedOptionletAdapter);
        std::map<std::string, ext::shared_ptr<OptionletVolatilityStructure> > retval;
        std::string name = std::string("CapletVola") + (volaType == Normal ? "Normal" : "LogNormal");
        retval.insert(make_pair(std::string("EURCapletVolaNormal"), vol.currentLink()));



        /*  Period f1 = optionletStripper1->optionletFixingTenors()[0];
          Period f2 = optionletStripper1->optionletFixingTenors()[1];
          Period f3 = optionletStripper1->optionletFixingTenors()[2];
          Period f4 = optionletStripper1->optionletFixingTenors()[3];
          Period f5 = optionletStripper1->optionletFixingTenors()[4];
          Period f6 = optionletStripper1->optionletFixingTenors()[5];
          Period f7 = optionletStripper1->optionletFixingTenors()[6];

          Date p1 = optionletStripper1->optionletPaymentDates()[0];
          Date p2 = optionletStripper1->optionletPaymentDates()[1];
          Date p3 = optionletStripper1->optionletPaymentDates()[2];
          Date p4 = optionletStripper1->optionletPaymentDates()[3];
          Date p5 = optionletStripper1->optionletPaymentDates()[4];
          Date p6 = optionletStripper1->optionletPaymentDates()[5];
          Date p7 = optionletStripper1->optionletPaymentDates()[6];

          Time ap1 = optionletStripper1->optionletAccrualPeriods()[0];
          Time ap2 = optionletStripper1->optionletAccrualPeriods()[1];
          Time ap3 = optionletStripper1->optionletAccrualPeriods()[2];
          Time ap4 = optionletStripper1->optionletAccrualPeriods()[3];
          Time ap5 = optionletStripper1->optionletAccrualPeriods()[4];
          Time ap6 = optionletStripper1->optionletAccrualPeriods()[5];
          Time ap7 = optionletStripper1->optionletAccrualPeriods()[6];

          Rate r1 = optionletStripper1->atmOptionletRates()[0];
          Rate r2 = optionletStripper1->atmOptionletRates()[1];
          Rate r3 = optionletStripper1->atmOptionletRates()[2];
          Rate r4 = optionletStripper1->atmOptionletRates()[3];
          Rate r5 = optionletStripper1->atmOptionletRates()[4];
          Rate r6 = optionletStripper1->atmOptionletRates()[5];
          Rate r7 = optionletStripper1->atmOptionletRates()[6];


          Real cpf1 = optionletStripper1->capFloorPrices()[0][0];
          Real cpf2 = optionletStripper1->capFloorPrices()[1][0];
          Real cpf3 = optionletStripper1->capFloorPrices()[2][0];
          Real cpf4 = optionletStripper1->capFloorPrices()[3][0];
          Real cpf5= optionletStripper1->capFloorPrices()[4][0];
          Real cpf6 = optionletStripper1->capFloorPrices()[5][0];
          Real cpf7 = optionletStripper1->capFloorPrices()[6][0];

          Volatility v1 = optionletStripper1->optionletVolatilities(0)[0];
          Volatility v2 = optionletStripper1->optionletVolatilities(1)[0];
          Volatility v3 = optionletStripper1->optionletVolatilities(2)[0];
          Volatility v4 = optionletStripper1->optionletVolatilities(3)[0];
          Volatility v5 = optionletStripper1->optionletVolatilities(4)[0];
          Volatility v6 = optionletStripper1->optionletVolatilities(5)[0];
          Volatility v7 = optionletStripper1->optionletVolatilities(6)[0];
    */

    /*Volatility vol1 = strippedOptionletAdapter->volatility(Period(6, Months), 0);
    Volatility vol2 = strippedOptionletAdapter->volatility(Period(9, Months), 0);
    Volatility vol3 = strippedOptionletAdapter->volatility(Period(12, Months), 0);*/

        return retval;

    }


}