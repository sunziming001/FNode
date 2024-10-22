#include "NetTaskManager.h"

NetTaskManager* NetTaskManager::instance=nullptr;

NetTaskManager* NetTaskManager::getInstance()
{
	if (nullptr == instance)
		instance = new NetTaskManager;
	return instance;
}

NetTaskManager::~NetTaskManager()
{

}

void NetTaskManager::init()
{
	naMgr_ = new QNetworkAccessManager(this);
}

void NetTaskManager::uninit()
{

}

QNetworkAccessManager* NetTaskManager::getNetworkAccessManager()
{
	return naMgr_;
}

void NetTaskManager::pushTask(AbstractNetTask* tsk)
{
	connect(tsk, &AbstractNetTask::finished, this, [this, tsk](bool isError, QString errInfo) {

		taskQueue_.pop_front();
		emit taskFinished(tsk);

		if (!taskQueue_.isEmpty())
		{
			taskQueue_.first()->start();
		}

	});

	if (taskQueue_.isEmpty())
	{
		
		taskQueue_.push_back(tsk);
		tsk->start();
	}
	else {
		taskQueue_.push_back(tsk);
	}

	

}

NetTaskManager::NetTaskManager()
{

}


