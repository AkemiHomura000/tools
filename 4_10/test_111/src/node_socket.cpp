#include<QDebug>
#include<QPair>
#include <spdlog/spdlog.h>

#include "node_socket.h"
#include"node_graphics_socket.h"
#include"node_node.h"
#include"node_graphics_node.h"

inline static bool NODESOCKETDEBUG = false;
unsigned int Socket::m_count = 300000;

Socket::Socket(std::weak_ptr<Node> node, int index, SOCKETPOS position, SOCKETTYPE socketType)
	:m_node(node),m_index(index),m_position(position),m_socketType(socketType)
{
	m_id = ++m_count;
	if (NODESOCKETDEBUG)
	{
		spdlog::info("socket is create");
		//qDebug() << "socket--creat with:" << m_index << " ," << (int)m_position << "node: " << m_node.get();
	}

}

Socket::~Socket()
{
	if (m_id > 399999)
	{
		spdlog::critical("Attention! edge id is more than limit");
	};

	m_grSocket=nullptr; //指针在，但是析构访问冲突；QT的内存管理
	if (NODESOCKETDEBUG != false)
	{
		spdlog::info("socket is delete");
		if (!m_node.expired())
		{
			spdlog::info("{0}'s socket is delete", m_node.lock()->getTitle().toStdString());
		}
	}
}

void Socket::initUi()
{
	//【Attention】【20240111】m_node在此函数中必然存在，无需判断。但限制是在Socket构造后之执行一次、多次执行有多点
	m_grSocket = new QDMGraphicsSocket(shared_from_this(), m_socketType, m_node.lock()->getGrNode());
	m_grSocket->setPos(m_node.lock()->getSocketPos(m_index, m_position));
}



std::weak_ptr<Node> Socket::getNode() const
{
	return m_node;
}
std::weak_ptr<Edge> Socket::getEdge() const
{
	return m_edge;
}

const bool Socket::hasEdge()
{
	return (!m_edge.expired()) ? true : false;
}

void Socket::setConnectedEdge(std::weak_ptr<Edge> edge)
{
	m_edge = edge;
}

void Socket::cancelConnectedEdge()
{
	m_edge.reset();
}

QPointF Socket::getSocketPostion() const
{
	if (NODESOCKETDEBUG)
	{
		//qDebug() << "GSP:" << m_index << " " <<(int)m_position<< "node: "<<m_node.get();
		//qDebug() << "res:" << m_node->getSocketPos(m_index, m_position);
	}
	
	return m_node.lock()->getSocketPos(m_index, m_position);
}

SOCKETPOS Socket::getPosition() const
{
	return m_position;
}

const rapidjson::Document Socket::serialize()
{
	rapidjson::Document d; // 创建JSON对象  
	d.SetObject();

	d.AddMember("id", m_id, d.GetAllocator());
	d.AddMember("index", m_index, d.GetAllocator());\
	d.AddMember("position", (unsigned int)m_position, d.GetAllocator());
	d.AddMember("socket_Type", (unsigned int)m_socketType, d.GetAllocator());

	return d;
}

void Socket::deserialize(const rapidjson::Value& value)
{
	if (value.HasMember("id"))
	{
		m_id = value["id"].GetUint();
	}
}


