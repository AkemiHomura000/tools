#include<QColor>
#include<QPainter>
#include <spdlog/spdlog.h>
#include"node_graphics_socket.h"
#include"node_node.h"


static bool GRSOCKETDEBUG = false;
QDMGraphicsSocket::QDMGraphicsSocket(std::weak_ptr <Socket> socket, SOCKETTYPE socketType,QGraphicsItem* parent)
    :QGraphicsItem(parent),m_socket(socket)
{
    //defalt,in,out,other
    QList<QColor> colors = { QColor("#FFFF7700") ,QColor("#FF52e220") ,QColor("#FFa86db1"),QColor("#FF0056a6")
                            ,QColor("#FFb54747")
                            };

    QColor outlineColor = QColor("#FF000000");
    QColor backgroundColor = colors[(int)socketType];
    m_pen = QPen(outlineColor);
    m_pen.setWidthF(m_outlineWidth);
    m_brush = QBrush(backgroundColor);
    if (GRSOCKETDEBUG != false)
    {
        spdlog::info("grSocket create sucess");
    }
}

QDMGraphicsSocket::QDMGraphicsSocket(std::weak_ptr <Socket> socket, QGraphicsItem* parent)
    :QGraphicsItem(parent), m_socket(socket)
{
    QColor outlineColor = QColor("#FF000000");
    QColor backgroundColor = QColor("#FFFF7700");
    m_pen= QPen(outlineColor);
    m_pen.setWidthF(m_outlineWidth);
    m_brush=QBrush(backgroundColor);
}

QDMGraphicsSocket::~QDMGraphicsSocket()
{
    if(GRSOCKETDEBUG != false)
    {
        spdlog::info("grSocket delete sucess");
        if (!m_socket.expired())
        {
        spdlog::info("{0}'s grSocket delete sucess", m_socket.lock()->getNode().lock()->getTitle().toStdString());
        }
    }
}

QRectF QDMGraphicsSocket::boundingRect() const
{
	return QRectF(-m_radius- m_outlineWidth, -m_radius-m_outlineWidth, 2 * (m_radius+m_outlineWidth), 2 * (m_radius + m_outlineWidth));
}

void QDMGraphicsSocket::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    //painting circle
    painter->setBrush(m_brush);
    painter->setPen(m_pen);
    painter->drawEllipse(-m_radius,-m_radius,2*m_radius, 2 * m_radius);
}

std::weak_ptr <Socket> QDMGraphicsSocket::getSocket() const
{
    return m_socket;
}

void QDMGraphicsSocket::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mousePressEvent(event);

    if (GRSOCKETDEBUG)
    {
        spdlog::info("socket is clicked");
    }
}
