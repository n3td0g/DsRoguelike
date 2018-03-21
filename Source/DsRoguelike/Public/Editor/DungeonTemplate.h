// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DungeonTemplate.generated.h"

class UMarkerNode;

UCLASS(Blueprintable)
class DSROGUELIKE_API UDungeonTemplate : public UObject
{
	GENERATED_BODY()
public:
	UDungeonTemplate();
	virtual ~UDungeonTemplate();

public:
	UMarkerNode* CreateMarkerNode();
	bool IsNameOccupied(const FName& MarkerName);
	
	
public:
	UPROPERTY(EditAnywhere, Category = "Properties")
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, UMarkerNode*> MarkerNodes;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UEdGraph* EdGraph;
#endif

private:
	FName GetFreeMarkerName();
	
};
