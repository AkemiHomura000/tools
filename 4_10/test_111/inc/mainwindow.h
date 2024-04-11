#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>

QT_BEGIN_NAMESPACE
class FlowChartView;
class FlowChartScene;
class Scene;
class Node;
class Edge;
QT_END_NAMESPACE


/*
* 内存管理说明：
* m_scene，m_node1智能指针管理
* m_page QT内存树管理，且为QPointer指针。安全
* 【警告】 由于大量使用shared_ptr,注意很多对象构造后必须执行initUI,init等函数。 后期或许用宏实现
*/

class MainWindow : public QMainWindow
{
    Q_OBJECT 
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
protected:
    void initUI();
    void addDebugItems();
private:
    std::shared_ptr<Scene> m_scene;
    QPointer<FlowChartView> m_page;

    //std::shared_ptr<Node> m_node1;
    //std::shared_ptr<Node> m_node2;
    // std::shared_ptr<Node> m_node3;
    std::shared_ptr<Edge> m_edge1;
    std::shared_ptr<Edge> m_edge2;
};
#endif // MAINWINDOW_H
