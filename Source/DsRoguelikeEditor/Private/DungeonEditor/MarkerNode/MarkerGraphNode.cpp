// Fill out your copyright notice in the Description page of Project Settings.
#include "DungeonEditor/MarkerNode/MarkerGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraph.h"
#include "Editor/MarkerNode.h"
#include "DungeonEditor/VisualNode/VisualGraphNode.h"
#include "DungeonEditor/EdGraph_DungeonTemplate.h"
#include "Editor/DungeonTemplate.h"


void UMarkerGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, TEXT("MultipleNodes"), nullptr, TEXT("Out"));
}

FText UMarkerGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (MarkerNode)
	{
		return FText::FromName(MarkerNode->MarkerName);
	}
	return FText::FromString(TEXT("Marker"));
}

FLinearColor UMarkerGraphNode::GetNodeTitleColor() const
{
	return FLinearColor::Gray;
}

void UMarkerGraphNode::PinConnectionListChanged(UEdGraphPin * Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (!MarkerNode)
	{
		return;
	}

	if (!Pin)
	{
		return;
	}

	MarkerNode->VisualNodes.Empty();

	for (auto Link: Pin->LinkedTo)
	{
		if (Link)
		{
			auto VisualNode = Cast<UVisualGraphNode>(Link->GetOwningNode());
			if (VisualNode)
			{
				MarkerNode->VisualNodes.Push(VisualNode->VisualNode);
			}
		}
	}
}

bool UMarkerGraphNode::CanUserDeleteNode() const
{
	return true;
}

void UMarkerGraphNode::DestroyNode()
{
	Super::DestroyNode();

	auto ParrentGraph = Cast<UEdGraph_DungeonTemplate>(GetGraph());

	if (ParrentGraph)
	{
		UDungeonTemplate* DungeonTemplate = ParrentGraph->GetDungeonTemplate();
		if (DungeonTemplate)
		{
			if (DungeonTemplate->MarkerNodes.Contains(MarkerNode->MarkerName))
			{
				DungeonTemplate->MarkerNodes.Remove(MarkerNode->MarkerName);
			}
		}
	}
}
