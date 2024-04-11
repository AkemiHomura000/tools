#pragma once
#include <QList>
#include <QPointer>

#include "Node_Serializable.h"

QT_BEGIN_NAMESPACE
class FlowChartScene;
class Node;
class Edge;
QT_END_NAMESPACE


/*
 @brief
    构造中传入一个父类来引入QT的内存管理机制如何呢？
*/
class Scene :public std::enable_shared_from_this<Scene>,public Serializable
{
public:
	Scene(qreal width=30000, qreal hight=30000);
    ~Scene();
    void initUI(QObject* parent = nullptr);
    QPointer <FlowChartScene> getFcScene() const;
    void addEdge(std::shared_ptr<Edge> edge);
    void removeEdge(std::shared_ptr<Edge> edge);
    QList<std::shared_ptr<Edge>> getEdges();

    void addNode(std::shared_ptr<Node> node);
    void removeNode(std::shared_ptr<Node> node);
    QList<std::shared_ptr<Node>> getNodes();

    bool saveToFile(std::string filename);
    bool loadFromFile(std::string filename);
    
    const rapidjson::Document serialize() override;
    void deserialize(const rapidjson::Value& doc) override;

    void clear();

protected:
    unsigned int m_id = 10086;
    qreal m_sceneWidth = 30000;
    qreal m_sceneHight = 30000;
    QPointer <FlowChartScene> m_fcScene; //【240112】这个好像可以智能指针管理
    
    QList<std::shared_ptr<Edge>> m_edges;
    QList<std::shared_ptr<Node>> m_nodes;
};