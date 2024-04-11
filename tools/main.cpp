#include <iostream>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/istreamwrapper.h>
#include <vector>
#include "yaml-cpp/yaml.h"
using namespace rapidjson;
struct TreeNode
{
    int id;
    std::string variable;
    std::string condition;
    int left_id;
    int right_id;
    std::string action;
};
int main()
{
    //-----------------------------------------------------------------------------------------------------------------------------------------
    std::vector<TreeNode> tree_nodes;
    // 读取JSON文件
    std::ifstream ifs("/home/lp1/qt_test/4_10/test_111/bin/graph.json");
    IStreamWrapper isw(ifs);
    // 解析JSON
    Document doc;
    doc.ParseStream(isw);
    // 提取节点数据
    const Value &nodes = doc["nodes"];
    for (SizeType i = 0; i < nodes.Size(); i++)
    {
        const Value &node = nodes[i];
        int nodeId = node["id"].GetInt();
        std::string variable = node["variable"].GetString();
        std::string condition = node["condition"].GetString();
        std::string action = node["action"].GetString();
        TreeNode tree_node;
        tree_node.id = nodeId;
        tree_node.left_id = -1;
        tree_node.right_id = -1;
        tree_node.variable = variable;
        tree_node.condition = condition;
        tree_node.action = action;
        tree_nodes.push_back(tree_node);
    }
    for (int i = 0; i < tree_nodes.size(); i++)
    {
        std::cout << "Node " << tree_nodes[i].id << ", Variable: " << tree_nodes[i].variable << ", Condition: " << tree_nodes[i].condition << ", Action: " << tree_nodes[i].action << std::endl;
    }
    // 提取边数据
    const Value &edges = doc["edges"];
    for (SizeType i = 0; i < edges.Size(); i++)
    {
        int startId = edges[i]["strat"].GetInt() - 300000;
        int endId = edges[i]["end"].GetInt() - 300000;
        if (startId % 3 == 0)
        {
            tree_nodes[startId / 3-1].left_id = (endId) / 3 + 1;
            std::cout << "tree_node" << startId / 3 << "\tleft_id=" << tree_nodes[startId / 3].left_id << std::endl;
        }
        if (startId % 3 == 2)
        {
            tree_nodes[(startId + 1) / 3-1].right_id = (endId) / 3 + 1;
            std::cout << "tree_node" << (startId + 1) / 3 << "\tright_id=" << tree_nodes[(startId + 1) / 3].right_id << std::endl;
        }
    }
    //--------------------------------------------------------------------------------------------------------------------------------------------------
    YAML::Node decision_tree;
    for (int i = 0; i < tree_nodes.size(); i++)
    {
            }
    std::vector<YAML::Node> yaml_nodes;
    for (int i = 0; i < tree_nodes.size(); i++)
    {
        std::cout << "Node " << tree_nodes[i].id << ", left_id: " << tree_nodes[i].left_id << ", right_id: " << tree_nodes[i].right_id<< std::endl;

        YAML::Node node;
        node["id"] = tree_nodes[i].id ;
        node["variable"] = tree_nodes[i].variable;
        node["condition"] = tree_nodes[i].condition;
        node["left_id"] = tree_nodes[i].left_id;
        node["right_id"] = tree_nodes[i].right_id;
        node["action"] = tree_nodes[i].action;
        yaml_nodes.push_back(node);
    }

    // Append decision tree nodes to decision_tree_1 key
    for (const auto &node : yaml_nodes)
    {
        decision_tree["decision_tree_1"].push_back(node);
    }

    // Output the decision tree to a YAML file
    std::ofstream fout("decision_tree.yaml");
    fout << decision_tree;
    fout.close();

    std::cout << "YAML file created successfully.\n";

    return 0;
}
