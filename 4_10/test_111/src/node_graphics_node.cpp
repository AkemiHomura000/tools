
#include<QPainter>
#include<QGraphicsProxyWidget>
#include<QGraphicsSceneMouseEvent>
#include <spdlog/spdlog.h>

#include"node_graphics_node.h"
#include"node_node.h"
#include"node_scene.h"
#include"node_content_widget.h"

static bool GRNODEDEBUG = false;

QDMGraphicsNode::QDMGraphicsNode(std::weak_ptr <Node> node,  QGraphicsItem* parent):QGraphicsItem(parent)
{
    m_node = node;
	initUI();
    
    //init title
	initTitle(m_node.lock()->getTitle());

    //init content
    initContent();

    if (GRNODEDEBUG)
    {
        spdlog::info("grNode {} create sucess",m_node.lock()->getTitle().toStdString());
    }
}

QDMGraphicsNode::~QDMGraphicsNode()
{
    delete m_contentWidget;//由于没有收到this管理，在程序结束后才会随着fcscene释放，所以早调取一下。但意外的是并没有发生多次删除的问题
    if (GRNODEDEBUG)
    {
        if (!m_node.expired())
        {
            spdlog::info("grNode {} delete sucess", m_node.lock()->getTitle().toStdString());
        }
    }
}



void QDMGraphicsNode::initUI()
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
}

/**
 * @brief 范围
 * 函数用于返回一个矩形，该矩形定义了图形项的边界框，即图形项所占空间的矩形范围。
   具体来说，这个函数返回了一个 QRectF 对象，该对象表示了一个矩形，其左上角坐标为 (0, 0)，宽度为 m_width，高度为m_height。
   然后调用了 normalized 函数对这个矩形进行规范化处理。
 * 
 */
QRectF QDMGraphicsNode::boundingRect() const
{
	return QRectF(0,0,m_width,m_height).normalized();
}

/**
 * @brief 绘制
	使用 QPainter 来绘制一个带有圆角的矩形节点。首先，它创建了三个不同的 QPainterPath 对象来表示标题、内容和轮廓的形状。
	然后，它使用 QPainter 对象来设置画笔和画刷，并使用 drawPath 函数来绘制这些形状。
 * @param 1. painter：这是一个指向 QPainter 对象的指针，它用于执行绘制操作。通过这个参数，你可以在 paint 函数中使用 QPainter 
	的各种绘制函数来绘制图形。
 * @param 2. option：这是一个指向 QStyleOptionGraphicsItem 对象的指针，
	它提供了有关绘制选项的信息，例如绘制状态、样式等。你可以使用这个参数来根据当前的绘制选项来调整绘制的行为。
 * @param 3. widget：这是一个指向 QWidget 对象的指针，它表示了与图形项相关联的小部件。在大多数情况下，你可以忽略这个参数，
	因为它通常用于在小部件上绘制图形项，而不是在场景中绘制。

*   @other path_outline.addRoundedRect 是一个用于向 QPainterPath 添加圆角矩形的函数。
    它接受六个参数，分别是矩形的左上角 x 坐标、左上角 y 坐标、矩形的宽度、矩形的高度、圆角的横向半径和圆角的纵向半径。
 * @return
 */
void QDMGraphicsNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
 // 三个部分组合成了一个带圆角的矩形。
    // title
    QPainterPath path_title;
    path_title.setFillRule(Qt::WindingFill);
    path_title.addRoundedRect(0, 0, m_width, m_title_height, m_edge_size, m_edge_size);
    //addRect函数用于向 QPainterPath 对象添加一个矩形路径。四个参数，分别是矩形的左上角 x 坐标、左上角 y 坐标、矩形的宽度和矩形的高度
    path_title.addRect(0, m_title_height - m_edge_size, m_edge_size, m_edge_size);
    path_title.addRect(m_width-m_edge_size, m_title_height - m_edge_size, m_edge_size, m_edge_size);
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_brush_title);
    painter->drawPath(path_title.simplified());

    // content
    QPainterPath path_content;
    path_content.setFillRule(Qt::WindingFill);
    path_content.addRoundedRect(0, m_title_height, m_width, m_height - m_title_height, m_edge_size, m_edge_size);
    path_content.addRect(0, m_title_height, m_edge_size, m_edge_size);
    path_content.addRect(m_width - m_edge_size, m_title_height, m_edge_size, m_edge_size);
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_brush_background);
    painter->drawPath(path_content.simplified());

    // outline
    QPainterPath path_outline;
    path_outline.addRoundedRect(0, 0, m_width , m_height , m_edge_size, m_edge_size);
    painter->setPen(isSelected() ? m_penSelected :m_penDefault);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path_outline.simplified());

}

void QDMGraphicsNode::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    //处理Item而非场景的按下事件，故为QGraphicsItem::mouseMoveEvent
    QGraphicsItem::mouseMoveEvent(event);
    setPos(event->scenePos() + m_mousePressPoint);  
    //m_node.lock()->updateConnectedEdge();

    //【todo】【20240113】 optimize me!just update the selected nodes   关于多选中存在移动问题，暂时不适用
    for (const auto& node : m_node.lock()->getScene().lock()->getNodes())
    {
        if (node->getGrNode()->isSelected())
        {
            m_node.lock()->updateConnectedEdge();
        }
    }

    if (GRNODEDEBUG)
    {
        //spdlog::info("i am moving");
    }
}

void QDMGraphicsNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (Qt::LeftButton != event->button())
    {
        event->ignore();
        return;
    }
    m_mousePressPoint = scenePos() - event->scenePos();
    setSelected(true);
}

void QDMGraphicsNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    setSelected(false);
}


/**
 * @brief initTitle
 * @todo [20240105]智能指针会和QT的内存管理冲突么
 *
 */
void QDMGraphicsNode::initTitle(const QString& title)
{
    //指定了父类，则存在QT的内存管理。此外一个问题是，智能指针会和QT的内存管理冲突么
	m_title_item =new QGraphicsTextItem(this);
	m_title_item->setDefaultTextColor(m_titleColor);
	m_title_item->setFont(m_titleFont);
    m_title_item->setPos(m_padding,0);
    //本宽度将被设置为整个项的宽度减去两倍的水平填充值
    m_title_item->setTextWidth(m_width-2*m_padding);

    m_title = title;
    m_title_item->setPlainText(m_title);
}


/**
 * @brief initContent
    在 Qt 中，如果一个图形项是另一个图形项的子项，那么当父项被添加到场景中时，子项也会被自动添加到场景中。
 */
void QDMGraphicsNode::initContent()
{
    m_contentWidgetProxy =new QGraphicsProxyWidget(this);
    //为节点的内容创建类
    m_contentWidget = new QDMNodeContentWidget(m_node);
    m_contentWidget->setGeometry(m_edge_size,m_title_height+m_edge_size,m_width-2*m_edge_size,(m_height-2*m_edge_size-m_title_height));
    //将wiget类变为GRraphicitem类，故m_contentWidget也受到QT管理了
    m_contentWidgetProxy->setWidget(m_contentWidget);
}

const QString QDMGraphicsNode::getTitle()
{
	return m_title;
}
