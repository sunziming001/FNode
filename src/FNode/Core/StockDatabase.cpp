#include "StockDatabase.h"
#include <QFileInfo>
#include <QSqlQuery>
#include <QVariant>

StockDataBase* StockDataBase::instance=nullptr;

StockDataBase::StockDataBase()
{
	initDatabasFile();
	initStockList();
}

StockDataBase::~StockDataBase()
{

}

StockDataBase* StockDataBase::getInstance()
{
	if (nullptr == instance)
		instance = new StockDataBase;

	return instance;
}

void StockDataBase::insertStockDividend(const StockDividend& data)
{
	if (!db_.open())
	{
		qWarning("failed open database");
		return;
	}

	if (!data.isValid())
	{
		qWarning("data is invalid");
		return;
	}

	QSqlQuery query(db_);
	QString sqlStr;

	sqlStr= "begin;";
	query.exec(sqlStr);

	sqlStr = "insert into tb_stock_dividend(StockId,Year,PureProfit,TotalDividend,PerStockProfid)values('%1',%2,%3,%4,%5);";
	sqlStr = sqlStr.arg(data.getStockId()).arg(data.getReportYear()).arg(data.getPureProfit()).arg(data.getTotalDividend()).arg(data.getPerStockProfit());
	query.exec(sqlStr);

	sqlStr = "commit;";
	query.exec(sqlStr);

	db_.close();
}

bool StockDataBase::hasStockDividend(const StockDividend& data)
{
	bool ret = false;
	
	if (!db_.open())
	{
		qWarning("failed open database");
		return ret;
	}

	QSqlQuery query(db_);
	QString sqlStr;
	sqlStr = "select StockId from tb_stock_dividend where StockId='%1' and Year=%2;";
	sqlStr = sqlStr.arg(data.getStockId()).arg(data.getReportYear());

	if (query.exec(sqlStr))
	{
		ret = query.next();
	}

	db_.close();

	return ret;
}

QMap<QString,StockDividend> StockDataBase::selectAllStockDividend(int year)
{
	QMap<QString, StockDividend> ret;
	if (!db_.open())
	{
		qWarning("failed open database");
		return ret;
	}

	QSqlQuery query(db_);
	QString sqlStr;
	sqlStr = "select StockId,PureProfit,TotalDividend,PerStockProfid from tb_stock_dividend where Year=%1;";
	sqlStr = sqlStr.arg(year);

	if (query.exec(sqlStr))
	{
		while (query.next())
		{
			StockDividend sd;
			QString stockId = query.value(0).toString();
			double purePorfit = query.value(1).toDouble();
			double totalDividend = query.value(2).toDouble();
			double perStockProfit = query.value(3).toDouble();

			sd.setReportYear(year);
			sd.setStockId(stockId);
			sd.setPureProfit(purePorfit);
			sd.setTotalDividend(totalDividend);
			sd.setPerStockProfit(perStockProfit);

			ret[stockId] = sd;
		}
	}
	db_.close();
	return ret;
}

QList<QString> StockDataBase::getStockList() const
{
	return stockList_;
}

bool StockDataBase::hasStockPrePrice(const QString& stockId)
{
	bool ret = false;

	if (!db_.open())
	{
		qWarning("failed open database");
		return ret;
	}

	QSqlQuery query(db_);
	QString sqlStr;
	sqlStr = "select StockId from tb_stock_pre_price where StockId='%1';";
	sqlStr = sqlStr.arg(stockId);

	if (query.exec(sqlStr))
	{
		ret = query.next();
	}

	db_.close();

	return ret;
}

void StockDataBase::insertStockPrePrice(const StockPrePrice& data)
{
	if (!db_.open())
	{
		qWarning("failed open database");
		return;
	}

	if (!data.isValid())
	{
		qWarning("data is invalid");
		return;
	}

	QSqlQuery query(db_);
	QString sqlStr;

	sqlStr = "begin;";
	query.exec(sqlStr);

	sqlStr = "insert into tb_stock_pre_price(StockId,PrePrice)values('%1',%2);";
	sqlStr = sqlStr.arg(data.getStockId()).arg(data.getPrePrice());
	query.exec(sqlStr);

	sqlStr = "commit;";
	query.exec(sqlStr);

	db_.close();
}

void StockDataBase::clearStockPrePrice()
{
	if (!db_.open())
	{
		qWarning("failed open database");
		return;
	}
	QSqlQuery query(db_);
	QString sqlStr;

	sqlStr = "begin;";
	query.exec(sqlStr);

	sqlStr = "delete from tb_stock_pre_price";
	query.exec(sqlStr);


	sqlStr = "commit;";
	query.exec(sqlStr);


	db_.close();
}

QMap<QString, StockPrePrice> StockDataBase::selectAllStockPrePrice()
{
	QMap<QString, StockPrePrice> ret;
	if (!db_.open())
	{
		qWarning("failed open database");
		return ret;
	}

	QSqlQuery query(db_);
	QString sqlStr;
	sqlStr = "select StockId,PrePrice from tb_stock_pre_price;";

	if (query.exec(sqlStr))
	{
		while (query.next())
		{
			StockPrePrice sp;
			QString stockId = query.value(0).toString();
			double prePrice = query.value(1).toDouble();

			sp.setStockId(stockId);
			sp.setPrePrice(prePrice);

			ret[stockId] = sp;
		}
	}
	db_.close();
	return ret;
}

void StockDataBase::clearStockPrice(KTypes ktypes)
{
	if (!db_.open())
	{
		qWarning("failed open database");
		return;
	}
	QSqlQuery query(db_);
	QString sqlStr;

	sqlStr = "begin;";
	query.exec(sqlStr);

	IterateKTypes(ktypes, [this, &query](KType kType) {
		QString sql = "delete from %1";
		QString tbName = this->getStockPriceTableName(kType);
		QString sqlStr = sql.arg(tbName);
		query.exec(sqlStr);
	});	


	sqlStr = "commit;";
	query.exec(sqlStr);


	db_.close();
}

void StockDataBase::insertStockPrice(const QList<StockPrice>& data, KType kType)
{
	if (!db_.open())
	{
		qWarning("failed open database");
		return;
	}

	QSqlQuery query(db_);
	QString tbName = getStockPriceTableName(kType);
	QString sqlStr;

	sqlStr = "begin;";
	query.exec(sqlStr);

	for (auto iter = data.begin(); iter != data.end(); iter++)
	{
		sqlStr = "insert into %1(StockId, Date,OpenPrice,ClosePrice,HighPrice,LowPrice,TurnOver,Volume,ChangeRate)values('%2','%3', %4, %5, %6, %7, %8, %9, %10);";
	
		sqlStr = sqlStr.arg(tbName)
			.arg(iter->getStockId())
			.arg(iter->getDate())
			.arg(iter->getOpenPrice())
			.arg(iter->getClosePrice())
			.arg(iter->getHighPrice())
			.arg(iter->getLowPrice())
			.arg(iter->getTurnOver())
			.arg(iter->getVolume())
			.arg(iter->getChangeRate())
			;

		query.exec(sqlStr);

	}
	sqlStr = "commit;";
	query.exec(sqlStr);

	db_.close();
}


bool StockDataBase::hasStockPrice(const QString& stockId, KType kType)
{
	bool ret = false;

	if (!db_.open())
	{
		qWarning("failed open database");
		return ret;
	}

	QSqlQuery query(db_);
	QString sqlStr;
	QString tbName = getStockPriceTableName(kType);
	sqlStr = "select StockId from %1 where StockId='%2';";
	sqlStr = sqlStr.arg(tbName)
		.arg(stockId);

	if (query.exec(sqlStr))
	{
		ret = query.next();
	}

	db_.close();

	return ret;
}

QList<StockPrice> StockDataBase::selectStockPriceById(const QString& stockId, KType kType)
{
	QList<StockPrice> ret;
	if (!db_.open())
	{
		qWarning("failed open database");
		return ret;
	}
	QSqlQuery query(db_);
	QString sqlStr;
	QString tbName = getStockPriceTableName(kType);

	sqlStr = "select Date, OpenPrice, ClosePrice, HighPrice, LowPrice,TurnOver, Volume, ChangeRate from %1 where StockId='%2' order by Date;";
	sqlStr = sqlStr.arg(tbName)
		.arg(stockId);
	if (query.exec(sqlStr))
	{
		while (query.next())
		{
			StockPrice price;
		
			QString date = query.value(0).toString();
			double openPrice = query.value(1).toDouble();
			double closePrice = query.value(2).toDouble();
			double highPrice = query.value(3).toDouble();
			double lowPrice = query.value(4).toDouble();
			double turnOver = query.value(5).toDouble();
			double volume = query.value(6).toDouble();
			double changeRate = query.value(7).toDouble();

			price.setStockId(stockId);
			price.setDate(date);
			price.setOpenPrice(openPrice);
			price.setClosePrice(closePrice);
			price.setHighPrice(highPrice);
			price.setLowPrice(lowPrice);
			price.setTurnOver(turnOver);
			price.setVolume(volume);
			price.setChangeRate(changeRate);

			ret.append(price);
		}
	}


	return ret;
}

QList<StockPrice> StockDataBase::selectStockPriceByDate(const QString& date, KType kType)
{
	QList<StockPrice> ret;
	if (!db_.open())
	{
		qWarning("failed open database");
		return ret;
	}
	QSqlQuery query(db_);
	QString sqlStr;
	QString tbName = getStockPriceTableName(kType);

	sqlStr = "select Date, OpenPrice, ClosePrice, HighPrice, LowPrice,TurnOver, Volume, ChangeRate,StockId from %1 where Date='%2';";
	sqlStr = sqlStr.arg(tbName)
		.arg(date);
	if (query.exec(sqlStr))
	{
		while (query.next())
		{
			StockPrice price;

			QString date = query.value(0).toString();
			double openPrice = query.value(1).toDouble();
			double closePrice = query.value(2).toDouble();
			double highPrice = query.value(3).toDouble();
			double lowPrice = query.value(4).toDouble();
			double turnOver = query.value(5).toDouble();
			double volume = query.value(6).toDouble();
			double changeRate = query.value(7).toDouble();
			QString stockId = query.value(8).toString();

			price.setStockId(stockId);
			price.setDate(date);
			price.setOpenPrice(openPrice);
			price.setClosePrice(closePrice);
			price.setHighPrice(highPrice);
			price.setLowPrice(lowPrice);
			price.setTurnOver(turnOver);
			price.setVolume(volume);
			price.setChangeRate(changeRate);

			ret.append(price);
		}
	}


	return ret;
}

QString StockDataBase::getStockPriceTableName(KType type) const
{
	QString ret;
	switch (type)
	{
	case KType::Day:
		ret = "tb_stock_price_d";
		break;
	case KType::Week:
		ret = "tb_stock_price_w";
		break;
	case KType::Month:
		ret = "tb_stock_price_m";
		break;
	case KType::Season:
		ret = "tb_stock_price_s";
		break;
	default:
		ret = "tb_stock_price_d";
		break;
	}

	return ret;
}

void StockDataBase::initStockList()
{
	initFromStockListFile("sh_list.csv");
	initFromStockListFile("sz_list.csv");
}

void StockDataBase::initDatabasFile()
{
	static const char* DBName = "./stock.db";
	QFileInfo file(DBName);
	if (file.exists() == false)
	{
		db_ = QSqlDatabase::addDatabase("QSQLITE");
		db_.setDatabaseName(DBName); //设置数据库名

		if (!db_.open())
		{
			qWarning("数据库不能打开");
		}

		QSqlQuery query(db_);
		///创建卡号数据库
		query.exec("CREATE TABLE tb_stock_dividend(ID integer PRIMARY KEY autoincrement, StockId varchar(20), Year integer,PureProfit double,TotalDividend double,PerStockProfid double);");
		query.clear();


		query.exec("CREATE TABLE tb_stock_pre_price(ID integer PRIMARY KEY autoincrement, StockId varchar(20), PrePrice double);");
		query.clear();

		query.exec("CREATE TABLE tb_stock_price_d(ID integer PRIMARY KEY autoincrement, StockId varchar(20), Date varchar(12),\
			OpenPrice double, ClosePrice double, HighPrice double, LowPrice double, \
			TurnOver double, Volume double, ChangeRate double);");

		query.exec("CREATE TABLE tb_stock_price_w(ID integer PRIMARY KEY autoincrement, StockId varchar(20), Date varchar(12),\
			OpenPrice double, ClosePrice double, HighPrice double, LowPrice double, \
			TurnOver double, Volume double, ChangeRate double);");

		query.exec("CREATE TABLE tb_stock_price_m(ID integer PRIMARY KEY autoincrement, StockId varchar(20), Date varchar(12),\
			OpenPrice double, ClosePrice double, HighPrice double, LowPrice double, \
			TurnOver double, Volume double, ChangeRate double);");

		query.exec("CREATE TABLE tb_stock_price_s(ID integer PRIMARY KEY autoincrement, StockId varchar(20), Date varchar(12),\
			OpenPrice double, ClosePrice double, HighPrice double, LowPrice double, \
			TurnOver double, Volume double, ChangeRate double);");

		query.clear();

	}
	else
	{
		if (!db_.isOpen())
		{
			if (db_.contains(DBName))
				db_ = QSqlDatabase::database(DBName);
			else
			{
				db_ = QSqlDatabase::addDatabase("QSQLITE", DBName);
			}
			db_.setDatabaseName(DBName); //设置数据库名
		}
	}
	db_.close();

}

void StockDataBase::initFromStockListFile(const char* fileName)
{
	FILE* fp = NULL;
	fopen_s(&fp, fileName, "r");
	if (fp)
	{
		char buff[200] = { 0 };

		while (fgets(buff, 200, fp))
		{
			QString id = buff;
			bool isOk = false;
			id.toInt(&isOk);
			if (isOk)
			{
				id = id.trimmed();
				if (id.length() < 6)
				{
					for (int i = 0; i = 6 - id.length(); i++)
					{
						id = "0" + id;
					}
				}
				stockList_.append(id);
			}

			memset(buff, 0, 200);
		}

		fclose(fp);
	}
}
