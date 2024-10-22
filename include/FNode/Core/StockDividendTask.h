#ifndef _STOCK_DIVIDEND_TASK_H
#define _STOCK_DIVIDEND_TASK_H

#include "NetTaskManager.h"
#include <QNetworkReply>
#include "StockDividend.h"

class StockDividendTask :public AbstractNetTask
{
public:
	StockDividendTask(QObject* parent = nullptr);
	~StockDividendTask();


	QString getFinicalUrl()const;
	QString getBonusUrl()const;
	void saveToSqlite();

	StockDividend getStockDividend() const { return stockDividend_; }
	void setStockDividend(StockDividend val) { stockDividend_ = val; }

protected:
	void onExecute()override;
private:
	void startFinicalTask();
	void startBonusTask();
	
	void notifyFinish(bool isError=false, QString errInfo=QString());
private:
	StockDividend stockDividend_;

	double expectStockDividend_ = 0.05;

	QNetworkReply* finicalReply_ = nullptr;
	QNetworkReply* BonusReply_ = nullptr;

	static QString FinicalUrlFormat;
	static QString BonusUrlFormat;
	int curType_ = 4;
};


#endif