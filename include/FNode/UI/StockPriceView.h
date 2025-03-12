#ifndef _STOCK_PRICE_VIEW_H
#define _STOCK_PRICE_VIEW_H

#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <thread>
#include <QCheckBox>
#include <QButtonGroup>
#include "StockPrice.h"

class StockPriceView :public QFrame
{
	Q_OBJECT
public:
	StockPriceView(QWidget* parent = nullptr);
	~StockPriceView();
	void getNegativeJ();
	void getBuy2();
	void setIsDay(bool v);
	void setIsWeek(bool v);
	void setIsMonth(bool v);
	void setIsSeason(bool v);
	KTypes getKTypes()const;
	bool isDay()const;
	bool isWeek()const;
	bool isMonth()const;
	bool isSeason()const;
signals:
	void sigAppendOutput(const QString& str);
	void sigClearOutput();
	void sigSaveOutput(const QString& fileName);
private:
	void initUI();
	void initCtrlLayout();
	void initConsoleOutput();

	void startAnalysis();
	void startRank();
	void onAppendOutput(const QString& str);
	void onSaveOutput(const QString& fileName);
	QMap<QString, int> getTopVolumeStock(const QDate& endDate, int dayCnt, int outputCnt, double minChangeRate = 4.5, bool hasOutput = false);
	void getNegativeJImp();
	void getBuy2Imp();
	QString procNegativeJ(const QString& stockId, KType kType);
	void procBuy2(const QString& stockId, bool& isFirst);
private:
	QVBoxLayout* mainLayout_;
	QHBoxLayout* ctrlLayout_;

	QPushButton* btnCrawlPrice_;
	QButtonGroup bgWeekMonth_;
	QCheckBox* cbUseDay_;
	QCheckBox* cbUseWeek_;
	QCheckBox* cbUseMonth_;
	QCheckBox* cbUseSeason_;
	QPushButton* btnClearPrice_;
	QPushButton* btnAnalysis_;
	QPushButton* btnGetNegativeJ_;
	QPushButton* btnGetBuy2_;
	QLineEdit* leDuration_;
	QLineEdit* leDate_;
	QPushButton* btnRank_;
	QTextEdit* consoleOutput_;

	std::thread analysisThread_;
	std::thread getNegativeJThread_;
	std::thread getBuy2Thread_;
	KTypes outputedKTypes_;
};




#endif
