#ifndef _DIVIDEND_FRAME_H
#define _DIVIDEND_FRAME_H

#include<QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

class StockSearchFrame;
class DividendDetailFrame;

class DividendFrame :public QFrame
{
public:
	DividendFrame(QWidget* parent = nullptr);
	~DividendFrame();

private:
	void initCrawelLayout();
	void initContentLayout();

private:
	QVBoxLayout* mainLayout_ = nullptr;
	QHBoxLayout* crawelLayout_ = nullptr;
	QHBoxLayout* contentLayout_ = nullptr;

	QLineEdit* leYear_ = nullptr;
	QPushButton* btnStartCrawel_ = nullptr;
	QPushButton* btnStartPrePrice_ = nullptr;
	QPushButton* btnClearPrePrice_ = nullptr;

	StockSearchFrame* stockSearchFrame_ = nullptr;
	DividendDetailFrame* dividendDetailFrame_ = nullptr;
};


#endif