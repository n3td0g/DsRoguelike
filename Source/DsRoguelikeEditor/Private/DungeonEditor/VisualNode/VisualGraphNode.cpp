// Fill out your copyright notice in the Description page of Project Settings.

#include "VisualGraphNode.h"
#include "VisualNode.h"
#include "EdGraph_DungeonTemplate.h"
#include "SlateIconFinder.h"
#include "ClassIconFinder.h"
#include "DungeonTemplate.h"

void UVisualGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "MultipleNodes", FString(), nullptr, TEXT("In"));
}

FText UVisualGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Visual"));
}

FLinearColor UVisualGraphNode::GetNodeTitleColor() const
{
	return FLinearColor::Green;
}

const FSlateBrush* UVisualGraphNode::GetIcon()
{
	if (!VisualNode) {
		return nullptr;
	}

	auto Object = VisualNode->GetObject();
	if (!Object) {
		return nullptr;
	}

	return FClassIconFinder::FindThumbnailForClass(Object->GetClass());

	/*if (Object->IsAsset()) {
		Object->GetPrimaryAssetId()
	}
	else {
		return FClassIconFinder::FindIconForClass(Object->GetClass());
	}*/
}