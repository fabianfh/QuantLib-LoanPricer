#include <ql/settings.hpp>
#include <ql/cashflows/cashflows.hpp>
//#include <boost/shared_ptr.hpp>
#include "Utils.hpp"
#include <string>
#include <iostream>

namespace BootStrapper {
    //using namespace QuantLib;
    //using namespace std;

    void printSchedule(std::string header, QuantLib::Schedule s)
    {        
        for (int i = 0; i < s.size();i++)
            std::cout <<  header << "\t" << QuantLib::io::iso_date(s[i]) << std::endl;
    }

	void printCashflows(QuantLib::Leg cf, QuantLib::Bond bond)
	{
		for (QuantLib::Size j = 0; j < cf.size() - 1; j++) {

			QuantLib::Date cfDate = cf[j]->date();
			QuantLib::Real nominal = QuantLib::CashFlows::nominal(cf, true, cfDate);
			QuantLib::Time yf = QuantLib::CashFlows::accrualPeriod(cf, true, cfDate);
			QuantLib::Real Amount = cf[j]->amount();

			std::cout << "Date is " << cfDate << "  | Notional " << nominal << "  |  YearFrac " << yf << "  |  Amount " << Amount << std::endl;

		}
	}

    void printOISwap(QuantLib::Schedule s, QuantLib::OvernightIndexedSwap swap)
    {
		QuantLib::Leg fixLeg = swap.fixedLeg();

        std::cout << "OISwap: fixed Leg" << std::endl;
		std::cout << "\t" << "LegNPV" << "\t" << "BPS" << "\t" << "Rate" << "\t" << std::endl;
		std::cout << "\t" << swap.fixedLegNPV() << "\t" << swap.fixedLegBPS() << "\t" << swap.fixedRate() << std::endl;
		std::cout << "\t" << "\t" << "Fixed cashflows:" << std::endl;
		std::cout << "\t" << "\t" << "Pay date" << "\t" << "YearFrac" << "\t" << "Amount" << "\t" << "Discount" << "\t" << "Premium" << std::endl;
        for (std::vector<boost::shared_ptr<QuantLib::CashFlow> >::size_type i = 0; i != fixLeg.size(); i++)

        {
            double yf = swap.fixedDayCount().yearFraction(i < 1 ? QuantLib::Settings::instance().evaluationDate() : fixLeg[i - 1]->date(), fixLeg[i]->date());
			std::cout << "\t" << "\t" << QuantLib::io::iso_date(fixLeg[i]->date())
                << "\t" << yf
                << "\t" << fixLeg[i]->amount()
                << "\t" << swap.endDiscounts(i)
                << "\t" << fixLeg[i]->amount() * swap.endDiscounts(i)
                << std::endl;
        }
		std::cout << std::endl;

		QuantLib::Leg oiLeg = swap.overnightLeg(); std::cout << "OISwap: Floating Leg" << std::endl;

		std::cout << "\t" << "LegNPV" << "\t" << "BPS" << "\t" << "Fix Rate" << "\t" << std::endl;
		std::cout << "\t" << swap.overnightLegNPV() << "\t" << swap.overnightLegBPS() << "\t" << swap.fixedRate() << std::endl;
		std::cout << "\t" << "\t" << "Fixed cashflows:" << std::endl;
		std::cout << "\t" << "\t" << "Pay date" << "\t" << "YearFrac" << "\t" << "Amount" << "\t" << "Discount" << "\t" << "Premium" << std::endl;
        for (std::vector<boost::shared_ptr<QuantLib::CashFlow> >::size_type i = 0; i != fixLeg.size(); i++)

        {

            double yf = swap.fixedDayCount().yearFraction(i < 1 ? QuantLib::Settings::instance().evaluationDate() : oiLeg[i - 1]->date(), oiLeg[i]->date());
			std::cout << "\t" << "\t" << QuantLib::io::iso_date(oiLeg[i]->date())
                << "\t" << yf
                << "\t" << oiLeg[i]->amount()
                << "\t" << swap.endDiscounts(i)
                << "\t" << oiLeg[i]->amount() * swap.endDiscounts(i)
                << std::endl;
        }
		std::cout << std::endl;
    }


}
