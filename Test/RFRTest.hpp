#ifndef RFRFUTURETEST_H__ // #include guards
#define RFRFUTURETEST_H__

//#include <boost/shared_ptr.hpp>
#include <ql/termstructures/yield/all.hpp>
#include "BootStrapper.hpp"
//#include <boost/shared_ptr.hpp>


namespace BootStrapper {
    class RFRTest {
    public:
        static bool testSofrFutureBootstrap(ext::shared_ptr<PiecewiseYieldCurve<Discount, Linear>> curve, MarketConventions mc, MarketData md, Real tolerance);
        static bool testRFRSwapBootstrap(ext::shared_ptr<PiecewiseYieldCurve<Discount, Linear>> sofrCurve, MarketConventions mc, std::vector<DepoSwapObs>& rfrSwaps, Real tolerance);
    };
} // namespace
#endif // RFRFUTURETEST_H__

