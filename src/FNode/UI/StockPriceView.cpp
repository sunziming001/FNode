#include "StockPriceView.h"
#include "MainWindow.h"
#include "StockDatabase.h"
#include <QDateTime>
#include <QDir>

StockPriceView::StockPriceView(QWidget* parent /*= nullptr*/)
	:QFrame(parent)
{
	initUI();
}

StockPriceView::~StockPriceView()
{
	if (analysisThread_.joinable())
	{
		analysisThread_.join();
	}
	
}

void StockPriceView::initUI()
{
	mainLayout_ = new QVBoxLayout;
	mainLayout_->setContentsMargins(0, 0, 0, 0);
	mainLayout_->setSpacing(0);
	this->setLayout(mainLayout_);

	initCtrlLayout();
	initConsoleOutput();

	mainLayout_->addStretch(1);
}

void StockPriceView::initCtrlLayout()
{
	MainWindow* mainWnd = dynamic_cast<MainWindow*>(parent());
	QDate date = QDate::currentDate();

	ctrlLayout_ = new QHBoxLayout;
	mainLayout_->addLayout(ctrlLayout_);

	btnCrawlPrice_ = new QPushButton(tr("CrawlPrice"), this);
	cbUseDay_ = new QCheckBox(tr("Day"), this);
	cbUseWeek_ = new QCheckBox(tr("Week"), this);
	cbUseMonth_ = new QCheckBox(tr("Month"), this);
	cbUseSeason_ = new QCheckBox(tr("Season"), this);

	btnClearPrice_ = new QPushButton(tr("ClearPrice"), this);
	btnAnalysis_ = new QPushButton(tr("Analysis"), this);
	btnGetNegativeJ_ = new QPushButton(tr("NegativeJ"), this);
	btnGetBuy2_ = new QPushButton(tr("Buy2"), this);

	btnRank_ = new QPushButton(tr("Rank"), this);
	leDuration_ = new QLineEdit(this);
	leDate_ = new QLineEdit(this);

	leDate_->setPlaceholderText(tr("Date:yyyy-MM-dd"));
	leDate_->setText(date.toString("yyyy-MM-dd"));
	leDate_->setFixedWidth(70);

	leDuration_->setPlaceholderText(tr("DayCnt"));
	leDuration_->setText("20");
	leDuration_->setFixedWidth(35);

	ctrlLayout_->addWidget(btnCrawlPrice_);
	ctrlLayout_->addWidget(cbUseDay_);
	ctrlLayout_->addWidget(cbUseWeek_);
	ctrlLayout_->addWidget(cbUseMonth_);
	ctrlLayout_->addWidget(cbUseSeason_);
	ctrlLayout_->addWidget(btnClearPrice_);
	ctrlLayout_->addWidget(btnAnalysis_);
	ctrlLayout_->addWidget(btnGetNegativeJ_);
	ctrlLayout_->addWidget(btnGetBuy2_);
	ctrlLayout_->addSpacing(5);
	ctrlLayout_->addWidget(leDate_);
	ctrlLayout_->addWidget(leDuration_);
	ctrlLayout_->addWidget(btnRank_);

	ctrlLayout_->addStretch(1);


	bgWeekMonth_.addButton(cbUseSeason_);
	bgWeekMonth_.addButton(cbUseWeek_);
	bgWeekMonth_.addButton(cbUseMonth_);
	bgWeekMonth_.addButton(cbUseDay_);
	bgWeekMonth_.setExclusive(false);
	

	if (mainWnd)
	{
		connect(btnCrawlPrice_, &QPushButton::clicked, mainWnd, &MainWindow::startPrice);
		connect(btnClearPrice_, &QPushButton::clicked, mainWnd, &MainWindow::clearPrice);
	}
	connect(btnAnalysis_, &QPushButton::clicked, this, &StockPriceView::startAnalysis);
	connect(btnGetNegativeJ_, &QPushButton::clicked, this, &StockPriceView::getNegativeJ);
	connect(btnRank_, &QPushButton::clicked, this, &StockPriceView::startRank);
	connect(btnGetBuy2_, &QPushButton::clicked, this, &StockPriceView::getBuy2);
	
}

void StockPriceView::initConsoleOutput()
{
	consoleOutput_ = new QTextEdit(this);
	consoleOutput_->setFixedHeight(300);
	consoleOutput_->setReadOnly(true);
	mainLayout_->addWidget(consoleOutput_);

	connect(this, &StockPriceView::sigAppendOutput, this, &StockPriceView::onAppendOutput, Qt::QueuedConnection);
	connect(this, &StockPriceView::sigClearOutput, consoleOutput_, &QTextEdit::clear, Qt::QueuedConnection);
	connect(this, &StockPriceView::sigSaveOutput, this, &StockPriceView::onSaveOutput, Qt::QueuedConnection);
}

void StockPriceView::startAnalysis()
{
	
	analysisThread_ = std::thread([this]() {
		btnAnalysis_->setEnabled(false);
		QString fileName = "analysis_";
		fileName += QDateTime::currentDateTime().toString("yyyy_MM_dd_hh");
		fileName += ".txt";
		emit sigClearOutput();

		emit sigAppendOutput("start analysis...");
		QList<QString> stockList = StockDataBase::getInstance()->getStockList();
		int matchCnt = 0;
		int winCnt = 0;
		for (auto iter = stockList.begin(); iter != stockList.end(); iter++)
		{
			QString stockId = *iter;
			QList<StockPrice> prices = StockDataBase::getInstance()->selectStockPriceById(stockId,KType::Day);
			int indx = prices.size() - 1;
			if(prices.size()==0)
				continue;
			
			int watchDur = 10;
			bool needHistory = false;
			do
			{
				bool isAroundMa = true;
				float highest = 0.0;
				float lowest = 0.0;
				double sumChangeRate = StockPrice::GetSumChangeRate(prices, indx, 10);
				const StockPrice& curPrice = prices[indx];

				for (int i = 0; i < watchDur; i++)
				{
					const StockPrice& tempPrice = prices[indx - i];
					double cur_ma_10 = StockPrice::GetMA(prices, indx - i, watchDur);

					if (tempPrice.getLowPrice() >=cur_ma_10)
					{
						isAroundMa = false;
						break;
					}

					if (highest < tempPrice.getHighPrice())
					{
						highest = tempPrice.getHighPrice();
					}

					if (lowest == 0.0 || lowest > tempPrice.getLowPrice())
					{
						lowest = tempPrice.getLowPrice();
					}

				}


				if (isAroundMa && sumChangeRate > 100 && highest<=1.15*lowest)
				{
					QString str = curPrice.getStockId()
						+ " h/l: " + QString::number(highest/lowest)
						+ " volume: " + QString::number(curPrice.getVolume()/(curPrice.getChangeRate()/100.0))
						+ " date: " + curPrice.getDate()
						+ " change rate sum: " + QString::number(sumChangeRate);
					emit sigAppendOutput(str);
					needHistory = true;

				}
				indx--;
				if(!needHistory)
					break;
			} while (indx>=60);

			
			
		}
		emit sigAppendOutput("analysis over");
		emit sigSaveOutput(fileName);
		btnAnalysis_->setEnabled(true);
	});
	analysisThread_.detach();
}

void StockPriceView::startRank()
{
	int dayLimit = 20;
	int topRankCnt = 15;
	emit sigClearOutput();
	QMap<QString, int> mapStockId2Cnt;
	QMap<QString, int> mapOldStockId2Cnt;
	QMap<QString, int> mapOldOldStockId2Cnt;
	QList<QPair<QString, int>> retList;
	QDate curDate = QDate::currentDate();
	int outputCnt = 15;

	if (!leDate_->text().isEmpty())
	{
		curDate = QDate::fromString(leDate_->text(), "yyyy-MM-dd");
	}

	if (!leDuration_->text().isEmpty())
	{
		dayLimit = leDuration_->text().toInt();
	}

	mapStockId2Cnt = getTopVolumeStock(curDate, dayLimit, outputCnt,4.5,true);
	mapOldStockId2Cnt = getTopVolumeStock(curDate.addDays(-1* dayLimit), dayLimit, outputCnt);
	mapOldOldStockId2Cnt = getTopVolumeStock(curDate.addDays(-2 * dayLimit), dayLimit, outputCnt);

	for (auto iter = mapStockId2Cnt.begin(); iter != mapStockId2Cnt.end(); iter++)
	{
		retList.append(qMakePair(iter.key(), iter.value()));
	}

	qStableSort(retList.begin(), retList.end(), [](const QPair<QString, int>& p1, const QPair<QString, int>& p2)->bool {
		return p1.second > p2.second;
	});
	for (auto iter = retList.begin(); iter != retList.end(); iter++)
	{
		int oldCnt = 0;
		int oldOldCnt = 0;
		if (mapOldStockId2Cnt.find(iter->first) != mapOldStockId2Cnt.end())
		{
			oldCnt = mapOldStockId2Cnt[iter->first];
		}

		if (mapOldOldStockId2Cnt.find(iter->first) != mapOldOldStockId2Cnt.end())
		{
			oldOldCnt = mapOldOldStockId2Cnt[iter->first];
		}

		QString str = iter->first + ": "+ QString::number(oldOldCnt) + " -> "+QString::number(oldCnt)+" -> " + QString::number(iter->second);
		emit sigAppendOutput(str);
	}

}

void StockPriceView::getNegativeJ()
{
	getNegativeJThread_ = std::thread([this]() {
		getNegativeJImp();
	});
	getNegativeJThread_.detach();
}

void StockPriceView::getBuy2()
{
	getBuy2Thread_ = std::thread([this]() {
		getBuy2Imp();
	});
	getBuy2Thread_.detach();
}

void StockPriceView::setIsDay(bool v)
{
	cbUseDay_->setChecked(v);
}

void StockPriceView::setIsWeek(bool v)
{
	cbUseWeek_->setChecked(v);
}

void StockPriceView::setIsMonth(bool v)
{
	cbUseMonth_->setChecked(v);
}

void StockPriceView::setIsSeason(bool v)
{
	cbUseSeason_->setChecked(v);
}

KTypes StockPriceView::getKTypes() const
{
	KTypes types;

	if (this->isDay())
	{
		types |= (KType::Day);
	}

	if (this->isWeek())
	{
		types |= (KType::Week);
	}

	if (this->isMonth())
	{
		types |= (KType::Month);
	}

	if (this->isSeason())
	{
		types |= (KType::Season);
	}

	return types;
}

bool StockPriceView::isDay() const
{
	return cbUseDay_->isChecked();
}

bool StockPriceView::isWeek() const
{
	return cbUseWeek_->isChecked();
}

bool StockPriceView::isMonth() const
{
	return cbUseMonth_->isChecked();
}

bool StockPriceView::isSeason() const
{
	return cbUseSeason_->isChecked();
}

void StockPriceView::onAppendOutput(const QString& str)
{
	if (consoleOutput_)
	{
		consoleOutput_->append(str);
	}
}

void StockPriceView::onSaveOutput(const QString& fileName)
{
	QDir().mkdir("record");
	QFile file(QString("record\\") + fileName);
	file.open(QIODevice::ReadWrite);

	file.write(consoleOutput_->toPlainText().toUtf8());

	file.close();
}

QMap<QString, int> StockPriceView::getTopVolumeStock(const QDate& endDate, int dayCnt, int outputCnt, double minChangeRate /*= 4.5*/, bool hasOutput /*= false*/)
{
	QMap<QString, int> ret;
	QDate curDate = endDate;
	int dayLimit = dayCnt;
	
	for (int curDay = 0; curDay < dayLimit; curDay++)
	{
		QString dateString = curDate.toString("yyyy-MM-dd");
		int retCnt = 0;
		int trueRank = 0;
		QList<StockPrice> prices = StockDataBase::getInstance()->selectStockPriceByDate(dateString,KType::Day);
		std::stable_sort(prices.begin(), prices.end(), [this](const StockPrice& p1, const StockPrice& p2)-> bool {
			return p1.getVolume() > p2.getVolume();
		});

		for (auto iter = prices.begin(); iter != prices.end(); iter++)
		{
			trueRank++;
			if (iter->getChangeRate() <= 4.5)
				continue;

			ret[iter->getStockId()]++;

			retCnt++;
			
			if (hasOutput && dayLimit -curDay== dayCnt)
			{
				QString str = "No." + QString::number(trueRank)+" stockId: "+iter->getStockId()+" volume: "+QString::number(iter->getVolume());
				emit sigAppendOutput(str);
			}

			if (retCnt >= outputCnt)
			{
				break;
			}
		}
		if (prices.size() <= 0)
		{
			dayLimit++;
		}
		curDate = curDate.addDays(-1);
	}

	return ret;
}

void StockPriceView::getNegativeJImp()
{
	btnGetNegativeJ_->setEnabled(false);
	QString fileName = "NegativeJ_";
	KTypes kTypes = getKTypes();
	IterateKTypes(kTypes, [this, &fileName](KType kType) {
		fileName += KTypeToShortString(kType);
	});
	fileName += "_";

	fileName += QDateTime::currentDateTime().toString("yyyy_MM_dd_hh");
	fileName += ".txt";

	emit sigClearOutput();
	emit sigAppendOutput("start get negative J...");
	emit sigAppendOutput("the smaller J the better.");
	emit sigAppendOutput("the bigger market the better.");
	emit sigAppendOutput("the bigger MaUpTrend the better.");
	emit sigAppendOutput("changeRate not over 10%.");
	QList<QString> stockList = StockDataBase::getInstance()->getStockList();
	int matchCnt = 0;
	int winCnt = 0;
	for (auto iter = stockList.begin(); iter != stockList.end(); iter++)
	{
		QString stockId = *iter;
		bool isFirst = true;
		IterateKTypes(kTypes, [this, stockId,&isFirst](KType kType) {
			procNegativeJ(stockId, kType,isFirst);
		});


	}

	emit sigAppendOutput("get negative J over");
	emit sigSaveOutput(fileName);
	btnGetNegativeJ_->setEnabled(true);
}

void StockPriceView::getBuy2Imp()
{
	btnGetBuy2_->setEnabled(false);
	QString fileName = "Buy2_";
	fileName += QDateTime::currentDateTime().toString("yyyy_MM_dd_hh");
	fileName += ".txt";
	emit sigClearOutput();
	emit sigAppendOutput("start get buy 2...");
	QList<QString> stockList = StockDataBase::getInstance()->getStockList();
	int matchCnt = 0;
	int winCnt = 0;
	for (auto iter = stockList.begin(); iter != stockList.end(); iter++)
	{
		QString stockId = *iter;
		bool isFirst = true;
		procBuy2(stockId, isFirst);

	}

	emit sigSaveOutput(fileName);
	btnGetBuy2_->setEnabled(true);
}

void StockPriceView::procNegativeJ(const QString& stockId, KType kType,bool& isFirst)
{
	double maUpTrend = 0.0;
	QList<StockPrice> price = StockDataBase::getInstance()->selectStockPriceById(stockId, kType);
	QList<std::tuple<double, double, double>> kdj = StockPrice::GetKDJ(price);
	if (kdj.size() > 0)
	{
		double J = std::get<2>(kdj.last());
		if (J < 0.0)
		{
			QList<double> sma20 = StockPrice::GetSMA20(price);
			double marketValue = price.last().getMarketValue();
			double changeRate = price.last().getChangeRate();
			QString output = stockId
				+ " " + KTypeToShortString(kType)
				+ " J: " + QString::number(J, 'f', 2)
				+ ", market: " + QString::number(marketValue / 100000000.0, 'f', 2)
				+ ", changeRate: " + QString::number(changeRate * 100, 'f', 2) + "%"
				;
			if (sma20.size() >= 2)
			{
				double last = sma20.last();
				double prev = sma20.at(sma20.size() - 2);
				maUpTrend = (last - prev) / prev;
				output += ", MaUpTrend: " + QString::number(maUpTrend * 100, 'f', 2) + "%";
			}

			if (maUpTrend > 0.0
				|| isMonth()
				|| isWeek()
				|| isSeason())
			{
				if (isFirst)
				{
					QString fitKString;
					IterateKTypes(outputedKTypes_, [&fitKString](KType k) {
						fitKString+=KTypeToShortString(k);
					});
					emit sigAppendOutput("====="+fitKString);
					isFirst = false;
					outputedKTypes_ = KType::None;
				}
				outputedKTypes_ |= kType;
				emit sigAppendOutput(output);
				
			}

		}
	}
}

void StockPriceView::procBuy2(const QString& stockId, bool& isFirst)
{
	double maUpTrend = 0.0;
	QList<StockPrice> price = StockDataBase::getInstance()->selectStockPriceById(stockId, KType::Day);
	/*QList<int> breakIdxes = StockPrice::GetBuy2Index(price);
	if (breakIdxes.size() > 0)
	{
		isFirst = false;
		emit sigAppendOutput(stockId);
		for (int i = 0; i < breakIdxes.size(); i++)
		{
			emit sigAppendOutput(price[breakIdxes[i]].getDate());
		}
	}*/
	bool isPriceBreak = StockPrice::isPriceBreak(price, price.size() - 1, 15);
	if (isPriceBreak)
	{
		emit sigAppendOutput(stockId+" breaked!!!");
	}
}
