/*#include "OISwapTest.hpp"
#include "LiborSwapTest.hpp"
#include "BootStrapper.hpp"
#include "optionletstripper.hpp"
#include "CapletStripperTest.hpp"
#include "Loans.hpp"
#include <ql/quantlib.hpp>
#include <ql/termstructures/volatility/volatilitytype.hpp>
#include <ql/termstructures/yield/zeroyieldstructure.hpp>
#include <ql/math/interpolations/all.hpp>*/

#include <iostream>
#include <ql/quantlib.hpp>
#include "Market/MarketData.hpp"
#include "Market/MarketConvention.hpp"
#include "BootStrapper/BootStrapper.hpp"
#include "Test/LiborSwapTest.hpp"
#include "Test/OISwapTest.hpp"

//using namespace QuantLib;
//using namespace std;

#pragma warning(suppress : 4996).
int main(int size, char *[])
{
	std::cout << size << std::endl;
	//std::cout << args << std::endl;

	QuantLib::Calendar calendar = QuantLib::TARGET();
	QuantLib::Date asOfDate = calendar.adjust(QuantLib::Date::todaysDate());
	std::cout << "Evaluation Date: " << QuantLib::io::iso_date(asOfDate) << std::endl;
	QuantLib::Settings::instance().evaluationDate() = asOfDate;

	BootStrapper::MarketConventions mc;
	BootStrapper::MarketData md;
	std::map<std::string, boost::shared_ptr<BootStrapper::PiecewiseFlatForward> > yieldCurves = Bootstrap(asOfDate, md, mc);
	boost::shared_ptr<BootStrapper::PiecewiseFlatForward> discCurve = yieldCurves["OISwapEUR"];
	boost::shared_ptr<BootStrapper::PiecewiseFlatForward> fwdCurve = yieldCurves["Libor3MEUR"];

	
	bool success = true;
	success &= BootStrapper::OISwapTest::testBootstrap(discCurve, mc, md, md.tolerance);
	success &= BootStrapper::LiborSwapTest::testBootstrap(discCurve, fwdCurve, mc, md, md.tolerance);	
/*
	boost::shared_ptr<QuantLib::YieldTermStructure> singleCurve = md.mdContainer2["EURZeroYields"];
	std::map<std::string, boost::shared_ptr<QuantLib::OptionletVolatilityStructure> > vols = BootStrapper::BootstrapCaplets(asOfDate, md, mc,
			QuantLib::Normal, discCurve, fwdCurve);
	boost::shared_ptr<QuantLib::OptionletVolatilityStructure> volCaplet = vols["EURCapletVolaNormal"];
	success &= BootStrapper::CapletStripperTest::testBootstrap(discCurve, fwdCurve, volCaplet, BootStrapper::Normal, mc, md, md.tolerance);
	*/

	//BootStrapper::OptionletStripperTest::testTermVolatilityStripping2(md, mc, asOfDate, discCurve, fwdCurve, volCaplet);

	//BootStrapper::OptionletStripperTest::testTermVolatilityStripping1(md,mc,asOfDate);
	//BootStrapper::OptionletStripperTest::testTermVolatilityStripping2();
	//BootStrapper::OptionletStripperTest::testSwitchStrike();
	//BootStrapper::OptionletStripperTest::testTermVolatilityStrippingNormalVol(md, mc, asOfDate,discCurve,fwdCurve, volCaplet);

	//std::map<std::string, boost::shared_ptr<QuantLib::OptionletVolatilityStructure> > vols = BootStrapper::BootstrapCaplets(asOfDate, md, mc,
	//QuantLib::Normal, discCurve, fwdCurve);
	//  boost::shared_ptr<QuantLib::OptionletVolatilityStructure> volCaplet = vols["EURCapletVolaNormal"];

	/*
		boost::shared_ptr<QuantLib::YieldTermStructure> singleCurve = md.mdContainer2["EURZeroYields"];
	  std::map<std::string, boost::shared_ptr<QuantLib::OptionletVolatilityStructure> > vols = BootStrapper::BootstrapCaplets(asOfDate, md, mc,
		  QuantLib::Normal, singleCurve, singleCurve);
		  boost::shared_ptr<QuantLib::OptionletVolatilityStructure> volCaplet = vols["EURCapletVolaNormal"];*/

	//BootStrapper::Loans::testLoan(asOfDate, calendar.advance(asOfDate, QuantLib::Period(10, QuantLib::Years)),QuantLib::Quarterly);
	std::cout << success << std::endl;
	return 0;
}

//void printSchedule(Schedule s)
//{
//	BOOST_FOREACH(Date d, s.dates())  cout << "\t"<< io::iso_date(d) << std::endl;
//}

//std::vector<boost::shared_ptr<RateHelper> > getFRAHelpers(FRAQuote*  quotes,int numQuotes, Natural fixingDays, const Calendar& calendar, BusinessDayConvention convention, bool endOfMonth, const DayCounter& dayCounter)
//{
//
//
//	std::vector<boost::shared_ptr<RateHelper> > fraInstruments;
//	for (int i = 0; i < numQuotes; i++)	{
//
//		boost::shared_ptr<SimpleQuote> fra1x4Rate(
//			new SimpleQuote(quotes[i].quote));
//
//		RelinkableHandle<Quote> h1x4;  h1x4.linkTo(fra1x4Rate);
//		boost::shared_ptr<RateHelper> fra1x4(
//			new FraRateHelper(h1x4, quotes[i].monthToStart, quotes[i].monthLength,
//			fixingDays, calendar, convention,
//			endOfMonth, dayCounter));
//
//		fraInstruments.push_back(fra1x4);
//	}
//
//	return fraInstruments;
//}

//std::vector<boost::shared_ptr<RateHelper> > getOISHelpers(OISQuote*  quotes, int numQuotes, Natural settlementDays, boost::shared_ptr<OvernightIndex> index)
//{
//
//	std::vector<boost::shared_ptr<RateHelper> > oisInstruments;
//	for (int i = 0; i < numQuotes; i++)	{
//
//		boost::shared_ptr<SimpleQuote> rate(new SimpleQuote(quotes[i].quote));
//		RelinkableHandle<Quote> handle;  handle.linkTo(rate);
//		boost::shared_ptr<RateHelper> helper = boost::make_shared<OISRateHelper>(settlementDays, quotes[i].tenor, handle,	index);
//
//
//		oisInstruments.push_back(helper);
//	}
//
//	return oisInstruments;
//};

//int RoundtripFRA()
//{
//	boost::timer timer;
//	std::cout << std::endl;
//
//	// 3 month term FRA quotes (index refers to monthsToStart)
//	FRAQuote quotes[5];
//	quotes[0].monthToStart = 1;
//	quotes[0].monthLength = 4;
//	quotes[0].quote = 0.030;
//	quotes[1].monthToStart = 2;
//	quotes[1].monthLength = 5;
//	quotes[1].quote = 0.031;
//	quotes[2].monthToStart = 3;
//	quotes[2].monthLength = 6;
//	quotes[2].quote = 0.032;
//	quotes[3].monthToStart = 6;
//	quotes[3].monthLength = 9;
//	quotes[3].quote = 0.033;
//	quotes[4].monthToStart = 9;
//	quotes[4].monthLength = 12;
//	quotes[4].quote = 0.034;
//
//	Date todaysDate = Settings::instance().evaluationDate();
//
//
//	RelinkableHandle<YieldTermStructure> euriborTermStructure;
//	boost::shared_ptr<IborIndex> euribor3m(new Euribor3M(euriborTermStructure));
//	DayCounter fraDayCounter = euribor3m->dayCounter();
//	BusinessDayConvention convention = euribor3m->businessDayConvention();
//	bool endOfMonth = euribor3m->endOfMonth();
//	Calendar calendar = euribor3m->fixingCalendar();
//	Integer fixingDays = euribor3m->fixingDays();
//	Date settlementDate = calendar.advance(todaysDate, fixingDays, Days);
//
//	std::vector<boost::shared_ptr<RateHelper> > fraInstruments = getFRAHelpers(quotes,LENGTH(quotes), fixingDays, calendar, convention, endOfMonth, fraDayCounter);
//
//	DayCounter termStructureDayCounter = ActualActual(ActualActual::ISDA);
//
//	double tolerance = 1.0e-15;
//	boost::shared_ptr<YieldTermStructure> fraTermStructure(
//		new PiecewiseYieldCurve<Discount, LogLinear>(
//		settlementDate, fraInstruments,
//		termStructureDayCounter,
//		tolerance));
//
//
//	// Term structures used for pricing/discounting
//
//	RelinkableHandle<YieldTermStructure> discountingTermStructure;
//	discountingTermStructure.linkTo(fraTermStructure);
//
//
//	/***********************
//	***  construct FRA's ***
//	***********************/
//
//	Calendar fraCalendar = euribor3m->fixingCalendar();
//	BusinessDayConvention fraBusinessDayConvention =
//		euribor3m->businessDayConvention();
//	Position::Type fraFwdType = Position::Long;
//	Real fraNotional = 100.0;
//
//
//	euriborTermStructure.linkTo(fraTermStructure);
//
//	cout << endl;
//	cout << "Test FRA construction, NPV calculation, and FRA purchase"
//		<< endl
//		<< endl;
//
//	Size i;
//	for (i = 0; i < LENGTH(quotes); i++) {
//
//		Date fraValueDate = fraCalendar.advance(
//			settlementDate, quotes[i].monthToStart, Months,
//			fraBusinessDayConvention);
//
//		Date fraMaturityDate = fraCalendar.advance(
//			fraValueDate, quotes[i].monthLength - quotes[i].monthToStart, Months,
//			fraBusinessDayConvention);
//
//
//
//		ForwardRateAgreement myFRA(fraValueDate, fraMaturityDate,
//			fraFwdType, quotes[i].quote,
//			fraNotional, euribor3m,
//			discountingTermStructure);
//
//		cout << "FRA " << quotes[i].monthToStart << "x" << quotes[i].monthLength << " quoted rate: " << io::rate(quotes[i].quote) << " NPV(K=" << io::rate(quotes[i].quote) << "): " << myFRA.NPV() << endl;
//	}
//	return 1;
//};

//	int RoundtripOIS()
//	{
//		boost::timer timer;
//		std::cout << std::endl;
//
//		// 3 month term FRA quotes (index refers to monthsToStart)
//		OISQuote quotes[14];
//		quotes[0].tenor = 1 * Weeks;
//		quotes[0].quote = 0.030;
//		quotes[1].tenor = 2 * Weeks;
//		quotes[1].quote = 0.031;
//		quotes[2].tenor = 1 * Months;
//		quotes[2].quote = 0.032;
//		quotes[3].tenor = 2 * Months;
//		quotes[3].quote = 0.033;
//		quotes[4].tenor = 3 * Months;
//		quotes[4].quote = 0.034;
//		quotes[5].tenor = 6 * Months;
//		quotes[5].quote = 0.035;
//		quotes[6].tenor = 9 * Months;
//		quotes[6].quote = 0.036;
//		quotes[7].tenor = 12 * Months;
//		quotes[7].quote = 0.038;
//		quotes[8].tenor = 18 * Months;
//		quotes[8].quote = 0.0386;
//		quotes[9].tenor = 2 * Years;
//		quotes[9].quote = 0.039;
//		quotes[10].tenor = 3 * Years;
//		quotes[10].quote = 0.0393;
//		quotes[11].tenor = 4 * Years;
//		quotes[11].quote = 0.0396;
//		quotes[12].tenor = 5 * Years;
//		quotes[12].quote = 0.040;
//		quotes[13].tenor = 10 * Years;
//		quotes[13].quote = 0.045;
//		Date todaysDate = Settings::instance().evaluationDate();
//
//
//		RelinkableHandle<YieldTermStructure> dummy;
//		boost::shared_ptr<OvernightIndex> eoniaForBootstrapping(new Eonia(dummy));
//		Calendar calendar = eoniaForBootstrapping->fixingCalendar();
//
//
//		Eonia index();
//		std::vector<boost::shared_ptr<RateHelper> > oisInstruments = getOISHelpers(quotes, LENGTH(quotes), eoniaForBootstrapping->fixingDays(), eoniaForBootstrapping);
//		double tolerance = 1.0e-15;
//		Integer settelmentDays = eoniaForBootstrapping->fixingDays();
//		Date settlementDate = calendar.advance(todaysDate, settelmentDays, Days);
//		boost::shared_ptr<YieldTermStructure> ytsEonia(new PiecewiseYieldCurve<Discount, LogLinear>(settelmentDays, TARGET(), oisInstruments, Actual365Fixed()));
//
//
//
//
//		// Term structures that will be used for pricing:
//		// the one used for discounting cash flows
//		RelinkableHandle<YieldTermStructure> discountingTermStructure;
//		discountingTermStructure.linkTo(ytsEonia);
//		// the one used for forward rate forecasting
//		RelinkableHandle<YieldTermStructure> forecastingTermStructure;
//		// Of course, you're not forced to really use different curves
//		forecastingTermStructure.linkTo(ytsEonia);
//
//		boost::shared_ptr<PricingEngine> swapEngine(new DiscountingSwapEngine(discountingTermStructure));
//		boost::shared_ptr<OvernightIndex> eoniaForPricing(new Eonia(forecastingTermStructure));
//
//		for (Size i = 0; i < LENGTH(quotes); i++)
//		{
//			Date endDate = calendar.advance(settlementDate, quotes[i].tenor, eoniaForBootstrapping->businessDayConvention());
//			Real notional = 10000000; Real spread = 0;
//			Schedule schedule = MakeSchedule().from(settlementDate).to(endDate).withFrequency(Annual);
//			OvernightIndexedSwap::Type type = OvernightIndexedSwap::Type::Receiver;
//			DayCounter fixedDC = eoniaForBootstrapping->dayCounter();
//			OvernightIndexedSwap myOiSwap(type, notional, schedule, quotes[i].quote, fixedDC, eoniaForPricing, spread);
//			myOiSwap.setPricingEngine(swapEngine);
//
//			cout << "OISwap " << quotes[i].tenor << ":  quoted rate: " << io::rate(quotes[i].quote) << "\tNPV(K=" << io::rate(quotes[i].quote) << "): " << myOiSwap.NPV() << endl;
//			printOISwap(schedule,myOiSwap);
//		}
//
//	return  1;
//}

////int RoundtripDepo()
////{
////	Calendar calendar = TARGET();
////	Date settlementDate(22, September, 2004);
////	// must be a business day
////	settlementDate = calendar.adjust(settlementDate);
////	Integer fixingDays = 2;
////	Date todaysDate = calendar.advance(settlementDate, -fixingDays, Days);
////	// nothing to do with Date::todaysDate
////	Settings::instance().evaluationDate() = todaysDate;
////
////	DayCounter depositDayCounter = Actual360();
////
////	DayCounter termStructureDayCounter =
////		ActualActual(ActualActual::ISDA);
////
////
////	Rate d1yQuote = 0.05;
////	boost::shared_ptr<Quote> d1yRate(new SimpleQuote(d1yQuote));
////	boost::shared_ptr<RateHelper> d1y(new DepositRateHelper(
////		Handle<Quote>(d1yRate),
////		1 * Years, fixingDays,
////		calendar, ModifiedFollowing,
////		true, depositDayCounter));
////
////	std::vector<boost::shared_ptr<RateHelper> > depoSwapInstruments;
////	depoSwapInstruments.push_back(d1y);
////
////	double tolerance = 1.0e-15;
////	boost::shared_ptr<YieldTermStructure> depoSwapTermStructure(
////		new PiecewiseYieldCurve<Discount, LogLinear>(
////		settlementDate, depoSwapInstruments,
////		termStructureDayCounter,
////		tolerance));
////
////
////	return 1;
////}

//int Example() {
//
//	try {
//
//		boost::timer timer;
//		std::cout << std::endl;
//
//		/*********************
//		***  MARKET DATA  ***
//		*********************/
//
//		Calendar calendar = TARGET();
//		Date settlementDate(22, September, 2004);
//		// must be a business day
//		settlementDate = calendar.adjust(settlementDate);
//
//		Integer fixingDays = 2;
//		Date todaysDate = calendar.advance(settlementDate, -fixingDays, Days);
//		// nothing to do with Date::todaysDate
//		Settings::instance().evaluationDate() = todaysDate;
//
//
//		todaysDate = Settings::instance().evaluationDate();
//		std::cout << "Today: " << todaysDate.weekday()
//			<< ", " << todaysDate << std::endl;
//
//		std::cout << "Settlement date: " << settlementDate.weekday()
//			<< ", " << settlementDate << std::endl;
//
//		// deposits
//		Rate d1wQuote = 0.0382;
//		Rate d1mQuote = 0.0372;
//		Rate d3mQuote = 0.0363;
//		Rate d6mQuote = 0.0353;
//		Rate d9mQuote = 0.0348;
//		Rate d1yQuote = 0.0345;
//		// FRAs
//		Rate fra3x6Quote = 0.037125;
//		Rate fra6x9Quote = 0.037125;
//		Rate fra6x12Quote = 0.037125;
//		// futures
//		Real fut1Quote = 96.2875;
//		Real fut2Quote = 96.7875;
//		Real fut3Quote = 96.9875;
//		Real fut4Quote = 96.6875;
//		Real fut5Quote = 96.4875;
//		Real fut6Quote = 96.3875;
//		Real fut7Quote = 96.2875;
//		Real fut8Quote = 96.0875;
//		// swaps
//		Rate s2yQuote = 0.037125;
//		Rate s3yQuote = 0.0398;
//		Rate s5yQuote = 0.0443;
//		Rate s10yQuote = 0.05165;
//		Rate s15yQuote = 0.055175;
//
//
//		/********************
//		***    QUOTES    ***
//		********************/
//
//		// SimpleQuote stores a value which can be manually changed;
//		// other Quote subclasses could read the value from a database
//		// or some kind of data feed.
//
//		// deposits
//		boost::shared_ptr<Quote> d1wRate(new SimpleQuote(d1wQuote));
//		boost::shared_ptr<Quote> d1mRate(new SimpleQuote(d1mQuote));
//		boost::shared_ptr<Quote> d3mRate(new SimpleQuote(d3mQuote));
//		boost::shared_ptr<Quote> d6mRate(new SimpleQuote(d6mQuote));
//		boost::shared_ptr<Quote> d9mRate(new SimpleQuote(d9mQuote));
//		boost::shared_ptr<Quote> d1yRate(new SimpleQuote(d1yQuote));
//		// FRAs
//		boost::shared_ptr<Quote> fra3x6Rate(new SimpleQuote(fra3x6Quote));
//		boost::shared_ptr<Quote> fra6x9Rate(new SimpleQuote(fra6x9Quote));
//		boost::shared_ptr<Quote> fra6x12Rate(new SimpleQuote(fra6x12Quote));
//		// futures
//		boost::shared_ptr<Quote> fut1Price(new SimpleQuote(fut1Quote));
//		boost::shared_ptr<Quote> fut2Price(new SimpleQuote(fut2Quote));
//		boost::shared_ptr<Quote> fut3Price(new SimpleQuote(fut3Quote));
//		boost::shared_ptr<Quote> fut4Price(new SimpleQuote(fut4Quote));
//		boost::shared_ptr<Quote> fut5Price(new SimpleQuote(fut5Quote));
//		boost::shared_ptr<Quote> fut6Price(new SimpleQuote(fut6Quote));
//		boost::shared_ptr<Quote> fut7Price(new SimpleQuote(fut7Quote));
//		boost::shared_ptr<Quote> fut8Price(new SimpleQuote(fut8Quote));
//		// swaps
//		boost::shared_ptr<Quote> s2yRate(new SimpleQuote(s2yQuote));
//		boost::shared_ptr<Quote> s3yRate(new SimpleQuote(s3yQuote));
//		boost::shared_ptr<Quote> s5yRate(new SimpleQuote(s5yQuote));
//		boost::shared_ptr<Quote> s10yRate(new SimpleQuote(s10yQuote));
//		boost::shared_ptr<Quote> s15yRate(new SimpleQuote(s15yQuote));
//
//
//		/*********************
//		***  RATE HELPERS ***
//		*********************/
//
//		// RateHelpers are built from the above quotes together with
//		// other instrument dependant infos.  Quotes are passed in
//		// relinkable handles which could be relinked to some other
//		// data source later.
//
//		// deposits
//		DayCounter depositDayCounter = Actual360();
//
//		boost::shared_ptr<RateHelper> d1w(new DepositRateHelper(
//			Handle<Quote>(d1wRate),
//			1 * Weeks, fixingDays,
//			calendar, ModifiedFollowing,
//			true, depositDayCounter));
//		boost::shared_ptr<RateHelper> d1m(new DepositRateHelper(
//			Handle<Quote>(d1mRate),
//			1 * Months, fixingDays,
//			calendar, ModifiedFollowing,
//			true, depositDayCounter));
//		boost::shared_ptr<RateHelper> d3m(new DepositRateHelper(
//			Handle<Quote>(d3mRate),
//			3 * Months, fixingDays,
//			calendar, ModifiedFollowing,
//			true, depositDayCounter));
//		boost::shared_ptr<RateHelper> d6m(new DepositRateHelper(
//			Handle<Quote>(d6mRate),
//			6 * Months, fixingDays,
//			calendar, ModifiedFollowing,
//			true, depositDayCounter));
//		boost::shared_ptr<RateHelper> d9m(new DepositRateHelper(
//			Handle<Quote>(d9mRate),
//			9 * Months, fixingDays,
//			calendar, ModifiedFollowing,
//			true, depositDayCounter));
//		boost::shared_ptr<RateHelper> d1y(new DepositRateHelper(
//			Handle<Quote>(d1yRate),
//			1 * Years, fixingDays,
//			calendar, ModifiedFollowing,
//			true, depositDayCounter));
//
//
//		// setup FRAs
//		boost::shared_ptr<RateHelper> fra3x6(new FraRateHelper(
//			Handle<Quote>(fra3x6Rate),
//			3, 6, fixingDays, calendar, ModifiedFollowing,
//			true, depositDayCounter));
//		boost::shared_ptr<RateHelper> fra6x9(new FraRateHelper(
//			Handle<Quote>(fra6x9Rate),
//			6, 9, fixingDays, calendar, ModifiedFollowing,
//			true, depositDayCounter));
//		boost::shared_ptr<RateHelper> fra6x12(new FraRateHelper(
//			Handle<Quote>(fra6x12Rate),
//			6, 12, fixingDays, calendar, ModifiedFollowing,
//			true, depositDayCounter));
//
//
//		// setup futures
//		// Rate convexityAdjustment = 0.0;
//		Integer futMonths = 3;
//		Date imm = IMM::nextDate(settlementDate);
//		boost::shared_ptr<RateHelper> fut1(new FuturesRateHelper(
//			Handle<Quote>(fut1Price),
//			imm,
//			futMonths, calendar, ModifiedFollowing,
//			true, depositDayCounter));
//		imm = IMM::nextDate(imm + 1);
//		boost::shared_ptr<RateHelper> fut2(new FuturesRateHelper(
//			Handle<Quote>(fut2Price),
//			imm,
//			futMonths, calendar, ModifiedFollowing,
//			true, depositDayCounter));
//		imm = IMM::nextDate(imm + 1);
//		boost::shared_ptr<RateHelper> fut3(new FuturesRateHelper(
//			Handle<Quote>(fut3Price),
//			imm,
//			futMonths, calendar, ModifiedFollowing,
//			true, depositDayCounter));
//		imm = IMM::nextDate(imm + 1);
//		boost::shared_ptr<RateHelper> fut4(new FuturesRateHelper(
//			Handle<Quote>(fut4Price),
//			imm,
//			futMonths, calendar, ModifiedFollowing,
//			true, depositDayCounter));
//		imm = IMM::nextDate(imm + 1);
//		boost::shared_ptr<RateHelper> fut5(new FuturesRateHelper(
//			Handle<Quote>(fut5Price),
//			imm,
//			futMonths, calendar, ModifiedFollowing,
//			true, depositDayCounter));
//		imm = IMM::nextDate(imm + 1);
//		boost::shared_ptr<RateHelper> fut6(new FuturesRateHelper(
//			Handle<Quote>(fut6Price),
//			imm,
//			futMonths, calendar, ModifiedFollowing,
//			true, depositDayCounter));
//		imm = IMM::nextDate(imm + 1);
//		boost::shared_ptr<RateHelper> fut7(new FuturesRateHelper(
//			Handle<Quote>(fut7Price),
//			imm,
//			futMonths, calendar, ModifiedFollowing,
//			true, depositDayCounter));
//		imm = IMM::nextDate(imm + 1);
//		boost::shared_ptr<RateHelper> fut8(new FuturesRateHelper(
//			Handle<Quote>(fut8Price),
//			imm,
//			futMonths, calendar, ModifiedFollowing,
//			true, depositDayCounter));
//
//
//		// setup swaps
//		Frequency swFixedLegFrequency = Annual;
//		BusinessDayConvention swFixedLegConvention = Unadjusted;
//		DayCounter swFixedLegDayCounter = Thirty360(Thirty360::European);
//		boost::shared_ptr<IborIndex> swFloatingLegIndex(new Euribor6M);
//
//		boost::shared_ptr<RateHelper> s2y(new SwapRateHelper(
//			Handle<Quote>(s2yRate), 2 * Years,
//			calendar, swFixedLegFrequency,
//			swFixedLegConvention, swFixedLegDayCounter,
//			swFloatingLegIndex));
//		boost::shared_ptr<RateHelper> s3y(new SwapRateHelper(
//			Handle<Quote>(s3yRate), 3 * Years,
//			calendar, swFixedLegFrequency,
//			swFixedLegConvention, swFixedLegDayCounter,
//			swFloatingLegIndex));
//		boost::shared_ptr<RateHelper> s5y(new SwapRateHelper(
//			Handle<Quote>(s5yRate), 5 * Years,
//			calendar, swFixedLegFrequency,
//			swFixedLegConvention, swFixedLegDayCounter,
//			swFloatingLegIndex));
//		boost::shared_ptr<RateHelper> s10y(new SwapRateHelper(
//			Handle<Quote>(s10yRate), 10 * Years,
//			calendar, swFixedLegFrequency,
//			swFixedLegConvention, swFixedLegDayCounter,
//			swFloatingLegIndex));
//		boost::shared_ptr<RateHelper> s15y(new SwapRateHelper(
//			Handle<Quote>(s15yRate), 15 * Years,
//			calendar, swFixedLegFrequency,
//			swFixedLegConvention, swFixedLegDayCounter,
//			swFloatingLegIndex));
//
//
//		/*********************
//		**  CURVE BUILDING **
//		*********************/
//
//		// Any DayCounter would be fine.
//		// ActualActual::ISDA ensures that 30 years is 30.0
//		DayCounter termStructureDayCounter =
//			ActualActual(ActualActual::ISDA);
//
//
//		double tolerance = 1.0e-15;
//
//		// A depo-swap curve
//		std::vector<boost::shared_ptr<RateHelper> > depoSwapInstruments;
//		depoSwapInstruments.push_back(d1w);
//		depoSwapInstruments.push_back(d1m);
//		depoSwapInstruments.push_back(d3m);
//		depoSwapInstruments.push_back(d6m);
//		depoSwapInstruments.push_back(d9m);
//		depoSwapInstruments.push_back(d1y);
//		depoSwapInstruments.push_back(s2y);
//		depoSwapInstruments.push_back(s3y);
//		depoSwapInstruments.push_back(s5y);
//		depoSwapInstruments.push_back(s10y);
//		depoSwapInstruments.push_back(s15y);
//		boost::shared_ptr<YieldTermStructure> depoSwapTermStructure(
//			new PiecewiseYieldCurve<Discount, LogLinear>(
//			settlementDate, depoSwapInstruments,
//			termStructureDayCounter,
//			tolerance));
//
//
//		// A depo-futures-swap curve
//		std::vector<boost::shared_ptr<RateHelper> > depoFutSwapInstruments;
//		depoFutSwapInstruments.push_back(d1w);
//		depoFutSwapInstruments.push_back(d1m);
//		depoFutSwapInstruments.push_back(fut1);
//		depoFutSwapInstruments.push_back(fut2);
//		depoFutSwapInstruments.push_back(fut3);
//		depoFutSwapInstruments.push_back(fut4);
//		depoFutSwapInstruments.push_back(fut5);
//		depoFutSwapInstruments.push_back(fut6);
//		depoFutSwapInstruments.push_back(fut7);
//		depoFutSwapInstruments.push_back(fut8);
//		depoFutSwapInstruments.push_back(s3y);
//		depoFutSwapInstruments.push_back(s5y);
//		depoFutSwapInstruments.push_back(s10y);
//		depoFutSwapInstruments.push_back(s15y);
//		boost::shared_ptr<YieldTermStructure> depoFutSwapTermStructure(
//			new PiecewiseYieldCurve<Discount, LogLinear>(
//			settlementDate, depoFutSwapInstruments,
//			termStructureDayCounter,
//			tolerance));
//
//
//		// A depo-FRA-swap curve
//		std::vector<boost::shared_ptr<RateHelper> > depoFRASwapInstruments;
//		depoFRASwapInstruments.push_back(d1w);
//		depoFRASwapInstruments.push_back(d1m);
//		depoFRASwapInstruments.push_back(d3m);
//		depoFRASwapInstruments.push_back(fra3x6);
//		depoFRASwapInstruments.push_back(fra6x9);
//		depoFRASwapInstruments.push_back(fra6x12);
//		depoFRASwapInstruments.push_back(s2y);
//		depoFRASwapInstruments.push_back(s3y);
//		depoFRASwapInstruments.push_back(s5y);
//		depoFRASwapInstruments.push_back(s10y);
//		depoFRASwapInstruments.push_back(s15y);
//		boost::shared_ptr<YieldTermStructure> depoFRASwapTermStructure(
//			new PiecewiseYieldCurve<Discount, LogLinear>(
//			settlementDate, depoFRASwapInstruments,
//			termStructureDayCounter,
//			tolerance));
//
//
//		// Term structures that will be used for pricing:
//		// the one used for discounting cash flows
//		RelinkableHandle<YieldTermStructure> discountingTermStructure;
//		// the one used for forward rate forecasting
//		RelinkableHandle<YieldTermStructure> forecastingTermStructure;
//
//
//		/*********************
//		* SWAPS TO BE PRICED *
//		**********************/
//
//		// constant nominal 1,000,000 Euro
//		Real nominal = 1000000.0;
//		// fixed leg
//		Frequency fixedLegFrequency = Annual;
//		BusinessDayConvention fixedLegConvention = Unadjusted;
//		BusinessDayConvention floatingLegConvention = ModifiedFollowing;
//		DayCounter fixedLegDayCounter = Thirty360(Thirty360::European);
//		Rate fixedRate = 0.04;
//		DayCounter floatingLegDayCounter = Actual360();
//
//		// floating leg
//		Frequency floatingLegFrequency = Semiannual;
//		boost::shared_ptr<IborIndex> euriborIndex(
//			new Euribor6M(forecastingTermStructure));
//		Spread spread = 0.0;
//
//		Integer lenghtInYears = 5;
//		VanillaSwap::Type swapType = VanillaSwap::Payer;
//
//		Date maturity = settlementDate + lenghtInYears*Years;
//		Schedule fixedSchedule(settlementDate, maturity,
//			Period(fixedLegFrequency),
//			calendar, fixedLegConvention,
//			fixedLegConvention,
//			DateGeneration::Forward, false);
//		Schedule floatSchedule(settlementDate, maturity,
//			Period(floatingLegFrequency),
//			calendar, floatingLegConvention,
//			floatingLegConvention,
//			DateGeneration::Forward, false);
//		VanillaSwap spot5YearSwap(swapType, nominal,
//			fixedSchedule, fixedRate, fixedLegDayCounter,
//			floatSchedule, euriborIndex, spread,
//			floatingLegDayCounter);
//
//		Date fwdStart = calendar.advance(settlementDate, 1, Years);
//		Date fwdMaturity = fwdStart + lenghtInYears*Years;
//		Schedule fwdFixedSchedule(fwdStart, fwdMaturity,
//			Period(fixedLegFrequency),
//			calendar, fixedLegConvention,
//			fixedLegConvention,
//			DateGeneration::Forward, false);
//		Schedule fwdFloatSchedule(fwdStart, fwdMaturity,
//			Period(floatingLegFrequency),
//			calendar, floatingLegConvention,
//			floatingLegConvention,
//			DateGeneration::Forward, false);
//		VanillaSwap oneYearForward5YearSwap(swapType, nominal,
//			fwdFixedSchedule, fixedRate, fixedLegDayCounter,
//			fwdFloatSchedule, euriborIndex, spread,
//			floatingLegDayCounter);
//
//
//		/***************
//		* SWAP PRICING *
//		****************/
//
//		// utilities for reporting
//		std::vector<std::string> headers(4);
//		headers[0] = "term structure";
//		headers[1] = "net present value";
//		headers[2] = "fair spread";
//		headers[3] = "fair fixed rate";
//		std::string separator = " | ";
//		Size width = headers[0].size() + separator.size()
//			+ headers[1].size() + separator.size()
//			+ headers[2].size() + separator.size()
//			+ headers[3].size() + separator.size() - 1;
//		std::string rule(width, '-'), dblrule(width, '=');
//		std::string tab(8, ' ');
//
//		// calculations
//		std::cout << dblrule << std::endl;
//		std::cout << "5-year market swap-rate = "
//			<< std::setprecision(2) << io::rate(s5yRate->value())
//			<< std::endl;
//		std::cout << dblrule << std::endl;
//
//		std::cout << tab << "5-years swap paying "
//			<< io::rate(fixedRate) << std::endl;
//		std::cout << headers[0] << separator
//			<< headers[1] << separator
//			<< headers[2] << separator
//			<< headers[3] << separator << std::endl;
//		std::cout << rule << std::endl;
//
//		Real NPV;
//		Rate fairRate;
//		Spread fairSpread;
//
//		boost::shared_ptr<PricingEngine> swapEngine(
//			new DiscountingSwapEngine(discountingTermStructure));
//
//		spot5YearSwap.setPricingEngine(swapEngine);
//		oneYearForward5YearSwap.setPricingEngine(swapEngine);
//
//		// Of course, you're not forced to really use different curves
//		forecastingTermStructure.linkTo(depoSwapTermStructure);
//		discountingTermStructure.linkTo(depoSwapTermStructure);
//
//		NPV = spot5YearSwap.NPV();
//		fairSpread = spot5YearSwap.fairSpread();
//		fairRate = spot5YearSwap.fairRate();
//
//		std::cout << std::setw(headers[0].size())
//			<< "depo-swap" << separator;
//		std::cout << std::setw(headers[1].size())
//			<< std::fixed << std::setprecision(2) << NPV << separator;
//		std::cout << std::setw(headers[2].size())
//			<< io::rate(fairSpread) << separator;
//		std::cout << std::setw(headers[3].size())
//			<< io::rate(fairRate) << separator;
//		std::cout << std::endl;
//
//
//		// let's check that the 5 years swap has been correctly re-priced
//		QL_REQUIRE(std::fabs(fairRate - s5yQuote)<1e-8,
//			"5-years swap mispriced by "
//			<< io::rate(std::fabs(fairRate - s5yQuote)));
//
//
//		forecastingTermStructure.linkTo(depoFutSwapTermStructure);
//		discountingTermStructure.linkTo(depoFutSwapTermStructure);
//
//		NPV = spot5YearSwap.NPV();
//		fairSpread = spot5YearSwap.fairSpread();
//		fairRate = spot5YearSwap.fairRate();
//
//		std::cout << std::setw(headers[0].size())
//			<< "depo-fut-swap" << separator;
//		std::cout << std::setw(headers[1].size())
//			<< std::fixed << std::setprecision(2) << NPV << separator;
//		std::cout << std::setw(headers[2].size())
//			<< io::rate(fairSpread) << separator;
//		std::cout << std::setw(headers[3].size())
//			<< io::rate(fairRate) << separator;
//		std::cout << std::endl;
//
//		QL_REQUIRE(std::fabs(fairRate - s5yQuote)<1e-8,
//			"5-years swap mispriced!");
//
//
//		forecastingTermStructure.linkTo(depoFRASwapTermStructure);
//		discountingTermStructure.linkTo(depoFRASwapTermStructure);
//
//		NPV = spot5YearSwap.NPV();
//		fairSpread = spot5YearSwap.fairSpread();
//		fairRate = spot5YearSwap.fairRate();
//
//		std::cout << std::setw(headers[0].size())
//			<< "depo-FRA-swap" << separator;
//		std::cout << std::setw(headers[1].size())
//			<< std::fixed << std::setprecision(2) << NPV << separator;
//		std::cout << std::setw(headers[2].size())
//			<< io::rate(fairSpread) << separator;
//		std::cout << std::setw(headers[3].size())
//			<< io::rate(fairRate) << separator;
//		std::cout << std::endl;
//
//		QL_REQUIRE(std::fabs(fairRate - s5yQuote)<1e-8,
//			"5-years swap mispriced!");
//
//
//		std::cout << rule << std::endl;
//
//		// now let's price the 1Y forward 5Y swap
//
//		std::cout << tab << "5-years, 1-year forward swap paying "
//			<< io::rate(fixedRate) << std::endl;
//		std::cout << headers[0] << separator
//			<< headers[1] << separator
//			<< headers[2] << separator
//			<< headers[3] << separator << std::endl;
//		std::cout << rule << std::endl;
//
//
//		forecastingTermStructure.linkTo(depoSwapTermStructure);
//		discountingTermStructure.linkTo(depoSwapTermStructure);
//
//		NPV = oneYearForward5YearSwap.NPV();
//		fairSpread = oneYearForward5YearSwap.fairSpread();
//		fairRate = oneYearForward5YearSwap.fairRate();
//
//		std::cout << std::setw(headers[0].size())
//			<< "depo-swap" << separator;
//		std::cout << std::setw(headers[1].size())
//			<< std::fixed << std::setprecision(2) << NPV << separator;
//		std::cout << std::setw(headers[2].size())
//			<< io::rate(fairSpread) << separator;
//		std::cout << std::setw(headers[3].size())
//			<< io::rate(fairRate) << separator;
//		std::cout << std::endl;
//
//
//		forecastingTermStructure.linkTo(depoFutSwapTermStructure);
//		discountingTermStructure.linkTo(depoFutSwapTermStructure);
//
//		NPV = oneYearForward5YearSwap.NPV();
//		fairSpread = oneYearForward5YearSwap.fairSpread();
//		fairRate = oneYearForward5YearSwap.fairRate();
//
//		std::cout << std::setw(headers[0].size())
//			<< "depo-fut-swap" << separator;
//		std::cout << std::setw(headers[1].size())
//			<< std::fixed << std::setprecision(2) << NPV << separator;
//		std::cout << std::setw(headers[2].size())
//			<< io::rate(fairSpread) << separator;
//		std::cout << std::setw(headers[3].size())
//			<< io::rate(fairRate) << separator;
//		std::cout << std::endl;
//
//
//		forecastingTermStructure.linkTo(depoFRASwapTermStructure);
//		discountingTermStructure.linkTo(depoFRASwapTermStructure);
//
//		NPV = oneYearForward5YearSwap.NPV();
//		fairSpread = oneYearForward5YearSwap.fairSpread();
//		fairRate = oneYearForward5YearSwap.fairRate();
//
//		std::cout << std::setw(headers[0].size())
//			<< "depo-FRA-swap" << separator;
//		std::cout << std::setw(headers[1].size())
//			<< std::fixed << std::setprecision(2) << NPV << separator;
//		std::cout << std::setw(headers[2].size())
//			<< io::rate(fairSpread) << separator;
//		std::cout << std::setw(headers[3].size())
//			<< io::rate(fairRate) << separator;
//		std::cout << std::endl;
//
//
//		// now let's say that the 5-years swap rate goes up to 4.60%.
//		// A smarter market element--say, connected to a data source-- would
//		// notice the change itself. Since we're using SimpleQuotes,
//		// we'll have to change the value manually--which forces us to
//		// downcast the handle and use the SimpleQuote
//		// interface. In any case, the point here is that a change in the
//		// value contained in the Quote triggers a new bootstrapping
//		// of the curve and a repricing of the swap.
//
//		boost::shared_ptr<SimpleQuote> fiveYearsRate =
//			boost::dynamic_pointer_cast<SimpleQuote>(s5yRate);
//		fiveYearsRate->setValue(0.0460);
//
//		std::cout << dblrule << std::endl;
//		std::cout << "5-year market swap-rate = "
//			<< io::rate(s5yRate->value()) << std::endl;
//		std::cout << dblrule << std::endl;
//
//		std::cout << tab << "5-years swap paying "
//			<< io::rate(fixedRate) << std::endl;
//		std::cout << headers[0] << separator
//			<< headers[1] << separator
//			<< headers[2] << separator
//			<< headers[3] << separator << std::endl;
//		std::cout << rule << std::endl;
//
//		// now get the updated results
//		forecastingTermStructure.linkTo(depoSwapTermStructure);
//		discountingTermStructure.linkTo(depoSwapTermStructure);
//
//		NPV = spot5YearSwap.NPV();
//		fairSpread = spot5YearSwap.fairSpread();
//		fairRate = spot5YearSwap.fairRate();
//
//		std::cout << std::setw(headers[0].size())
//			<< "depo-swap" << separator;
//		std::cout << std::setw(headers[1].size())
//			<< std::fixed << std::setprecision(2) << NPV << separator;
//		std::cout << std::setw(headers[2].size())
//			<< io::rate(fairSpread) << separator;
//		std::cout << std::setw(headers[3].size())
//			<< io::rate(fairRate) << separator;
//		std::cout << std::endl;
//
//		QL_REQUIRE(std::fabs(fairRate - s5yRate->value())<1e-8,
//			"5-years swap mispriced!");
//
//
//		forecastingTermStructure.linkTo(depoFutSwapTermStructure);
//		discountingTermStructure.linkTo(depoFutSwapTermStructure);
//
//		NPV = spot5YearSwap.NPV();
//		fairSpread = spot5YearSwap.fairSpread();
//		fairRate = spot5YearSwap.fairRate();
//
//		std::cout << std::setw(headers[0].size())
//			<< "depo-fut-swap" << separator;
//		std::cout << std::setw(headers[1].size())
//			<< std::fixed << std::setprecision(2) << NPV << separator;
//		std::cout << std::setw(headers[2].size())
//			<< io::rate(fairSpread) << separator;
//		std::cout << std::setw(headers[3].size())
//			<< io::rate(fairRate) << separator;
//		std::cout << std::endl;
//
//		QL_REQUIRE(std::fabs(fairRate - s5yRate->value())<1e-8,
//			"5-years swap mispriced!");
//
//
//		forecastingTermStructure.linkTo(depoFRASwapTermStructure);
//		discountingTermStructure.linkTo(depoFRASwapTermStructure);
//
//		NPV = spot5YearSwap.NPV();
//		fairSpread = spot5YearSwap.fairSpread();
//		fairRate = spot5YearSwap.fairRate();
//
//		std::cout << std::setw(headers[0].size())
//			<< "depo-FRA-swap" << separator;
//		std::cout << std::setw(headers[1].size())
//			<< std::fixed << std::setprecision(2) << NPV << separator;
//		std::cout << std::setw(headers[2].size())
//			<< io::rate(fairSpread) << separator;
//		std::cout << std::setw(headers[3].size())
//			<< io::rate(fairRate) << separator;
//		std::cout << std::endl;
//
//		QL_REQUIRE(std::fabs(fairRate - s5yRate->value())<1e-8,
//			"5-years swap mispriced!");
//
//		std::cout << rule << std::endl;
//
//		// the 1Y forward 5Y swap changes as well
//
//		std::cout << tab << "5-years, 1-year forward swap paying "
//			<< io::rate(fixedRate) << std::endl;
//		std::cout << headers[0] << separator
//			<< headers[1] << separator
//			<< headers[2] << separator
//			<< headers[3] << separator << std::endl;
//		std::cout << rule << std::endl;
//
//
//		forecastingTermStructure.linkTo(depoSwapTermStructure);
//		discountingTermStructure.linkTo(depoSwapTermStructure);
//
//		NPV = oneYearForward5YearSwap.NPV();
//		fairSpread = oneYearForward5YearSwap.fairSpread();
//		fairRate = oneYearForward5YearSwap.fairRate();
//
//		std::cout << std::setw(headers[0].size())
//			<< "depo-swap" << separator;
//		std::cout << std::setw(headers[1].size())
//			<< std::fixed << std::setprecision(2) << NPV << separator;
//		std::cout << std::setw(headers[2].size())
//			<< io::rate(fairSpread) << separator;
//		std::cout << std::setw(headers[3].size())
//			<< io::rate(fairRate) << separator;
//		std::cout << std::endl;
//
//
//		forecastingTermStructure.linkTo(depoFutSwapTermStructure);
//		discountingTermStructure.linkTo(depoFutSwapTermStructure);
//
//		NPV = oneYearForward5YearSwap.NPV();
//		fairSpread = oneYearForward5YearSwap.fairSpread();
//		fairRate = oneYearForward5YearSwap.fairRate();
//
//		std::cout << std::setw(headers[0].size())
//			<< "depo-fut-swap" << separator;
//		std::cout << std::setw(headers[1].size())
//			<< std::fixed << std::setprecision(2) << NPV << separator;
//		std::cout << std::setw(headers[2].size())
//			<< io::rate(fairSpread) << separator;
//		std::cout << std::setw(headers[3].size())
//			<< io::rate(fairRate) << separator;
//		std::cout << std::endl;
//
//
//		forecastingTermStructure.linkTo(depoFRASwapTermStructure);
//		discountingTermStructure.linkTo(depoFRASwapTermStructure);
//
//		NPV = oneYearForward5YearSwap.NPV();
//		fairSpread = oneYearForward5YearSwap.fairSpread();
//		fairRate = oneYearForward5YearSwap.fairRate();
//
//		std::cout << std::setw(headers[0].size())
//			<< "depo-FRA-swap" << separator;
//		std::cout << std::setw(headers[1].size())
//			<< std::fixed << std::setprecision(2) << NPV << separator;
//		std::cout << std::setw(headers[2].size())
//			<< io::rate(fairSpread) << separator;
//		std::cout << std::setw(headers[3].size())
//			<< io::rate(fairRate) << separator;
//		std::cout << std::endl;
//
//		Real seconds = timer.elapsed();
//		Integer hours = int(seconds / 3600);
//		seconds -= hours * 3600;
//		Integer minutes = int(seconds / 60);
//		seconds -= minutes * 60;
//		std::cout << " \nRun completed in ";
//		if (hours > 0)
//			std::cout << hours << " h ";
//		if (hours > 0 || minutes > 0)
//			std::cout << minutes << " m ";
//		std::cout << std::fixed << std::setprecision(0)
//			<< seconds << " s\n" << std::endl;
//
//		return 0;
//
//	}
//	catch (std::exception& e) {
//		std::cerr << e.what() << std::endl;
//		return 1;
//	}
//	catch (...) {
//		std::cerr << "unknown error" << std::endl;
//		return 1;
//	}
//}
