#pragma once

#include <QObject>
#include <QGraphicsScene>
#include<QGraphicsSceneDragDropEvent>

QT_BEGIN_NAMESPACE
class Scene;
QT_END_NAMESPACE

class FlowChartScene : public QGraphicsScene
{
	Q_OBJECT

public:
	FlowChartScene(std::shared_ptr<Scene> scene=nullptr,QObject* parent = nullptr);
	~FlowChartScene();

	void setSceneRange(qreal width, qreal hight);

protected:

	 //GraphicsScene中的虚函数，用于在场景的背景上绘制自定义的图形元素。当场景需要绘制背景时，会调用这个函数来执行绘制操作。
	 void drawBackground(QPainter* painter, const QRectF& rect) override;
private:
	qreal m_sceneWidth = 30000;
	qreal m_sceneHight = 30000;
	QPen m_penLight;
	QPen m_penDark;
	int m_gridSize = 40;
	int m_gridSquares = 5;
	std::shared_ptr<Scene> m_scene;
};

