//#include "BootStrapper.hpp"
//#include "Utils.hpp"
#include "OISwapTest.hpp"
//#include <ql/quantlib.hpp>
#include <ql/instruments/makeois.hpp>



namespace BootStrapper {
	using namespace QuantLib;


#define LENGTH(a) (sizeof(a)/sizeof(a[0]))

	bool  OISwapTest::testEONIABootstrap(ext::shared_ptr<PiecewiseYieldCurve<Discount, Linear>> discCurve, MarketConventions mc, MarketData md, Real tolerance)
	{
		Real nominal = 100; bool success = true;
		RelinkableHandle<YieldTermStructure> eoniaTermStructure(discCurve);
		ext::shared_ptr<Eonia> eoniaIndex = ext::shared_ptr<Eonia>(new Eonia(eoniaTermStructure));
		for (Size i = 0; i < md.eoniaSwapEUR.size(); i++) {
			Rate expected = md.eoniaSwapEUR[i].rate;
			Period term = md.eoniaSwapEUR[i].n * md.eoniaSwapEUR[i].unit;

			ext::shared_ptr<OvernightIndexedSwap> swap = MakeOIS(term, eoniaIndex, expected).withEffectiveDate(mc.settlement).withOvernightLegSpread(0).withNominal(nominal).withDiscountingTermStructure(eoniaTermStructure);

			Rate calculated = swap->fairRate();
			Rate error = std::fabs(expected - calculated);
			Real npv = swap->NPV();

			std::cout << "OISwap " << md.eoniaSwapEUR[i].n << "x" << md.eoniaSwapEUR[i].unit << ":  quoted rate: " << io::rate(md.eoniaSwapEUR[i].rate) << "\t Fair Rate: " << io::rate(swap->fairRate()) << std::endl;

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



}




