// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DungeonTemplate.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DSROGUELIKE_API UDungeonTemplate : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "My Object Properties")
	FString Name;
	
};
