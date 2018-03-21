// Fill out your copyright notice in the Description page of Project Settings.

#include "VisualNode.h"


FString UVisualNode::GetNodeName(EVisualNodeType Type)
{
	switch (Type)
	{
	case EVisualNodeType::StaticMesh:
		return TEXT("Static Mesh Node");
	default:
		return TEXT("None");
	}
}
