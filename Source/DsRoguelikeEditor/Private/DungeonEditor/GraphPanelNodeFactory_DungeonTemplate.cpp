#include "GraphPanelNodeFactory_DungeonTemplate.h"
#include "GraphNode_MarkerNode.h"
#include "GraphNode_VisualNode.h"
#include "GraphNode_EmitterNode.h"
#include "MarkerGraphNode.h"
#include "VisualGraphNode.h"
#include "EmitterGraphNode.h"

FGraphPanelNodeFactory_DungeonTemplate::FGraphPanelNodeFactory_DungeonTemplate()
{

}

TSharedPtr<class SGraphNode> FGraphPanelNodeFactory_DungeonTemplate::CreateNode(UEdGraphNode* Node) const
{
	if (UMarkerGraphNode* EdNode_GraphNode = Cast<UMarkerGraphNode>(Node))
	{
		return SNew(SGraphNode_MarkerNode, EdNode_GraphNode);
	}
	if (UVisualGraphNode* EdNode_GraphNode = Cast<UVisualGraphNode>(Node))
	{
		return SNew(SGraphNode_VisualNode, EdNode_GraphNode);
	}
	if (UEmitterGraphNode* EdNode_GraphNode = Cast<UEmitterGraphNode>(Node))
	{
		return SNew(SGraphNode_EmitterNode, EdNode_GraphNode);
	}
	return nullptr;
}
