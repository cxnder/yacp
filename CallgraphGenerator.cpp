//
// Created by Kat on 8/17/22.
//

#include "CallgraphGenerator.h"

CallgraphGenerator* CallgraphGenerator::GetInstance(Ref<BinaryView> view)
{
    if (!callGraphInstances.count(view)) {
        callGraphInstances[view] = new CallgraphGenerator(view);
    }

    return callGraphInstances[view];
}

CallgraphGenerator::CallgraphGenerator(Ref<BinaryView> data) : m_data(data)
{
    m_data->RegisterNotification(this);
    RebuildCache();
}

void CallgraphGenerator::RebuildCache()
{
    m_nodes.clear();
    m_nodeMap.clear();

    // Generate an initial set of nodes
    for (auto f : m_data->GetAnalysisFunctionList())
    {
        CallGraphNode *node = new CallGraphNode();
        node->func = f;
        m_nodes.push_back(node);
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

    m_cacheValid = true;
}

void CallgraphGenerator::OnAnalysisFunctionAdded(BinaryView*, Function*)
{
    m_cacheValid = false;
}

void CallgraphGenerator::OnAnalysisFunctionUpdated(BinaryView*, Function*)
{
    m_cacheValid = false;
}

void CallgraphGenerator::OnAnalysisFunctionRemoved(BinaryView*, Function*)
{
    m_cacheValid = false;
}

Ref<FlowGraph> CallgraphGenerator::GenerateCallgraphInDirection(Ref<Function> func, bool up, bool down, bool full)
{
    if (!m_cacheValid)
        RebuildCache();

    Ref<FlowGraph> graph = new FlowGraph();

    auto baseNode = m_nodeMap[func];

    if (!baseNode)
        RebuildCache();
    if (!baseNode)
    {
        LogAlert("A serious issue occurred with flowgraph generation");

        auto node = new FlowGraphNode(graph);
        DisassemblyTextLine line;
        line.tokens.emplace_back(TextToken, "base node was null.");
        DisassemblyTextLine line2;
        line2.tokens.emplace_back(TextToken, "This shouldn't ever occur. Please file an issue");
        node->SetLines({line, line2});
        graph->AddNode(node);
        return graph;
    }

    vector<CallGraphNode*> validNodes;

    if (!full)
    {
        validNodes.push_back(baseNode);

        vector<CallGraphNode*> upwardValidNodes;
        vector<CallGraphNode*> downwardValidNodes;

        int added = 0;
        if (up) {
            for (auto incoming: baseNode->incomingEdges)
            {
                upwardValidNodes.push_back(incoming);
                added += 1;
            }
            while (added)
            {
                added = 0;
                for (auto node: upwardValidNodes)
                {
                    if (node)
                    {
                        for (auto incoming : node->incomingEdges)
                        {
                            if (std::count(upwardValidNodes.begin(), upwardValidNodes.end(), incoming) == 0)
                            {
                                if (incoming)
                                {
                                    upwardValidNodes.push_back(incoming);
                                    added += 1;
                                }
                            }
                        }
                    }
                }
            }
        }

        added = 0;
        if (down)
        {
            for (auto incoming: baseNode->outgoingEdges)
            {
                downwardValidNodes.push_back(incoming);
                added += 1;
            }
            while (added)
            {
                added = 0;
                for (auto node: downwardValidNodes)
                {
                    for (auto incoming: node->outgoingEdges)
                    {
                        if (std::count(downwardValidNodes.begin(), downwardValidNodes.end(), incoming) == 0)
                        {
                            if (incoming)
                            {
                                downwardValidNodes.push_back(incoming);
                                added += 1;
                            }
                        }
                    }
                }
            }
        }

        if (!upwardValidNodes.empty())
            validNodes.insert( validNodes.end(), upwardValidNodes.begin(), upwardValidNodes.end() );
        if (!downwardValidNodes.empty())
            validNodes.insert( validNodes.end(), downwardValidNodes.begin(), downwardValidNodes.end() );
    }
    else
    {
        validNodes = m_nodes;
    }

    for (auto node : validNodes)
    {
        // We need to always clear out and replace the FlowGraphNodes
        // BN's FlowGraph parser segfaults if you have edges to nodes not in the graph
        // and there is no way to clear out edges.
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
