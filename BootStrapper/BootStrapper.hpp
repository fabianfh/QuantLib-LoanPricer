#ifndef BOOTSTRAPPER_H__ // #include guards
#define BOOTSTRAPPER_H__

#include <ql/termstructures/volatility/optionlet/all.hpp>
#include <ql/time/daycounters/all.hpp>
#include <ql/time/calendars/all.hpp>
#include <ql/termstructures/yield/all.hpp>
#include <ql/indexes/ibor/all.hpp>
#include <ql/time/schedule.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assign/std/vector.hpp>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include "../Market/MarketData.hpp"
#include "../Market/MarketConvention.hpp"

namespace BootStrapper {
	using namespace QuantLib;
#define LENGTH(a) (sizeof(a)/sizeof(a[0]))



	typedef PiecewiseYieldCurve<ForwardRate, BackwardFlat> PiecewiseFlatForward;

	std::map<std::string, boost::shared_ptr<PiecewiseFlatForward> > Bootstrap(Date asOfDate, MarketData& md, MarketConventions& mc);
	boost::shared_ptr<PiecewiseFlatForward>  BootstrapLIBOR(Date asOfDate, MarketData& md, MarketConventions& mc, Handle<YieldTermStructure> discountCurve);
	boost::shared_ptr<PiecewiseFlatForward> BootstrapOIS(Date asOfDate, MarketData& md, MarketConventions& mc);
	std::map<std::string, boost::shared_ptr<OptionletVolatilityStructure> >
		BootstrapCaplets(Date asOfDate, MarketData& md, MarketConventions& mc, VolatilityType volaType, boost::shared_ptr<YieldTermStructure>  discCurve, boost::shared_ptr<YieldTermStructure>  liborCurve);



}// namespace
#endif // BOOTSTRAPPER_H__


