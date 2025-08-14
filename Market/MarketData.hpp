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
#include <ql/cashflows/rateaveraging.hpp>
//#include <boost/shared_ptr.hpp>




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


	struct DepoSwapObs {
		Integer settlementDays;
		Integer n;
		TimeUnit unit;
		Rate rate;
	};

	struct FraObs {
		Integer settlementDays;
		Integer nExpiry;
		Integer nMaturity;
		Rate rate;
	};

	struct LiborSwapObs {
		Integer settlementDays;
		Integer nIndexUnits;
		TimeUnit indexUnit;
		Integer nTermUnits;
		TimeUnit termUnit;
		Rate rate;
	};

	struct SofrFutureObs {
		Frequency freq;
		Month month;
		Year year;
		Real price;
		Rate rate;
		RateAveraging::Type averagingMethod;
	};

	struct MarketConventions {
		Natural settlementDays;
		Date settlement;
		Calendar calendar;
		Period fixedEoniaPeriod, floatingEoniaPeriod;
		DayCounter fixedEoniaDayCount;
		BusinessDayConvention fixedEoniaConvention, floatingEoniaConvention;

		Frequency fixedSwapFrequency;
		DayCounter fixedSwapDayCount;
		BusinessDayConvention fixedSwapConvention;


		Frequency floatSwapFrequency;
		DayCounter floatSwapDayCount;
		BusinessDayConvention floatSwapConvention;



		MarketConventions() {

			settlementDays = 2;
			calendar = TARGET();
			settlement = calendar.advance(Settings::instance().evaluationDate(), settlementDays * Days, Following);

			fixedEoniaConvention = ModifiedFollowing;
			floatingEoniaConvention = ModifiedFollowing;
			fixedEoniaPeriod = 1 * Years;
			floatingEoniaPeriod = 1 * Years;
			fixedEoniaDayCount = Actual360();

			fixedSwapConvention = ModifiedFollowing;
			fixedSwapFrequency = Annual;
			fixedSwapDayCount = Thirty360(Thirty360::German);


			floatSwapConvention = ModifiedFollowing;
			floatSwapFrequency = Quarterly;
			floatSwapDayCount = Actual360();

		}
	};

	class MarketData {
	public:
		MarketData();
		std::vector<SofrFutureObs> sofrFutureUSD;
		std::vector<DepoSwapObs> depoEUR;
		std::vector<DepoSwapObs> eoniaSwapEUR;
		std::vector<DepoSwapObs> sofrSwapUSD;
		std::vector<SofrFutureObs> SwapEUR;
		std::vector<FraObs> fraData;
		std::vector<LiborSwapObs> liborSwap6mEUR;
		std::vector<LiborSwapObs> zeroYield6mLiborEUR;
		std::vector<VolaObs> capfloorVolaEUR;


		std::vector<Rate> strikes;
		std::vector<Period> optionTenors;
		Matrix termV;

		std::map<std::string, ext::shared_ptr<CapFloorTermVolSurface> >  mdContainer;
		std::map<std::string, ext::shared_ptr<YieldTermStructure> >  mdContainer2;
		Handle<CapFloorTermVolCurve> atmVols;

		Real tolerance = 0.000000001;
		Natural maxIter = 10000;



	};
}

#endif 