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

const UObject* UVisualGraphNode::GetObject()
{
	if (!VisualNode) {
		return nullptr;
	}

	return VisualNode->GetObject();
}

UEdGraphPin* UVisualGraphNode::GetInputPin(int32 InputIndex) const
{
	check(InputIndex >= 0);

	for (int32 PinIndex = 0, FoundInputs = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if (Pins[PinIndex]->Direction == EGPD_Input)
		{
			if (InputIndex == FoundInputs)
			{
				return Pins[PinIndex];
			}
			else
			{
				FoundInputs++;
			}
		}
	}

	return nullptr;
}
