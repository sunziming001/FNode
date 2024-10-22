#include "HoldersFrame.h"
#include "MainWindow.h"

HoldersFrame::HoldersFrame(QWidget* parent /*= nullptr*/)
	:QFrame(parent)
{
	initUI();
}

HoldersFrame::~HoldersFrame()
{

}

void HoldersFrame::initUI()
{
	mainLayout_ = new QVBoxLayout;
	this->setLayout(mainLayout_);

	btnStartQuery_ = new QPushButton(tr("StartQuery"), this);
	mainLayout_->addWidget(btnStartQuery_);

	MainWindow* mwnd = dynamic_cast<MainWindow*>(parent());

	if (mwnd)
	{
		connect(btnStartQuery_, &QPushButton::clicked,
			mwnd, &MainWindow::startCrawelHolders);
	}
	
}
