#ifndef OISWAPTEST_H__ // #include guards
#define OISWAPTEST_H__

#include <boost/shared_ptr.hpp>
#include <ql/termstructures/yield/all.hpp>
#include "../BootStrapper/BootStrapper.hpp" 

namespace BootStrapper { 
  class OISwapTest {
  public:
    static bool testBootstrap(boost::shared_ptr<PiecewiseFlatForward> discCurve, MarketConventions mc, MarketData md, Real tolerance);
       
  };
} // namespace
#endif // OISWAPTEST_H__


