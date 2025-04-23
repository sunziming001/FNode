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
	initSettings();
	initUI();
	initTimers();
}

MainWindow::~MainWindow()
{

}

void MainWindow::initSettings()
{
	settings_ = new QSettings("config.ini",QSettings::IniFormat, this);
	settings_->beginGroup("proxy");
	proxyHost_ = settings_->value("host").toString();
	proxyPort_ = settings_->value("port").toUInt();
	settings_->endGroup();
}

void MainWindow::initUI()
{
	mainLayout_ = new QVBoxLayout();
	this->setLayout(mainLayout_);


	initSettingLayout();
	initTabLayout();
	
	initStackedLayout();
	initProgressLayout();

	initDividendFrame();
	initPriceFrame();
	initHolderFrame();

	initTrayIcon();
}

void MainWindow::initTimers()
{
	priceTimer_.setSingleShot(false);
	priceTimer_.setInterval(1000*60 );

	connect(&priceTimer_, &QTimer::timeout, this, &MainWindow::onPriceTimeOut);

	priceTimer_.start();
}

void MainWindow::initSettingLayout()
{
	settingLayout_ = new QHBoxLayout();

	cbAutoTask_ = new QCheckBox(tr("Auto"), this);
	cbUseProxy_ = new QCheckBox(tr("Proxy"), this);

	settingLayout_->addWidget(cbAutoTask_);
	settingLayout_->addWidget(cbUseProxy_);
	settingLayout_->addStretch(1);

	mainLayout_->addLayout(settingLayout_);

	cbAutoTask_->setChecked(true);
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

void MainWindow::initTrayIcon()
{
	QAction* showAction = nullptr;
	QAction* hideAction = nullptr;
	QAction* exitAction = nullptr;
	trayIcon_ = new QSystemTrayIcon(QIcon(":/image/coin.ico"), this);

	trayIconMenu_ = new QMenu();
	showAction = trayIconMenu_->addAction(tr("show"));
	hideAction = trayIconMenu_->addAction(tr("hide"));
	exitAction = trayIconMenu_->addAction(tr("exit"));
	trayIcon_->setContextMenu(trayIconMenu_);
	trayIcon_->show();

	connect(showAction, &QAction::triggered, this, [this]() {
		this->show();
	});

	connect(hideAction, &QAction::triggered, this, [this]() {
		this->hide();
	});

	connect(exitAction, &QAction::triggered, this, [this]() {
		this->close();
	});
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

void MainWindow::onPriceTimeOut()
{
	QDateTime curDateTime = QDateTime::currentDateTime();
	int dayOfWeek = curDateTime.date().dayOfWeek();
	int hour = curDateTime.time().hour();
	int minutes = curDateTime.time().minute();
	bool isEndOfDay = (hour == 15 && minutes == 0);

	if (dayOfWeek > 5 || !cbAutoTask_->isChecked())
	{
		return;
	}
		

	if (isEndOfDay)
	{
		priceOnceConnect_ = new QMetaObject::Connection;
			
		*priceOnceConnect_=	connect(this, &MainWindow::sigTaskFinished, this, [this]() {

			priceFrame_->getNegativeJ();

			QObject::disconnect(*priceOnceConnect_);
			delete priceOnceConnect_;
			priceOnceConnect_ = nullptr;
		},Qt::QueuedConnection);

		priceFrame_->setIsDay(true);
		priceFrame_->setIsWeek(false);
		priceFrame_->setIsMonth(false);
		priceFrame_->setIsSeason(false);
		clearPrice();
		startPrice();
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
		QList<StockBrief> stockList = StockDataBase::getInstance()->getStockList();
		for (auto iter = stockList.begin(); iter != stockList.end(); iter++)
		{
			QString stockId = iter->id;
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

	QList<StockBrief> stockList = StockDataBase::getInstance()->getStockList();
	for (auto iter = stockList.begin(); iter != stockList.end(); iter++)
	{
		QString stockId = iter->id;
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

	QList<StockBrief> stockList = StockDataBase::getInstance()->getStockList();
	for (auto iter = stockList.begin(); iter != stockList.end(); iter++)
	{
		QString stockId = iter->id;
		KTypes kTypes = priceFrame_->getKTypes();
		if (iter->market <= 110.0 * 100000000
			&& iter->market>0.0)
		{
			continue;
		}

		IterateKTypes(kTypes, [this, stockId](KType kType) {
			pushPriceTask(stockId, kType);
		});
		

	}
}

void MainWindow::clearPrice()
{
	KTypes types = priceFrame_->getKTypes();
	StockDataBase::getInstance()->clearStockPrice(types);
}


void MainWindow::startCrawelHolders()
{
	if (totalTaskCnt_ > 0)
	{
		QMessageBox::warning(this, tr("warning"), tr("wait all task finished."));
		return;
	}

	QList<StockBrief> stockList = StockDataBase::getInstance()->getStockList();
	for (auto iter = stockList.begin(); iter != stockList.end(); iter++)
	{
		QString stockId = iter->id;

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
			emit sigTaskFinished();
		}
		else {
			QString text = QString::number(finishedTasksCnt_) + "/" + QString::number(totalTaskCnt_);
			lbProgress_->setText(text);
		}
		
	}
}

void MainWindow::pushPriceTask(const QString& stockId, KType kType)
{

	StockPriceTask* tsk = new StockPriceTask(this);
	tsk->setStockId(stockId);
	tsk->setKType(kType);
	tsk->setUrlSource(StockPriceTask::UrlSource::EastMoney);
	if (cbUseProxy_->isChecked())
	{
		tsk->setSocks5Proxy(proxyHost_, proxyPort_);
	}
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

void MainWindow::closeEvent(QCloseEvent* e)
{
	if (this->isVisible())
	{
		this->hide();
		e->ignore();
	}
	else {
		e->accept();
	}
	
}
