#ifndef LOANTEST_H__ // #include guards
#define LOANTEST_H__

#include <ql/time/all.hpp>
//#include <ql/quantlib.hpp>
namespace BootStrapper {



    class Loans {
    public:
        static void testLoan(QuantLib::Date startDate, QuantLib::Date maturityDate, QuantLib::Frequency freq);
        static void testLoan2(QuantLib::Date startDate, QuantLib::Date maturityDate, QuantLib::Period period);

    };


} 
#endif 
