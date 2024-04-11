#include <iostream>
#include <QMetaObject>  
#include <QMouseEvent>
#include <QGraphicsItem>
#include <spdlog/spdlog.h>
#include <QApplication>

#include "FlowChartView.h"
#include "FlowChartScene.h"

#include "node_scene.h"
#include "node_graphics_socket.h"
#include "node_graphics_edge.h"
#include "node_graphics_node.h"
#include "node_socket.h"
#include "node_edge.h"
#include "node_node.h"
#include"node_graphics_cutLine.h"


inline static bool FCVDEBUG=false;
FlowChartView::FlowChartView(QWidget* parent ) :QGraphicsView(parent)
{
    spdlog::info("FlowChartView creat success");
	initUI();

}

FlowChartView::~FlowChartView()
{
    
    spdlog::info( "FlowChartView delete success");
}

//m_cutLine = std::make_unique<QDMGraphicsCutLine>();
void FlowChartView::init()
{
    //构造cutline
    m_cutLine = new QDMGraphicsCutLine();
    m_Scene.lock()->getFcScene()->addItem(m_cutLine);
}

/**
 * @brief 视窗的部分设置
 * @todo 
 * @param 
 * @return 
 */
void FlowChartView::initUI()
{
    //该函数用于设置渲染选项
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::RubberBandDrag); 

    //进行缩放或平移操作时，变换的中心点将会是鼠标指针所在的位置，而不是视图的中心点
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    // enable dropping
    setAcceptDrops(true);
}


void FlowChartView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton) 
    {
        middleMouseButtonPress(event);
    }
    else if (event->button() == Qt::LeftButton) 
    {
        leftMouseButtonPress(event);
    }
    else if (event->button() == Qt::RightButton) 
    {
        rightMouseButtonPress(event);
    }
    else 
    {
        QGraphicsView::mousePressEvent(event);
    }
}

/**
 * @brief middleMouseButtonPress
 * 1.在用户按下鼠标中键时，先模拟释放鼠标事件，然后设置拖拽模式为手形滚动拖拽，最后再模拟按下鼠标事件，以实现启用场景的拖拽功能。
 * 2. 对于QMouseEvent和其他Qt事件对象，更常见的做法是不使用new动态分配它们，而是让它们作为局部变量在栈上分配。
 * 这样，当事件处理函数返回时，它们会自动被销毁，无需手动管理内存
 * 3.&releaseEvent是一个取地址操作，返回一个具体的内存地址
 * @todo 有点没明白为啥要先后模拟按下和释放左键
 * @todo [20240104]如果将releaseEvent和fakeEvent在堆上创建，QT是否会自行管理其生命周期呢？目前保险起见在堆上创建
 * @param param1 参数1的描述
 * @return 
 * setDragMode用户可以使用鼠标拖动视图来进行滚动
 */
void FlowChartView::middleMouseButtonPress(QMouseEvent* event)
{
    // debug printout  
    if (FCVDEBUG) { spdlog::debug("MMB DEBUG:"); }  
    
    // Create a mouse event with the button released  
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, event->localPos(), event->screenPos(),  
                             Qt::LeftButton, Qt::NoButton, event->modifiers());  
    QGraphicsView::mouseReleaseEvent(&releaseEvent);  
  
    // Change to scrollhand mode  
    setDragMode(QGraphicsView::ScrollHandDrag);  
  
    // Create a fake mouse event with the button pressed  
    QMouseEvent fakeEvent(event->type(), event->localPos(), event->screenPos(),  
                          Qt::LeftButton, event->buttons() | Qt::LeftButton, event->modifiers());  
    QGraphicsView::mousePressEvent(&fakeEvent);  

}

void FlowChartView::leftMouseButtonPress(QMouseEvent* event)
{
    auto item = getItemClicked(event);
    m_last_scene_mouse_position = mapToScene(event->pos());

    //shift多选 -------------------------------------------------------------------------------------begin
    //【240113】【class15】 shift按住实现多选 item父类是node / item能转换到edge  /item为空
    if (item != nullptr)
    {
        if (dynamic_cast<QDMGraphicsNode*>(item->parentItem()) || dynamic_cast<QDMGraphicsEdge*>(item))
        {
            if (event->modifiers() == Qt::ShiftModifier)
            {
                event->ignore();
                auto fakeEvent = QMouseEvent(QEvent::MouseButtonPress, event->localPos(), event->screenPos(),
                    Qt::LeftButton, event->buttons() | Qt::LeftButton,
                    event->modifiers() | Qt::ControlModifier);
                QGraphicsView::mousePressEvent(&fakeEvent);
                return;
            }
        }
    }
    else
    { 
        if (event->modifiers() == Qt::ShiftModifier)
        {
            event->ignore();
            auto fakeEvent = QMouseEvent(QEvent::MouseButtonPress, event->localPos(), event->screenPos(),
                Qt::LeftButton, event->buttons() | Qt::LeftButton,
                event->modifiers() | Qt::ControlModifier);
            QGraphicsView::mousePressEvent(&fakeEvent);
            return;
        }
    }
    //shift多选 -------------------------------------------------------------------------------------end



    //socket拖线出现代码----------------------- start
    if (item != nullptr && dynamic_cast<QDMGraphicsSocket*>(item))
    {
        if (m_operationMode == OPERATIONMODE::MODE_NOOP)
        {
            m_operationMode = OPERATIONMODE::MODE_EDGE_DRAG;
            edgeDragStart(item);
            return;
        }
    }

    if (m_operationMode == OPERATIONMODE::MODE_EDGE_DRAG)
    {
        auto res = edgeDragEnd(item);
        if (res)
        {
            return;
        }
    }
    //socket拖线出现代码----------------------- end

    //cutline----------------------------------start
    if (item==nullptr)
    {
        if(event->modifiers() == Qt::ControlModifier)
        {
           // 设置操作模式为边缘检测模式
            m_operationMode = OPERATIONMODE::MODE_EDGE_CUT;
            // 创建一个假的事件，用于模拟鼠标释放事件
            auto fakeEvent = QMouseEvent(QEvent::MouseButtonRelease, event->localPos(), event->screenPos(),
            Qt::LeftButton, Qt::NoButton,event->modifiers());
            spdlog::info("control +Lmb Press");
            // 调用鼠标释放事件函数
            QGraphicsView::mouseReleaseEvent(&fakeEvent);
            QApplication::setOverrideCursor(Qt::CrossCursor);
            // 返回函数
            return;
        }
    }
    //-----------------------------------------end

    QGraphicsView::mousePressEvent(event);
    if (FCVDEBUG) { qDebug() << "LB DEBUG:"; qDebug() << item; }
}

void FlowChartView::rightMouseButtonPress(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);
    auto item = getItemClicked(event); //无法自动识别子类类型似乎,只能强转判断

    //240122测试用，添加node---------------------------------------
    if (event->modifiers() == Qt::ControlModifier)
    {
        auto node = std::make_shared<Node>(m_Scene, "Node Test");
        node->initUi(QList<int>{1}, QList<int>{1,2});
        node->setPos(mapToScene(event->pos()));
    }
    //测试用，添加node---------------------------------------


    if (FCVDEBUG)
    {
        qDebug() << "RMB DEBUG:" << item;
        if (item != nullptr)
        {
            if (dynamic_cast<QDMGraphicsEdge*>(item))
            {
                auto gredge = dynamic_cast<QDMGraphicsEdge*>(item);
                //qDebug() << "edge is:" << gredge->getEdge() << "connect socket is:" << gredge->getEdge()->getStartSocket()
                //    << "<--->" << gredge->getEdge()->getEndSocket();
            }

            if (dynamic_cast<QDMGraphicsSocket*>(item))
            {
                //qDebug() << "socket is:" << dynamic_cast<QDMGraphicsSocket*>(item)->getSocket() << "edge is:" <<
                //    dynamic_cast<QDMGraphicsSocket*>(item)->getSocket()->getEdge();
            }
            qDebug() << "RB PRESS is:" << item << "pos is:" << item->pos();
        }
        else
        {
            qDebug() << "RB PRESS pos is no item";
            qDebug() << "nodes :";
            for (int i = 0; i < m_Scene.lock()->getNodes().size(); i++)
            {
                //qDebug() << m_Scene->getNodes()[i];
            }
            qDebug() << "Edges :";
            for (int i = 0; i < m_Scene.lock()->getEdges().size(); i++)
            {
                //qDebug() << m_Scene->getEdges()[i];
            }
        }
        //dynamic_cast<FlowChartScene*>(scene())->getScene()->getNodes() 逐级寻找法，有点麻烦。待定
    }
}

void FlowChartView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        middleMouseButtonRelease(event);
    }
    else if (event->button() == Qt::LeftButton)
    {
        leftMouseButtonRelease(event);
    }
    else if (event->button() == Qt::RightButton)
    {
        rightMouseButtonRelease(event);
    }
    else
    {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void FlowChartView::middleMouseButtonRelease(QMouseEvent* event)
{
    if (FCVDEBUG){spdlog::debug("MMB RELEASR DEBUG:");}
    // When Middle mouse button was released
    QMouseEvent fakeEvent(event->type(), event->localPos(), event->screenPos(),
        Qt::LeftButton, event->buttons() & ~Qt::LeftButton, event->modifiers());
    QGraphicsView::mouseReleaseEvent(&fakeEvent);
    //用户可以使用鼠标在视图中创建一个矩形框来选择多个图形项
    setDragMode(QGraphicsView::RubberBandDrag);
}

void FlowChartView::leftMouseButtonRelease(QMouseEvent* event)
{
    auto item = getItemClicked(event);

    //shift 【24240113】多选 -------------------------begin
    if (item != nullptr)
    {
        if (dynamic_cast<QDMGraphicsNode*>(item->parentItem()) || dynamic_cast<QDMGraphicsEdge*>(item))
        {
            if (event->modifiers() == Qt::ShiftModifier)
            {
                event->ignore();
                auto fakeEvent = QMouseEvent(event->type(), event->localPos(), event->screenPos(),
                    Qt::LeftButton, Qt::NoButton,
                    event->modifiers() | Qt::ControlModifier);
                QGraphicsView::mousePressEvent(&fakeEvent);
                return;
            }
        }
    }
    else
    {
        if (event->modifiers() == Qt::ShiftModifier)
        {
            event->ignore();
            auto fakeEvent = QMouseEvent(event->type(), event->localPos(), event->screenPos(),
                Qt::LeftButton, Qt::NoButton,
                event->modifiers() | Qt::ControlModifier);
            QGraphicsView::mousePressEvent(&fakeEvent);
            return;
        }
    }
    //shift多选 --------------------------------------------------------------end


    if (m_operationMode == OPERATIONMODE::MODE_EDGE_DRAG)
    {
        //关于释放距离的判断,大于一定距离会绘制新线。
        if (distanceBetweenClickedAndReleaseIsOff(event))
        {
            auto res = edgeDragEnd(item);
            if (res)
            {
                return;
            }
        }
    }

    if (m_operationMode == OPERATIONMODE::MODE_EDGE_CUT)
    {      
        cutIntersectingEdges();
        m_cutLine->clearLinePoint();
        m_cutLine->update();
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        m_operationMode = OPERATIONMODE::MODE_NOOP;
        spdlog::info("control +Lmb Release");
        return;
    }

QGraphicsView::mousePressEvent(event);
if (FCVDEBUG) { qDebug() << "lB RELEASR DEBUG:"; }
}

void FlowChartView::rightMouseButtonRelease(QMouseEvent* event)
{

    QGraphicsView::mousePressEvent(event);
    if (FCVDEBUG) { spdlog::debug("RB RELEASR DEBUG:"); }
}



/**
 * @brief 滚动事件
 * @param param1 参数1的描述
 * @return

 */
void FlowChartView::wheelEvent(QWheelEvent* event)
{
    // 如果按下了Ctrl键
    if (event->modifiers() == Qt::ControlModifier)
    {
        // calculate our zoom Factor
        // 计算出缩放因子
        qreal zoomOutFactor = 1 / m_zoomInFactor;

        // calculate zoom  取鼠标滚轮滚动的垂直方向上的滚动量
        // 计算出缩放比例
        qreal zoomFactor = 0.0;
        if (event->angleDelta().y() > 0)
        {
            // 向上滚动，放大
            zoomFactor = m_zoomInFactor;
            m_zoom += m_zoomStep;
        }
        else
        {
            // 向下滚动，缩小
            zoomFactor = zoomOutFactor;
            m_zoom -= m_zoomStep;
        }

        // 检查缩放比例是否在范围内
        bool clamped = false;
        if (m_zoom < m_zoomRange.first)
        {
            m_zoom = m_zoomRange.first;
            clamped = true;
        }
        if (m_zoom > m_zoomRange.second)
        {
            m_zoom = m_zoomRange.second;
            clamped = true;
        }

        // set scene scale
        // 设置场景缩放比例，且在允许的范围内进行缩放，同时不会进入死循环
        if (!clamped || m_zoomClamp == false)
        {
            scale(zoomFactor, zoomFactor);
        }
    }
    else
    {
        // 如果按下的不是Ctrl键，则调用父类的wheelEvent函数
        QGraphicsView::wheelEvent(event);
    }
}

void FlowChartView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_operationMode == OPERATIONMODE::MODE_EDGE_DRAG)
    {
        auto pos = mapToScene(event->pos());
        m_dragEdge->getGrEdge()->setPosDestination(pos);
        m_dragEdge->getGrEdge()->update();
    }

    if (m_operationMode == OPERATIONMODE::MODE_EDGE_CUT)
    {
        auto pos = mapToScene(event->pos());
        m_cutLine->addLinePoint(pos);
        m_cutLine->update();
    }
    QGraphicsView::mouseMoveEvent(event);
}

void FlowChartView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete)
    {
        delteSelected();
    }
    else if(event->key() == Qt::Key_S && event->modifiers() == Qt::ControlModifier)
    {
        m_Scene.lock()->saveToFile("graph.json");
    }
    else if (event->key() == Qt::Key_L && event->modifiers() == Qt::ControlModifier)
    {
        m_Scene.lock()->loadFromFile("graph.json");
    }
    else
    {
        QGraphicsView::keyPressEvent(event);
    }
}

void FlowChartView::keyReleaseEvent(QKeyEvent* event)
{
}

//dynamic_cast<QDMGraphicsNode*>(item->parentItem())
void FlowChartView::delteSelected()
{
    for (const auto& item : m_Scene.lock()->getFcScene()->selectedItems())
    {
        if(dynamic_cast<QDMGraphicsNode*>(item)) //也删除了其端口所连线
        {
            dynamic_cast<QDMGraphicsNode*>(item)->getNode().lock()->remove();
        }

    }
    //不放在上一个循环中是为了避免多次删除
    for (const auto& item : m_Scene.lock()->getFcScene()->selectedItems())
    {
        if (item != nullptr && dynamic_cast<QDMGraphicsEdge*>(item))
        {
            dynamic_cast<QDMGraphicsEdge*>(item)->getEdge().lock()->remove();
        }
    }
}


/*
 //auto x = itemAt(pos);
 //auto cc1 = dynamic_cast<QDMGraphicsEdge*>(x);  //对象能指向QDMGraphicsEdge么？ 可以，向上转换 
 //auto x2 = itemAt(pos)->parentItem();
 //auto x = dynamic_cast<QDMGraphicsNode*>(itemAt(pos));
*/
QGraphicsItem* FlowChartView::getItemClicked(QMouseEvent* event)
{
    //return the object on which we're clecked/release
    QPoint pos = event->pos();
    return  itemAt(pos);
}


/**
 * @brief edgeDragStart
 * 1.dynamic_cast<QDMGraphicsSocket*>(item)->getSocket() 不用检查的原因是在上级函数已经检查，此处暂没必要
 @todo 【240117】
 * A 每次调用都会创建一个新的 Edge 对象，并将其赋值给 m_dragEdge。由于 m_dragEdge 是一个 std::shared_ptr，
   所以在新的 Edge对象被赋值给 m_dragEdge之前，旧的 Edge对象会被自动删除（如果没有其他 std::shared_ptr 指向它
   在此处是合理的，每次构造一个内容不同的对象，并且在edgeDragEnd后自动删除
   B 在初始化时候构造一个，在edgeDragStart开始时候改变其初始点似乎更合理
 */
void FlowChartView::edgeDragStart(QGraphicsItem* item)
{
    auto edgeDragsocket = dynamic_cast<QDMGraphicsSocket*>(item)->getSocket();
    m_previousEdge = edgeDragsocket.lock()->getEdge().lock(); //可能返回一个有指向智能指针或空指针智能指针
    m_last_start_socket = edgeDragsocket;
    //【attention】 这里由于终点未知，所以出端口指定一个不存在的对象
    // std::weak_ptr<Socket>() 构造了一个空的 std::weak_ptr 对象，这个对象不持有任何 Socket 类型的对象的所有权或引用
    m_dragEdge = std::make_shared<Edge>(m_Scene, edgeDragsocket, std::weak_ptr<Socket>(), EDGEPATHTYPE::BezierCurve);
    m_dragEdge->initUi();

    if (FCVDEBUG)
    {
        qDebug() << "view::edgeDdragStart~Start dragging edge";
        qDebug() << "view::edgeDdragStart~assign Start socket to";
        qDebug() << "view::edgeDdragStart~dragEdge is:" ;
    }
}

bool  FlowChartView::edgeDragEnd(QGraphicsItem* item)
{
    //如果有端口跳过剩余的代码并返回true 
    m_operationMode = OPERATIONMODE::MODE_NOOP;
    if (dynamic_cast<QDMGraphicsSocket*>(item))
    {
        auto socket = dynamic_cast<QDMGraphicsSocket*>(item);
        //如果起点终点一样，删除正在画的，再次为起始点设置原有edge
        if (m_last_start_socket.lock() == socket->getSocket().lock())
        {
            m_dragEdge->remove();
            if (!m_previousEdge.expired())
            {
                m_previousEdge.lock()->getStartSocket().lock()->setConnectedEdge(m_previousEdge);
            }
            return false;
        }

        if (FCVDEBUG) { qDebug() << "View :: edgeDragEnd~previous edge :"; }

        //if (FCVDEBUG) { qDebug() << "View :: edgeDragEnd~assign End socket:" << socket->getSocket(); }
        if (!m_previousEdge.expired()) //先去的edge先移除
        {
            m_previousEdge.lock()->remove();
            if (FCVDEBUG) { qDebug() << "View :: edgeDragEnd~previous edge removed"; }
        }
        m_dragEdge->setStartSocket(m_last_start_socket);
        m_dragEdge->setEndSocket(socket->getSocket());
        m_dragEdge->getStartSocket().lock()->setConnectedEdge(m_dragEdge);
        m_dragEdge->getEndSocket().lock()->setConnectedEdge(m_dragEdge);
        if (FCVDEBUG) { qDebug() << "View :: edgeDragEnd~assign start & End socket to drag edge"; }
        m_dragEdge->updatePosition();
        return true;
    }

    //【240112】没有就取消这个对象目前存在的一切
    if (FCVDEBUG) { qDebug() << "View :: edgeDragEnd~End draging edge"; }
    m_dragEdge->remove();
    //m_dragEdge = nullptr;
    //if (FCVDEBUG) { qDebug() << "View :: edgeDragEnd~about to set socket to previous edge" << m_previousEdge; }
    if (!m_previousEdge.expired())
    {
        m_previousEdge.lock()->getStartSocket().lock()->setConnectedEdge(m_previousEdge);
    }
    if (FCVDEBUG) { qDebug() << "View :: edgeDragEnd~everything done"; }


    return false;
}


/**
 * @brief 范围
    当我们距离上次 LMb 点击场景位置过于遥远时, 可以使用以下方法进行测量:
    计算两个位置之间的距离, 可以使用欧几里得距离公式来计算。
    比较当前位置与上次点击位置之间的距离是否超过了某个阈值。
    如果超过了阈值, 则认为我们与上次点击位置过于遥远, 需要进行重新定位。
 *
 * 函数计算一个阈值,即边开始阈值,并检查该距离的平方是否大于边开始阈值的平方。
    如果大于阈值,则函数返回 true,表示用户可能在画布上进行了有效的绘制操作,否则返回 false。
*@todo 还未非常清晰，后续思考整理20231209
 */

bool FlowChartView::distanceBetweenClickedAndReleaseIsOff(QMouseEvent* event)
{
    //mapToScene 函数接受一个 QPoint 类型的视口坐标作为参数,并将其转换为 QPointF 类型的场景坐标
    auto new_lmb_release_scene_pos = mapToScene(event->pos());
    auto dist_scene = new_lmb_release_scene_pos - m_last_scene_mouse_position;
    auto edge_start_threshold_sq = m_edge_start_threshold * m_edge_start_threshold;
    return (dist_scene.x() * dist_scene.x() + dist_scene.y() * dist_scene.y()) > edge_start_threshold_sq;
}

void FlowChartView::setCustomSence(std::weak_ptr<Scene> sence)
{
    m_Scene = sence;
}

void FlowChartView::cutIntersectingEdges()
{
    for (int ix = 0; ix < m_cutLine->getlinePoints().size()-1; ix++)
    {
        auto p1 = m_cutLine->getlinePoints()[ix];
        auto p2 = m_cutLine->getlinePoints()[ix + 1];

        for (const auto& edge : m_Scene.lock()->getEdges())
        {
            if (edge->getGrEdge()->intersectsWith(p1, p2))
            {
                edge->remove();
            }
        }
    }

}

