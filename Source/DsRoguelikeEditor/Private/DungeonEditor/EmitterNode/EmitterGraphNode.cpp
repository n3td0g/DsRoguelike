// Fill out your copyright notice in the Description page of Project Settings.

#include "EmitterGraphNode.h"




FText UEmitterGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromName(MarkerName);
}

FLinearColor UEmitterGraphNode::GetNodeTitleColor() const
{
	return FLinearColor::Red;
}
