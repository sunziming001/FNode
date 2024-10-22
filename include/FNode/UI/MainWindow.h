#ifndef _MAIN_WINDIW_H
#define _MAIN_WINDIW_H

#include <QFrame>
#include "CommonNodeView.h"
#include "CommonScene.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QButtonGroup>
#include <QStackedLayout>


class StockSearchFrame;
class DividendDetailFrame;
class StockPriceView;
class DividendFrame;
class HoldersFrame;


class MainWindow :public QFrame
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent=nullptr);
	~MainWindow();

public slots:
	void startCrawel(int year);
	void startPrePrice();
	void clearPrePrice();
	void startPrice();
	void clearPrice();

	void startCrawelHolders();
private:
	void initUI();

	void initTabLayout();
	void initStackedLayout();
	void initProgressLayout();
	void initDividendFrame();
	void initPriceFrame();
	void initHolderFrame();
	
	void freshProgress();
private slots:
	void onSwitchView(int id, bool checked);
private:
	QVBoxLayout *mainLayout_=nullptr;
	QHBoxLayout* tabLayout_ = nullptr;
	QStackedLayout* stackedLayout_ = nullptr;

	QCheckBox* cbDividen_ = nullptr;
	QCheckBox* cbPrice_ = nullptr;
	QCheckBox* cbHolders_ = nullptr;
	QButtonGroup* cbGroup_ = nullptr;




	QPushButton* btnStartPrice_ = nullptr;
	QPushButton* btnClearPrice_ = nullptr;
	QPushButton* btnPriceCheck_ = nullptr;

	QLabel* lbProgress_ = nullptr;


	DividendFrame* dividendFrame_ = nullptr;
	StockPriceView* priceFrame_= nullptr;
	HoldersFrame* holderFrame_ = nullptr;

	int totalTaskCnt_ = 0;
	int finishedTasksCnt_ = 0;
};

#endif