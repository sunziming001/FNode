#ifndef _STOCK_PRE_PRICE_H
#define _STOCK_PRE_PRICE_H
#include <QString>

class StockPrePrice
{
public:
	StockPrePrice();
	~StockPrePrice();

	QString getStockSecid()const;
	QString getStockId() const { return stockId_; }
	void setStockId(QString val) { stockId_ = val; }


	double getPrePrice() const { return prePrice_; }
	void setPrePrice(double val) { prePrice_ = val; }

	bool isValid()const;
private:
	QString stockId_;
	double prePrice_ = -1.0;
};


#endif