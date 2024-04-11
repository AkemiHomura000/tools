#pragma once
#include <QString>
#include <QList>
#include <QPair>
#include <QPointF>
#include <QPointer>

#include "node_socket.h"
#include "Node_Serializable.h"

QT_BEGIN_NAMESPACE
class Scene;
class QDMGraphicsNode;
QT_END_NAMESPACE

class Node : public std::enable_shared_from_this<Node>, public Serializable
{
public:
    Node(std::weak_ptr<Scene> scene, QString title = "undefined title");
    ~Node();
    void initUi(QList<int> inputindexs = {}, QList<int> outputindexs = {});
    const QString getTitle();
    void setPos(QPointF pos);
    void setPos(qreal x, qreal y);
    const QPointF getPos();
    QDMGraphicsNode *getGrNode() { return m_grNode; };
    std::weak_ptr<Scene> getScene() { return m_scene; };
    const QPointF getSocketPos(int index, SOCKETPOS postion);
    QList<std::shared_ptr<Socket>> getInputs();
    QList<std::shared_ptr<Socket>> getOutputs();
    void updateConnectedEdge();

    void remove();

    const rapidjson::Document serialize() override;
    void deserialize(const rapidjson::Value &value) override;
    void deserializePre(const rapidjson::Value &value);
    void deserializePost(const rapidjson::Value &value);
    std::string variable="";  // 判定变量
    std::string condition=""; // 判定条件
    std::string action=""; // 动作
protected:
private:
    std::weak_ptr<Scene> m_scene;
    QString m_title = "undefined";
    QDMGraphicsNode *m_grNode = nullptr;
    qreal m_socketSpacing = 25.0;

    /**
    //【20240106】保存图元上的入出口,生命周期是Node的子集
    【todo】 1. A智能指针管理方案QList<std::shared_ptr<Socket>> m_outputs / B 循环删除 ;
    【todo】 2.不在堆上创建，以下两个list生命周期与node一致   QList<Socket> m_inputs;
    【todo】 3.如果Socket的生命周期和Node不一致，比如可以增删Node,则需用在堆上创建并管理，后续看情况,先这样
    【todo】 4. 2的做法不对，经过析构打印测试，虽然图像还在，但在循环添加后socket立刻被析构，后续连接啥的要用，所以2法不对
    【todo】 5. unique_ptr 代表的是专属所有权，即由 unique_ptr 管理的内存，只能被一个对象持有；是否考虑用unique指针，尚未清晰
     对象析构时，成员会释放内存。unique_ptr 代表的是专属所有权，即由 unique_ptr 管理的内存，只能被一个对象持有。感觉更合理，因为
     每个socket对象只会被一个node对象管理。而非同一个socket属于不同的node

     【说明】std::unique_ptr 是独占式的，即一个 unique_ptr 在任何时候都只能指向一个对象，并且该对象只能被一个 unique_ptr 管理。
        这确保了对象的所有权在任何时候都清晰明了。而 std::shared_ptr 是共享式的，允许多个 shared_ptr 实例共享同一个对象的所有权。
    */
    QList<std::shared_ptr<Socket>> m_inputs;
    QList<std::shared_ptr<Socket>> m_outputs;

    unsigned int m_id = 10000;   // 1开头代表node
    static unsigned int m_count; // 静态计数器  ,【240120】需考虑线程安全，但暂时不考虑
};
