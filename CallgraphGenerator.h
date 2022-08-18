//
// Created by Kat on 8/17/22.
//

#ifndef YACP_CALLGRAPHGENERATOR_H
#define YACP_CALLGRAPHGENERATOR_H

#include "binaryninjaapi.h"
#include <map>


using namespace std;
using namespace BinaryNinja;


struct CallGraphNode
{
    Ref<Function> func;
    vector<Ref<Function>> edges;

    vector<CallGraphNode*> incomingEdges;
    vector<CallGraphNode*> outgoingEdges;

    Ref<FlowGraphNode> node;
};


class CallgraphGenerator
{
    Ref<BinaryView> m_data;
    Ref<Function> m_baseFunction;

    map<Ref<Function>, CallGraphNode*> m_nodeMap;

public:
    CallgraphGenerator(Ref<BinaryView> data, Ref<Function> func);

    Ref<FlowGraph> GenerateCallgraphInDirection(bool up, bool down, bool full = false);
};


#endif //YACP_CALLGRAPHGENERATOR_H
