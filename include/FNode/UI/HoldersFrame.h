#ifndef _HOLDERS_FRAME_H
#define _HOLDERS_FRAME_H

#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>

class HoldersFrame :public QFrame
{
public:
	HoldersFrame(QWidget* parent = nullptr);
	~HoldersFrame();
private:
	void initUI();
private:
	QPushButton* btnStartQuery_;
	QVBoxLayout* mainLayout_;
};


#endif