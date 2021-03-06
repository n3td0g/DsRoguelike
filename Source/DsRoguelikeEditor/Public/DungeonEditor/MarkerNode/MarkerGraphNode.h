// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "MarkerGraphNode.generated.h"

/**
 *
 */
UCLASS()
class DSROGUELIKEEDITOR_API UMarkerGraphNode : public UEdGraphNode
{
	GENERATED_BODY()
public:
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FLinearColor GetNodeTitleColor() const override;

	virtual void PinConnectionListChanged(UEdGraphPin * Pin) override;
	virtual bool CanUserDeleteNode() const override;
	virtual void DestroyNode() override;
public:
	UPROPERTY(VisibleAnywhere, Instanced, Category = "Marker")
	class UMarkerNode* MarkerNode;

};
