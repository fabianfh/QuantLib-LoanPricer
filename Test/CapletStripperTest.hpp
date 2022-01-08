
#ifndef CAPPLET_H__ // #include guards
#define CAPPLET_H__


#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include "BootStrapper.hpp"

namespace BootStrapper {
    using namespace QuantLib;
    #define LENGTH(a) (sizeof(a)/sizeof(a[0]))

    class CapletStripperTest {
    public:
        static bool testBootstrap(
			boost::shared_ptr<YieldTermStructure> fwdCurve,
			boost::shared_ptr<YieldTermStructure> discCurve,
			boost::shared_ptr<OptionletVolatilityStructure> capletVols,
			VolatilityType volaType,
			MarketConventions mc, MarketData md,Real tolerace);

    };
    

    //std::map<std::string, boost::shared_ptr<VolatilityTermStructure> > BootstrapCapletsLogNormal(Date asOfDate, MarketData& md, MarketConventions& mc, boost::shared_ptr<PiecewiseFlatForward>  discCurve, boost::shared_ptr<PiecewiseFlatForward>  liborCurve);


} // namespace
#endif // CAPPLET_H__
