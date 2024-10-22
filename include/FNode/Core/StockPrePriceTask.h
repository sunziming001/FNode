#ifndef _STOCK_PRE_PRICE_TASK_H
#define _STOCK_PRE_PRICE_TASK_H

#include "NetTaskManager.h"
#include <QNetworkReply>
#include "StockPrePrice.h"

class StockPrePriceTask :public AbstractNetTask
{
public:
	StockPrePriceTask(QObject* parent = nullptr);
	~StockPrePriceTask();

	QString getPrePriceUrl()const;

	StockPrePrice getStockPrePrice() const { return stockPrePrice_; }
	void setStockPrePrice(StockPrePrice val) { stockPrePrice_ = val; }

	void notifyFinish(bool isError = false, QString errInfo = QString());
protected:
	void onExecute()override;
private:
	void startPrePriceRequest();
private:
	static QString PrePriceUrlFormat;
	StockPrePrice stockPrePrice_;

	QNetworkReply* prePriceReply_ = nullptr;
};

#endif
