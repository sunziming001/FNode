#ifndef _STOCK_DIVIDEND_H
#define _STOCK_DIVIDEND_H

#include <QString>

class StockDividend
{
public:
	StockDividend();
	~StockDividend();
	
	QString getStockSecId() const;
	double getProfitSavRate()const;
	
	QString getStockId() const { return stockId_; }
	void setStockId(QString val) { stockId_ = val; }
	
	int getReportYear() const { return reportYear_; }
	void setReportYear(int val) { reportYear_ = val; }
	
	double getPureProfit() const { return pureProfit_; }
	void setPureProfit(double val) { pureProfit_ = val; }
	
	double getPerStockProfit() const { return perStockProfit_; }
	void setPerStockProfit(double val) { perStockProfit_ = val; }
	
	double getTotalDividend() const { return totalDividend_; }
	void setTotalDividend(double val) { totalDividend_ = val; }

	bool isValid()const;
	double getExpectStockPrice(double expectRate)const;
	double getProfitRate(double price);

	bool operator<(const StockDividend& sd)const;
private:
	QString stockId_;
	int reportYear_ = 0;
	double pureProfit_ = 0.0;		///<净利润
	double perStockProfit_ = 0.0;	///<每股收益
	double totalDividend_ = 0.0;	///<现金股利
};

#endif