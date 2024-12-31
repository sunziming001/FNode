#include "StockPriceTask.h"
#include "StockDatabase.h"
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

QString StockPriceTask::PriceUrlFormat="https://16.push2his.eastmoney.com/api/qt/stock/kline/get?secid=%s.%s&ut=fa5fd1943c7b386f172d6893dbfba10b&fields1=f1%2Cf2%2Cf3%2Cf4%2Cf5%2Cf6&fields2=f51%2Cf52%2Cf53%2Cf54%2Cf55%2Cf56%2Cf57%2Cf58%2Cf59%2Cf60%2Cf61&klt=%s&fqt=1&end=20500101&lmt=120&_=1686031957995";
//ref https://quote.eastmoney.com/sh600839.html
StockPriceTask::StockPriceTask(QObject* parent /*= nullptr*/)
	:AbstractNetTask(parent)
{

}

StockPriceTask::~StockPriceTask()
{

}

QString StockPriceTask::getPriceUrl() const
{
	QString ret;
	QString header = "0";
	QString kTypeStr = "101";
	if (stockId_.startsWith("688")
		|| stockId_.startsWith("60"))
	{
		header = "1";
	}

	switch (kType_)
	{
	case KType::Day:
		kTypeStr = "101";
		break;
	case KType::Week:
		kTypeStr = "102";
		break;
	case KType::Month:
		kTypeStr = "103";
		break;
	case KType::Season:
		kTypeStr = "104";
		break;
	default:
		break;
	}

	ret.sprintf(PriceUrlFormat.toStdString().c_str(), header.toStdString().c_str(), stockId_.toStdString().c_str(), kTypeStr.toStdString().c_str());
	return ret;
}

void StockPriceTask::setStockId(QString stockId)
{
	stockId_ = stockId;
}

void StockPriceTask::notifyFinish(bool isError /*= false*/, QString errInfo /*= QString()*/)
{
	emit finished(isError, errInfo);
}

void StockPriceTask::setKType(KType t)
{
	kType_ = t;
}

void StockPriceTask::onExecute()
{
	if (!StockDataBase::getInstance()->hasStockPrice(stockId_, kType_))
	{
		startQuest();
	}
	else {
		notifyFinish();
	}
}

void StockPriceTask::startQuest()
{
	QNetworkRequest req;
	QString url = getPriceUrl();
	req.setUrl(url);

	QNetworkAccessManager* mgr = NetTaskManager::getInstance()->getNetworkAccessManager();
	priceReply_ = mgr->get(req);

	connect(priceReply_, &QNetworkReply::finished, this, [this]() {
		QString jsonStr = priceReply_->readAll();
		QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
		QJsonObject joRoot = doc.object();
		QJsonObject joData = joRoot["data"].toObject();
		QJsonArray klineArr = joData["klines"].toArray();
		int cnt = klineArr.size();
		QList<StockPrice> prices;
		
		if (priceReply_->error() == QNetworkReply::NoError)
		{
			for (int i = 0; i < cnt; i++)
			{
				StockPrice curPrice;
				curPrice.setStockId(stockId_);

				QString line = klineArr.at(i).toString();
				QStringList dataList = line.split(",");
				if (dataList.size() >= 11)
				{
					curPrice.setDate(dataList[0]);
					curPrice.setOpenPrice(dataList[1].toDouble());
					curPrice.setClosePrice(dataList[2].toDouble());
					curPrice.setHighPrice(dataList[3].toDouble());
					curPrice.setLowPrice(dataList[4].toDouble());
					curPrice.setTurnOver(dataList[5].toDouble());
					curPrice.setVolume(dataList[6].toDouble());
					curPrice.setChangeRate(dataList[10].toDouble()/100);

					prices.append(curPrice);
				}
			}

			StockDataBase::getInstance()->insertStockPrice(prices, kType_);
			notifyFinish();
		}
		else {
			notifyFinish(true, priceReply_->errorString());
		}

	});
}

