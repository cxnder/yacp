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


class CallgraphGenerator : public BinaryDataNotification
{

    Ref<BinaryView> m_data;
    Ref<Logger> m_logger;

    map<Ref<Function>, CallGraphNode*> m_nodeMap;
    vector<CallGraphNode*> m_nodes;

    bool m_cacheValid;

public:

    static CallgraphGenerator* GetInstance(Ref<BinaryView> view);

    CallgraphGenerator(Ref<BinaryView> data);

    void OnAnalysisFunctionAdded(BinaryView *view, Function *func) override;
    void OnAnalysisFunctionUpdated(BinaryView *view, Function *func) override;
    void OnAnalysisFunctionRemoved(BinaryView *view, Function *func) override;

    void RebuildCache();

    Ref<FlowGraph> GenerateCallgraphInDirection(Ref<Function> func, bool up, bool down, bool full = false);
};

static map<Ref<BinaryView>, CallgraphGenerator*> callGraphInstances;

#endif //YACP_CALLGRAPHGENERATOR_H
