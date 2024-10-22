#include "StockPrePrice.h"


StockPrePrice::StockPrePrice()
{

}

StockPrePrice::~StockPrePrice()
{

}

QString StockPrePrice::getStockSecid() const
{
	QString header = "0.";
	if (stockId_.startsWith("688")
		|| stockId_.startsWith("60"))
	{
		header = "1.";
	}
	return header + stockId_;
}

bool StockPrePrice::isValid() const
{
	return !stockId_.isEmpty() && prePrice_ > 0.0;
}

