#include<QPainter>
#include"spdlog/spdlog.h"

#include"node_graphics_cutLine.h"

// 设置画笔（m_pen）的虚线模式。虚线模式是指在绘制图形时，画笔会在指定的点之间绘制一条由短线段组成的虚线。
// 这里设置的虚线模式为：连续绘制3个点，然后跳过3个点，再绘制3个点，以此类推。
QDMGraphicsCutLine::QDMGraphicsCutLine(QGraphicsItem* parent)
	:QGraphicsItem(parent)
{
	m_pen.setColor(Qt::white);
	m_pen.setWidthF(2.0);
	m_pen.setDashPattern({3,3});
	setZValue(2);
	spdlog::info("cutLIne create sucess");
}

QDMGraphicsCutLine::~QDMGraphicsCutLine()
{
	spdlog::info("cutLIne delete sucess");

}

QRectF QDMGraphicsCutLine::boundingRect() const
{
	return QRectF(0, 0, 1, 1);
}

void QDMGraphicsCutLine::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	//设置反锯齿
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(m_pen);
	painter->setBrush(Qt::NoBrush);	
	//QVector<QPointF> line_points { QPointF(0.0,0.0),QPointF(0.0,10.0),QPointF(0.0,20.0) };
	auto poly=QPolygonF(m_line_points);
	painter->drawPolyline(poly);
}
void QDMGraphicsCutLine::addLinePoint(QPointF& pos)
{
	m_line_points.append(pos);
}

void QDMGraphicsCutLine::clearLinePoint()
{
	m_line_points.clear();
}



