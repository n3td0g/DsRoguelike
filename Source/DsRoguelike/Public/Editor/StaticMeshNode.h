// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/VisualNode.h"
#include "StaticMeshNode.generated.h"

/**
 * 
 */
UCLASS()
class DSROGUELIKE_API UStaticMeshNode : public UVisualNode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Visual")
	UStaticMesh* StaticMesh;

private:	
	
};
