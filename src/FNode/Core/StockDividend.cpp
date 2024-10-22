#include "StockDividend.h"

StockDividend::StockDividend()
{

}

StockDividend::~StockDividend()
{

}

QString StockDividend::getStockSecId() const
{
	QString header = "SZ";
	if(stockId_.startsWith("688")
		|| stockId_.startsWith("60"))
	{
		header = "SH";
	}
	return header+stockId_;
}

double StockDividend::getProfitSavRate() const
{
	return (pureProfit_ - totalDividend_) / pureProfit_;
}

bool StockDividend::isValid() const
{
	return !stockId_.isEmpty()
		&& reportYear_ != 0
		&& pureProfit_ != 0.0
		&& perStockProfit_ != 0.0;
}

double StockDividend::getExpectStockPrice(double expectRate) const
{
	double stockCnt = 0.0;
	double perStockProfitRate = 0.0;
	if (pureProfit_ <= 0 || perStockProfit_<=0 || totalDividend_<=0)
	{
		return -99999.0;
	}
	else
	{
		stockCnt = pureProfit_ / perStockProfit_;	///<总股数
		perStockProfitRate = totalDividend_ / stockCnt;	///<每股股利
	}


	return perStockProfitRate / expectRate;
}

double StockDividend::getProfitRate(double price)
{
	double stockCnt = 0.0;
	double perStockProfitRate = 0.0;
	if (pureProfit_ <= 0 || perStockProfit_ <= 0 || totalDividend_ <= 0)
	{
		return -99999.0;
	}
	else {
		stockCnt = pureProfit_ / perStockProfit_;	///<总股数
		perStockProfitRate = totalDividend_ / stockCnt;	///<每股股利
	}
	return perStockProfitRate / price;
}

bool StockDividend::operator<(const StockDividend& sd) const
{
	return getExpectStockPrice(0.05) > sd.getExpectStockPrice(0.05);
}
