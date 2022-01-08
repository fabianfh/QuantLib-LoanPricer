#include "LiborSwapTest.hpp"

namespace BootStrapper {
	using namespace QuantLib;

	std::vector<Date> getFixingDates(VanillaSwap swap)
	{
		Leg cashflows = swap.floatingLeg();
		std::vector<Date> fixingDates;

		for (Size i = 0; i < cashflows.size(); ++i) {
			boost::shared_ptr<FloatingRateCoupon> coupon =
				boost::dynamic_pointer_cast<FloatingRateCoupon>(cashflows[i]);
			if (coupon)
				fixingDates.push_back(coupon->fixingDate());
		}
		return fixingDates;
	}


#define LENGTH(a) (sizeof(a)/sizeof(a[0]))

	bool  LiborSwapTest::testBootstrap(
		boost::shared_ptr<PiecewiseFlatForward> discCurve,
		boost::shared_ptr<PiecewiseFlatForward> fwdCurve,
		MarketConventions mc, MarketData md, Real tolerance)
	{
		bool success = true;
		Real nominal = 100;Real spread = 0;
		RelinkableHandle<YieldTermStructure> eoniaTermStructure;
		eoniaTermStructure.linkTo(discCurve);
		boost::shared_ptr<Eonia> eoniaIndex = boost::shared_ptr<Eonia>(new Eonia(eoniaTermStructure));

		RelinkableHandle<YieldTermStructure> Libor3MTermStructure;
		Libor3MTermStructure.linkTo(fwdCurve);
		boost::shared_ptr<IborIndex> euriborIndex(new Euribor3M(Libor3MTermStructure));

		for (Size i = 0; i < md.swapData.size(); i++) {
			Rate expected = md.swapData[i].rate;
			Period term = md.swapData[i].nTermUnits * md.swapData[i].termUnit;

			VanillaSwap::Type swapType = VanillaSwap::Payer;
			Period tenor = md.swapData[i].nIndexUnits* md.swapData[i].indexUnit;

			Period fixedLegTenor = 1 * Years;
			Period floatingLegTenor = md.swapData[i].nIndexUnits * md.swapData[i].indexUnit;
			Period fwdStart = 0 * Days;

			VanillaSwap swap = MakeVanillaSwap(term, euriborIndex, expected, fwdStart)
				.withDiscountingTermStructure(eoniaTermStructure)
				.withFixedLegDayCount(mc.fixedSwapDayCount)
				.withFloatingLegDayCount(mc.floatSwapDayCount)
				.withFixedLegTenor(fixedLegTenor)
				.withFloatingLegTenor(floatingLegTenor)
				.withRule(DateGeneration::Backward)
				.withSettlementDays(mc.settlementDays);

			Rate calculated = swap.fairRate();
			Rate error = std::fabs(expected - calculated);
			Real npv = swap.NPV();

			std::cout << "Libor Swap " << md.swapData[i].nTermUnits << "x" << md.swapData[i].termUnit << ":  quoted rate: " << io::rate(md.swapData[i].rate) << "\t Fair Rate: " << io::rate(swap.fairRate()) << std::endl;

			if (error > tolerance)
			{
				success = false;
				std::cout << std::setprecision(10) <<
					"\n swap length:     " << term <<
					"\n quoted rate:     " << expected <<
					"\n calculated rate: " << calculated <<
					"\n error:           " << error <<
					"\n tolerance:       " << tolerance << std::endl;
			}

		}
		return success;
	}
}



