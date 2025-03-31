#include <iostream>
#include <set>
#include <string>
#include <queue>
#include <vector>

#include "Algorithms.h"
#include "iwany.h"
#include "Delete.h"

enum PortType
{
    INPUT_PORT = 0,
    OUTPUT_PORT = 1
};

class Port
{
public:
    Port(std::string id, Any defaultData, PortType type) : m_id{ id }, m_defaultData{ defaultData }, m_isDirty{ false }, m_type { type }
    {
        reset();
    }

    std::string id() const
    {
        return m_id;
    }

    void setId(std::string id)
    {
        m_id = id;
    }

    Any data() const
    {
        return m_data;
    }

    template<typename T> void setData(T data)
    {
        setData(data);
    }

    void setData(Any data)
    {
        m_data = data;
        m_isDirty = true;
    }

    Any defaultData() const
    {
        return m_defaultData;
    }

    void setDefaultData(Any data)
    {
        m_defaultData = data;
    }

    template<typename T> T get() const
    {
        // Convience function to use port->get() instead of port->data.get()
        return m_data.get<T>();
    }

    bool isDirty() const
    {
        return m_isDirty;
    }

    void setIsDirty(bool isDirty)
    {
        m_isDirty = isDirty;
    }

    std::vector<Port*> linkedInputPorts()
    {
        return m_linkedInputPorts;
    }

    std::vector<Port*> linkedOutputPorts()
    {
        return m_linkedOutputPorts;
    }

    void linkPort(Port* port)
    {
        if (m_type == PortType::INPUT_PORT && port->type() == PortType::OUTPUT_PORT) {
            m_linkedOutputPorts.push_back(port);
            port->m_linkedInputPorts.push_back(this);
        }
        else if (m_type == PortType::OUTPUT_PORT && port->type() == PortType::INPUT_PORT) {
            m_linkedInputPorts.push_back(port);
            port->m_linkedOutputPorts.push_back(this);
        }
    }

    PortType type() const
    {
        return m_type;
    }

    void setType(PortType type)
    {
        m_type = type;
    }

    void reset()
    {
        m_data = m_defaultData;
    }

protected:
    std::string m_id;
    Any m_data;
    Any m_defaultData;
    bool m_isDirty;
    std::vector<Port*> m_linkedInputPorts;
    std::vector<Port*> m_linkedOutputPorts;
    PortType m_type;
};

class Node
{
public:
    ~Node()
    {
        DEL_STD_VEC(m_ports);
    }

    virtual void exec() {}

    void addPort(Port* port)
    {
        m_ports.push_back(port);
    }

    Port* getPort(int index)
    {
        return m_ports[index];
    }

    Port* getPort(std::string id)
    {
        if (m_ports.size() == 1 && m_ports[0]->id() == id)
            return m_ports[0];

        int res;
        BINARY_SEARCH(id, m_ports, id, res);

        if (res > -1) {
            return m_ports[res];
        }

        return nullptr;
    }
protected:
    std::vector<Port*> m_ports;
};

class Graph
{
public:
    ~Graph()
    {
        DEL_STD_VEC(m_nodes);
    }

    void addNode(Node* node)
    {
        m_nodes.push_back(node);
    }

    void addOutputNodes(Node* node)
    {
        m_outputNodes.push_back(node);
    }

    void exec()
    {
        std::queue<Node*> nodeExecOrder;
    }

    std::vector<Node*> nodes()
    {
        return m_nodes;
    }

    std::vector<Node*> outputNodes()
    {
        return m_outputNodes;
    }

protected:
    std::vector<Node*> m_nodes;
    std::vector<Node*> m_outputNodes;
};

class NumNode : public Node
{
public:
    NumNode()
    {
        Port* outputPort = new Port("output", Any(0), PortType::OUTPUT_PORT);
        addPort(outputPort);
    }
};

class IntNode : public Node
{
public:
    IntNode()
    {
        Port* outputPort = new Port("output", Any(0), PortType::OUTPUT_PORT);
        addPort(outputPort);
    }
};

class AddNode : public Node
{
public:
    AddNode()
    {
        m_num1Port = new Port("num1", Any(0), PortType::INPUT_PORT);
        addPort(m_num1Port);

        m_num2Port = new Port("num2", Any(0), PortType::INPUT_PORT);
        addPort(m_num2Port);

        m_outputPort = new Port("output", Any(0), PortType::OUTPUT_PORT);
        addPort(m_outputPort);
    }

    void exec() override
    {
        m_outputPort->setData(Any(m_num1Port->get<int>() + m_num2Port->get<int>()));
    }
private:
    Port* m_num1Port;
    Port* m_num2Port;
    Port* m_outputPort;
};

class OutputNode : public Node
{
public:
    OutputNode()
    {
        Port* outputPort = new Port("output", Any(0), PortType::OUTPUT_PORT);
        addPort(outputPort);
    }

    void exec() override
    {
        std::cout << "[Output Node] output: " << getPort("output")->get<int>() << std::endl;
    }
};


int main()
{
    IntNode* int1Node = new IntNode();
    int1Node->getPort("output")->setData(Any(2));

    IntNode* int2Node = new IntNode();
    int2Node->getPort("output")->setData(Any(3));

    AddNode* addNode = new AddNode();

    OutputNode* outputNode = new OutputNode();

    int1Node->getPort("output")->linkPort(addNode->getPort("num1"));
    int2Node->getPort("output")->linkPort(addNode->getPort("num2"));
    addNode->getPort("output")->linkPort(outputNode->getPort("output"));

    Graph* graph = new Graph();
    graph->addNode(int1Node);
    graph->addNode(int2Node);
    graph->addNode(addNode);
    graph->addNode(outputNode);

    graph->exec();

    delete graph;

    return 0;
}
