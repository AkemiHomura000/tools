#pragma once
#include<QGraphicsItem>
#include <unordered_set>
#include<QPen>


QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

class QDMGraphicsCutLine:public QGraphicsItem
{
public:
    QDMGraphicsCutLine( QGraphicsItem* parent = nullptr);
    ~QDMGraphicsCutLine();
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    QVector<QPointF> getlinePoints() const { return m_line_points; }
    void addLinePoint(QPointF &pos);
    void clearLinePoint();
protected:

private:
    QVector<QPointF> m_line_points; //使用哈希表存数据的尝试 std::unordered_set<QPointF>
    QPen m_pen;

};