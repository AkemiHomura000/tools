#pragma once
#include<QPointer>

#include "Node_Serializable.h"


QT_BEGIN_NAMESPACE
class QDMGraphicsSocket;
class Node;
class Edge;
QT_END_NAMESPACE

enum class SOCKETPOS
{
    LEFT_TOP=1,
    LEFT_BOTTOM=2,
    RIGHT_TOP= 3,
    RIGHT_BOTTOM=4,
    BOTTOM=5,
    TOP=6,
};
enum class SOCKETTYPE
{
    SCOKETIN=1,
    SCOKETOUT=2,
    SOCKETNOTDEFIND=3
};

class Socket :public std::enable_shared_from_this<Socket>, public Serializable
{
public:
    /**
    * @brief Node
    1.属于哪一个节点(图)需要被socket知道
    2.在节点中的序号需要被知道
    3.点的位置需要被记录
    */

    Socket(std::weak_ptr<Node>,int index, SOCKETPOS position, SOCKETTYPE socketType);
    ~Socket();
    void initUi();

    std::weak_ptr<Node> getNode() const;
    std::weak_ptr<Edge> getEdge() const;
    const bool hasEdge();
    void setConnectedEdge(std::weak_ptr<Edge> edge);
    void cancelConnectedEdge();
    QPointF getSocketPostion() const;
    SOCKETPOS getPosition() const;
    const unsigned int getId()const { return m_id; };

    const rapidjson::Document serialize() override;
    void deserialize(const rapidjson::Value& value) ;

protected:

private:
    QDMGraphicsSocket* m_grSocket=nullptr; //socket的gr图形,  
    std::weak_ptr<Node> m_node; //所属的node
    std::weak_ptr<Edge> m_edge; //点上连接的edge对象
    int m_index = 0;
    SOCKETPOS m_position= SOCKETPOS::LEFT_TOP;
    SOCKETTYPE m_socketType = SOCKETTYPE::SOCKETNOTDEFIND;

    unsigned int m_id = 300000; //3代表socket ,端口范围大一些
    static unsigned int m_count; // 静态计数器  
};