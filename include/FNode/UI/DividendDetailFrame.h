#ifndef _DIVIDEND_DETAIL_FRAME_H
#define _DIVIDEND_DETAIL_FRAME_H


#include <QFrame>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxlayout>
#include <QHBoxLayout>

class DividendDetailFrame :public QFrame
{
	Q_OBJECT
public:
	DividendDetailFrame(QWidget* parent = nullptr);
	~DividendDetailFrame();
private:
	void initUI();
	void onSearchBtnClicked();
private:
	QVBoxLayout* mainLayout_ = nullptr;
	QHBoxLayout* searchLayout_ = nullptr;
	QVBoxLayout* detailLayout_ = nullptr;
	QLineEdit* leStockId_ = nullptr;
	QPushButton* btnSearchStock_ = nullptr;

	QLabel* lbDetail_ = nullptr;

};


#endif