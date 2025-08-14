#ifndef UTILS_H__
#define UTILS_H__
#include <ql/time/schedule.hpp>
#include <ql/cashflow.hpp>
#include <ql/instruments/swap.hpp>
#include <ql/instruments/bond.hpp>
#include <ql/instruments/overnightindexedswap.hpp>
//#include <boost/shared_ptr.hpp>

//#include "BootStrapper.hpp"
#include <string>
namespace BootStrapper
{
#define LENGTH(a) (sizeof(a)/sizeof(a[0]))

    void printSchedule(std::string header, const QuantLib::Schedule s);
    void printCashflows(QuantLib::Leg cf, QuantLib::Bond bond);
    void printOISwap(QuantLib::Schedule s, QuantLib::OvernightIndexedSwap swap);


}
#endif

