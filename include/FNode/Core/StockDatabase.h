#ifndef _STOCK_DATA_BASE_H
#define _STOCK_DATA_BASE_H

#include <QSqlDatabase>
#include "StockDividend.h"
#include "StockPrePrice.h"
#include "StockPrice.h"
#include <QString>
#include <QList>
#include <QMap>

class StockDataBase {
private:
	static StockDataBase* instance;
	StockDataBase();
public:
	~StockDataBase();
	static StockDataBase* getInstance();

	void insertStockDividend(const StockDividend& data);
	bool hasStockDividend(const StockDividend& data);
	QMap<QString,StockDividend> selectAllStockDividend(int year);
	QList<QString> getStockList()const;

	bool hasStockPrePrice(const QString& stockId);
	void insertStockPrePrice(const StockPrePrice& data);
	void clearStockPrePrice();
	QMap<QString, StockPrePrice> selectAllStockPrePrice();

	void clearStockPrice();
	void insertStockPrice(const QList<StockPrice>& data);
	bool hasStockPrice(const QString& stockId);
	QList<StockPrice> selectStockPriceById(const QString& stockId);
	QList<StockPrice> selectStockPriceByDate(const QString& date);
private:
	void initStockList();
	void initDatabasFile();
	void initFromStockListFile(const char* fileName);
private:
	QSqlDatabase db_;
	QList<QString> stockList_;
};

#endif