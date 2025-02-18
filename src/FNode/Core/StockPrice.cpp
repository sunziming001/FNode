#include "StockPrice.h"
#include <QDebug>

StockPrice::StockPrice()
{

}

StockPrice::~StockPrice()
{

}


double StockPrice::GetMA(const QList<StockPrice>& source, unsigned int idx, unsigned int dur)
{
	double ret = 0.0;
	int startIdx = idx - dur+1;
	int endIdx = idx;
	double volSum = 0.0;
	double turnOverSum = 0.0;
	if (startIdx >= 0 
		&& endIdx < source.size()
		&& startIdx < endIdx)
	{
		for (int i = startIdx; i != endIdx; i++)
		{
			volSum += source[i].getVolume();
			turnOverSum += source[i].getTurnOver();
		}

		ret = volSum / turnOverSum/100.0;
	}

	return ret;
}

double StockPrice::GetVMA(const QList<StockPrice>& source, unsigned int idx, unsigned int dur)
{
	double ret = 0.0;
	int startIdx = idx - dur + 1;
	int endIdx = idx;
	double volSum = 0.0;

	if (startIdx >= 0
		&& endIdx < source.size()
		&& startIdx < endIdx)
	{
		for (int i = startIdx; i != endIdx; i++)
		{
			volSum += source[i].getVolume();
		}

		ret = volSum / dur;
	}

	return ret;
}

double StockPrice::GetChangeMA(const QList<StockPrice>& source, unsigned int idx, unsigned int dur)
{
	double ret = 0.0;
	int startIdx = idx - dur + 1;
	int endIdx = idx;

	if (startIdx >= 0
		&& endIdx < source.size()
		&& startIdx < endIdx)
	{
		for (int i = startIdx; i != endIdx; i++)
		{
			ret += source[i].getChangeRate();
		}

		ret = ret / dur;
	}

	return ret;
}

double StockPrice::GetPrevLowest(const QList<StockPrice>& source, unsigned int idx, unsigned int dur)
{
	double ret = 0.0;
	int startIdx = idx - dur + 1;
	int endIdx = idx;


	if (startIdx >= 0
		&& endIdx < source.size()
		&& startIdx < endIdx)
	{
		for (int i = startIdx; i != endIdx; i++)
		{
			if (ret == 0.0)
			{
				ret = source[i].getLowPrice();
			}
			else if (ret < source[i].getLowPrice())
			{
				ret = source[i].getLowPrice();
			}
		}
	}

	return ret;
}

double StockPrice::GetPriceStdDev(const QList<StockPrice>& source, unsigned int idx, unsigned int dur)
{
	double ret = 0.0;
	double tmpSum = 0.0;
	double aver = 0.0;
	int startIdx = idx - dur + 1;
	int endIdx = idx;


	if (startIdx >= 0
		&& endIdx < source.size()
		&& startIdx < endIdx)
	{
		for (int i = startIdx; i != endIdx; i++)
		{
			tmpSum += source[i].getAveragePrice();
		}
		aver = tmpSum / dur;
		tmpSum = 0.0;

		for (int i = startIdx; i != endIdx; i++)
		{
			tmpSum+= pow(source[i].getAveragePrice()-aver,2.0);
		}

		ret = sqrt(tmpSum / dur);
	}
	return ret;
}

double StockPrice::GetSumChangeRate(const QList<StockPrice>& source, unsigned int idx, unsigned int dur)
{
	double ret = 0.0;
	int startIdx = idx - dur + 1;
	int endIdx = idx;


	if (startIdx >= 0
		&& endIdx < source.size()
		&& startIdx < endIdx)
	{
		for (int i = startIdx; i != endIdx; i++)
		{
			ret += source[i].getChangeRate();
		}
	}

	return ret;
}

QList<double> StockPrice::GetSMA20(const QList<StockPrice>& source)
{
	QList<double> ret;
	for (int i = 0; i < source.size(); i++)
	{
		double closeSum = 0.0;
		double curValue = 0.0;
		int closeCount = 0;
		int curIndx = i;
		double minPrice = 0.0;
		double maxPrice = 0.0;

		while (curIndx >= 0 && curIndx > i - 20)
		{
			closeSum += source.at(curIndx).getClosePrice();
			closeCount++;
			curIndx--;
		}

		curValue = closeSum / closeCount;
		ret.append(curValue);
	}

	return ret;
}

QList<double> StockPrice::GetRSV(const QList<StockPrice>& source)
{
	QList<double> ret;
	for (int i =0; i<source.size();i++)
	{
		double curValue = 0.0;
		int curIndx = i;
		double minPrice = 0.0;
		double maxPrice = 0.0;

		while (curIndx >= 0 && curIndx>=i-8)
		{
			const StockPrice& curPrice = source.at(curIndx);

			if (minPrice == 0.0
				|| minPrice > curPrice.getLowPrice())
			{
				minPrice = curPrice.getLowPrice();
			}

			if (maxPrice < curPrice.getHighPrice())
			{
				maxPrice = curPrice.getHighPrice();
			}

			 
			curIndx--;
		}
		
		curValue = (source.at(i).getClosePrice()-minPrice) / (maxPrice - minPrice) * 100;
		ret.append(curValue);
	}

	return ret;
}

QList<double> StockPrice::GetK(const QList<StockPrice>& source)
{
	QList<double> ret;
	QList<double> rsv = GetRSV(source);
	double lastK = 0.0;
	for (int i = 0; i < rsv.size(); i++)
	{
		double curK = 2.0 / 3.0 * lastK + 1.0 / 3.0 * rsv.at(i);

		lastK = curK;
		ret.append(curK);
	}
	return ret;
}

QList<std::pair<double, double>> StockPrice::GetKD(const QList<StockPrice>& source)
{
	QList<std::pair<double, double>> ret;
	QList<double> K = GetK(source);
	double lastD = 0.0;

	for (int i = 0; i < K.size(); i++)
	{
		double curD = 2.0 / 3.0 * lastD + 1.0 / 3.0 * K.at(i);

		lastD = curD;
		ret.append(std::make_pair(K.at(i),curD));
	}
	return ret;
}

QList<std::tuple<double, double, double>> StockPrice::GetKDJ(const QList<StockPrice>& source)
{
	QList<std::tuple<double, double, double>> ret;
	QList<std::pair<double, double>> KD= GetKD(source);
	for (int i = 0; i < KD.size(); i++)
	{
		double curJ = 3.0 * KD.at(i).first - 2.0 * KD.at(i).second;

		ret.append(std::make_tuple(KD.at(i).first, KD.at(i).second,curJ));
	}

	return ret;
}


QList<int> StockPrice::GetBuy2Index(const QList<StockPrice>& source)
{
	QList<int> ret;
	QList<StockPrice> peeks;
	
	int obWidth = 15;
	int startIndex = obWidth;
	
	if (source.size() <= obWidth)
	{
		return ret;
	}

	for (int i = startIndex+1; i < source.length()-1; i++)
	{
		
		int obStartIndex = i - obWidth;
		if (isPriceBreak(source, i, obWidth))
		{
			ret.push_back(i);
		}
		
		
	}


	return ret;
}

bool StockPrice::isPriceBreak(const QList<StockPrice>& source, int checkIndex, int obWidth)
{
	int obStartIndex = checkIndex-obWidth;
	double averageVolume = 0.0;
	double maxClose = 0.0;
	bool isCloseOverAllPeek = true;
	bool isPrevCloseOverAllPeek = true;
	QList<StockPrice> peeks;
	bool ret = false;
	/*
		1. 000625 2012/12/19 vol max in 20 day ,close is highest, after pre high 7 days
		2. 600016 2012/12/5 vol max in 20 day ,close is highest, after pre high 4 days (next day vol retrun to average)
		3. 000776 2024/7/31(next day vol 0.4~0.8) 2024/9/24(next day vol 1.2)
		4. 600839 2024/10/14
		5. 600056	2009/10/26
	*/
	if (source.size() <= checkIndex
		|| obStartIndex < 0)
	{
		return ret;
	}

	for (int j = obStartIndex; j < checkIndex; j++)
	{
		StockPrice pre = source[j - 1];
		StockPrice cur = source[j];
		StockPrice next = source[j + 1];


		if (cur.getClosePrice() > pre.getClosePrice()
			&& cur.getClosePrice() > next.getClosePrice())
		{
			peeks.append(cur);

			if (maxClose < cur.getClosePrice())
			{
				maxClose = cur.getClosePrice();
			}
		}

		averageVolume += source[j].getVolume();
	}
	averageVolume = averageVolume / obWidth;
	if (peeks.size() > 0)
	{
		auto& price = peeks.last();
		isCloseOverAllPeek &= source[checkIndex].getClosePrice() > maxClose;
		isPrevCloseOverAllPeek &= source[checkIndex - 1].getClosePrice() > maxClose;
	}

	if (isCloseOverAllPeek
		&& !isPrevCloseOverAllPeek
		&& peeks.size() > 0
		&& averageVolume * 2.0 <= source[checkIndex].getVolume())
	{
		ret = true;
	}

	return ret;
}

int StockPrice::GetPrevValleyIndx(const QList<StockPrice>& source, unsigned int idx)
{
	int ret = -1;
	int checkStartIndx = idx - 3;

	if (checkStartIndx >= source.size()
		|| idx >= source.size())
	{
		return ret;
	}

	for (int i = checkStartIndx; i >= 0; i--)
	{

		const StockPrice& left = source[i];
		const StockPrice& middle = source[i + 1];
		const StockPrice& right = source[i + 2];

		bool isValley = left.getHighPrice() > middle.getHighPrice()
			&& right.getHighPrice() > middle.getHighPrice()
			&& left.getLowPrice() > middle.getLowPrice()
			&& right.getLowPrice() > middle.getLowPrice();

		if (isValley)
		{
			ret = i + 1;
			break;
		}
	}

	return ret;
}

int StockPrice::GetPrevPeekIndx(const QList<StockPrice>& source, unsigned int idx)
{
	int ret = -1;
	int checkStartIndx = idx - 3;

	if (checkStartIndx >= source.size()
		|| idx>=source.size())
	{
		return ret;
	}

	for (int i = checkStartIndx; i >= 0; i--)
	{
		
		const StockPrice& left = source[i];
		const StockPrice& middle = source[i + 1];
		const StockPrice& right = source[i + 2];
	
		bool isPeek = left.getHighPrice() < middle.getHighPrice()
			&& right.getHighPrice() < middle.getHighPrice()
			&& left.getLowPrice() < middle.getLowPrice()
			&& right.getLowPrice() < middle.getLowPrice();

		if (isPeek)
		{
			ret = i+1;
			break;
		}
	}


	return ret;
}

int StockPrice::GetNextPeekIndx(const QList<StockPrice>& source, unsigned int idx)
{
	int ret = -1;
	int checkStartIndx = idx + 3;

	if (checkStartIndx >= source.size()
		|| idx >= source.size())
	{
		return ret;
	}

	for (int i = checkStartIndx; i <source.size(); i++)
	{

		const StockPrice& left = source[i-2];
		const StockPrice& middle = source[i - 1];
		const StockPrice& right = source[i];

		bool isPeek = left.getHighPrice() < middle.getHighPrice()
			&& right.getHighPrice() < middle.getHighPrice()
			&& left.getLowPrice() < middle.getLowPrice()
			&& right.getLowPrice() < middle.getLowPrice();

		if (isPeek)
		{
			ret = i - 1;
			break;
		}
	}
	return ret;
}

QList<QPair<bool, unsigned int>> StockPrice::GetPrevPeekOrValleyIndex(const QList<StockPrice>& source, unsigned int idx, int retCnt)
{
	int checkStartIndx = idx - 3;
	QList<QPair<bool, unsigned int>> ret;

	if (checkStartIndx >= source.size()
		|| idx >= source.size())
	{
		return ret;
	}
	if (source[0].getStockId() == "600008")
	{
		qDebug() << "111";
	}
	for (int i = checkStartIndx; i >= 0; i--)
	{

		const StockPrice& left = source[i];
		const StockPrice& middle = source[i + 1];
		const StockPrice& right = source[i + 2];

		bool isPeek = IsPeek(left, middle, right);
		bool isValley = IsValley(left, middle, right);
		if (isPeek)
		{
			if (ret.size() > 0 && ret[ret.size() - 1].first == true)
			{
				const StockPrice& peekPrice = source[ret[ret.size() - 1].second];
				const StockPrice& curPrice = source[i + 1];
				if (curPrice.getHighPrice() > peekPrice.getHighPrice())
				{
					ret[ret.size() - 1].second = i + 1;
					i -= 2;
				}
				
			}
			else {
				ret.append(qMakePair(true, i + 1));	
				i -= 2;
			}
		}
		else if (isValley)
		{

			if (ret.size() > 0 && ret[ret.size()-1].first == false)
			{
				const StockPrice& valleyPrice = source[ret[ret.size() - 1].second];
				const StockPrice& curPrice = source[i + 1];
				if (curPrice.getLowPrice() < valleyPrice.getLowPrice())
				{
					ret[ret.size() - 1].second = i + 1;
					i-=2;
				}
			}
			else {
				ret.append(qMakePair(false, i + 1));
				i -= 2;
			}
		}

		if (ret.size() == retCnt+1)
		{
			ret.removeLast();
			break;
		}
	}

	return ret;
}

bool StockPrice::IsPeek(const StockPrice& left, const StockPrice& middle, const StockPrice& right)
{
	bool isPeek = left.getHighPrice() < middle.getHighPrice()
		&& right.getHighPrice() < middle.getHighPrice();

	return isPeek;
}

bool StockPrice::IsValley(const StockPrice& left, const StockPrice& middle, const StockPrice& right)
{
	bool isValley = left.getLowPrice() > middle.getLowPrice()
		&& right.getLowPrice() > middle.getLowPrice();

	return isValley;
}

double StockPrice::getAveragePrice() const
{
	return getVolume() / getTurnOver() / 100.0;
}

double StockPrice::getMarketValue() const
{
	return 1.0 / getChangeRate() * getVolume();
}

void IterateKTypes(KTypes kTypes, std::function<void(KType)> onKType)
{
	if (onKType)
	{
		if (kTypes & KType::Day)
		{
			onKType(KType::Day);
		}

		if (kTypes & KType::Week)
		{
			onKType(KType::Week);
		}

		if (kTypes & KType::Month)
		{
			onKType(KType::Month);
		}

		if (kTypes & KType::Season)
		{
			onKType(KType::Season);
		}
	}
	
}

QString KTypeToShortString(KType kType)
{
	QString ret;
	switch (kType)
	{
	case KType::Day:
		ret = "D";
		break;
	case KType::Week:
		ret = "W";
		break;
	case KType::Month:
		ret = "M";
		break;
	case KType::Season:
		ret = "S";
		break;
	default:
		ret = "UK";
		break;
	}
	return ret;
}
