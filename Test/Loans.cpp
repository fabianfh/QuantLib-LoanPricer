#include <ql/quantlib.hpp>
#include <iostream>
#include "Loans.hpp"
#include "BootStrapper.hpp"
#include "Utils.hpp"
#include <boost/shared_ptr.hpp>
#include <ql/quantlib.hpp>
#include <ql/pricingengines/bond/all.hpp>


namespace BootStrapper {
  using namespace QuantLib;


#define LENGTH(a) (sizeof(a)/sizeof(a[0]))

    void  Loans::testLoan(Date startDate, Date maturityDate, Frequency freq) {

        Date asOfDate = Settings::instance().evaluationDate();
        MarketConventions mc; MarketData md;

        std::map<std::string, boost::shared_ptr<PiecewiseFlatForward> > TS = Bootstrap(asOfDate, md, mc);
        RelinkableHandle<YieldTermStructure> eoniaTermStructure;
        eoniaTermStructure.linkTo(TS["OIS"]);
        boost::shared_ptr<Eonia> eoniaIndex = boost::shared_ptr<Eonia>(new Eonia(eoniaTermStructure));
        RelinkableHandle<YieldTermStructure> Libor3MTermStructure;
        Libor3MTermStructure.linkTo(TS["Libor3M"]);

		boost::shared_ptr<PricingEngine> bondEngine(new DiscountingBondEngine(eoniaTermStructure));

        std::map<std::string, boost::shared_ptr<OptionletVolatilityStructure> > vols = BootstrapCaplets(asOfDate, md, mc, QuantLib::VolatilityType::ShiftedLognormal, TS["OIS"], TS["Libor3M"]);
		boost::shared_ptr<OptionletVolatilityStructure > vol(boost::dynamic_pointer_cast<OptionletVolatilityStructure >(vols["CapletVola"]));
		vol->enableExtrapolation();
		Handle<OptionletVolatilityStructure> volHandle(vol);
		Handle<YieldTermStructure> yieldCurveHandle(eoniaTermStructure);

	
		/*boost::shared_ptr< BlackCapFloorEngine > strippedVolEngine(
			new BlackCapFloorEngine(yieldCurveHandle, volHandle));*/
		boost::shared_ptr<IborCouponPricer> couponPricer(new BlackIborCouponPricer(Handle<OptionletVolatilityStructure>(vol)));

      
        Schedule mySchedule(startDate, maturityDate, Period(freq) , mc.calendar, Unadjusted, ModifiedFollowing, DateGeneration::Forward, false);

        std::vector<Real> NotionalSchedule; double notionalPrincipal = 100000;
        for (int i = 0; i < mySchedule.size() - 1; i++)
        {
            NotionalSchedule.push_back(notionalPrincipal);
        }
        std::vector<Real> Notional;
		boost::shared_ptr<IborIndex> index(new Euribor3M(Libor3MTermStructure));    
        AmortizingFloatingRateBond MyFloater(mc.settlementDays, NotionalSchedule, mySchedule, index, Actual360(), ModifiedFollowing, mc.settlementDays, std::vector<Real>(), std::vector<Spread>(), std::vector<Rate>(), std::vector<Rate>(), true, asOfDate);
       

        //ConstantOptionletVolatility* volatility = new ConstantOptionletVolatility(mc.settlementDays, mc.calendar, mc.fixedSwapConvention, Volatility(0), Thirty360());
        //boost::shared_ptr<OptionletVolatilityStructure> volaTermStructure(volatility);
        //boost::shared_ptr<OptionletVolatilityStructure> volaTermStructure2(vols["EURImplVola"]);


	



        MyFloater.setPricingEngine(bondEngine);
        setCouponPricer(MyFloater.cashflows(), couponPricer); 
        Real price = MyFloater.NPV();
        std::vector<boost::shared_ptr<CashFlow> > leg = MyFloater.cashflows();


        printCashflows(leg, MyFloater);
	
 
    
        
    }

    void  Loans::testLoan2(Date startDate, Date maturityDate, Period tenor) {

        /*
        * Following data is generated from Excel using function pmt with Nper = 360, PV = 100.0
        */

        Real rates[] = { 0.0, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10, 0.11, 0.12 };
        Real amounts[] = { 0.277777778, 0.321639520, 0.369619473, 0.421604034,
            0.477415295, 0.536821623, 0.599550525,
            0.665302495, 0.733764574, 0.804622617,
            0.877571570, 0.952323396, 1.028612597 };

        Frequency freq = Monthly;

        Date refDate = Settings::instance().evaluationDate();

        const Real tolerance = 1.0e-6;

        for (Size i = 0; i < LENGTH(rates); ++i) {

            AmortizingFixedRateBond
                myBond(0, NullCalendar(), 100.0, refDate, Period(30, Years), freq, rates[i], ActualActual(ActualActual::ISMA));
            /*AmortizingFloatingRateBond
                myBond2(0, NullCalendar(), 100.0, refDate, Period(30, Years), freq, rates[i], ActualActual(ActualActual::ISMA));*/

            Leg cashflows = myBond.cashflows();

            std::vector<Real> notionals = myBond.notionals();

            for (Size k = 0; k < cashflows.size() / 2; ++k) {
                Real coupon = cashflows[2 * k]->amount();
                Real principal = cashflows[2 * k + 1]->amount();
                Real totalAmount = coupon + principal;

                // Check the amount is same as pmt returned

                Real error = std::fabs(totalAmount - amounts[i]);
                if (error > tolerance) {
                    std::cout << "\n" <<
                        " Rate: " << rates[i] <<
                        " " << k << "th cash flow "
                        " Failed!" <<
                        " Expected Amount: " << amounts[i] <<
                        " Calculated Amount: " << totalAmount;
                }

                // Check the coupon result
                Real expectedCoupon = notionals[k] * rates[i] / freq;
                error = std::fabs(coupon - expectedCoupon);

                if (error > tolerance) {
                    std::cout << "\n" <<
                        " Rate: " << rates[i] <<
                        " " << k << "th cash flow "
                        " Failed!" <<
                        " Expected Coupon: " << expectedCoupon <<
                        " Calculated Coupon: " << coupon;
                }

            }
        }
    }
}

    
