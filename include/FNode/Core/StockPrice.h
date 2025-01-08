#ifndef STOCK_PRICE_H
#define STOCK_PRICE_H

#include <QString>
#include <QList>
#include <QPair>
#include <QFlags>
#include <functional>

enum class KType
{
	None = 0x00,
	Day = 0x01,
	Week = 0x02,
	Month = 0x04,
	Season = 0x08,
};
Q_DECLARE_FLAGS(KTypes, KType)
Q_DECLARE_OPERATORS_FOR_FLAGS(KTypes)

void IterateKTypes(KTypes kTypes, std::function<void(KType)> onKType);
QString KTypeToShortString(KType kType);


class StockPrice
{
public:
	StockPrice();
	~StockPrice();

	static double GetMA(const QList<StockPrice>& source, unsigned int idx, unsigned int dur);
	static double GetVMA(const QList<StockPrice>& source, unsigned int idx, unsigned int dur);
	static double GetChangeMA(const QList<StockPrice>& source, unsigned int idx, unsigned int dur);
	static double GetPrevLowest(const QList<StockPrice>& source, unsigned int idx, unsigned int dur);
	static double GetPriceStdDev(const QList<StockPrice>& source, unsigned int idx, unsigned int dur);
	static double GetSumChangeRate(const QList<StockPrice>& source, unsigned int idx, unsigned int dur);
	static QList<double> GetSMA20(const QList<StockPrice>& source);
	static QList<double> GetRSV(const QList<StockPrice>& source);
	static QList<double> GetK(const QList<StockPrice>& source);
	static QList<std::pair<double,double>> GetKD(const QList<StockPrice>& source);
	static QList<std::tuple<double,double,double>> GetKDJ(const QList<StockPrice>& source);

	static int GetPrevValleyIndx(const QList<StockPrice>& source, unsigned int idx);
	static int GetPrevPeekIndx(const QList<StockPrice>& source, unsigned int idx);
	static int GetNextPeekIndx(const QList<StockPrice>& source, unsigned int idx);
	static QList<QPair<bool, unsigned int>> GetPrevPeekOrValleyIndex(const QList<StockPrice>& source, unsigned int idx, int retCnt);

	static bool IsPeek(const StockPrice& left, const StockPrice& middle, const StockPrice& right);
	static bool IsValley(const StockPrice& left, const StockPrice& middle, const StockPrice& right);
	
	QString getStockId() const { return stockId_; }
	void setStockId(QString val) { stockId_ = val; }

	QString getDate() const { return date_; }
	void setDate(QString val) { date_ = val; }

	double getOpenPrice() const { return openPrice_; }
	void setOpenPrice(double val) { openPrice_ = val; }

	double getClosePrice() const { return closePrice_; }
	void setClosePrice(double val) { closePrice_ = val; }

	double getHighPrice() const { return highPrice_; }
	void setHighPrice(double val) { highPrice_ = val; }

	double getLowPrice() const { return lowPrice_; }
	void setLowPrice(double val) { lowPrice_ = val; }

	double getTurnOver() const { return turnOver_; }
	void setTurnOver(double val) { turnOver_ = val; }

	double getVolume() const { return volume_; }
	void setVolume(double val) { volume_ = val; }

	double getChangeRate() const { return changeRate_; }
	void setChangeRate(double val) { changeRate_ = val; }

	double getAveragePrice()const;
	double getMarketValue()const;
private:
	QString stockId_;
	QString date_;
	double openPrice_ = 0.0;
	double closePrice_ = 0.0;
	double highPrice_ = 0.0;
	double lowPrice_ = 0.0;
	double turnOver_ = 0.0;		//<成交量（万）
	double volume_ = 0.0;			//<成交额（万）
	double changeRate_ = 0.0;		//<换手率
};


#endif
