#ifndef _STOCK_HOLDERS_TASK_H
#define _STOCK_HOLDERS_TASK_H

#include "NetTaskManager.h"
#include <QNetworkReply>
#include "StockHolders.h"

class StockHoldersTask:public AbstractNetTask
{
public:
	StockHoldersTask(QObject* parent = nullptr);
	~StockHoldersTask();

	QString getHoldersUrl()const;
	void setStockHolders(const StockHolders&);
	StockHolders getStockHolders()const;

protected:
	void onExecute() override;
private:
	void startHoldersSumTask();
private:
	StockHolders stockHolders_;
	QNetworkReply* holdersSumReply_ = nullptr;
	static QString holdersUrlFormat;
};

#endif