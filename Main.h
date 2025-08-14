#pragma once
#include "Market/MarketData.hpp"

void LiborCashflows(const QuantLib::Date& asOfDate, BootStrapper::MarketData& md, BootStrapper::MarketConventions& mc, bool& success);

void RFRCashflows(const QuantLib::Date& asOfDate, BootStrapper::MarketData& md, BootStrapper::MarketConventions& mc, bool& success);
