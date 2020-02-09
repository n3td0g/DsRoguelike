// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "MarkerNode.generated.h"

class UDungeonTemplate;

UCLASS()
class DSROGUELIKE_API UMarkerNode : public UObject
{
	GENERATED_BODY()

public:
	UMarkerNode();
	virtual ~UMarkerNode();

public:
	UPROPERTY(BlueprintReadOnly, Category = "MarkerNode")
	UDungeonTemplate* DungeonTemplate;

	UPROPERTY(BlueprintReadOnly, Category = "Visual")
	TArray<class UVisualNode*> VisualNodes;

	UPROPERTY(BlueprintReadOnly, Category = "MarkerNode")
	FName MarkerName;

protected:
};
