#include "StockPrePriceTask.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "StockDatabase.h"

QString StockPrePriceTask::PrePriceUrlFormat="http://push2.eastmoney.com/api/qt/stock/details/get?fields1=f1,f2,f3,f4&fields2=f51,f52,f53,f54,f55&fltt=2&pos=-11&secid=%1";

StockPrePriceTask::StockPrePriceTask(QObject* parent /*= nullptr*/)
{

}

StockPrePriceTask::~StockPrePriceTask()
{

}

QString StockPrePriceTask::getPrePriceUrl() const
{
	return PrePriceUrlFormat.arg(stockPrePrice_.getStockSecid());
}

void StockPrePriceTask::notifyFinish(bool isError /*= false*/, QString errInfo /*= QString()*/)
{
	emit finished(isError, errInfo);
}

void StockPrePriceTask::onExecute()
{
	if (!StockDataBase::getInstance()->hasStockPrePrice(stockPrePrice_.getStockId()))
	{
		startPrePriceRequest();
	}
	else {
		notifyFinish();
	}
	

}


void StockPrePriceTask::startPrePriceRequest()
{
	QNetworkRequest req;
	QString url = getPrePriceUrl();
	req.setUrl(url);

	QNetworkAccessManager* mgr = NetTaskManager::getInstance()->getNetworkAccessManager();
	prePriceReply_ = mgr->get(req);
	connect(prePriceReply_, &QNetworkReply::finished, this, [this]() {
		QString jsonStr = prePriceReply_->readAll();
		QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
		QJsonObject joRoot = doc.object();
		QJsonObject joData = joRoot["data"].toObject();
		double prePrice = joData["prePrice"].toDouble(-1.0f);

		if (prePriceReply_->error() == QNetworkReply::NoError)
		{
			stockPrePrice_.setPrePrice(prePrice);
			if (stockPrePrice_.isValid())
			{
				StockDataBase::getInstance()->insertStockPrePrice(stockPrePrice_);
				notifyFinish();
			}
			else {
				notifyFinish(true, "Price parse error, stock: " + stockPrePrice_.getStockId());
			}

		}
		else {
			notifyFinish(true, prePriceReply_->errorString());
		}

		});
}