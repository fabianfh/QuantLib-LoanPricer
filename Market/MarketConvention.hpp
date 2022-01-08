#ifndef MARKETCONVENTION_H__ // #include guards
#define MARKETCONVENTION_H__

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
			settlement = calendar.advance(Settings::instance().evaluationDate(), settlementDays*Days, Following);

			fixedEoniaConvention = ModifiedFollowing;
			floatingEoniaConvention = ModifiedFollowing;
			fixedEoniaPeriod = 1 * Years;
			floatingEoniaPeriod = 1 * Years;
			fixedEoniaDayCount = Actual360();

			fixedSwapConvention = ModifiedFollowing;
			fixedSwapFrequency = Annual;
			fixedSwapDayCount = Thirty360(Thirty360::Convention::ISDA);


			floatSwapConvention = ModifiedFollowing;
			floatSwapFrequency = Quarterly;
			floatSwapDayCount = Actual360();

		}
	};


}



#endif