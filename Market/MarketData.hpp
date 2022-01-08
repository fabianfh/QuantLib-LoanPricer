#ifndef MARKETDATA_H__ // #include guards
#define MARKETDATA_H__

#include <ql/time/daycounters/all.hpp>
#include <ql/time/calendars/all.hpp>
#include <ql/types.hpp>
#include <ql/settings.hpp>
#include <ql/math/all.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <ql/termstructures/volatility/capfloor/capfloortermvolsurface.hpp>
#include <ql/termstructures/volatility/capfloor/capfloortermvolcurve.hpp>
#include <boost/shared_ptr.hpp>




namespace BootStrapper {
	using namespace QuantLib;
#define LENGTH(a) (sizeof(a)/sizeof(a[0]))
	
	struct VolaObs {
		Integer settlementDays;
		Integer nIndexUnits;
		TimeUnit indexUnit;
		Integer nTermUnits;
		TimeUnit termUnit;
		std::string strikeUnit;
		Real strike;
		Volatility vola;
	};
	
	struct DepoObs {
		Integer settlementDays;
		Integer n;
		TimeUnit unit;
		Rate rate;
	};

	struct FraObs {
		Integer settlementDays;
		Integer nExpiry;
		Integer nMaturity;
		//        TimeUnit units;
		Rate rate;
	};

	struct SwapObs {
		Integer settlementDays;
		Integer nIndexUnits;
		TimeUnit indexUnit;
		Integer nTermUnits;
		TimeUnit termUnit;
		Rate rate;
	};

	class MarketData {
	public:
		MarketData();
		std::vector<DepoObs> depositData;		
		std::vector<DepoObs> eoniaSwapData;
		std::vector<FraObs> fraData;
		std::vector<SwapObs> swapData;
		std::vector<SwapObs> zeroBondData;
		std::vector<VolaObs> volSurfaceData;

		std::vector<Rate> strikes;
		std::vector<Period> optionTenors;
		Matrix termV;

		std::map<std::string, boost::shared_ptr<CapFloorTermVolSurface> >  mdContainer;
		std::map<std::string, boost::shared_ptr<YieldTermStructure> >  mdContainer2;
		Handle<CapFloorTermVolCurve> atmVols;

		Real tolerance = 0.000000001;
		Natural maxIter = 10000;	
	
	};
}
#endif 