// Fill out your copyright notice in the Description page of Project Settings.
#include "DungeonEditor/EmitterNode/EmitterGraphNode.h"

#include "Editor/MarkerNode.h"

FText UEmitterGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (MarkerNode)
	{
		return FText::FromName(MarkerNode->MarkerName);
	}
	return FText::FromString(TEXT("Emitter"));
}

FLinearColor UEmitterGraphNode::GetNodeTitleColor() const
{
	return FLinearColor::Red;
}
