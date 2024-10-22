#ifndef _STOCK_HOLDERS_H
#define _STOCK_HOLDERS_H

#include <QString>

class StockHolders
{
public:
	StockHolders();
	~StockHolders();
	QString getSecuCode()const;
	double getLastChange() const { return lastChange_; }
	void setLastChange(double val) { lastChange_ = val; }

	double getAverageChange() const { return averageChange_; }
	void setAverageChange(double val) { averageChange_ = val; }

	QString getStockId() const { return stockId_; }
	void setStockId(QString val) { stockId_ = val; }
private:

	QString stockId_;
	double lastChange_;
	double averageChange_;
};


#endif