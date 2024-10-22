#ifndef _STOCK_SEARCH_FRAME_H
#define _STOCK_SEARCH_FRAME_H

#include<QFrame>
#include<QLineEdit>
#include<QLabel>
#include<QPushButton>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QTableWidget>

struct StockSearchRowData
{
	QString stockId;
	double profitSavRat;
	double oldProfitSavRat;
	double pureProfitGrowth;
	double expandRate;
	double expPrice;
	double curPrice;
	double curProfitRate;
};

class StockSearchFrame :public QFrame 
{
	Q_OBJECT
public:
	StockSearchFrame(QWidget* parent = nullptr);
	~StockSearchFrame();
private:
	void initUI();

	void onSearchClicked();
private:
	QVBoxLayout* mainLayout_ = nullptr;
	QHBoxLayout* searchLayout_ = nullptr;
	QLineEdit* leYear_ =nullptr;
	QLineEdit* leExpPrefitRate_ = nullptr;
	QPushButton* btnSearch_ =nullptr;
	QTableWidget* twResult_ = nullptr;
};

#endif