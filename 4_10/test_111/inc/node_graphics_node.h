#pragma once
#include<QString>
#include<QGraphicsItem>
#include<QColor>
#include<QFont>
#include<QPen>
#include<QPointer>

QT_BEGIN_NAMESPACE
class Node;
class QDMNodeContentWidget;
class QGraphicsProxyWidget;
QT_END_NAMESPACE

//GraphicsView框架结构主要包含三个主要的类QGraphicsScene（场景）、QGraphicsView（视图）、QGraphicsItem（图元）。
class QDMGraphicsNode : public QGraphicsItem 
{
public:
    QDMGraphicsNode(std::weak_ptr <Node> node, QGraphicsItem* parent = nullptr);
    ~QDMGraphicsNode();


    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;


    const QString getTitle();
    const qreal getWidth() { return m_width; };
    const qreal getHeight() { return m_height; };
    const qreal getTitleHeight() { return m_title_height; };
    const qreal getPadding() { return m_padding; };
    const qreal getEdgeSize() { return m_edge_size; };
    std::weak_ptr<Node> getNode() { return m_node; };
    const QPointer<QDMNodeContentWidget> getContentWidget() { return m_contentWidget; };

private:
    void initUI();
    void initTitle(const QString& title);
    void initContent();

    QString m_title = "undefined";
    QColor m_titleColor = QColor(Qt::white);
    QFont m_titleFont = QFont("Ubuntu",10);

    qreal m_width=150, m_height=130, m_edge_size=10, m_title_height=17, m_padding=5;
    
    QPen m_penDefault = QPen(QColor("#7F000000"));
    QPen m_penSelected = QPen(QColor("#FFFFA637"));
    QBrush m_brush_title = QBrush(QColor("#FF313131"));
    QBrush m_brush_background= QBrush(QColor("#E3212121"));

    std::weak_ptr<Node> m_node;
    QPointer<QGraphicsTextItem> m_title_item;
    QPointer<QGraphicsProxyWidget> m_contentWidgetProxy;
    QPointer<QDMNodeContentWidget> m_contentWidget;
    QPointF m_mousePressPoint = QPointF(0,0);
};
