#ifndef _NET_TASK_MANAGER_H
#define _NET_TASK_MANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QQueue>

class AbstractNetTask :public QObject
{
	Q_OBJECT
public:
	AbstractNetTask(QObject* parent = nullptr):QObject(parent) {}
	void start()
	{
		isRunning_ = true;
		connect(this, &AbstractNetTask::finished, this, [this](bool isError, QString errInfo) {
			isRunning_ = false;
			isError_ = isError;
			errInfo_ = errInfo;
		});

		onExecute();
	}
	QString getErrorInfo()const { return errInfo_; };
	bool isError()const { return isError_; };
signals:
	void finished(bool isError,QString errInfo);
protected:
	virtual void onExecute() = 0;
	bool isRunning_ = false;
	bool isError_ = false;
	QString errInfo_;
};

class NetTaskManager :public QObject
{
	Q_OBJECT
public:
	static 	NetTaskManager* getInstance();
	~NetTaskManager();
	void init();
	void uninit();
	QNetworkAccessManager* getNetworkAccessManager();
	void pushTask(AbstractNetTask *tsk);
signals:
	void taskFinished(AbstractNetTask* tsk);
private:
	NetTaskManager();

private:
	static NetTaskManager* instance;
	QNetworkAccessManager* naMgr_ = nullptr;
	QQueue<AbstractNetTask*> taskQueue_;
};

#endif