#pragma once
#include<QGraphicsItem>
#include<QPen>
#include<QBrush>
#include "node_edge.h"

class QDMGraphicsEdge:public QGraphicsPathItem
{
public:
    QDMGraphicsEdge(std::weak_ptr <Edge> edge, QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
   
    virtual void updatePath()=0; ////绘制点A到B，解耦到子类实现
    void setPosSource(QPointF posSource);
    const QPointF getPosSource();
    void setPosDestination(QPointF posDestination);
    const QPointF getPosDestination();
    std::weak_ptr <Edge> getEdge() const;

    //该函数主要用于判断一个线段（由 p1 和 p2 定义）是否与m_path相交
    bool intersectsWith(QPointF p1, QPointF p2);
protected:
    //成员函数好处生成一次后无需在paint函数中多次生成
    QColor m_color=QColor("#001000");;
    QColor m_color_selected= QColor("#00ff00");
    QPen    m_pen;
    QPen    m_pen_selected;
    QPen    m_pen_dragging;


    std::weak_ptr <Edge> m_edge; //grEdge不和Edge强关联，管理者不同
    qreal m_outlineWidth = 1.0;
    QPainterPath m_path;
    EDGEPATHTYPE m_pathType = EDGEPATHTYPE::Line;

    QPointF m_posSource = QPointF(0,0);
    QPointF m_posDestination = QPointF(500, 500);
};

class QDMGraphicsEdgeDirect :public QDMGraphicsEdge
{
public:
    QDMGraphicsEdgeDirect(std::weak_ptr <Edge> edge, QGraphicsItem* parent = nullptr);
    void updatePath() override;
};

class QDMGraphicsEdgeBezier :public QDMGraphicsEdge
{
public:
    QDMGraphicsEdgeBezier(std::weak_ptr <Edge>, QGraphicsItem* parent = nullptr);
    void updatePath() override;
};
