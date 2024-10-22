#include "MainWindow.h"
#include <QMessageBox>
#include "StockDataBase.h"
#include "NetTaskManager.h"
#include "StockDividendTask.h"
#include "StockPrePriceTask.h"
#include "DividendFrame.h"
#include "StockPriceTask.h"
#include <QDate>
#include "StockPriceView.h"
#include "HoldersFrame.h"
#include "StockHoldersTask.h"

#define DIVIDEND_VIEW_ID 1
#define PRICE_VIEW_ID 2
#define HOLDERS_VIEW_ID 3

MainWindow::MainWindow(QWidget* parent)
	:QFrame(parent)
{
	this->setWindowFlags(Qt::Window);
	initUI();
}

MainWindow::~MainWindow()
{

}

void MainWindow::initUI()
{
	mainLayout_ = new QVBoxLayout();
	this->setLayout(mainLayout_);

	initTabLayout();
	initStackedLayout();
	initProgressLayout();

	initDividendFrame();
	initPriceFrame();
	initHolderFrame();
}

void MainWindow::initTabLayout()
{
	tabLayout_ = new QHBoxLayout;
	mainLayout_->addLayout(tabLayout_);

	cbGroup_ = new QButtonGroup(this);
	cbDividen_ = new QCheckBox(tr("Dividend"), this);
	cbPrice_ = new QCheckBox(tr("Price"), this);
	cbHolders_ = new QCheckBox(tr("Holders"), this);

	cbGroup_->addButton(cbDividen_, DIVIDEND_VIEW_ID);
	cbGroup_->addButton(cbPrice_, PRICE_VIEW_ID);
	cbGroup_->addButton(cbHolders_, HOLDERS_VIEW_ID);
	cbGroup_->setExclusive(true);

	cbDividen_->setChecked(true);


	tabLayout_->addWidget(cbDividen_);
	tabLayout_->addWidget(cbPrice_);
	tabLayout_->addWidget(cbHolders_);
	tabLayout_->addStretch(1);

	connect(cbGroup_, SIGNAL(buttonToggled(int,bool)), this, SLOT(onSwitchView(int,bool)));
}

void MainWindow::initStackedLayout()
{
	stackedLayout_ = new QStackedLayout;
	mainLayout_->addLayout(stackedLayout_);
}

void MainWindow::initDividendFrame()
{
	dividendFrame_ = new DividendFrame(this);
	stackedLayout_->addWidget(dividendFrame_);
}


void MainWindow::initPriceFrame()
{
	priceFrame_ = new StockPriceView(this);
	stackedLayout_->addWidget(priceFrame_);
}

void MainWindow::initHolderFrame()
{
	holderFrame_ = new HoldersFrame(this);
	stackedLayout_->addWidget(holderFrame_);
}

void MainWindow::onSwitchView(int id, bool checked)
{
	if (checked)
	{
		switch (id)
		{
		case DIVIDEND_VIEW_ID:
			stackedLayout_->setCurrentWidget(dividendFrame_);
			break;
		case PRICE_VIEW_ID:
			stackedLayout_->setCurrentWidget(priceFrame_);
			break;
		case HOLDERS_VIEW_ID:
			stackedLayout_->setCurrentWidget(holderFrame_);
			break;
		default:
			break;
		}
	}
}

void MainWindow::initProgressLayout()
{
	lbProgress_ = new QLabel(this);
	mainLayout_->addWidget(lbProgress_);
	freshProgress();
}


void MainWindow::startCrawel(int year)
{
	if (totalTaskCnt_ > 0)
	{
		QMessageBox::warning(this, tr("warning"), tr("wait all task finished."));
		return;
	}

	if (year > 0)
	{
		QList<QString> stockList = StockDataBase::getInstance()->getStockList();
		for (auto iter = stockList.begin(); iter != stockList.end(); iter++)
		{
			QString stockId = *iter;
			StockDividend sd;
			sd.setStockId(stockId);
			sd.setReportYear(year);
			StockDividendTask *tsk = new StockDividendTask(this);
			tsk->setStockDividend(sd);

			totalTaskCnt_++;

			
			connect(tsk, &StockDividendTask::finished, this, [this](bool isError, QString errStr) {
				finishedTasksCnt_++;
				if (isError)
				{
					qWarning() << errStr;
					//QMessageBox::warning(this, tr("warning"), errStr);
				}
				freshProgress();
			});
			NetTaskManager::getInstance()->pushTask(tsk);
		}
		
	}
}

void MainWindow::startPrePrice()
{
	if (totalTaskCnt_ > 0)
	{
		QMessageBox::warning(this, tr("warning"), tr("wait all task finished."));
		return;
	}

	QList<QString> stockList = StockDataBase::getInstance()->getStockList();
	for (auto iter = stockList.begin(); iter != stockList.end(); iter++)
	{
		QString stockId = *iter;
		StockPrePrice sp;
		sp.setStockId(stockId);
		StockPrePriceTask* tsk = new StockPrePriceTask(this);
		tsk->setStockPrePrice(sp);

		totalTaskCnt_++;


		connect(tsk, &StockPrePriceTask::finished, this, [this](bool isError, QString errStr) {
			finishedTasksCnt_++;
			if (isError)
			{
				QMessageBox::warning(this, tr("warning"), errStr);
			}
			freshProgress();
		});
		NetTaskManager::getInstance()->pushTask(tsk);
	}
}

void MainWindow::clearPrePrice()
{
	StockDataBase::getInstance()->clearStockPrePrice();
}

void MainWindow::startPrice()
{
	if (totalTaskCnt_ > 0)
	{
		QMessageBox::warning(this, tr("warning"), tr("wait all task finished."));
		return;
	}

	QList<QString> stockList = StockDataBase::getInstance()->getStockList();
	for (auto iter = stockList.begin(); iter != stockList.end(); iter++)
	{
		QString stockId = *iter;
		
		StockPriceTask* tsk = new StockPriceTask(this);
		tsk->setStockId(stockId);

		totalTaskCnt_++;


		connect(tsk, &StockPrePriceTask::finished, this, [this](bool isError, QString errStr) {
			finishedTasksCnt_++;
			if (isError)
			{
				QMessageBox::warning(this, tr("warning"), errStr);
			}
			freshProgress();
		});
		NetTaskManager::getInstance()->pushTask(tsk);
	}
}

void MainWindow::clearPrice()
{
	StockDataBase::getInstance()->clearStockPrice();
}


void MainWindow::startCrawelHolders()
{
	if (totalTaskCnt_ > 0)
	{
		QMessageBox::warning(this, tr("warning"), tr("wait all task finished."));
		return;
	}

	QList<QString> stockList = StockDataBase::getInstance()->getStockList();
	for (auto iter = stockList.begin(); iter != stockList.end(); iter++)
	{
		QString stockId = *iter;

		StockHoldersTask* tsk = new StockHoldersTask(this);
		StockHolders holders;
		holders.setStockId(stockId);

		tsk->setStockHolders(holders);

		totalTaskCnt_++;


		connect(tsk, &StockHoldersTask::finished, this, [this](bool isError, QString errStr) {
			finishedTasksCnt_++;
			if (isError)
			{
				QMessageBox::warning(this, tr("warning"), errStr);
			}
			freshProgress();
		});
		NetTaskManager::getInstance()->pushTask(tsk);
	}
	freshProgress();
}

void MainWindow::freshProgress()
{
	if (lbProgress_)
	{
		if (finishedTasksCnt_ == totalTaskCnt_)
		{
			finishedTasksCnt_ = 0;
			totalTaskCnt_ = 0;
			lbProgress_->setText(tr("no tasks"));
		}
		else {
			QString text = QString::number(finishedTasksCnt_) + "/" + QString::number(totalTaskCnt_);
			lbProgress_->setText(text);
		}
		
	}
}