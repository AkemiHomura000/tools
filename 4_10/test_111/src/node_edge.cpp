
#include <memory>
#include <spdlog/spdlog.h>

#include"node_edge.h"
#include"node_scene.h"
#include"FlowChartScene.h"
#include"node_graphics_edge.h"
#include"node_socket.h"
#include"node_node.h"
#include"node_graphics_node.h"

inline static bool NODEEDGEDEBUG = false;
unsigned int Edge::m_count = 20000;


Edge::Edge(std::weak_ptr <Scene> sence, std::weak_ptr<Socket> start_socket, std::weak_ptr<Socket> end_socket, EDGEPATHTYPE type)
	:m_sence(sence),m_start_socket(start_socket),m_end_socket(end_socket),m_edgeType(type)
{
	m_id = ++m_count;
	spdlog::info("Edge is create");
}

Edge::Edge(std::weak_ptr <Scene> sence, EDGEPATHTYPE type)
	: m_sence(sence),m_edgeType(type)
{
	m_id = ++m_count;
	//【240112】尚未构造
}

void Edge::initUi()
{
	if (m_id > 29999)
	{
		spdlog::critical("Attention! edge id is more than limit");
	};

	if (m_start_socket.expired()) 
	{
		return;
	}

	//可能空操作后续注意
	m_start_socket.lock()->setConnectedEdge(shared_from_this());
	if (!m_end_socket.expired())
	{
		m_end_socket.lock()->setConnectedEdge(shared_from_this());
	}
	//根据类型生成 1.选择句  2. 父类强转子类？ 3.swith
	m_grEdge = nullptr;
	if (m_edgeType == EDGEPATHTYPE::Line)
	{
		m_grEdge = new QDMGraphicsEdgeDirect(shared_from_this());
	}
	else if (m_edgeType == EDGEPATHTYPE::BezierCurve)
	{
		m_grEdge = new QDMGraphicsEdgeBezier(shared_from_this());
	}

	updatePosition();
	if (NODEEDGEDEBUG)
	{
		qDebug() << "Edge From:" << m_grEdge->getPosSource() << "To" << m_grEdge->getPosDestination();
	}
	m_sence.lock()->getFcScene()->addItem(m_grEdge);
	m_sence.lock()->addEdge(shared_from_this());
}

Edge::~Edge()
{
	m_grEdge = nullptr;
	spdlog::info("Edge is delete");
}

/**
 * @brief remove_from_socket
 * 1.将端点绑定的线，端点本省置为空指针
 */
void Edge::remove_from_socket()
{
	if (!m_start_socket.expired())  //【逻辑】 如果edge没有开始端口了，设置开始端口中的edge为空
	{
		m_start_socket.lock()->cancelConnectedEdge();
	}
	if (!m_end_socket.expired())
	{
		m_end_socket.lock()->cancelConnectedEdge();
	}
	m_start_socket.reset();
	m_end_socket.reset();
}

void Edge::remove()
{
	remove_from_socket();
	m_grEdge ->hide();
	m_sence.lock()->getFcScene()->removeItem(m_grEdge);// 从场景中删除线，保护一下
	m_sence.lock()->getFcScene()->update();
	m_sence.lock()->removeEdge(shared_from_this()); //在场景中删除Edge
}

/**
 * @brief updatePostion
 * 1.m_grEdge->update();: 这一行代码调用 m_grEdge 对象的 update() 函数,更新边的绘制。update() 函数通常用于更新边的外观
 */
void Edge::updatePosition()
{
	if (m_start_socket.expired() )
	{
		return;
	}
	auto source_pos = m_start_socket.lock()->getSocketPostion();
	source_pos += m_start_socket.lock()->getNode().lock()->getGrNode()->pos();
	m_grEdge->setPosSource(source_pos);//起点

	if (!m_end_socket.expired())
	{
		auto end_pos = m_end_socket.lock()->getSocketPostion();
		end_pos += m_end_socket.lock()->getNode().lock()->getGrNode()->pos();
		m_grEdge->setPosDestination(end_pos);
	}
	else
	{
		m_grEdge->setPosDestination(source_pos);
	}
	m_grEdge->update(); //更新边的显示。这个操作通常在改变边的位置后执行，以确保边的新位置被正确显示。

	if (NODEEDGEDEBUG)
	{
		//qDebug() << "SS:" <<m_start_socket.get() << "\nES" << m_end_socket.get();
		//qDebug() << "Node title is:" << m_start_socket->getNode().lock()->getTitle() << "Node pos is:" << m_start_socket->getNode().lock()->getGrNode()->pos() << "\nStartSockect pos is" <<
		//	m_start_socket->getSocketPostion();
	}
}

void Edge::setStartSocket(std::weak_ptr<Socket> start_socket)
{
	m_start_socket = start_socket;
}

void Edge::setEndSocket(std::weak_ptr<Socket> end_socket)
{
	m_end_socket = end_socket;
}

std::weak_ptr<Socket> Edge::getStartSocket() const
{
	return m_start_socket;
}

std::weak_ptr<Socket> Edge::getEndSocket() const
{
	return m_end_socket;
}

QDMGraphicsEdge* Edge::getGrEdge() const
{
	return m_grEdge;
}


const rapidjson::Document Edge::serialize()
{
	// 创建一个空的Document对象
	rapidjson::Document d; // 创建JSON对象  
	d.SetObject();

	// 创建value对象来表示节点
	// rapidjson::Value nodeObj(rapidjson::kObjectType);
	d.AddMember("id", m_id, d.GetAllocator());
	d.AddMember("edge_type",(unsigned int)m_edgeType,d.GetAllocator());
	d.AddMember("strat", m_start_socket.lock()->getId(), d.GetAllocator());
	d.AddMember("end", m_end_socket.lock()->getId(), d.GetAllocator());

	return d;
}

void Edge::deserialize(const rapidjson::Value& value)
{
	if (value.HasMember("id"))
	{
		m_id = value["id"].GetUint();
	}

	if (value.HasMember("edge_type"))
	{
		m_edgeType = (EDGEPATHTYPE)value["edge_type"].GetUint();
	}

	if (value.HasMember("strat")&& value.HasMember("end"))
	{
		//指向序号所在的socket 1.找到scene 2.找到node下的入出端口, 3端口符合则赋值
		for (const auto& node : m_sence.lock()->getNodes())
		{
			for (const auto& socket : node->getInputs() + node->getOutputs())
			{
				if (socket->getId() == value["strat"].GetUint())
				{
					m_start_socket = socket;
				}
				if (socket->getId() == value["end"].GetUint())
				{
					m_end_socket = socket;
				}
			}
		}
	}


}