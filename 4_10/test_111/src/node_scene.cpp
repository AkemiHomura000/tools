
#include <fstream>  
#include <iostream>
#include <QDebug>
#include <spdlog/spdlog.h>
#include <sstream> 


#include"node_scene.h"
#include"node_node.h"
#include"node_edge.h"
#include "FlowChartScene.h"

#include "rapidjson/document.h"  
#include "rapidjson/writer.h"  
#include "rapidjson/stringbuffer.h"  

inline static bool SCENEDEBUG = false;

Scene::Scene(qreal width, qreal hight):m_sceneWidth(width),m_sceneHight(hight)
{
	if (SCENEDEBUG){spdlog::info("Scene create sucess");}
}

void Scene::initUI(QObject* parent)
{
	m_fcScene = new FlowChartScene(shared_from_this(), parent);
	m_fcScene->setSceneRange(m_sceneWidth, m_sceneHight);
}

QPointer <FlowChartScene> Scene::getFcScene() const
{
	return m_fcScene; 
}


Scene::~Scene()
{
	spdlog::info("Scene delete sucess");
	if (SCENEDEBUG) {
	}
}
void Scene::addEdge(std::shared_ptr<Edge> edge)
{
	m_edges.append(edge);
}

/*
如果std::find函数找不到edge对象，它会返回向量的结束迭代器。
因此，该语句通过检查返回的迭代器是否等于edges.end()来确定edge对象是否存在于edges向量中。
如果迭代器等于edges.end()，则表示edge对象不存在于向量中
*/
void Scene::removeEdge(std::shared_ptr<Edge> edge)
{
	//返回的不是结束迭代器说明存在，思考：这样对性能要求如何
	if (std::find(m_edges.begin(), m_edges.end(), edge) != m_edges.end())
	{
		m_edges.erase(std::remove(m_edges.begin(), m_edges.end(), edge));
	}
	else
	{
		qDebug() << " it's not in the list!";
	}
}

QList<std::shared_ptr<Edge>> Scene::getEdges()
{
	return m_edges;
}

void Scene::addNode(std::shared_ptr<Node> node)
{
	m_nodes.append(node);
}

QList<std::shared_ptr<Node>> Scene::getNodes()
{
	return m_nodes;
}


void Scene::removeNode(std::shared_ptr<Node> node)
{
	if (std::find(m_nodes.begin(), m_nodes.end(), node) != m_nodes.end())
	{
		m_nodes.erase(std::remove(m_nodes.begin(), m_nodes.end(), node));
	}
	else
	{
		qDebug() << " node's not in the list!";
	}
}

bool Scene::saveToFile(std::string filename)
{
	std::ofstream out_file(filename, std::ios::out); // 打开文件以进行写入  
	//清空历史file
	out_file.clear();
	if (!out_file) 
	{
		spdlog::error("Failed to open file {}:", filename);
		return false;
	}

	rapidjson::StringBuffer buffer; // 创建字符串缓冲区  
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer); // 创建写入器对象  
	serialize().Accept(writer); // 将JSON数据序列化到写入器对象中  
	//spdlog::info( buffer.GetString());

	out_file << buffer.GetString();  // 将JSON数据写入文件  
	out_file.close(); // 关闭文件  
	spdlog::info("saving to {} sucessful:", filename);
	return true;
}

//从文件加载
bool Scene::loadFromFile(std::string filename)
{
	std::ofstream in_file(filename, std::ios::in);//std::ios::in是一个标志，指示这是一个输入流
	if (!in_file) {
		spdlog::error("Failed to open file {}:", filename);
		return false;
	}

	//std::istringstream和std::ostringstream。std::istringstream用于从字符串中读取数据，而std::ostringstream用于将数据写入字符串。
	std::stringstream buffer; //字符串流对象
	buffer << in_file.rdbuf();

	//解析json并由一个doc管理
	rapidjson::Document document;
	document.Parse(buffer.str().c_str());

	in_file.close(); // 关闭文件  

	//判断解析从流中读取的字符串是否有错误
	if (document.HasParseError())
	{
		spdlog::info("Json Parse error:{}", document.GetParseError()); //打印错误编号
		spdlog::info("deserializing failed ");
		return false;
	}

	//检查正确才传入
	deserialize(document);

	spdlog::info("load to {} sucessful:", filename);

	return true;
}



const rapidjson::Document Scene::serialize()
{
	rapidjson::Document d; // 创建JSON对象  
	d.SetObject();// 设置JSON对象  
	d.AddMember("id", m_id, d.GetAllocator()); // 添加成员变量  
	d.AddMember("sceneWidth", m_sceneWidth, d.GetAllocator());
	d.AddMember("sceneHight", m_sceneHight, d.GetAllocator());

	 
    rapidjson::Value nodesArray(rapidjson::kArrayType);//生成一个Array类型的元素，用来存放Object
	for (const auto &node:m_nodes)
	{
		rapidjson::Value nodeValue;
		nodeValue.CopyFrom(node->serialize(), d.GetAllocator());
		nodesArray.PushBack(nodeValue, d.GetAllocator());
	}
	d.AddMember("nodes", nodesArray, d.GetAllocator());

	rapidjson::Value edgesArray(rapidjson::kArrayType);//生成一个Array类型的元素，用来存放Object
	for (const auto& edge : m_edges)
	{
			rapidjson::Value edgeValue;
			edgeValue.CopyFrom(edge->serialize(), d.GetAllocator());
			edgesArray.PushBack(edgeValue, d.GetAllocator());
	}
	d.AddMember("edges", edgesArray, d.GetAllocator());


	return d;
}

// 在这里可以使用解析后的JSON对象进行操作  
void Scene::deserialize(const rapidjson::Value& doc)
{

	//清除node(所连edge,socket自动清除)
	//如果多次清除-->理论上没有问题,后续构造的会再次删除
	clear();
	
	
	if (doc["nodes"].IsArray()) //nodes不存在会返回false
	{
		int size = doc["nodes"].Size();
		for (int i = 0; i < size; i++) 
		{
			//创建node,node依旧属于当前场景。逻辑成立
			auto node = std::make_shared<Node>(shared_from_this());
			node->deserializePre(doc["nodes"][i]);
			node->initUi();//这个函数默认是要有入出端口数量的,或许得重构一下这个函数
			node->deserializePost(doc["nodes"][i]);//只需要此nodes[x]节点下的信息，在新的没有创建完成前不能为新的设置内容
		}
	}

	//m_edge1 = std::make_shared <Edge>(m_scene, m_node1->getOutputs()[0], m_node2->getInputs()[0], EDGEPATHTYPE::BezierCurve);
	//m_edge1->initUi();
	if (doc["edges"].IsArray()) //nodes不存在会返回false
	{
		int size = doc["edges"].Size();
		for (int i = 0; i < size; i++)
		{
			//创建node,node依旧属于当前场景。逻辑成立
			auto edge = std::make_shared<Edge>(shared_from_this());
			edge->deserialize(doc["edges"][i]);
			edge->initUi();
		}
	}



	//create node

	if (SCENEDEBUG)
	{
		spdlog::info("deserializing Scene sucess ");
	}
}

void Scene::clear()
{
	//列表循环删除,每次尺寸会减一，用for循环会出问题
	while (m_nodes.size() > 0)
	{
		m_nodes[0]->remove();
	}
	//m_nodes.clear();
}

