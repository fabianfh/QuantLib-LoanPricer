#ifndef LIBORSWAPTEST_H__ // #include guards
#define LIBORSWAPTEST_H__

//#include <boost/shared_ptr.hpp>
#include <ql/termstructures/yield/all.hpp>
#include <ql/cashflows/floatingratecoupon.hpp>
#include <ql/instruments/makevanillaswap.hpp>
#include "BootStrapper.hpp"

namespace BootStrapper {
	class LiborSwapTest {
	public:
		static bool testLiborBootstrap(ext::shared_ptr<PiecewiseYieldCurve<Discount, Linear>> discCurve, ext::shared_ptr<PiecewiseYieldCurve<Discount, Linear>> fwdCurve, MarketConventions mc, MarketData md, Real tolerance);
	};
}// namespace
#endif // LIBORSWAPTEST_H__
