#ifndef _STOCK_PRICE_TASK_H
#define _STOCK_PRICE_TASK_H

#include "NetTaskManager.h"
#include <QNetworkReply>
#include "StockPrice.h"



class StockPriceTask :public AbstractNetTask
{
public:
	enum class UrlSource
	{
		EastMoney,
		Baidu,
	};
public:
	StockPriceTask(QObject* parent = nullptr);
	~StockPriceTask();

	QString getPriceUrl(UrlSource urlSource = UrlSource::EastMoney)const;
	void setStockId(QString stockId);

	void notifyFinish(bool isError = false, QString errInfo = QString());
	void setKType(KType t);
	void setUrlSource(UrlSource src);
	void setSocks5Proxy(const QString& host, quint16 port);
private:
	void onExecute()override;

	void startQuest();
	
	QString getPriceUrl_EastMoney()const;
	QList<StockPrice> parseReplay_EastMoney()const;

	QString getPriceUrl_Baidu()const;
	QList<StockPrice> parseReplay_Baidu()const;
private:
	static QString PriceUrlFormat_EastMoney;
	static QString PriceUrlFormat_Baidu;
	QString stockId_;

	QNetworkReply* priceReply_ = nullptr;
	KType kType_ = KType::Day;
	UrlSource urlSource_ = UrlSource::Baidu;

	QString proxyHost_;
	quint16 proxyPort_=0;
};


#endif