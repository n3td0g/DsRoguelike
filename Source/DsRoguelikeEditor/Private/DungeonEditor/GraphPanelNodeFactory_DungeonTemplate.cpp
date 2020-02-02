#include "DungeonEditor/GraphPanelNodeFactory_DungeonTemplate.h"
#include "DungeonEditor/MarkerNode/GraphNode_MarkerNode.h"
#include "DungeonEditor/VisualNode/GraphNode_VisualNode.h"
#include "DungeonEditor/EmitterNode/GraphNode_EmitterNode.h"
#include "DungeonEditor/MarkerNode/MarkerGraphNode.h"
#include "DungeonEditor/VisualNode/VisualGraphNode.h"
#include "DungeonEditor/EmitterNode/EmitterGraphNode.h"

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
