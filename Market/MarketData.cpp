#include "MarketData.hpp"
#include "MarketConvention.hpp"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <map>
#include <algorithm>
#include <ql/termstructures/yield/zeroyieldstructure.hpp>
#include <ql/math/interpolations/all.hpp>
#include <ql/termstructures/yield/all.hpp>
#include <ql/settings.hpp>

namespace BootStrapper
{

	std::vector<SwapObs> readSwapObsFromXml(std::istream &is, std::string mdType)
	{
		std::map<std::string, TimeUnit> mapTimeUnit = boost::assign::map_list_of("Days", Days)("Weeks", Weeks)("Months", Months)("Years", Years);

		// populate tree structure pt
		using boost::property_tree::ptree;
		ptree pt;
		read_xml(is, pt);
		// traverse pt
		std::vector<SwapObs> retval;
		;
		BOOST_FOREACH (ptree::value_type const &v, pt.get_child(mdType))
		{

			if (v.first == "SwapObs")
			{
				SwapObs f;
				f.settlementDays = v.second.get<int>("settlementDays");
				f.nIndexUnits = v.second.get<int>("nIndexUnits");
				std::string sIndexUnit = v.second.get<std::string>("indexUnit");
				f.indexUnit = mapTimeUnit[sIndexUnit];
				std::string sTermUnit = v.second.get<std::string>("termUnit");
				f.termUnit = mapTimeUnit[sTermUnit];
				f.nTermUnits = v.second.get<int>("nTermUnits");
				f.rate = v.second.get<double>("rate");
				retval.push_back(f);
			}
		}
		return retval;
	}

	std::vector<DepoObs> readDepoObsFromXml(std::istream &is, std::string mdType)
	{
		std::map<std::string, TimeUnit> mapTimeUnit = boost::assign::map_list_of("Days", Days)("Weeks", Weeks)("Months", Months)("Years", Years);

		// populate tree structure pt
		using boost::property_tree::ptree;
		ptree pt;
		read_xml(is, pt);
		// traverse pt
		std::vector<DepoObs> retval;
		;
		BOOST_FOREACH (ptree::value_type const &v, pt.get_child(mdType))
		{
			if (v.first == "DepoObs")
			{
				DepoObs f;
				f.settlementDays = v.second.get<int>("settlementDays");
				f.n = v.second.get<int>("n");
				std::string sunit = v.second.get<std::string>("unit");
				f.unit = mapTimeUnit[sunit];
				f.rate = v.second.get<double>("rate", false);
				retval.push_back(f);
			}
		}
		return retval;
	}

	std::vector<VolaObs> readVolaFromXml(std::istream &is, std::string mdType)
	{
		std::map<std::string, TimeUnit> mapTimeUnit = boost::assign::map_list_of("Days", Days)("Weeks", Weeks)("Months", Months)("Years", Years);

		// populate tree structure pt
		using boost::property_tree::ptree;
		ptree pt;
		read_xml(is, pt);
		// traverse pt
		std::vector<VolaObs> retval;
		;
		BOOST_FOREACH (ptree::value_type const &v, pt.get_child(mdType))
		{
			if (v.first == "VolaObs")
			{
				VolaObs f;
				f.settlementDays = v.second.get<int>("settlementDays");
				f.nIndexUnits = v.second.get<int>("nIndexUnits");
				std::string sIndexUnit = v.second.get<std::string>("indexUnit");
				f.indexUnit = mapTimeUnit[sIndexUnit];
				std::string sTermUnit = v.second.get<std::string>("termUnit");
				f.termUnit = mapTimeUnit[sTermUnit];
				f.nTermUnits = v.second.get<int>("nTermUnits");
				f.vola = v.second.get<double>("vola");
				f.strike = v.second.get<double>("strike");
				retval.push_back(f);
			}
		}
		return retval;
	}

	boost::shared_ptr<YieldTermStructure> makeZeroYieldCurve(std::vector<SwapObs> surface, MarketConventions mc)
	{
		Date asOfDate = QuantLib::Settings::instance().evaluationDate();
		std::vector<boost::shared_ptr<RateHelper> > rateHelpers;
		std::vector<SwapObs>::const_iterator curr = surface.begin();
		std::vector<SwapObs>::const_iterator end = surface.end();
		std::vector<Date> dates;
		std::vector<Rate> rates;
		for (; curr != end; curr++)
		{
			QuantLib::Period period(curr->nTermUnits, curr->termUnit);
			Date date = mc.calendar.advance(asOfDate, period);
			dates.push_back(date);
			rates.push_back(curr->rate);
		}

		// first date is taken as reference date; Wrong referebce date leads to exceptions():
		Date settlementDate = mc.calendar.advance(Settings::instance().evaluationDate(), mc.settlementDays, Days, mc.fixedEoniaConvention, false); //dummy
		Rate ccRateAtSett = rates.front();
		dates.insert(dates.begin(), settlementDate);
		rates.insert(rates.begin(), ccRateAtSett);

		ActualActual dcc1(ActualActual::Convention::ISDA);
		Actual360 dcc2();

		QuantLib::ZeroCurve *zcurvep = new QuantLib::ZeroCurve(dates, rates, dcc1);
		zcurvep->enableExtrapolation(true);
		//zcurvep->referenceDate = settlementDate;
		//QuantLib::InterpolatedZeroCurve<Linear>* zcurvep = new  QuantLib::InterpolatedZeroCurve<Linear>(Actual365Fixed(),);
		boost::shared_ptr<YieldTermStructure> retVal(zcurvep);
		return retVal;
	}

	boost::shared_ptr<CapFloorTermVolSurface> makeCapFloorVolaSurface(std::vector<VolaObs> surface)
	{
		std::map<Real, int> strikes = std::map<Real, int>();
		std::map<Period, int> optionTenors = std::map<Period, int>();
		int iStrike = 0;
		int iTerm = 0;
		for (int i = 0; i < surface.size(); i++)
		{
			VolaObs ob = surface[i];
			double strike = ob.strike;
			Period term = Period(ob.nTermUnits, ob.termUnit);

			std::map<Real, int>::iterator foundStrike = strikes.find(strike);
			if (foundStrike == strikes.end())
				strikes.insert(std::make_pair(strike, iStrike++));

			std::map<Period, int>::iterator foundTenor = optionTenors.find(term);
			if (foundTenor == optionTenors.end())
				optionTenors.insert(std::make_pair(term, iTerm++));
		}

		Matrix termV = Matrix(optionTenors.size(), strikes.size());
		for (int i = 0; i < surface.size(); i++)
		{
			double strike = surface[i].strike;
			Period term = Period(surface[i].nTermUnits, surface[i].termUnit);
			iStrike = strikes[strike];
			iTerm = optionTenors[term];
			termV[iTerm][iStrike] = surface[i].vola;
		}

		int settlementDays = 0; // for vola settlementdays =0
		Calendar calendar = TARGET();
		BusinessDayConvention bdc = ModifiedFollowing;
		DayCounter dcc = Actual365Fixed();

		std::vector<Real> vStrikes;
		for (std::map<Real, int>::iterator it = strikes.begin(); it != strikes.end(); ++it)
		{
			vStrikes.push_back(it->first);
		}
		std::vector<Period> vTenors;
		for (std::map<Period, int>::iterator it = optionTenors.begin(); it != optionTenors.end(); ++it)
		{
			vTenors.push_back(it->first);
		}
		return boost::shared_ptr<CapFloorTermVolSurface>(new CapFloorTermVolSurface(settlementDays, calendar, bdc, vTenors, vStrikes, termV, dcc));
	}

	MarketData::MarketData()
	{
		std::ifstream input("MarketDataDepoEUR.xml");
		depositData = readDepoObsFromXml(input, "vectorDepoObs");
		input.close();
		std::ifstream input2("MarketDataOISwapEUR.xml");
		eoniaSwapData = readDepoObsFromXml(input2, "vectorDepoObs");
		input2.close();
		std::ifstream input3("MarketDataLibor6MSwapEUR.xml");
		swapData = readSwapObsFromXml(input3, "vectorSwapObs");
		input3.close();
		std::ifstream input4("MarketDataFlatCFVolaEUR.xml");
		volSurfaceData = readVolaFromXml(input4, "vectorVolaObs");
		boost::shared_ptr<CapFloorTermVolSurface> bvs = makeCapFloorVolaSurface(volSurfaceData);
		mdContainer.insert(make_pair(std::string("EURCapFlatVolaNormal"), bvs));
		std::ifstream input5("MarketDataZeroYield6MEUR.xml");
		zeroBondData = readSwapObsFromXml(input5, "vectorSwapObs");
		MarketConventions mc;
		boost::shared_ptr<YieldTermStructure> zyc = makeZeroYieldCurve(zeroBondData, mc);
		mdContainer2.insert(make_pair(std::string("EURZeroYields"), zyc));
		input5.close();

		//std::vector<Rate> sstrikes = bvs->strikes();
		//std::vector<Rate>::iterator it1;
		//for (it1 = sstrikes.begin(); it1 != sstrikes.end(); it1++) {
		//	strikes.push_back(*it1);
		//}

		//std::vector<Period> ooptionTenors = bvs->optionTenors();
		//std::vector<Period>::iterator it2;
		//for (it2 = ooptionTenors.begin(); it2 != ooptionTenors.end(); it2++) {
		//	optionTenors.push_back(*it2);
		//}

		//optionTenors.resize(10);
		//for (Size i = 0; i < optionTenors.size(); ++i)
		//	optionTenors[i] = Period(i + 1, Years);

		/*Volatility flatVol = .18;

		std::vector<Handle<Quote> >  curveVHandle(bvs->optionTenors().size());
		for (Size i = 0; i<bvs->optionTenors().size(); ++i)
			curveVHandle[i] = Handle<Quote>(boost::shared_ptr<Quote>(new
				SimpleQuote(flatVol)));

		int settlementDays = 0; // for vola settlementdays =0
		Calendar calendar = TARGET();
		BusinessDayConvention bdc = ModifiedFollowing;
		DayCounter dcc = Actual365Fixed();

		atmVols = Handle<CapFloorTermVolCurve>(
		boost::shared_ptr<CapFloorTermVolCurve>(new CapFloorTermVolCurve(0, calendar, bdc, bvs->optionTenors(),	curveVHandle, dcc)));*/
	}

}
