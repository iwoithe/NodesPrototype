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

class Node;

class Port
{
public:
    Port(std::string id, Any defaultData, PortType type) : m_id{ id }, m_defaultData{ defaultData }, m_isDirty{ false }, m_type { type }
    {
        reset();
    }

    ~Port()
    {
        m_node = nullptr;
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

    Node* node()
    {
        return m_node;
    }

    void setNode(Node* node)
    {
        m_node = node;
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
    Node* m_node;
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
        port->setNode(this);
        m_ports.push_back(port);
    }

    Port* port(int index)
    {
        return m_ports[index];
    }

    Port* port(std::string id)
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

    bool isInput() const
    {
        // Input nodes only have output ports
        for (Port* p : m_ports) {
            if (p->type() == PortType::INPUT_PORT)
                return false;
        }

        return true;
    }

    bool isOutput() const
    {
        // Output nodes only have input ports
        for (Port* p : m_ports) {
            if (p->type() == PortType::OUTPUT_PORT)
                return false;
        }

        return true;
    }

    std::string name() const
    {
        return m_name;
    }

    void setName(std::string name)
    {
        m_name = name;
    }

    std::vector<Port*> ports()
    {
        return m_ports;
    }
protected:
    std::string m_name;
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

    void addOutputNode(Node* node)
    {
        m_outputNodes.push_back(node);
    }

    void exec()
    {
        // Step 1: Get all input nodes
        std::queue<Node*> inputNodes;

        for (Node* outputNode : m_outputNodes) {
            findInputNode(outputNode, inputNodes);
        }

        while (!inputNodes.empty()) {
            Node* n = inputNodes.front();
            std::cout << n->name() << std::endl;
            inputNodes.pop();
        }

        // Step 2: Perform a topological sort using Kahn's algorithm
        std::queue<Node*> nodeExecOrder;

        // Step 3: Run each node's execution method in correct order

        while (!nodeExecOrder.empty())
        {
            Node* node = nodeExecOrder.front();

            node->exec();
            for (int i = 0; i < node->ports().size(); i++) {
                node->ports()[i]->setIsDirty(false);
            }

            nodeExecOrder.pop();
        }
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
    void findInputNode(Node* node, std::queue<Node*>& inputNodes)
    {
        for (Port* curNodePort: node->ports()) {
            if (curNodePort->type() == PortType::OUTPUT_PORT && curNodePort->node()->isInput())
            {
                inputNodes.push(curNodePort->node());
            }
            else {
                for (Port* port : curNodePort->linkedOutputPorts()) {
                    findInputNode(port->node(), inputNodes);
                }
            }
        }
    }

    std::vector<Node*> m_nodes;
    std::vector<Node*> m_outputNodes;
};

class IntNode : public Node
{
public:
    IntNode()
    {
        m_name = "Integer";

        Port* outputPort = new Port("output", Any(0), PortType::OUTPUT_PORT);
        addPort(outputPort);
    }
};

class AddNode : public Node
{
public:
    AddNode()
    {
        m_name = "Add";

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
        m_name = "Output";

        Port* outputPort = new Port("output", Any(0), PortType::INPUT_PORT);
        addPort(outputPort);
    }

    void exec() override
    {
        std::cout << "[Output Node] output: " << port("output")->get<int>() << std::endl;
    }
};


int main()
{
    AddNode* add1Node = new AddNode();
    add1Node->port("num1")->setData(Any(1));
    add1Node->port("num2")->setData(Any(1));

    IntNode* int1Node = new IntNode();
    int1Node->port("output")->setData(Any(3));

    AddNode* add2Node = new AddNode();

    OutputNode* output1Node = new OutputNode();

    add1Node->port("output")->linkPort(add2Node->port("num1"));
    int1Node->port("output")->linkPort(add2Node->port("num2"));
    add2Node->port("output")->linkPort(output1Node->port("output"));

    IntNode* int2Node = new IntNode();
    int2Node->port("output")->setData(Any(4));
    
    OutputNode* output2Node = new OutputNode();
    
    int2Node->port("output")->linkPort(output2Node->port("output"));

    Graph* graph = new Graph();
    graph->addNode(add1Node);
    graph->addNode(int1Node);
    graph->addNode(add2Node);
    graph->addNode(output1Node);
    graph->addOutputNode(output1Node);

    graph->addNode(int2Node);
    graph->addNode(output2Node);
    graph->addOutputNode(output2Node);

    graph->exec();

    delete graph;

    return 0;
}
