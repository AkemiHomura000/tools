#include<QPainter>
#include<QPainter>
#include<QColor>
#include<QPen>
#include<QBrush>
#include<QPainterPath>
#include"node_graphics_edge.h"
#include "node_socket.h"

/**
 * @brief QDMGraphicsEdge
 * 1.在 QGraphicsPathItem 中,setZValue() 函数用于设置项目的层次结构值。该值决定了该对象在图层中的位置,层次结构值越高,
 对象在图层中的位置就越高。

 */
QDMGraphicsEdge::QDMGraphicsEdge(std::weak_ptr <Edge> edge, QGraphicsItem* parent)
	:QGraphicsPathItem(parent), m_edge (edge)
{
	m_color = QColor("#001000");
	m_pen = QPen(m_color);
	m_pen_selected = QPen(m_color_selected);
	m_pen_dragging = QPen(m_color);
	m_pen.setWidthF(2.0);
	m_pen_selected.setWidthF(2.0);
	m_pen_dragging.setWidthF(2.0);
	m_pen_dragging.setStyle(Qt::DashLine);

	setZValue(-1);
	setFlag(QGraphicsItem::ItemIsSelectable);
	//setFlag(QGraphicsItem::ItemIsMovable);
}

QRectF QDMGraphicsEdge::boundingRect() const
{
	//用于计算路径的边界框。该函数返回一个 QRectF 对象,表示路径的边界框,它包含了路径的所有控制点
	QRectF rect = m_path.controlPointRect();

	// 将 rect 对象的边界框大小向左右两侧调整,调整后的边界框大小将包括路径的所有像素,以及左右两侧的额外宽度
	rect.adjust(-5, -5, 5, 5);

	return rect;
}

void QDMGraphicsEdge::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	updatePath();

	if (m_edge.lock()->getEndSocket().expired())
	{
		painter->setPen(m_pen_dragging);
	}
	else
	{
		if (isSelected())
		{
			painter->setPen(m_pen_selected);

		}
		else
		{
			painter->setPen(m_pen);
		}
	}

	painter->setBrush(Qt::NoBrush);
	painter->drawPath(m_path);
}

void QDMGraphicsEdge::setPosSource(QPointF posSource)
{
	m_posSource = posSource;
}

const QPointF QDMGraphicsEdge::getPosSource()
{
	return m_posSource;
}

void QDMGraphicsEdge::setPosDestination(QPointF posDestination)
{
	m_posDestination = posDestination;
}

const QPointF QDMGraphicsEdge::getPosDestination()
{
	return m_posDestination;
}

std::weak_ptr <Edge> QDMGraphicsEdge::getEdge() const
{
	return m_edge;
}

bool QDMGraphicsEdge::intersectsWith(QPointF p1, QPointF p2)
{
	auto cutPath = QPainterPath(p1);
	cutPath.lineTo(p2);
	return cutPath.intersects(m_path);
}


QDMGraphicsEdgeDirect::QDMGraphicsEdgeDirect(std::weak_ptr <Edge> edge, QGraphicsItem* parent)
:QDMGraphicsEdge(edge, parent)
{

}

void QDMGraphicsEdgeDirect::updatePath()
{
	//设置路径的起点和终点
	m_path=QPainterPath(m_posSource);
	m_path.lineTo(m_posDestination);
	setPath(m_path);
}

QDMGraphicsEdgeBezier::QDMGraphicsEdgeBezier(std::weak_ptr <Edge> edge, QGraphicsItem* parent)
: QDMGraphicsEdge(edge, parent)
{

}


/**
 * @brief updatePath
 * 1.cubicTo() 是 QPainterPath 类中的一个函数, 用于添加一个三次贝塞尔曲线到路径中, 以便路径能够更平滑地连接两个点
 * 2.ubicTo(QPointF c1, QPointF c2, QPointF p): 这行代码表示将三个点连接成一个三次贝塞尔曲线,并将该曲线添加到路径中。
	c1 和 c2 是控制点,表示贝塞尔曲线的两个突变点,p 是终点,表示曲线连接的最后一个点。
 */
void QDMGraphicsEdgeBezier::updatePath()
{
	//计算了路径的宽度的一半
	qreal dist = (m_posDestination.x()-m_posSource.x())*0.5;


	//control point (x,y) set  cpx_s: control point x_source
	qreal cpx_s = 0, cpx_d = 0, cpy_s = 0, cpy_d = 0;
	cpx_s += dist;
	cpx_d -= dist;

	//startsocketposition
	auto sspos = m_edge.lock()->getStartSocket().lock()->getPosition();
	bool resultA = (sspos == SOCKETPOS::RIGHT_TOP || sspos == SOCKETPOS::RIGHT_BOTTOM) ? true : false;
	bool resultB = (sspos == SOCKETPOS::LEFT_TOP || sspos == SOCKETPOS::LEFT_BOTTOM) ? true : false;
	if ((m_posSource.x() > m_posDestination.x() && resultA ) || 
		(m_posSource.x() < m_posDestination.x() && resultB))
	{
		cpx_d *= -1;
		cpx_s *= -1;
		//画出来更漂亮？改变控制点位置
		qreal edge_cp_roundness = 20.0;
		qreal calc = (m_posSource.y() - m_posDestination.y());
		qreal temp_yd = (calc != 0.0) ? calc : 0.00001;
		qreal temp_ys = (-calc != 0.0) ? -calc : 0.00001;
		cpy_d = temp_yd / fabsf(temp_yd)* edge_cp_roundness;
		cpy_s = temp_ys / fabsf(temp_ys) * edge_cp_roundness;

	}
	
	m_path = QPainterPath(m_posSource);
	m_path.cubicTo(QPointF(m_posSource.x()+ cpx_s, m_posSource.y()+cpy_s), QPointF(m_posDestination.x()+cpx_d, m_posDestination.y() + cpy_d), m_posDestination);
	setPath(m_path);
}
