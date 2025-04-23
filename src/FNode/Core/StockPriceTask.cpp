#include "StockPriceTask.h"
#include "StockDatabase.h"
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>
#include <QMessageBox>
#include <QNetworkProxy>

QString StockPriceTask::PriceUrlFormat_EastMoney="https://16.push2his.eastmoney.com/api/qt/stock/kline/get?secid=%s.%s&ut=fa5fd1943c7b386f172d6893dbfba10b&fields1=f1%2Cf2%2Cf3%2Cf4%2Cf5%2Cf6&fields2=f51%2Cf52%2Cf53%2Cf54%2Cf55%2Cf56%2Cf57%2Cf58%2Cf59%2Cf60%2Cf61&klt=%s&fqt=1&end=20500101&lmt=120&_=1686031957995";
//ref https://quote.eastmoney.com/sh600839.html

QString StockPriceTask::PriceUrlFormat_Baidu = "https://finance.pae.baidu.com/vapi/v1/getquotation?srcid=%d&pointType=string&group=quotation_kline_ab&query=%s&code=%s&market_type=ab&newFormat=1&is_kc=0&ktype=%s&finClientType=pc";
// ref https://gushitong.baidu.com/stock/ab-600161?name=%25E5%25A4%25A9%25E5%259D%259B%25E7%2594%259F%25E7%2589%25A9


StockPriceTask::StockPriceTask(QObject* parent /*= nullptr*/)
	:AbstractNetTask(parent)
{

}

StockPriceTask::~StockPriceTask()
{

}

QString StockPriceTask::getPriceUrl(UrlSource urlSource) const
{
	QString ret;
	switch (urlSource)
	{
	case UrlSource::Baidu:
		ret = getPriceUrl_Baidu();
		break;
	case UrlSource::EastMoney:
	default:
		ret = getPriceUrl_EastMoney();
		break;
	}

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

void StockPriceTask::setUrlSource(UrlSource src)
{
	urlSource_ = src;
}

void StockPriceTask::setSocks5Proxy(const QString& host, quint16 port)
{
	proxyHost_ = host;
	proxyPort_ = port;
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
	QString url = getPriceUrl(urlSource_);
	req.setUrl(url);

	QNetworkAccessManager* mgr = NetTaskManager::getInstance()->getNetworkAccessManager();
	QNetworkProxy proxy;
	if (!proxyHost_.isEmpty())
	{
		proxy.setType(QNetworkProxy::Socks5Proxy);
		proxy.setHostName(proxyHost_);
		proxy.setPort(proxyPort_);
		mgr->setProxy(proxy);
	}
	

	priceReply_ = mgr->get(req);

	connect(priceReply_, &QNetworkReply::finished, this, [this]() {

		if (priceReply_->error() == QNetworkReply::NoError)
		{
			QList<StockPrice> prices;
			switch (urlSource_)
			{
			case UrlSource::EastMoney:
				prices = parseReplay_EastMoney();
				break;
			case UrlSource::Baidu:
				prices = parseReplay_Baidu();
				break;
			default:
				break;
			}
			StockDataBase::getInstance()->insertStockPrice(prices, kType_);
			notifyFinish();
		}
		else {
			notifyFinish(true, priceReply_->errorString());
		}

	});
}

QString StockPriceTask::getPriceUrl_EastMoney() const
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

	ret.sprintf(PriceUrlFormat_EastMoney.toStdString().c_str(), header.toStdString().c_str(), stockId_.toStdString().c_str(), kTypeStr.toStdString().c_str());
	return ret;
}

QList<StockPrice> StockPriceTask::parseReplay_EastMoney() const
{
	QString jsonStr = priceReply_->readAll();
	QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
	QJsonObject joRoot = doc.object();
	QJsonObject joData = joRoot["data"].toObject();
	QJsonArray klineArr = joData["klines"].toArray();
	int cnt = klineArr.size();
	QList<StockPrice> prices;
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
			curPrice.setChangeRate(dataList[10].toDouble() / 100);

			prices.append(curPrice);
		}
	}
	return prices;

}

QString StockPriceTask::getPriceUrl_Baidu() const
{
	QString ret;
	QString kTypeStr = "day";
	unsigned int id = rand() % 100 + 5000;
	switch (kType_)
	{
	case KType::Day:
		kTypeStr = "day";
		break;
	case KType::Week:
		kTypeStr = "week";
		break;
	case KType::Month:
		kTypeStr = "month";
		break;
	case KType::Season:
		kTypeStr = "quarter";
		break;
	default:
		break;
	}

	ret.sprintf(PriceUrlFormat_Baidu.toStdString().c_str(),
		id,
		stockId_.toStdString().c_str(),
		stockId_.toStdString().c_str(),
		kTypeStr.toStdString().c_str());
	return ret;
}

QList<StockPrice> StockPriceTask::parseReplay_Baidu() const
{
	QList<StockPrice> ret;
	QString jsonStr = priceReply_->readAll();
	QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
	QJsonObject joRoot = doc.object();
	QJsonObject joResult = joRoot["Result"].toObject();
	QJsonObject joNewData = joResult["newMarketData"].toObject();
	QJsonArray joKeys = joNewData["keys"].toArray();
	QString joMarketData = joNewData["marketData"].toString();
	QStringList marketDataList = joMarketData.split(";");
	int dateIdx = -1;
	int openIdx = -1;
	int closeIdx = -1;
	int highIdx = -1;
	int lowIdx = -1;
	int turnOverIdx = -1;
	int volumeIdx = -1;
	int changeRateIdx = -1;
	int foundCnt = 0;
	//parse keys
	for (int i = 0; i < joKeys.count(); i++)
	{
		QString curKey = joKeys.at(i).toString();
		if (curKey == "time") {
			dateIdx = i;
			foundCnt++;
		}
		else if (curKey == "open") {
			openIdx = i;
			foundCnt++;
		}
		else if (curKey == "close") {
			closeIdx = i;
			foundCnt++;
		}
		else if (curKey == "volume") {
			turnOverIdx = i;
			foundCnt++;
		}
		else if (curKey == "high") {
			highIdx = i;
			foundCnt++;
		}
		else if (curKey == "low") {
			lowIdx = i;
			foundCnt++;
		}
		else if (curKey == "amount") {
			volumeIdx = i;
			foundCnt++;
		}
		else if (curKey == "turnoverratio") {
			changeRateIdx = i;
			foundCnt++;
		}
	}

	if (foundCnt < 8)
	{
		QMessageBox::warning(0, "Error", "Baidu Json key error!");
		return ret;
	}

	auto iter = marketDataList.begin();
	if (marketDataList.size() >= 120)
	{
		iter = marketDataList.end() - 120;
	}
	while(iter != marketDataList.end())
	{
		QString oneData = *iter;
		StockPrice curPrice;
		curPrice.setStockId(stockId_);
		int curIdx = 0;
		QStringList dataList= oneData.split(",");

		curPrice.setDate(dataList[dateIdx]);
		curPrice.setOpenPrice(dataList[openIdx].toDouble());
		curPrice.setClosePrice(dataList[closeIdx].toDouble());
		curPrice.setHighPrice(dataList[highIdx].toDouble());
		curPrice.setLowPrice(dataList[lowIdx].toDouble());
		curPrice.setTurnOver(dataList[turnOverIdx].toDouble());
		curPrice.setVolume(dataList[volumeIdx].toDouble() / 10000.0);
		curPrice.setChangeRate(dataList[changeRateIdx].toDouble()/10000.0);
		
		ret.append(curPrice);
		iter++;
	}

	return ret;
}

