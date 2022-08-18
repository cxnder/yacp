//
// Created by Kat on 8/17/22.
//

#include "CallgraphGenerator.h"

CallgraphGenerator::CallgraphGenerator(Ref<BinaryView> data, Ref<Function> func) : m_data(data), m_baseFunction(func)
{

}

Ref<FlowGraph> CallgraphGenerator::GenerateCallgraphInDirection(bool up, bool down, bool full)
{
    vector<CallGraphNode*> nodes;
    Ref<FlowGraph> graph = new FlowGraph();

    // Generate an initial set of nodes
    for (auto f : m_data->GetAnalysisFunctionList())
    {
        CallGraphNode *node = new CallGraphNode();
        node->func = f;
        nodes.push_back(node);
        m_nodeMap[f] = node;
    }

    // Add edges to the nodes
    for (auto f : m_data->GetAnalysisFunctionList())
    {
        for (auto edge : m_data->GetCallers(f->GetStart()))
        {
            auto callerNode = m_nodeMap[edge.func];
            callerNode->edges.push_back(f);
            m_nodeMap[f]->incomingEdges.push_back(callerNode);
            callerNode->outgoingEdges.push_back(m_nodeMap[f]);
        }
    }


    auto baseNode = m_nodeMap[m_baseFunction];

    vector<CallGraphNode*> validNodes;
    if (!full)
    {
        validNodes.push_back(baseNode);

        int added = 0;
        if (up) {
            for (auto incoming: baseNode->incomingEdges) {
                validNodes.push_back(incoming);
                added += 1;
            }

            while (added) {
                added = 0;
                for (auto node: validNodes) {
                    for (auto incoming: node->incomingEdges) {
                        if (std::count(validNodes.begin(), validNodes.end(), incoming) == 0) {
                            validNodes.push_back(incoming);
                            added += 1;
                        }
                    }
                }
            }
        }

        added = 0;
        if (down) {
            for (auto incoming: baseNode->outgoingEdges) {
                validNodes.push_back(incoming);
                added += 1;
            }
            while (added) {
                added = 0;
                for (auto node: validNodes) {
                    for (auto incoming: node->outgoingEdges) {
                        if (std::count(validNodes.begin(), validNodes.end(), incoming) == 0) {
                            validNodes.push_back(incoming);
                            added += 1;
                        }
                    }
                }
            }
        }
    }
    else
    {
        validNodes = nodes;
    }

    for (auto node : validNodes)
    {
        node->node = new FlowGraphNode(graph);
        DisassemblyTextLine line;
        line.tokens.emplace_back(CodeSymbolToken, node->func->GetSymbol()->GetFullName(), node->func->GetStart());
        node->node->SetLines({line});
    }

    for (auto node : validNodes)
    {

        for (auto edge : node->edges)
        {
            if (m_nodeMap[edge]->node && std::count(validNodes.begin(), validNodes.end(), m_nodeMap[edge]))
            {
                node->node->AddOutgoingEdge(UnconditionalBranch, m_nodeMap[edge]->node);
            }
        }
        if (node->node)
            graph->AddNode(node->node);
    }

    return graph;
}
