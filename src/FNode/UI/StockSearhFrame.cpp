#include "StockSearhFrame.h"
#include "StockDatabase.h"
#include <QDate>
#include <QDebug>

StockSearchFrame::StockSearchFrame(QWidget* parent /*= nullptr*/)
	:QFrame(parent)
{
	initUI();
}

StockSearchFrame::~StockSearchFrame()
{

}

void StockSearchFrame::initUI()
{
	QDate date = QDate::currentDate();

	mainLayout_ = new QVBoxLayout;
	mainLayout_->setContentsMargins(0, 0, 0, 0);
	mainLayout_->setSpacing(0);
	this->setLayout(mainLayout_);

	searchLayout_ = new QHBoxLayout;
	searchLayout_->setContentsMargins(0, 0, 0, 0);
	searchLayout_->setSpacing(0);
	mainLayout_->addLayout(searchLayout_);


	leYear_ = new QLineEdit(this);
	leYear_->setPlaceholderText(tr("Year"));
	leYear_->setText(QString::number(date.year()-1));

	leExpPrefitRate_ = new QLineEdit(this);
	leExpPrefitRate_->setPlaceholderText(tr("ExpPrefitRate"));
	leExpPrefitRate_->setText("0.05");

	btnSearch_ = new QPushButton(tr("Search"), this);

	searchLayout_->addWidget(leYear_);
	searchLayout_->addWidget(leExpPrefitRate_);
	searchLayout_->addStretch(1);
	searchLayout_->addWidget(btnSearch_);


	twResult_ = new QTableWidget(this);
	mainLayout_->addWidget(twResult_, 1);

	connect(btnSearch_, &QPushButton::clicked,
		this, &StockSearchFrame::onSearchClicked);
}

void StockSearchFrame::onSearchClicked()
{
	int year = leYear_->text().toInt();
	double expPerfitRate = leExpPrefitRate_->text().toDouble();
	QList<StockBrief> stockList = StockDataBase::getInstance()->getStockList();
	QMap<QString, StockDividend> sds = StockDataBase::getInstance()->selectAllStockDividend(year);
	QMap<QString, StockDividend> sdsOld = StockDataBase::getInstance()->selectAllStockDividend(year-1);
	QMap<QString, StockPrePrice> sps = StockDataBase::getInstance()->selectAllStockPrePrice();
	QList<StockSearchRowData> datas;
	for (auto iter = stockList.begin();
		iter != stockList.end();
		iter++)
	{
		StockSearchRowData rowData;
		QString stockId = iter->id;
		if (sps.find(stockId) == sps.end()
			|| sds.find(stockId) == sds.end()
			|| sdsOld.find(stockId) == sdsOld.end())
		{
			continue;
		}

		StockPrePrice sp = sps.find(stockId).value();
		StockDividend sd = sds.find(stockId).value();
		StockDividend sdOld = sdsOld.find(stockId).value();
		if (sd.getProfitSavRate() >= 0.0)
		{
			rowData.stockId = stockId;
			rowData.profitSavRat = sd.getProfitSavRate();
			rowData.oldProfitSavRat = sdOld.getProfitSavRate();
			rowData.pureProfitGrowth = sd.getPureProfit() / sdOld.getPureProfit();

			rowData.expPrice = sd.getExpectStockPrice(expPerfitRate);
			rowData.curPrice = sp.getPrePrice();
			rowData.expandRate = rowData.expPrice / rowData.curPrice;
			rowData.curProfitRate = sd.getProfitRate(rowData.curPrice);
			if (rowData.expandRate >= 1.0
				//&& rowData.profitSavRat >= 0.75 * rowData.oldProfitSavRat
				&& rowData.pureProfitGrowth >= 0.75
				&& sd.getPureProfit() > 0.0)
			{
				datas.append(rowData);
			}
		}
	}

	std::sort(datas.begin(), datas.end(), [](const StockSearchRowData& d1, const StockSearchRowData& d2)->bool {
		return d1.curProfitRate > d2.curProfitRate;
	});

	twResult_->clear();
	twResult_->setColumnCount(7);
	twResult_->setRowCount(datas.size());
	twResult_->setHorizontalHeaderItem(0,  new QTableWidgetItem(tr("StockId")));
	twResult_->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("ProfitRate")));
	twResult_->setHorizontalHeaderItem(2,  new QTableWidgetItem(tr("ProfitSavRate")));
	twResult_->setHorizontalHeaderItem(3,  new QTableWidgetItem(tr("PreProfitSavRate")));
	twResult_->setHorizontalHeaderItem(4,  new QTableWidgetItem(tr("ProfitGrowth")));
	twResult_->setHorizontalHeaderItem(5,  new QTableWidgetItem(tr("ExpandRate")));
	twResult_->setHorizontalHeaderItem(6,  new QTableWidgetItem(tr("CurPrice")));
	
	int rowIdx = 0;
	for (auto iter = datas.begin();
		iter != datas.end();
		iter++)
	{
		twResult_->setItem(rowIdx, 0,  new QTableWidgetItem(iter->stockId));
		twResult_->setItem(rowIdx, 1, new QTableWidgetItem(QString::number(iter->curProfitRate)));

		twResult_->setItem(rowIdx, 2, new QTableWidgetItem(QString::number(iter->profitSavRat)));
		twResult_->setItem(rowIdx, 3, new QTableWidgetItem(QString::number(iter->oldProfitSavRat)));
		twResult_->setItem(rowIdx, 4, new QTableWidgetItem(QString::number(iter->pureProfitGrowth)));

		twResult_->setItem(rowIdx, 5, new QTableWidgetItem(QString::number(iter->expandRate)));
		twResult_->setItem(rowIdx, 6, new QTableWidgetItem(QString::number(iter->curPrice)));
		rowIdx++;
	}

}
