#ifndef _COMMON_NODE_VIEW_H
#define _COMMON_NODE_VIEW_H

#include <QGraphicsView>
#include "CommonScene.h"

class CommonNodeView :public QGraphicsView
{
public:
	CommonNodeView(CommonScene* scene, QWidget* parent);
	~CommonNodeView();
};

#endif