#include "StockDividendTask.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "StockDatabase.h"

QString StockDividendTask::FinicalUrlFormat = "https://emweb.securities.eastmoney.com/PC_HSF10/NewFinanceAnalysis/ZYZBAjaxNew?type=1&code=%1";

//QString StockDividendTask::BonusUrlFormat="https://emweb.securities.eastmoney.com/PC_HSF10/BonusFinancing/PageAjax?code=%1";
/*!
BonusUrlFormat ref from https://emweb.securities.eastmoney.com/PC_HSF10/FinanceAnalysis/Index?type=web&code=SZ002013
*/
QString StockDividendTask::BonusUrlFormat = "https://emweb.securities.eastmoney.com/PC_HSF10/BonusFinancing/PageAjax?code=%1";


StockDividendTask::StockDividendTask(QObject* parent /*= nullptr*/)
{

}

StockDividendTask::~StockDividendTask()
{

}

QString StockDividendTask::getFinicalUrl() const
{
	return FinicalUrlFormat.arg(stockDividend_.getStockSecId());
}

QString StockDividendTask::getBonusUrl() const
{
	return BonusUrlFormat.arg(stockDividend_.getStockSecId());
}

void StockDividendTask::onExecute()
{
	if (!StockDataBase::getInstance()->hasStockDividend(stockDividend_))
	{
		startFinicalTask();
	}
	else {
		notifyFinish();
	}
	
}


void StockDividendTask::startFinicalTask()
{
	QNetworkRequest req;
	QString url = getFinicalUrl();
	//url = "https://emweb.securities.eastmoney.com/PC_HSF10/NewFinanceAnalysis/ZYZBAjaxNew?type=1&code=SH600295";
	req.setUrl(url);
	QNetworkAccessManager* mgr = NetTaskManager::getInstance()->getNetworkAccessManager();
	finicalReply_ = mgr->get(req);
	connect(finicalReply_, &QNetworkReply::finished, this, [url,this]() {
		QString jsonStr = finicalReply_->readAll();
		QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
		QJsonObject joRoot = doc.object();
		QJsonArray joArray = joRoot["data"].toArray();
		bool foundDividend = false;
		for (int i = 0; i < joArray.size(); i++)
		{
			QJsonObject jo = joArray[i].toObject();
			int year = jo["REPORT_YEAR"].toString().toInt();
			QString reportName = jo["REPORT_DATE_NAME"].toString();
			QString targetName = QString::number(stockDividend_.getReportYear()) + QString::fromLocal8Bit("年报");

			if (QString::compare(reportName,targetName, Qt::CaseInsensitive)==0)
			{
				double perStockProfit = jo["EPSJB"].toDouble();//每股收益
				double pureProfit = jo["PARENTNETPROFIT"].toDouble(); //净利润

				stockDividend_.setPerStockProfit(perStockProfit);
				stockDividend_.setPureProfit(pureProfit);
				foundDividend = true;
				break;
			}
		}
		if (finicalReply_->error() == QNetworkReply::NoError)
		{
			if (foundDividend)
			{
				startBonusTask();
			}
			else {
				notifyFinish(true, stockDividend_ .getStockSecId()+" no dividend found");
				
			}
			
		}
		else {
			notifyFinish(true, finicalReply_->errorString());
		}
		
	});

}

void StockDividendTask::startBonusTask()
{
	QNetworkRequest req;
	QString url = getBonusUrl();
	req.setUrl(url);
	QNetworkAccessManager* mgr = NetTaskManager::getInstance()->getNetworkAccessManager();
	BonusReply_ = mgr->get(req);
	connect(BonusReply_, &QNetworkReply::finished, this, [this,url]() {
		QString jsonStr = BonusReply_->readAll();
		QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
		QJsonObject joRoot = doc.object();
		QJsonArray joArray = joRoot["fhyx"].toArray();
		bool foundBonus = false;
		for (int i = 0; i < joArray.size(); i++)
		{
			QJsonObject jo = joArray[i].toObject();
			QString date = jo["NOTICE_DATE"].toString();
			QString plan = jo["IMPL_PLAN_PROFILE"].toString();
			if (date.contains(QString::number(stockDividend_.getReportYear() + 1)))
			{
				int idxStart = plan.indexOf(QString::fromLocal8Bit("派"));
				int idxEnd = plan.indexOf(QString::fromLocal8Bit("元"));
				double sendMoney = 0.0;
				double cnt = stockDividend_.getPureProfit() / stockDividend_.getPerStockProfit();
				bool foundPlan = false;
				if (idxStart >= 0 && idxEnd >= 0)
				{
					QString money = plan.mid(idxStart + 1, idxEnd - idxStart-1);
					sendMoney = money.toDouble();
					foundPlan = true;

				}
				sendMoney /= 10.0f;


				double totalDividend = sendMoney*cnt;//现金股利
				stockDividend_.setTotalDividend(totalDividend);
				foundBonus = true;

				if(foundPlan)
					break;
			}
		}

		if (BonusReply_->error() == QNetworkReply::NoError)
		{
			if (!foundBonus)
			{
				curType_--;
				if (curType_ >= 0)
				{
					startBonusTask();
				}
				else {
					notifyFinish(true, stockDividend_.getStockSecId()+" no bonus found");
				}
				
			}
			else {
				saveToSqlite();
			}
			
		}
		else {
			notifyFinish(true, BonusReply_->errorString());
		}
		
	});

}

void StockDividendTask::notifyFinish(bool isError, QString errInfo)
{
	emit finished(isError, errInfo);
}

void StockDividendTask::saveToSqlite()
{
	if (stockDividend_.isValid())
	{
		StockDataBase::getInstance()->insertStockDividend(stockDividend_);
		notifyFinish();
	}
	else {
		notifyFinish(true, "invalid stock dividend of stock "+ stockDividend_.getStockId());
	}
	
}

