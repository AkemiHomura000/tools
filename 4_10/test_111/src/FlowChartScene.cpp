
#include<iostream>
#include<QDebug>
#include<spdlog/spdlog.h>
#include<QMimeData>
#include<QIODevice>
#include<QPainter>

#include "FlowChartScene.h"


inline static bool FCSDEBUG = true;
FlowChartScene::FlowChartScene(std::shared_ptr<Scene> scene, QObject* parent )
	: QGraphicsScene(parent)
{
	m_scene = scene;
	spdlog::info("FlowChartScene creat success" );
	setSceneRange(30000, 30000);
    QColor backgroundColor = QColor("#D3D3D3");
	setBackgroundBrush(backgroundColor);

	QColor colorLight = QColor("#2f2f2f");
	m_penLight.setColor(colorLight);
	m_penLight.setWidth(1);

	QColor colordark = QColor("#292929");
	m_penDark.setColor(colordark);
	m_penDark.setWidth(2);

};


FlowChartScene::~FlowChartScene()
{
	spdlog::info("FlowChartScene delete success" );
}


/**
 * @brief 设置scene左上点和长宽
 * @todo
 * @param
 * @return
 */

void FlowChartScene::setSceneRange(qreal width, qreal hight)
{
	setSceneRect(-width / 2, -hight / 2, width, hight);
}

/**
 * @brief 绘制背景
 * @todo
 * @param
 * @return
 */
void FlowChartScene::drawBackground(QPainter* painter, const QRectF& rect) 
{
	//调用父类的drawBackground函数以保留默认的背景绘制行为
	QGraphicsScene::drawBackground(painter, rect);

	//creat grid
	//返回不大于指定浮点数的最大整数值
	int left =std::floor(rect.left());
	//返回不小于指定浮点数的最小整数值
	int right = std::ceil(rect.right());
	int top = std::floor(rect.top());
	int bottom =std::ceil(rect.bottom());
	//spdlog::debug("left is {0},right is{1}, top is{2},bottom is{3}",left,right,top,bottom);

	int firstLeft = left - (left%m_gridSize);
	int firstTop = top- (top%m_gridSize);
	//spdlog::debug("firstLeft is{0}",firstLeft);


	//首先创建了一个QList<QLine>类型的linesLight列表，然后向其中添加QLine对象，该对象表示从点(0,0)到点(100,100)的直线。
	//最后，使用painter->drawLines(linesLight)函数将这些直线绘制到画布上。
	QList<QLine> linesLight;
	QList<QLine> linesDark;
	//生成一个从a开始、小于b的整数序列，步长为c
	for (int x= firstLeft; x<right; x+=m_gridSize)
	{
		if (x % (m_gridSize *m_gridSquares) != 0)
		{
			linesLight.append(QLine(x, top, x, bottom));
		}
		else
		{
			linesDark.append(QLine(x, top, x, bottom));
		}
	}

	for (int y = firstTop; y <bottom; y += m_gridSize)
	{
		if (y % (m_gridSize * m_gridSquares) != 0)
		{
			linesLight.append(QLine(left, y, right, y));
		}
		else
		{
			linesDark.append(QLine(left, y, right, y));
		}

	}
	painter->setPen(m_penLight);
	painter->drawLines(linesLight);
	painter->setPen(m_penDark);
	painter->drawLines(linesDark);
}



