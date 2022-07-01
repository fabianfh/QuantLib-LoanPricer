
#ifndef quantlib_test_optionletstripper_hpp
#define quantlib_test_optionletstripper_hpp

#include <boost/test/unit_test.hpp>
#include "../Market/MarketData.hpp"
#include "BootStrapper.hpp"


/* remember to document new and/or updated tests in the Doxygen
   comment block of the corresponding class */
namespace BootStrapper {
    class OptionletStripperTest {
    public:
        static void testFlatTermVolatilityStripping1();
        static void testTermVolatilityStripping1(MarketData md, MarketConventions mc, Date asOfDate);
        static void testTermVolatilityStrippingNormalVol(MarketData md, MarketConventions mc, Date asOfDate, 
			boost::shared_ptr<BootStrapper::PiecewiseFlatForward> discCurve, boost::shared_ptr<BootStrapper::PiecewiseFlatForward> fwdCurve, boost::shared_ptr<OptionletVolatilityStructure> capFloorVolSurface);
        static void testTermVolatilityStrippingShiftedLogNormalVol();
        static void testFlatTermVolatilityStripping2();
        static void testTermVolatilityStripping2(MarketData md, MarketConventions mc, Date asOfDate,
			boost::shared_ptr<BootStrapper::PiecewiseFlatForward> discCurve, boost::shared_ptr<BootStrapper::PiecewiseFlatForward> fwdCurve, boost::shared_ptr<OptionletVolatilityStructure> capFloorVolSurface);
        static void testSwitchStrike();

    };
}
#endif
