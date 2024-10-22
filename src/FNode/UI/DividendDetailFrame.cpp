#include "DividendDetailFrame.h"
#include "StockDatabase.h"
#include <QDate>

DividendDetailFrame::DividendDetailFrame(QWidget* parent /*= nullptr*/)
	:QFrame(parent)
{
	initUI();
}

DividendDetailFrame::~DividendDetailFrame()
{

}

void DividendDetailFrame::initUI()
{
	mainLayout_ = new QVBoxLayout;
	mainLayout_->setSpacing(0);
	mainLayout_->setContentsMargins(0, 0, 0, 0);
	this->setLayout(mainLayout_);

	searchLayout_ = new QHBoxLayout;
	mainLayout_->addLayout(searchLayout_);

	leStockId_ = new QLineEdit(this);
	leStockId_->setPlaceholderText(tr("StockId"));
	
	btnSearchStock_ = new QPushButton(tr("search"),this);

	searchLayout_->addWidget(leStockId_);
	searchLayout_->addStretch(1);
	searchLayout_->addWidget(btnSearchStock_);

	detailLayout_ = new QVBoxLayout;
	mainLayout_->addLayout(detailLayout_);

	lbDetail_ = new QLabel(this);
	lbDetail_->setTextFormat(Qt::RichText);
	detailLayout_->addWidget(lbDetail_,1,Qt::AlignTop);

	mainLayout_->addStretch(0);

	connect(btnSearchStock_, &QPushButton::clicked, this, &DividendDetailFrame::onSearchBtnClicked);
}

void DividendDetailFrame::onSearchBtnClicked()
{
	QDate curDate = QDate::currentDate();
	QString lines;
	QMap<QString, StockDividend> sds = StockDataBase::getInstance()->selectAllStockDividend(curDate.year()-1);
	QMap<QString, StockDividend> oldSds = StockDataBase::getInstance()->selectAllStockDividend(curDate.year() - 2);
	QMap<QString, StockPrePrice> sps = StockDataBase::getInstance()->selectAllStockPrePrice();
	auto sdIter = sds.find(leStockId_->text());
	auto oldSdIter = oldSds.find(leStockId_->text());
	auto spIter = sps.find(leStockId_->text());
	if (sdIter != sds.end()
		&& spIter != sps.end()
		&& oldSdIter != oldSds.end())
	{
		lines += tr("stockId: %1").arg(leStockId_->text());
		lines += "<br>";

		lines += tr("price: %1").arg(spIter->getPrePrice());
		lines += "<br>";

		lines += tr("dividend profit rate: %1").arg(sdIter->getProfitRate(spIter->getPrePrice()));
		lines += "<br>";

		lines += tr("profit save rate: %1").arg(sdIter->getProfitSavRate());
		lines += "<br>";

		lines += tr("pre dividend profit rate: %1").arg(oldSdIter->getProfitRate(spIter->getPrePrice()));
		lines += "<br>";

		lines += tr("pre profit save rate: %1").arg(oldSdIter->getProfitSavRate());
		lines += "<br>";

		lines += tr("pure profit growth: %1").arg(sdIter->getPureProfit()/oldSdIter->getPureProfit());
		lines += "<br>";

	}
	else {
		lines =tr("No Data");
	}
	lbDetail_->setText(lines);
}
