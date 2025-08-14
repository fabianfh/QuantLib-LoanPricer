//#include "BootStrapper.hpp"
//#include "Utils.hpp"
#include "RFRTest.hpp"
//#include <ql/quantlib.hpp>
#include <ql/instruments/makeois.hpp>
#include <ql/time/date.hpp>
#include <ql/time/timeunit.hpp>
#include <ql/time/imm.hpp>
#include <ql/experimental/averageois/arithmeticaverageois.hpp>
#include <ql/experimental/averageois/makearithmeticaverageois.hpp>

namespace BootStrapper {
	using namespace QuantLib;


#define LENGTH(a) (sizeof(a)/sizeof(a[0]))

	bool  RFRTest::testRFRSwapBootstrap(ext::shared_ptr<PiecewiseYieldCurve<Discount, Linear>> sofrCurve, MarketConventions mc, std::vector<DepoSwapObs>& rfrSwaps, Real tolerance)
	{
		Real nominal = 100; bool success = true;
		RelinkableHandle<YieldTermStructure> sofrTermStructure(sofrCurve);
		ext::shared_ptr<Sofr> sofrIndex = ext::shared_ptr<Sofr>(new Sofr(sofrTermStructure));
		for (Size i = 0; i < rfrSwaps.size(); i++) {
			Rate expected = rfrSwaps[i].rate;
			Period term = rfrSwaps[i].n * rfrSwaps[i].unit;

			ext::shared_ptr<OvernightIndexedSwap> swap = MakeOIS(term, sofrIndex, expected).withEffectiveDate(mc.settlement).withOvernightLegSpread(0).withNominal(nominal).withDiscountingTermStructure(sofrTermStructure);
			//ext::shared_ptr<ArithmeticAverageOIS> swapAvg = MakeArithmeticAverageOIS(term, sofrIndex, expected).withEffectiveDate(mc.settlement).withOvernightLegSpread(0).withNominal(nominal).withDiscountingTermStructure(sofrTermStructure);

			Rate calculated = swap->fairRate();
			Rate error = std::fabs(expected - calculated);
			Real npv = swap->NPV();

			std::cout << "SOFRSwap " << rfrSwaps[i].n << "x" << rfrSwaps[i].unit << ":  quoted rate: " << io::rate(rfrSwaps[i].rate) << "\t Fair Rate: " << io::rate(swap->fairRate()) << std::endl;

			if (error > tolerance)
			{
				success = false;
				std::cout << std::setprecision(10) <<
					"\n swap length:     " << term <<
					"\n quoted rate:     " << expected <<
					"\n calculated rate: " << calculated <<
					"\n error:           " << error <<
					"\n tolerance:       " << tolerance
					<< std::endl;
			}

		}
		return success;
	}


	bool  RFRTest::testSofrFutureBootstrap(ext::shared_ptr<PiecewiseYieldCurve<Discount, Linear>> curve, MarketConventions mc, MarketData md, Real tolerance)
	{
		Real nominal = 100; bool success = true;
		RelinkableHandle<YieldTermStructure> rfrTermStructure(curve);
		ext::shared_ptr<OvernightIndex> eoniaIndex = ext::shared_ptr<OvernightIndex>(new Sofr(rfrTermStructure));

		for (Size i = 0; i < md.sofrFutureUSD.size(); i++) {
			Rate expected = md.sofrFutureUSD[i].rate;
			Month month = md.sofrFutureUSD[i].month;
			Year year = md.sofrFutureUSD[i].year;
			Frequency freq = md.sofrFutureUSD[i].freq;
			Real price = md.sofrFutureUSD[i].price;
			RateAveraging::Type avgMethod = md.sofrFutureUSD[i].averagingMethod;

			ext::shared_ptr<OvernightIndex> sofr = ext::make_shared<Sofr>(Handle<YieldTermStructure>(curve));
			Date maturityDate = QuantLib::IMM::nextDate(Date(1, month, year), freq == Frequency::Quarterly);
			Date valueDate = QuantLib::Settings::instance().evaluationDate();

			OvernightIndexFuture sf(sofr, valueDate, maturityDate);
			Real calculated = sf.NPV();
			Real convexity = sf.convexityAdjustment();
			Rate error = std::fabs(expected - calculated);

			success = false;
			std::cout << "RFRFuture(" << md.sofrFutureUSD[i].month << ";" << md.sofrFutureUSD[i].year << ");"
				<< "\tMaturity date: " << QuantLib::io::iso_date(maturityDate)
				<< "\tQuote: " << price << "\tNPV: " << calculated << std::endl;


		}
		return success;
	}







}