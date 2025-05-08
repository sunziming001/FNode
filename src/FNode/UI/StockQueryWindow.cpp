#include "StockQueryWindow.h"
#include <QPainter>
#include <QPaintEvent>

StockBorder::StockBorder(QWidget* parent /*= nullptr*/)
{
	setWindowFlags(Qt::Widget);
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	this->setMouseTracking(true);
}

StockBorder::~StockBorder()
{

}

void StockBorder::showStock(const QString& stockId, KType kType)
{
	price_ = StockDataBase::getInstance()->selectStockPriceById(stockId, kType);
	mainForce_ = StockPrice::GetMainForceRate(price_);
	priceRate_ = StockPrice::GetPriceRate(price_);
	this->update();
}


QSize StockBorder::sizeHint() const
{
	return QSize(800, 600);
}

void StockBorder::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	QRect geo = e->rect();
	int x, y, width, height;
	x = geo.x();
	y = geo.y();
	width = geo.width();
	height = geo.height();

	painter.fillRect(x, y, width, height, QColor(255, 255, 255));

	painter.setPen(QColor(0, 0, 0));
	painter.drawRect(x, y, width-1, height-1);

	drawPrice(painter, geo);
	drawMainForce(painter, geo);
	drawMouseCross(painter, geo);
}

void StockBorder::mouseMoveEvent(QMouseEvent* e)
{
	this->update();
}

void StockBorder::drawPrice(QPainter& p, const QRect& area)
{
	p.save();
	p.setPen(QColor(0, 0, 255));
	float step = 1.0f * area.width() / (price_.size() + 2);

	QPointF lastPoint;
	for (int i = 0; i < priceRate_.size(); i++)
	{
		QPointF pt;
		pt.setX((i + 1) * step);
		pt.setY((1.0f - priceRate_[i]) * area.height());

		if (i != 0)
		{
			p.drawLine(lastPoint, pt);
		}
		lastPoint = pt;
	}

	
	p.restore();
}

void StockBorder::drawMainForce(QPainter& p, const QRect& area)
{
	p.save();
	p.setPen(QColor(255, 0, 0));
	float step = 1.0f * area.width() / (price_.size()+2);

	QPointF lastPoint;
	for (int i = 0; i < mainForce_.size(); i++)
	{
		QPointF pt;
		pt.setX((i + 1) * step);
		pt.setY((1.0f - mainForce_[i]) * area.height());
	
		if (i != 0)
		{
			p.drawLine(lastPoint, pt);
		}
		lastPoint = pt;
	}
	p.restore();
}

void StockBorder::drawMouseCross(QPainter& p, const QRect& area)
{
	p.save();
	p.setPen(QColor(0, 0, 0));
	
 	QPoint cursorPos = QCursor::pos();
	cursorPos = mapFromGlobal(cursorPos);

	p.drawLine(QPoint(area.x(), cursorPos.y()), QPoint(area.width(), cursorPos.y()));
	p.drawLine(QPoint(cursorPos.x(), area.y()), QPoint(cursorPos.x(), area.width()));

	p.restore();
}

StockQueryWindow::StockQueryWindow(QWidget* parent /*= nullptr*/)
{
	setWindowFlags(Qt::Window);
	initUI();
}

StockQueryWindow::~StockQueryWindow()
{

}

void StockQueryWindow::setKType(KType k)
{
	kType_ = k;
}

void StockQueryWindow::initUI()
{
	mainLayout_ = new QVBoxLayout();
	this->setLayout(mainLayout_);

	initCtrlUI();
	
	stockBoard_ = new StockBorder(this);
	mainLayout_->addWidget(stockBoard_);
}

void StockQueryWindow::initCtrlUI()
{
	ctrlLayout_ = new QHBoxLayout();
	mainLayout_->addLayout(ctrlLayout_);

	btnConfirm_ = new QPushButton(tr("Confirm"), this);
	leStockId_ = new QLineEdit(this);

	ctrlLayout_->addWidget(leStockId_);
	ctrlLayout_->addWidget(btnConfirm_);

	connect(btnConfirm_, &QPushButton::clicked, this, &StockQueryWindow::onConfirmClicked);
}

void StockQueryWindow::onConfirmClicked()
{
	QString stockId = leStockId_->text();
	
	stockBoard_->showStock(stockId, kType_);
}
