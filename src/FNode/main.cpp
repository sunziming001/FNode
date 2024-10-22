#include <QApplication>
#include <QTranslator>
#include "MainWindow.h"
#include "NetTaskManager.h"
#include "StockDividendTask.h"
#include "StockPrePriceTask.h"
#include "StockDatabase.h"

void test()
{
	QMap<QString,StockDividend> sds =StockDataBase::getInstance()->selectAllStockDividend(2021);
	QMap<QString, StockPrePrice> sps =StockDataBase::getInstance()->selectAllStockPrePrice();
	QList<QString> stockIds = StockDataBase::getInstance()->getStockList();
	QList<StockDividend> ret;
	for (auto iter = stockIds.begin(); iter != stockIds.end(); iter++)
	{
		QString stockId = *iter;
		if (sps.find(stockId) == sps.end()
			|| sds.find(stockId) == sds.end())
		{
			continue;
		}

		StockPrePrice sp = sps.find(stockId).value();
		StockDividend sd = sds.find(stockId).value();
		

		double expPrice = sd.getExpectStockPrice(0.15);
		double truePrice = sp.getPrePrice();

		if (expPrice > truePrice
			&& sd.getProfitSavRate()>0.0)
		{
			ret.append(sd);
			qDebug() << stockId << " " << (expPrice / truePrice);
		}
	}
}

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	QTranslator translator;
	translator.load(QApplication::applicationDirPath() + "\\fnode_zh.qm");
	app.installTranslator(&translator);

	NetTaskManager::getInstance()->init();
	StockDataBase::getInstance();


	MainWindow mw;
	mw.setMinimumSize(800, 600);
	mw.show();

	app.exec();
	return 0;
}