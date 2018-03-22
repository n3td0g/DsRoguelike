// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VisualNode.generated.h"

UENUM(BlueprintType)
enum class EVisualNodeType : uint8
{
	StaticMesh,
	NumEnums
};

/**
 * 
 */
UCLASS()
class DSROGUELIKE_API UVisualNode : public UObject
{
	GENERATED_BODY()
public:
	static FString GetNodeName(EVisualNodeType Type);

	virtual void Process(const FTransform& MarkerTransform, UWorld* World) {}
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transform")
	FTransform Transform;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, class UMarkerNode*> MarkerEmitters;

	UPROPERTY(BlueprintReadOnly)
	EVisualNodeType Type;

private:
	
};
