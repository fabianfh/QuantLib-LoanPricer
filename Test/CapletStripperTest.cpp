#include "CapletStripperTest.hpp"
#include "BootStrapper.hpp"
#include <ql/pricingengines/capfloor/all.hpp>
#include <ql/instruments/makecapfloor.hpp>

//#include <boost/shared_ptr.hpp>


namespace BootStrapper {
	using namespace QuantLib;



	bool  CapletStripperTest::testBootstrap(
		ext::shared_ptr<YieldTermStructure> discCurve,
		ext::shared_ptr<YieldTermStructure> fwdCurve,
		ext::shared_ptr<OptionletVolatilityStructure> capletVols,
		VolatilityType volaType,
		MarketConventions mc, MarketData md, Real tolerance = 1.0e-8)
	{
		bool success = true;
		Handle<YieldTermStructure> fwdCurveHandle(fwdCurve);
		Handle<YieldTermStructure> discCurveHandle(discCurve);
		Handle<OptionletVolatilityStructure> capletVolCurveHandle(capletVols);
		capletVols->enableExtrapolation();

		ext::shared_ptr<CapFloorTermVolSurface> flatVols = md.mdContainer["EURCapFlatVolaNormal"];
		//std::vector<Period> periods = flatVols->optionTenors();


		ext::shared_ptr< IborIndex > iborIndex(new Euribor3M(fwdCurveHandle));

		PricingEngine* pe;
		if (volaType == Normal)
			pe = new BachelierCapFloorEngine(discCurveHandle, capletVolCurveHandle);
		else
			pe = new BlackCapFloorEngine(discCurveHandle, capletVolCurveHandle);

		ext::shared_ptr<PricingEngine> strippedVolEngine(pe);

		for (Size strikeIndex = 0; strikeIndex < flatVols->strikes().size(); ++strikeIndex)
		{
			for (Size tenorIndex = 0; tenorIndex < flatVols->optionTenors().size(); ++tenorIndex)
			{
				Period period = flatVols->optionTenors()[tenorIndex];
				Real strike = flatVols->strikes()[strikeIndex];
				Period fwdStart = 0 * Days;

				boost::shared_ptr< CapFloor > cap = MakeCapFloor(CapFloor::Cap, period, iborIndex, strike, fwdStart);
				cap->setPricingEngine(strippedVolEngine);
				Real actual = cap->NPV();

				PricingEngine* pe2; Real flatVol = flatVols->volatility(period, strike);
				if (volaType == Normal)
					pe2 = new BachelierCapFloorEngine(discCurveHandle, flatVol);
				else
					pe2 = new BlackCapFloorEngine(discCurveHandle, flatVol);


				boost::shared_ptr<PricingEngine> flatVolEngine(pe2);
				cap->setPricingEngine(flatVolEngine);
				Real target = cap->NPV();

				Real error = std::fabs(actual - target);
				if (error > tolerance)
				{
					success = false;
					std::cout << "\noption tenor:       "
						<< flatVols->optionTenors()[tenorIndex] << "\nstrike:             "
						<< io::rate(flatVols->strikes()[strikeIndex])
						<< "\nstripped vol price: "
						<< io::rate(actual)
						<< "\nconstant vol price: "
						<< io::rate(target)
						<< "\nerror:              " << io::rate(error)
						<< "\ntolerance:          " << io::rate(tolerance)
						<< "\nsigma(flat) = " << io::rate(flatVol)
						<< "\nsigma(flat) = " << io::rate(flatVol)
						<< std::endl;
				}
				else
				{
					std::cout
						<< "Libor Cap(" << period << "," << strike << ") premium target: " << io::rate(target) << "    " << "actual: " << io::rate(actual) << std::endl;
				}
			}
		}
		return success;
	}
}






                
    
    

        
