// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "VisualGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class DSROGUELIKEEDITOR_API UVisualGraphNode : public UEdGraphNode
{
	GENERATED_BODY()
public:
	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FLinearColor GetNodeTitleColor() const override;

public:
	UPROPERTY(VisibleAnywhere, Instanced, Category = "Visual Node")
	class UVisualNode* VisualNode;
	
};
