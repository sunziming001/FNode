#include "StockHoldersTask.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "StockDatabase.h"

QString StockHoldersTask::holdersUrlFormat="https://datacenter.eastmoney.com/securities/api/data/v1/get?reportName=RPT_F10_EH_HOLDERNUM&columns=SECUCODE%2CSECURITY_CODE%2CEND_DATE%2CHOLDER_TOTAL_NUM%2CTOTAL_NUM_RATIO%2CAVG_FREE_SHARES%2CAVG_FREESHARES_RATIO%2CHOLD_FOCUS%2CPRICE%2CAVG_HOLD_AMT%2CHOLD_RATIO_TOTAL%2CFREEHOLD_RATIO_TOTAL&quoteColumns=&filter=(%1)&pageNumber=1&pageSize=10&sortTypes=-1&sortColumns=END_DATE&source=HSF10&client=PC&v=09594843330339957";

StockHoldersTask::StockHoldersTask(QObject* parent /*= nullptr*/)
{

}

StockHoldersTask::~StockHoldersTask()
{

}

QString StockHoldersTask::getHoldersUrl() const
{
	QString secCode = stockHolders_.getSecuCode();
	return holdersUrlFormat.arg(secCode);
}

void StockHoldersTask::setStockHolders(const StockHolders& v)
{
	stockHolders_ = v;
}

StockHolders StockHoldersTask::getStockHolders() const
{
	return stockHolders_;
}


void StockHoldersTask::onExecute()
{
	startHoldersSumTask();
}


void StockHoldersTask::startHoldersSumTask()
{
	QNetworkRequest req;
	QString url = getHoldersUrl();
	req.setUrl(url);

	QNetworkAccessManager* mgr = NetTaskManager::getInstance()->getNetworkAccessManager();
	holdersSumReply_ = mgr->get(req);
	connect(holdersSumReply_, &QNetworkReply::finished, this, [url, this]() {
		QString jsonStr = holdersSumReply_->readAll();
		QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
		QJsonObject joRoot = doc.object();
		bool isSuccess = false;
		isSuccess = joRoot["success"].toBool(false);
		if(isSuccess){
			QJsonObject joResult = joRoot["result"].toObject();
			QJsonArray jarr = joResult["data"].toArray();
			QList<QPair<QString, double>> listDate2Rate;
			QString holdFocus;
			for (int i = 0; i < jarr.count(); i++)
			{
				QPair<QString, double> pair;
				QJsonObject jo = jarr[i].toObject();
				pair.first = jo["END_DATE"].toString();
				pair.second = jo["TOTAL_NUM_RATIO"].toDouble();
				
				listDate2Rate.push_back(pair);
				if (i == 0)
				{
					holdFocus = jo["HOLD_FOCUS"].toString();
				}
			}
			
			qSort(listDate2Rate.begin(), listDate2Rate.end(), [](const QPair<QString, double>& p1, const QPair<QString, double>& p2)->bool {
				return p1.first > p2.first;
			});

			int startIndex = 0;
			int obsCnt = 3;
			if (listDate2Rate.size() >= startIndex+ obsCnt)
			{
				stockHolders_.setLastChange(listDate2Rate[startIndex].second);

				if (listDate2Rate[startIndex].second < 0
					&& listDate2Rate[startIndex+1].second < 0
					&& listDate2Rate[startIndex+2].second < 0)
				{
					
					qDebug() << stockHolders_.getStockId() <<" "<< holdFocus;
					for (int i = startIndex; i < startIndex+3; i++)
					{
						qDebug() << listDate2Rate[i].first<<": "<<listDate2Rate[i].second;
					}
					qDebug() << "======";
				}
				
			}
			
		}
		emit finished(false, "");

	});
}

