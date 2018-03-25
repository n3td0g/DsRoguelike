// Fill out your copyright notice in the Description page of Project Settings.

#include "MarkerGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraph.h"
#include "VisualGraphNode.h"
#include "VisualNode.h"
#include "MarkerNode.h"
#include "EdGraph_DungeonTemplate.h"
#include "DungeonTemplate.h"


void UMarkerGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, "MultipleNodes", FString(), nullptr, TEXT("Out"));
}

FText UMarkerGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (MarkerNode) {
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

	if (!MarkerNode) {
		return;
	}

	if (!Pin) {
		return;
	}

	MarkerNode->VisualNodes.Empty();

	for (auto Link: Pin->LinkedTo) {
		if (Link) {
			auto VisualNode = Cast<UVisualGraphNode>(Link->GetOwningNode());
			if (VisualNode) {
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

	if (ParrentGraph) {
		auto DungeonTemplate = ParrentGraph->GetDungeonTemplate();
		if (DungeonTemplate) {
			if (DungeonTemplate->MarkerNodes.Contains(MarkerNode->MarkerName)) {
				DungeonTemplate->MarkerNodes.Remove(MarkerNode->MarkerName);
			}
		}
	}
}
