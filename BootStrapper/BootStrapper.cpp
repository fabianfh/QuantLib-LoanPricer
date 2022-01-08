#include "BootStrapper.hpp"
#include <boost/shared_ptr.hpp>
#include <ql/termstructures/volatility/volatilitytype.hpp>
#include <string>




namespace BootStrapper {
  using namespace QuantLib;

  void printSwapRateHelper(SwapObs sd,MarketConventions mc)
  {
    Period tenor = sd.nIndexUnits * sd.indexUnit;
    Period term = sd.nTermUnits * sd.termUnit;

    std::cout << "SwapRateHelper created:" << std::endl
	      << "\t rate:\t" << 0.01 * sd.rate << std::endl 
	      << "\t term:\t" << term << std::endl
	      << "\t tenor:\t" << tenor << std::endl 
	      << "\t calendar:\t" << mc.calendar << std::endl
	      << "\t settlemend days:\t" << mc.settlementDays << std::endl ;
  
  }
  void printDepoHelper(DepoObs sd,MarketConventions mc)
  {
    
    Period term = sd.n * sd.unit;

    std::cout << "DepoRateHelper created:" << std::endl
	      << "\t rate:\t" << 0.01 * sd.rate << std::endl 
	      << "\t term:\t" << term << std::endl
	      << "\t calendar:\t" << mc.calendar << std::endl
	      << "\t settlemend days:\t" << mc.settlementDays << std::endl ;
  
  }

  void PrintCurve(boost::shared_ptr<PiecewiseFlatForward> curve)
  {
      std::vector<std::pair<Date,Real> > data = curve.get()->nodes();
      for (std::vector<std::pair<Date, Real> >::iterator it = data.begin(); it != data.end(); ++it) {
          std::cout << io::iso_date(it->first) << ": " << io::percent(it->second) << std::endl;
      }

      

  }

  boost::shared_ptr<PiecewiseFlatForward>  BootstrapLIBOR(Date asOfDate, MarketData& md, MarketConventions& mc, Handle<YieldTermStructure> discountCurve)
  {
      std::vector<boost::shared_ptr<RateHelper> > swap3mHelpers;
      boost::shared_ptr<IborIndex> euribor3m(new Euribor3M);


      for (Size i = 0; i < md.depositData.size(); i++) {
          Real rate =  md.depositData[i].rate;
          boost::shared_ptr<SimpleQuote> simple = boost::shared_ptr<SimpleQuote>(new SimpleQuote(rate));
          boost::shared_ptr<Quote> quote(simple);
          Period term = md.depositData[i].n * md.depositData[i].unit;
          boost::shared_ptr<RateHelper> helper(new DepositRateHelper(Handle<Quote>(quote), term, md.depositData[i].settlementDays, euribor3m->fixingCalendar(), euribor3m->businessDayConvention(), euribor3m->endOfMonth(), euribor3m->dayCounter()));

      }

      for (Size i = 0; i < md.swapData.size(); i++) {
          SimpleQuote quote( md.swapData[i].rate);
          Handle<Quote> quoteHandle(boost::make_shared<SimpleQuote>(quote));

          SimpleQuote spread(0);
          Handle<Quote> spreadHandle(boost::make_shared<SimpleQuote>(spread));

          Period tenor = md.swapData[i].nIndexUnits * md.swapData[i].indexUnit;
          Period term = md.swapData[i].nTermUnits * md.swapData[i].termUnit;

          Period  fwdStart = 0 * Days;
          Natural settlementDays = 0;
          bool useExternalDiscount = true;
          boost::shared_ptr<RateHelper> helper;
          if (useExternalDiscount)
          {
              boost::shared_ptr<RateHelper> helperWith(new SwapRateHelper(quoteHandle, term, mc.calendar, mc.fixedSwapFrequency, mc.fixedSwapConvention, mc.fixedSwapDayCount, euribor3m, spreadHandle, fwdStart, discountCurve, md.swapData[i].settlementDays));
              helper = helperWith;
          }
          else
          {
              boost::shared_ptr<RateHelper> helperWithout(new SwapRateHelper(quoteHandle, term, mc.calendar, mc.fixedSwapFrequency, mc.fixedSwapConvention, mc.fixedSwapDayCount, euribor3m, spreadHandle, fwdStart));
              helper = helperWithout;

          }


          if (tenor == 3 * Months)
          {
              swap3mHelpers.push_back(helper);
              //printSwapRateHelper(md.swapData[i], mc);

          }
      }


      return boost::shared_ptr<PiecewiseFlatForward>(new PiecewiseFlatForward(asOfDate, swap3mHelpers, Actual365Fixed()));
  }

  boost::shared_ptr<PiecewiseFlatForward> BootstrapOIS(Date asOfDate, MarketData& md, MarketConventions& mc)
  {

      std::vector<boost::shared_ptr<RateHelper> > eoniaHelpers;
      //std::vector<boost::shared_ptr<RateHelper> > swap3mHelpers;

      boost::shared_ptr<IborIndex> euribor3m(new Euribor3M);
      boost::shared_ptr<Eonia> eonia(new Eonia);

      for (Size i = 0; i < md.depositData.size(); i++) {
          Real rate = md.depositData[i].rate;
          boost::shared_ptr<SimpleQuote> simple = boost::shared_ptr<SimpleQuote>(new SimpleQuote(rate));
          boost::shared_ptr<Quote> quote(simple);
          Period term = md.depositData[i].n * md.depositData[i].unit;
          boost::shared_ptr<RateHelper> helper(new DepositRateHelper(Handle<Quote>(quote), term, md.depositData[i].settlementDays, euribor3m->fixingCalendar(), euribor3m->businessDayConvention(), euribor3m->endOfMonth(), euribor3m->dayCounter()));

          if (term <= 2 * Days)
              eoniaHelpers.push_back(helper);
          // if (term <= 3 * Months)
          // 	swap3mHelpers.push_back(helper);
      }


      for (Size i = 0; i < md.eoniaSwapData.size(); i++) {
          Real rate = md.eoniaSwapData[i].rate;
          boost::shared_ptr<SimpleQuote> simple = boost::shared_ptr<SimpleQuote>(new SimpleQuote(rate));
          boost::shared_ptr<Quote> quote(simple);
          Period term = md.eoniaSwapData[i].n * md.eoniaSwapData[i].unit;
          boost::shared_ptr<RateHelper> helper(new OISRateHelper(md.eoniaSwapData[i].settlementDays, term, Handle<Quote>(quote), eonia));
          eoniaHelpers.push_back(helper);
      }
      
      return boost::shared_ptr<PiecewiseFlatForward>(new PiecewiseFlatForward(asOfDate, eoniaHelpers, Actual365Fixed()));

  }
	
  std::map<std::string,boost::shared_ptr<PiecewiseFlatForward> > Bootstrap(Date asOfDate,MarketData& md,MarketConventions& mc)
  {
	
    
    std::map<std::string,boost::shared_ptr<PiecewiseFlatForward> > ret;
    boost::shared_ptr<PiecewiseFlatForward>  oisCurve =BootstrapOIS(asOfDate,md,mc);

    Handle<YieldTermStructure> oisCurveHandle(oisCurve);
    boost::shared_ptr<PiecewiseFlatForward>  liborCurve = BootstrapLIBOR(asOfDate, md, mc, oisCurveHandle);
	
    ret.insert(make_pair(std::string("OISwapEUR"),oisCurve));
    ret.insert(make_pair(std::string("Libor3MEUR"),liborCurve));
    
    return ret;
  }

  std::map<std::string, boost::shared_ptr<OptionletVolatilityStructure> > BootstrapCaplets(Date asOfDate,
	  MarketData& md, MarketConventions& mc,
	  VolatilityType volaType,
	  boost::shared_ptr<YieldTermStructure>  discCurve,
	  boost::shared_ptr<YieldTermStructure>  liborCurve)
  {

	  boost::shared_ptr< IborIndex > iborIndex(new Euribor3M(Handle<YieldTermStructure>(liborCurve)));
	  boost::shared_ptr<CapFloorTermVolSurface> capFloorVolRealSurface = md.mdContainer["EURCapFlatVolaNormal"];

	  bool dontThrow = false;
	  double accuracy = 0.000000001; Natural maxIter = 10000; Real shift = 0;
	  Rate switchStrike = Null< Rate >();
	  boost::shared_ptr< OptionletStripper1 > optionletStripper1(
		  new OptionletStripper1(capFloorVolRealSurface, iborIndex, switchStrike, accuracy, maxIter,
			  Handle<YieldTermStructure>(discCurve), volaType, shift, dontThrow));

	  boost::shared_ptr< StrippedOptionletAdapter > strippedOptionletAdapter =
		  boost::shared_ptr< StrippedOptionletAdapter >(
			  new StrippedOptionletAdapter(optionletStripper1));

	  Handle<OptionletVolatilityStructure> vol(strippedOptionletAdapter);
	  std::map<std::string, boost::shared_ptr<OptionletVolatilityStructure> > retval;
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
