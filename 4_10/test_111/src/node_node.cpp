
#include <spdlog/spdlog.h>
#include <iostream>
#include "node_node.h"
#include "node_scene.h"
#include "node_edge.h"
#include "node_graphics_node.h"
#include "FlowChartScene.h"

#include "node_content_widget.h"

inline static bool NODEDEBUG = true;

unsigned int Node::m_count = 0;
/**
 * @brief Node
 * 1.在这个类中创建了QDMGraphicsNode对象
 * 2.向FlowChart场景中加入了QDMGraphicsNode对象
 * 3.此类中可获取所绑定的场景（Scene）
 * 4.为啥不直接在QDMGraphicsNode中实现所有功能而是要使用Node这个类来添加暂未明晰。可能和代码的优化有一定关系？
 * 5.感觉主要是为了明晰场景中有哪些图形项而实现的此级代码
 * 6. 【todo】此构造函数中有必要强制指定必须为智能指针么？
 */
Node::Node(std::weak_ptr<Scene> scene, QString title)
	: m_scene(scene), m_title(title)
{
	m_id = ++m_count;
	m_title = QString::number(m_id);
	if (NODEDEBUG)
	{
		spdlog::info("Node {0}  create sucess", m_id);
	}
}

Node::~Node()
{
	m_grNode = nullptr; // 这里不要调用删除，m_grnode被被FcScene管理，赋空即可
	spdlog::info("Node {} delete sucess", m_title.toStdString());
}

void Node::initUi(QList<int> inputindexs, QList<int> outputindexs)
{
	if (m_id > 19999)
	{
		spdlog::critical("Attention! node id is more than limit");
	};

	// 创建一个QDMGraphicsNode对象 此处的m_grNode被FcScene管理
	m_grNode = new QDMGraphicsNode(shared_from_this());
	// 向场景中添加此对象，会被scene管理，所以不用delete
	m_scene.lock()->getFcScene()->addItem(m_grNode);
	m_scene.lock()->addNode(shared_from_this());

	// 向Node中添加socket 且在此处定义了socket的方向
	int counter = 0;
	for (int i = 0; i < inputindexs.size(); ++i)
	{
		auto socket = std::make_shared<Socket>(shared_from_this(), counter, SOCKETPOS::RIGHT_TOP, SOCKETTYPE::SCOKETIN);
		socket->initUi();
		counter += 1;
		m_inputs.append(socket);
		// 在这里对每个元素进行操作
	}

	counter = 0;
	for (int i = 0; i < outputindexs.size(); ++i)
	{
		auto socket = std::make_shared<Socket>(shared_from_this(), counter, SOCKETPOS::BOTTOM, SOCKETTYPE::SCOKETOUT);
		socket->initUi();
		counter += 1;
		m_outputs.append(socket);
		// 在这里对每个元素进行操作
	}
}

const QString Node::getTitle()
{
	return m_title;
}

void Node::setPos(QPointF pos)
{
	m_grNode->setPos(pos);
}

void Node::setPos(qreal x, qreal y)
{
	m_grNode->setPos(x, y);
}

const QPointF Node::getPos()
{
	return m_grNode->pos();
}

const QPointF Node::getSocketPos(int index, SOCKETPOS postion)
{

	qreal x = 0.0;
	if (postion == SOCKETPOS::LEFT_TOP || postion == SOCKETPOS::LEFT_BOTTOM)
	{
		x = 0.0;
	}
	if (postion == SOCKETPOS::RIGHT_TOP)
	{
		x = getGrNode()->getWidth() * 0.5;
	}
	if (postion == SOCKETPOS::BOTTOM && index == 0)
	{

		x = getGrNode()->getWidth() * 0.75;
	}
	if (postion == SOCKETPOS::BOTTOM && index == 1)
	{
		x = getGrNode()->getWidth() * 0.25;
	}

	qreal y = 0;
	if (postion == SOCKETPOS::LEFT_BOTTOM || postion == SOCKETPOS::RIGHT_BOTTOM)
	{
		// start from bottom
		y = getGrNode()->getHeight() - getGrNode()->getEdgeSize() - index * m_socketSpacing;
	}
	if (postion == SOCKETPOS::BOTTOM)
	{
		y = getGrNode()->getHeight() - getGrNode()->getEdgeSize() + 10;
	}
	if (postion == SOCKETPOS::RIGHT_TOP)
	{
		y = 0;
	}

	return QPointF(x, y);
}

QList<std::shared_ptr<Socket>> Node::getInputs()
{
	return m_inputs;
}

QList<std::shared_ptr<Socket>> Node::getOutputs()
{
	return m_outputs;
}

void Node::updateConnectedEdge()
{

	for (int i = 0; i < m_inputs.count(); ++i)
	{
		if (!m_inputs[i]->getEdge().expired())
		{
			m_inputs[i]->getEdge().lock()->updatePosition();
		}
	}
	for (int i = 0; i < m_outputs.count(); ++i)
	{
		if (!m_outputs[i]->getEdge().expired())
		{
			m_outputs[i]->getEdge().lock()->updatePosition();
		}
	}
}

void Node::remove()
{
	m_grNode->hide();
	// 从gr场景中删除grNode,子类grSocket会一起删除
	for (auto const &socket : m_inputs + m_outputs)
	{
		if (socket->hasEdge())
		{
			socket->getEdge().lock()->remove();
		}
	}
	m_scene.lock()->getFcScene()->removeItem(m_grNode);
	m_scene.lock()->getFcScene()->update();
	// 在场景中删除Node
	m_scene.lock()->removeNode(shared_from_this());
}

const rapidjson::Document Node::serialize()
{
	// 创建一个空的Document对象
	rapidjson::Document d; // 创建JSON对象
	d.SetObject();

	d.AddMember("id", m_id, d.GetAllocator());

	// d.AddMember需要一个Value对象作为其值。const char* 不是RapidJSON的Value类型，因此你不能直接这样使用。
	// 创建一个Value对象，并将其设置为字符串。
	const std::string &str = m_title.toStdString();
	rapidjson::Value val(str.c_str(), d.GetAllocator());
	rapidjson::Value variable_1(variable.c_str(), d.GetAllocator());
	rapidjson::Value condition_1(condition.c_str(), d.GetAllocator());
	rapidjson::Value action_1(action.c_str(), d.GetAllocator());
	d.AddMember("title", val, d.GetAllocator());
	d.AddMember("variable", variable_1, d.GetAllocator());
	d.AddMember("condition", condition_1, d.GetAllocator());
	d.AddMember("action", action_1, d.GetAllocator());
	d.AddMember("pos_x", getPos().x(), d.GetAllocator());
	d.AddMember("pos_y", getPos().y(), d.GetAllocator());

	rapidjson::Value inputSocketsArray(rapidjson::kArrayType); // 生成一个Array类型的元素，用来存放Object
	for (const auto &socket : m_inputs)
	{
		rapidjson::Value nodeValue;
		nodeValue.CopyFrom(socket->serialize(), d.GetAllocator());
		inputSocketsArray.PushBack(nodeValue, d.GetAllocator());
	}
	d.AddMember("inputs", inputSocketsArray, d.GetAllocator());

	rapidjson::Value outSocketsArray(rapidjson::kArrayType); // 生成一个Array类型的元素，用来存放Object
	for (const auto &socket : m_outputs)
	{
		rapidjson::Value nodeValue;
		nodeValue.CopyFrom(socket->serialize(), d.GetAllocator());
		outSocketsArray.PushBack(nodeValue, d.GetAllocator());
	}
	d.AddMember("outputs", outSocketsArray, d.GetAllocator());

	// content serialize
	// 生成一个object
	rapidjson::Value contentObj(rapidjson::kObjectType); // 生成一个Array类型的元素，用来存放Object
	contentObj.CopyFrom(m_grNode->getContentWidget()->serialize(), d.GetAllocator());
	d.AddMember("Content", contentObj, d.GetAllocator());

	return d;
}

void Node::deserialize(const rapidjson::Value &value)
{
	// nothing
}

void Node::deserializePre(const rapidjson::Value &value)
{
	// Node初始化完成前更新
	if (value.HasMember("id"))
	{
		m_id = value["id"].GetUint();
	}

	if (value.HasMember("title"))
	{
		m_title = value["title"].GetString();
	}
	if (value.HasMember("variable"))
	{
		variable = value["variable"].GetString();
	}
	if (value.HasMember("condition"))
	{
		condition = value["condition"].GetString();
	}
	if (value.HasMember("action"))
	{
		action = value["action"].GetString();
	}
}

void Node::deserializePost(const rapidjson::Value &value)
{
	// Node初始化完成后更新
	qreal posx = 0.0, posy = 0.0;
	if (value.HasMember("pos_x"))
	{
		posx = value["pos_x"].GetDouble();
	}
	if (value.HasMember("pos_y"))
	{
		posy = value["pos_y"].GetDouble();
	}
	setPos(posx, posy);
	m_grNode->update();

	if (value["inputs"].IsArray())
	{
		int size = value["inputs"].Size();
		// socket属于调用它的node
		for (int i = 0; i < size; i++)
		{
			int counter = 0;
			unsigned int postion = 0;
			unsigned int socketType = 0;

			if (value["inputs"][i].HasMember("index"))
			{
				counter = value["inputs"][i]["index"].GetUint();
			};
			if (value["inputs"][i].HasMember("position"))
			{
				postion = value["inputs"][i]["position"].GetUint();
			};
			if (value["inputs"][i].HasMember("socket_Type"))
			{
				socketType = value["inputs"][i]["socket_Type"].GetUint();
			};
			auto socket = std::make_shared<Socket>(shared_from_this(), counter, SOCKETPOS(postion), SOCKETTYPE(socketType));
			socket->initUi();
			socket->deserialize(value["inputs"][i]);
			m_inputs.append(socket);
			// 在这里对每个元素进行操作
		}
	}

	if (value["outputs"].IsArray())
	{
		int size = value["outputs"].Size();
		// socket属于调用它的node
		for (int i = 0; i < size; i++)
		{
			int counter = 0;
			unsigned int postion = 0;
			unsigned int socketType = 0;

			if (value["outputs"][i].HasMember("index"))
			{
				counter = value["outputs"][i]["index"].GetUint();
			};
			if (value["outputs"][i].HasMember("position"))
			{
				postion = value["outputs"][i]["position"].GetUint();
			};
			if (value["outputs"][i].HasMember("socket_Type"))
			{
				socketType = value["outputs"][i]["socket_Type"].GetUint();
			};
			auto socket = std::make_shared<Socket>(shared_from_this(), counter, SOCKETPOS(postion), SOCKETTYPE(socketType));
			socket->initUi();
			socket->deserialize(value["outputs"][i]);
			m_outputs.append(socket);
			// 在这里对每个元素进行操作
		}
	}
}
