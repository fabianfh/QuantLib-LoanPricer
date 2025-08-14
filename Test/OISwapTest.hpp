#ifndef OISWAPTEST_H__ // #include guards
#define OISWAPTEST_H__

//#include <boost/shared_ptr.hpp>
#include <ql/termstructures/yield/all.hpp>
#include "BootStrapper.hpp"

namespace BootStrapper {
    class OISwapTest {
    public:
        static bool testEONIABootstrap(ext::shared_ptr<PiecewiseYieldCurve<Discount, Linear>> curve, MarketConventions mc, MarketData md, Real tolerance);

    };
} // namespace
#endif // OISWAPTEST_H__


