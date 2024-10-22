#ifndef _COMMON_SCENE_H
#define _COMMON_SCENE_H

#include <QGraphicsScene>

class CommonScene :public QGraphicsScene
{
public:
	CommonScene(QObject *parent=nullptr);
	~CommonScene();
};


#endif