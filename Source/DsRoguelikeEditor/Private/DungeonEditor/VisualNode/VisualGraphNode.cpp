// Fill out your copyright notice in the Description page of Project Settings.

#include "VisualGraphNode.h"
#include "VisualNode.h"

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
