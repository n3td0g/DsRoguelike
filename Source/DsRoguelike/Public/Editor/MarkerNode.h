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

	UPROPERTY(VisibleDefaultsOnly, Category = "MarkerNode")
	UDungeonTemplate* DungeonTemplate;

	UPROPERTY(VisibleAnywhere, Category = "MarkerNode")
	FName MarkerName;

	UPROPERTY(VisibleDefaultsOnly, Category = "Visual")
	TArray<class UVisualNode*> VisualNodes;

protected:
	
	
};
