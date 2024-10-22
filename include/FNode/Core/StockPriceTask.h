#ifndef _STOCK_PRICE_TASK_H
#define _STOCK_PRICE_TASK_H

#include "NetTaskManager.h"
#include <QNetworkReply>
#include "StockPrice.h"

class StockPriceTask :public AbstractNetTask
{
public:
	StockPriceTask(QObject* parent = nullptr);
	~StockPriceTask();

	QString getPriceUrl()const;
	void setStockId(QString stockId);

	void notifyFinish(bool isError = false, QString errInfo = QString());
private:
	void onExecute()override;

	void startQuest();
private:
	static QString PriceUrlFormat;
	QString stockId_;

	QNetworkReply* priceReply_ = nullptr;
};


#endif