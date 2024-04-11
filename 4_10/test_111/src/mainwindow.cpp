
#include <iostream>
#include <QtWidgets>
#include <QDebug>
#include <spdlog/spdlog.h>

#include "FlowChartScene.h"
#include "FlowChartView.h"
#include "node_scene.h"
#include "node_node.h"
#include "node_edge.h"

#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    spdlog::info("MainWindow create sucess");
    initUI();
    //添加调试的item
    addDebugItems();
}

MainWindow::~MainWindow()
{
    //[level-1] 小心此处没创建便删除会导致内存问题，因为要删除node析构函数中要删除grnode对象
    spdlog::info("MainWindow delete sucess");  

}

/*
/// @brief 
    //其实是Qt有一套回收内存的机制，子窗口可以通过指定父窗口，来托管子窗口内存的释放，而父窗口又通过它的父窗口来销毁对象，
    顶级窗口一般在main函数中实例化为可自动销毁的栈对象，子窗口无需进行额外的释放。
*/
void MainWindow::initUI()
{

    //创建一个场景scene在m_page视图中显示，可在fcscene中里面添加item对象.
    m_scene = std::make_shared<Scene>();
    m_scene->initUI(this);

    //此处的m_page便是交由this来处理内存回收  【240116】这个类感觉有一定优化空间
    m_page = new FlowChartView(this);
    setCentralWidget(m_page);
    m_page->setCustomSence(m_scene);
    m_page->setScene(m_scene->getFcScene());
    m_page->init();

}


/*
@brief 
QGraphicsItem 会自动释放内存。当 QGraphicsItem 不再需要时，它会自动将其从其父对象（如果有的话）中删除，
从而触发 Qt 的垃圾回收机制。此外，当 QGraphicsItem 所在的 QGraphicsScene 或 QGraphicsView 对象被销毁时，
其中的所有项目也会被自动删除，从而确保不会内存泄漏。
 * @todo [20240104]QScene会自行管理其中的item的生命周期么？ 目前查询到的是在Scene删除时候便会删除其中的item
*/
void MainWindow::addDebugItems()
{
    // m_node1 = std::make_shared<Node>(m_scene, "My Test Node1 ");
    // m_node1->initUi(QList<int>{1}, QList<int>{1,2});
    // m_node1->setPos(-350.0, -250.0);

    // m_node2 = std::make_shared <Node>(m_scene, "My Test Node2 ");
    // m_node2->initUi(QList<int>{1}, QList<int>{1,2});
    // m_node2->setPos(-75, 0.0);

    // m_node3 = std::make_shared <Node>(m_scene, "My Test Node3 ");
    // m_node3->initUi(QList<int>{1}, QList<int>{1,2});
    // m_node3->setPos(100, 20.0);


    // m_edge1 = std::make_shared <Edge>(m_scene, m_node1->getOutputs()[0], m_node2->getInputs()[0], EDGEPATHTYPE::BezierCurve);
    // m_edge1->initUi();



    //Test code begin-------------------------------------------------------------------------------------------------------
   
    //Test end-------------------------------------------------------------------------------------------------------

}

