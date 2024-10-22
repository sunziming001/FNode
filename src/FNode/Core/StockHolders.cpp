#include "StockHolders.h"

StockHolders::StockHolders()
{

}

StockHolders::~StockHolders()
{

}

QString StockHolders::getSecuCode() const
{
	QString header = "SECUCODE%3D%22";
	QString tail = ".SZ";
	if (stockId_.startsWith("688")
		|| stockId_.startsWith("60"))
	{
		tail = ".SH";
	}

	return header + stockId_ + tail + "%22";
}
