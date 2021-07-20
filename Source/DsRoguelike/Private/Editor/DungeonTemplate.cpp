// Fill out your copyright notice in the Description page of Project Settings.

#include "Editor/DungeonTemplate.h"

#include "Editor/MarkerNode.h"

UDungeonTemplate::UDungeonTemplate()
{
#if WITH_EDITORONLY_DATA
	EdGraph = nullptr;
#endif
}

UDungeonTemplate::~UDungeonTemplate()
{
}

UMarkerNode* UDungeonTemplate::CreateMarkerNode()
{
	UMarkerNode* MarkerNode = NewObject<UMarkerNode>(this, UMarkerNode::StaticClass());
	MarkerNode->MarkerName = GetFreeMarkerName();
	MarkerNodes.Add(MarkerNode->MarkerName, MarkerNode);

	return MarkerNode;
}

bool UDungeonTemplate::IsNameOccupied(const FName& MarkerName)
{
	return MarkerNodes.Find(MarkerName) != nullptr;
}

FName UDungeonTemplate::GetFreeMarkerName()
{
	int32 Index = 0;
	while (true)
	{
		FName NewMarkerName(*FString::Printf(TEXT("Marker%d"), Index));
		if (!IsNameOccupied(NewMarkerName))
		{
			return NewMarkerName;
		}
		++Index;
	}
}
