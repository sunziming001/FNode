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
signals:
	void sigAppendOutput(const QString& str);
	void sigClearOutput();
private:
	void initUI();
	void initCtrlLayout();
	void initConsoleOutput();

	void startAnalysis();
	void startRank();
	void getNegativeJ();
	void onAppendOutput(const QString& str);

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
