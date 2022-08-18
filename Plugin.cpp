//
// Created by Kat on 8/17/22.
//

#include "binja/binaryninjaapi.h"
#include "CallgraphGenerator.h"

using namespace std;
using namespace BinaryNinja;


void displayCallGraphUpward(BinaryView *view, Function *func)
{
    auto graph = CallgraphGenerator::GetInstance(view)->GenerateCallgraphInDirection(func, true, false);

    view->ShowGraphReport("Calls to " + func->GetSymbol()->GetFullName(), graph);
}

void displayCallGraphDownward(BinaryView *view, Function *func)
{
    auto graph = CallgraphGenerator::GetInstance(view)->GenerateCallgraphInDirection(func, false, true);

    view->ShowGraphReport("Calls from " + func->GetSymbol()->GetFullName(), graph);
}

void displayCallGraphBidirectional(BinaryView *view, Function *func)
{
    auto graph = CallgraphGenerator::GetInstance(view)->GenerateCallgraphInDirection(func, true, true);

    view->ShowGraphReport("Calls to and from " + func->GetSymbol()->GetFullName(), graph);
}


void displayCallGraphFromEntry(BinaryView *view)
{
   auto graph = CallgraphGenerator::GetInstance(view)->GenerateCallgraphInDirection(view->GetAnalysisEntryPoint(), false, true);

    view->ShowGraphReport("Calls from " + view->GetAnalysisEntryPoint()->GetSymbol()->GetFullName(), graph);
}


void displayFullCallgraph(BinaryView* view)
{
    auto graph = CallgraphGenerator::GetInstance(view)->GenerateCallgraphInDirection(view->GetAnalysisFunctionList().at(0), true, true, true);

    view->ShowGraphReport(view->GetFile()->GetFilename() + " Callgraph", graph);
}

extern "C" {
    BN_DECLARE_CORE_ABI_VERSION

    BINARYNINJAPLUGIN bool CorePluginInit()
    {
        // Dont do this.
        PluginCommand::Register("Callgraph\\\x01 From Entry Point", "Generate a callgraph", displayCallGraphFromEntry,
                                [](BinaryView* view){return view->GetAnalysisEntryPoint().GetPtr();});
        // Null entry where isValid always == false, so we can have extra spacing :D
        PluginCommand::Register(           "Callgraph\\\x02————————————————", "\x01", nullptr, [](BinaryView* view){return false;});

        PluginCommand::RegisterForFunction("Callgraph\\\x03 Upward", "Generate a callgraph", displayCallGraphUpward);
        PluginCommand::RegisterForFunction("Callgraph\\\x04 Downward", "Generate a callgraph", displayCallGraphDownward);
        PluginCommand::RegisterForFunction("Callgraph\\\x05 Bidirectional", "Generate a callgraph", displayCallGraphBidirectional);

        PluginCommand::Register("Callgraph\\\x06————————————————", "\x01", nullptr, [](BinaryView* view){return false;});
        PluginCommand::Register("Callgraph\\\x07 Full Program Callgraph", "Generate a callgraph", displayFullCallgraph);

        return true;
    }
}