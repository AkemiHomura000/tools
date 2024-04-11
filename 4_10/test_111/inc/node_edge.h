#pragma once
#include<QString>
#include<QList>
#include<QPair>
#include<QPointF>
#include<QPointer>

#include "Node_Serializable.h"

enum class EDGEPATHTYPE
{
	Line = 1,
	CubicCurve = 2,
	BezierCurve = 3
};

QT_BEGIN_NAMESPACE
class Scene;
class Socket;
class QDMGraphicsEdge;
QT_END_NAMESPACE

/* 
@todo 【20240112】完全不依赖端口的线段，理论上需要单独设计。 思想是先有线，再绑定端口。原则上可以实现，具体看需求整理
*/
class Edge:public std::enable_shared_from_this<Edge>,public  Serializable
{
public:
	//凭借端口生成线
	Edge(std::weak_ptr<Scene> sence, std::weak_ptr<Socket> start_socket, std::weak_ptr<Socket> end_socket, EDGEPATHTYPE type = EDGEPATHTYPE::Line);
	//无端口生成线
	Edge(std::weak_ptr<Scene> sence, EDGEPATHTYPE type = EDGEPATHTYPE::Line);
	void initUi();
	~Edge();

	void remove_from_socket();
	void remove();
	void updatePosition();
	void setStartSocket(std::weak_ptr<Socket> start_socket);
	void setEndSocket(std::weak_ptr<Socket> end_socket);
	std::weak_ptr<Socket> getStartSocket()const;
	std::weak_ptr<Socket> getEndSocket() const;
	QDMGraphicsEdge* getGrEdge()const;

	const rapidjson::Document serialize() override;
	void deserialize(const rapidjson::Value& value) override;
protected:
private:
	std::weak_ptr <Scene> m_sence;//【20240112】edge不影响scene的生命周期
	std::weak_ptr<Socket> m_start_socket; //【20240112】有需要用weak保存吗.目前认为有必要，Socket生命周期不应该和edge绑定
	std::weak_ptr<Socket> m_end_socket;
	QDMGraphicsEdge* m_grEdge=nullptr;
	EDGEPATHTYPE m_edgeType = EDGEPATHTYPE::Line;

	unsigned int m_id = 20000; //2开头代表edge
	static unsigned int m_count; // 静态计数器  
};
