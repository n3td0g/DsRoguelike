// Fill out your copyright notice in the Description page of Project Settings.

#include "Editor/VisualNode.h"
#include "Editor/StaticMeshNode.h"
#include "Editor/ActorNode.h"

UVisualNode::UVisualNode()
{
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}

FString UVisualNode::GetNodeName(EVisualNodeType Type)
{
	switch (Type)
	{
	case EVisualNodeType::StaticMesh:
		return TEXT("Static Mesh Node");
	case EVisualNodeType::Actor:
		return TEXT("Actor Node");
	default:
		return TEXT("None");
	}
}

UClass* UVisualNode::GetNodeClass(EVisualNodeType Type)
{
	switch (Type)
	{
	case EVisualNodeType::StaticMesh:
		return UStaticMeshNode::StaticClass();
	case EVisualNodeType::Actor:
		return UActorNode::StaticClass();
	default:
		return UVisualNode::StaticClass();
	}
}
