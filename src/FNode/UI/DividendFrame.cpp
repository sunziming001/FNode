#include "DividendFrame.h"
#include <QDate>
#include "StockSearhFrame.h"
#include "DividendDetailFrame.h"
#include "MainWindow.h"

DividendFrame::DividendFrame(QWidget* parent /*= nullptr*/)
	:QFrame(parent)
{
	mainLayout_ = new QVBoxLayout;
	mainLayout_->setContentsMargins(0, 0, 0, 0);
	mainLayout_->setSpacing(0);
	this->setLayout(mainLayout_);

	initCrawelLayout();
	initContentLayout();
}

DividendFrame::~DividendFrame()
{

}

void DividendFrame::initCrawelLayout()
{
	QDate date = QDate::currentDate();
	crawelLayout_ = new QHBoxLayout;
	mainLayout_->addLayout(crawelLayout_);

	leYear_ = new QLineEdit(this);
	leYear_->setPlaceholderText(tr("year"));
	leYear_->setText(QString::number(date.year() - 1));

	btnStartCrawel_ = new QPushButton(tr("CrawelDividend"), this);

	btnStartPrePrice_ = new QPushButton(tr("CrawelPrePrice"), this);
	btnClearPrePrice_ = new QPushButton(tr("ClearPrePrice"), this);

	crawelLayout_->addWidget(leYear_);
	crawelLayout_->addStretch(1);
	crawelLayout_->addWidget(btnStartCrawel_);
	crawelLayout_->addWidget(btnStartPrePrice_);
	crawelLayout_->addWidget(btnClearPrePrice_);
}


void DividendFrame::initContentLayout()
{
	MainWindow* mainWnd =dynamic_cast<MainWindow*>(parent());
	contentLayout_ = new QHBoxLayout;
	mainLayout_->addLayout(contentLayout_, 1);

	stockSearchFrame_ = new StockSearchFrame(this);

	dividendDetailFrame_ = new DividendDetailFrame(this);

	contentLayout_->addWidget(stockSearchFrame_, 2);
	contentLayout_->addWidget(dividendDetailFrame_, 1);

	if (mainWnd)
	{
		connect(btnStartCrawel_, &QPushButton::clicked, this, [this, mainWnd]() {
			int year = leYear_->text().toInt();
			mainWnd->startCrawel(year);
		});

		connect(btnStartPrePrice_, &QPushButton::clicked, mainWnd, &MainWindow::startPrePrice);
		connect(btnClearPrePrice_, &QPushButton::clicked, mainWnd, &MainWindow::clearPrePrice);
	}
}
