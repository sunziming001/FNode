#ifndef _STOCK_PRICE_VIEW_H
#define _STOCK_PRICE_VIEW_H

#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <thread>

class StockPriceView :public QFrame
{
	Q_OBJECT
public:
	StockPriceView(QWidget* parent = nullptr);
	~StockPriceView();
	void getNegativeJ();
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
private:
	QVBoxLayout* mainLayout_;
	QHBoxLayout* ctrlLayout_;

	QPushButton* btnCrawlPrice_;
	QPushButton* btnClearPrice_;
	QPushButton* btnAnalysis_;
	QPushButton* btnGetNegativeJ_;
	QLineEdit* leDuration_;
	QLineEdit* leDate_;
	QPushButton* btnRank_;
	QTextEdit* consoleOutput_;

	std::thread analysisThread_;
	std::thread getNegativeJThread_;

};




#endif
