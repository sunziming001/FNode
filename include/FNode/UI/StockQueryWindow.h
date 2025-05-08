#pragma once

#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include "StockDatabase.h"

class StockBorder :public QFrame
{
public:
	StockBorder(QWidget* parent = nullptr);
	~StockBorder();
	void showStock(const QString& stockId, KType kType);
	virtual QSize sizeHint()const;
protected:
	virtual void paintEvent(QPaintEvent* e);
	virtual void mouseMoveEvent(QMouseEvent* e);
private:
	void drawPrice(QPainter& p, const QRect& area);
	void drawMainForce(QPainter& p, const QRect& area);
	void drawMouseCross(QPainter& p, const QRect& area);
private:
	QList<StockPrice> price_;
	QList<double> mainForce_;
	QList<double> priceRate_;
};


class StockQueryWindow :public QFrame
{
public:
	StockQueryWindow(QWidget* parent = nullptr);
	~StockQueryWindow();
	void setKType(KType k);
private:
	void initUI();
	void initCtrlUI();

	void onConfirmClicked();
private:
	QVBoxLayout* mainLayout_;
	QHBoxLayout* ctrlLayout_;
	QPushButton* btnConfirm_;
	QLineEdit* leStockId_;
	StockBorder* stockBoard_;

	KType kType_ = KType::Week;
};