// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "EmitterGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class DSROGUELIKEEDITOR_API UEmitterGraphNode : public UEdGraphNode
{
	GENERATED_BODY()
public:

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FLinearColor GetNodeTitleColor() const override;

public:
	FName MarkerName;
};
