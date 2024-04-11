#pragma once
#include<QGraphicsItem>
#include<QPen>
#include<QBrush>
#include"node_socket.h"

QT_BEGIN_NAMESPACE
class Socket;
QT_END_NAMESPACE

class QDMGraphicsSocket:public QGraphicsItem
{
public:
    QDMGraphicsSocket(std::weak_ptr <Socket> socket,  SOCKETTYPE socketType, QGraphicsItem* parent = nullptr);
    QDMGraphicsSocket(std::weak_ptr <Socket> socket, QGraphicsItem* parent = nullptr);
    ~QDMGraphicsSocket();

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    std::weak_ptr <Socket> getSocket() const;

    void mousePressEvent(QGraphicsSceneMouseEvent* event);

protected:

private:
    qreal m_radius = 6.0;
    qreal m_outlineWidth = 1.0;
    QPen m_pen;
    QBrush m_brush;
    std::weak_ptr <Socket> m_socket;
};