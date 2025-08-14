

#include <iostream>

#include <ql/quantlib.hpp>
#include "Market/MarketData.hpp"
#include "BootStrapper.hpp"
#include "Test/OISwapTest.hpp"
#include "Test/LiborSwapTest.hpp"
#include "Test/CapletStripperTest.hpp"
#include "Test/RFRTest.hpp"
#include "Test/Loans.hpp"
#include <ql/termstructures/yield/all.hpp>
#include <ql/termstructures/yield/piecewiseyieldcurve.hpp>
#include "Main.h"







//using namespace QuantLib;
//using namespace std;

int main(int, char* [])
{
	QuantLib::Calendar calendar = QuantLib::TARGET();
	QuantLib::Date asOfDate = calendar.adjust(QuantLib::Date::todaysDate());
	std::cout << "Evaluation Date: " << std::endl; //QuantLib::io::iso_date(asOfDate) << std::endl;
	QuantLib::Settings::instance().evaluationDate() = asOfDate;

	BootStrapper::MarketConventions mc; BootStrapper::MarketData md;

	bool success = true;

	LiborCashflows(asOfDate, md, mc, success);
	RFRCashflows(asOfDate, md, mc, success);


	return 0;
}

void RFRCashflows(const QuantLib::Date& asOfDate, BootStrapper::MarketData& md, BootStrapper::MarketConventions& mc, bool& success)
{
	//RFR-Swap
	std::vector<BootStrapper::DepoSwapObs> dummy;
	boost::shared_ptr<QuantLib::PiecewiseYieldCurve<QuantLib::Discount, QuantLib::Linear>>  sofrCurve
		= BootStrapper::BootstrapOIS(asOfDate, dummy, md.sofrSwapUSD, mc);
	success &= BootStrapper::RFRTest::testRFRSwapBootstrap(sofrCurve, mc, md.sofrSwapUSD, md.tolerance);

	//RfR-Futures
	//sofrCurve = BootStrapper::BootstrapSOFRFutures(asOfDate, md.sofrFutureUSD, mc);
	//success &= BootStrapper::RFRTest::testSofrFutureBootstrap(sofrCurve, mc, md, md.tolerance);



	boost::shared_ptr<QuantLib::OptionletVolatilityStructure> _ = boost::shared_ptr<QuantLib::OptionletVolatilityStructure>();
	//BootStrapper::Loans::testRFRLoans(sofrCurve, sofrCurve, _, BootStrapper::Normal, mc, md, md.tolerance);

}

void LiborCashflows(const QuantLib::Date& asOfDate, BootStrapper::MarketData& md, BootStrapper::MarketConventions& mc, bool& success)
{
	//EONIA
	boost::shared_ptr<QuantLib::PiecewiseYieldCurve<QuantLib::Discount, QuantLib::Linear>>  eoniaSwapCurve
		= BootStrapper::BootstrapOIS(asOfDate, md.depoEUR, md.eoniaSwapEUR, mc);
	success &= BootStrapper::OISwapTest::testEONIABootstrap(eoniaSwapCurve, mc, md, md.tolerance);

	//Libor
	boost::shared_ptr<QuantLib::PiecewiseYieldCurve<QuantLib::Discount, QuantLib::Linear>>  libor3MCurve
		= BootStrapper::BootstrapLIBOR(asOfDate, md, mc, eoniaSwapCurve);
	success &= BootStrapper::LiborSwapTest::testLiborBootstrap(eoniaSwapCurve, libor3MCurve, mc, md, md.tolerance);

	//caplet
	std::map<std::string, boost::shared_ptr<QuantLib::OptionletVolatilityStructure> > vols = BootStrapper::BootstrapCaplets(asOfDate, md, mc, QuantLib::Normal, eoniaSwapCurve, libor3MCurve);
	boost::shared_ptr<QuantLib::OptionletVolatilityStructure> volCaplet = vols["EURCapletVolaNormal"];
	success &= BootStrapper::CapletStripperTest::testBootstrap(eoniaSwapCurve, libor3MCurve, volCaplet, BootStrapper::Normal, mc, md, md.tolerance);

	//loan
	BootStrapper::Loans::testLiborLoans(libor3MCurve, eoniaSwapCurve, volCaplet, BootStrapper::Normal, mc, md, md.tolerance);


	/*BootStrapper::OptionletStrip::perTest::testTermVolatilityStripping2(md, mc, asOfDate, discCurve, fwdCurve, volCaplet);
	BootStrapper::OptionletStripperTest::testTermVolatilityStripping1(md,mc,asOfDate);
	BootStrapper::OptionletStripperTest::testTermVolatilityStripping2();
	BootStrapper::OptionletStripperTest::testSwitchStrike();
	BootStrapper::OptionletStripperTest::testTermVolatilityStrippingNormalVol(md, mc, asOfDate,discCurve,fwdCurve, volCaplet);



	boost::shared_ptr<QuantLib::YieldTermStructure> singleCurve = md.mdContainer2["EURZeroYields"];
	std::map<std::string, boost::shared_ptr<QuantLib::OptionletVolatilityStructure> > vol = BootStrapper::BootstrapCaplets(asOfDate, md, mc, QuantLib::Normal, singleCurve, singleCurve);
	*/
}






