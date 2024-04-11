#ifndef FLOWCHARTVIEW_H
#define  FLOWCHARTVIEW_H
#include <QGraphicsView>

QT_BEGIN_NAMESPACE
class FlowChartScene;
class Scene;
class Socket;
class Edge;
class QDMGraphicsCutLine;
QT_END_NAMESPACE


enum class OPERATIONMODE
{
    MODE_NOOP = 1, //nooperater
    MODE_EDGE_DRAG = 2,
    MODE_EDGE_CUT=3
};

class FlowChartView :public QGraphicsView
{
	Q_OBJECT
public:
	FlowChartView(QWidget* parent = nullptr);
	~FlowChartView();

    //【240116目前存在析构问题】这个函数目前为了cutline而创建出来，后续思考优化 
    void init();

    //这个函数临时这么用， 后续优化看情况
    void setCustomSence(std::weak_ptr<Scene> sence);

protected:
    void initUI();
	void mousePressEvent(QMouseEvent* event) override;
    void middleMouseButtonPress(QMouseEvent* event) ;
    void leftMouseButtonPress(QMouseEvent* event);
    void rightMouseButtonPress(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event) override;
    void middleMouseButtonRelease(QMouseEvent* event);
    void leftMouseButtonRelease(QMouseEvent* event);
    void rightMouseButtonRelease(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void delteSelected();

    QGraphicsItem* getItemClicked(QMouseEvent* event);
    bool edgeDragEnd(QGraphicsItem* item);
    void edgeDragStart(QGraphicsItem* item);
    bool distanceBetweenClickedAndReleaseIsOff(QMouseEvent* event);

    void cutIntersectingEdges();

protected:
    QPointF m_last_scene_mouse_position = QPoint(0, 10);
    qreal m_zoomInFactor = 1.25;
    bool m_zoomClamp = true;
    int m_zoom = 2;
    int m_zoomStep = 1;
	QPair<qreal, qreal> m_zoomRange = QPair<qreal, qreal>(0, 10);

    OPERATIONMODE m_operationMode = OPERATIONMODE::MODE_NOOP;
    int m_edge_start_threshold = 10.0;

    std::weak_ptr<Scene> m_Scene; //【temporary】或许会不合适，因为似乎多个scene可设置一个view
    std::weak_ptr<Socket> m_last_start_socket;
    std::weak_ptr<Edge> m_previousEdge; //之前连接的，是已经构造的
    //正在绘制的边界,共享或独占指针应该是.不共享无法为socket设置连接.ps 【析构时候grEdge已经析构，此处只是析构Edge不会有问题】
    std::shared_ptr<Edge> m_dragEdge; 

    /*
    //【todo】【240116】unique指针可能会影响析构，因为是个item. 智能会受到多重管理，在析构顺序不合适情况下，便会出问题。
    // 这个直接受grscene管理，一旦多次析构，会出问题。所以使用智能指针只应该释放所有权,其内容已经删除
    // 普通指针会被grscene删除，然后grView再次析构删除
    */
    QDMGraphicsCutLine *m_cutLine;
    //std::unique_ptr<QDMGraphicsCutLine> m_cutLine;

};

#endif // FLOWCHARTVIEW_H