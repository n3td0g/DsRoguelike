// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "MarkerEmitter.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class DSROGUELIKE_API UMarkerEmitter : public UObject
{
	GENERATED_BODY()

public:
	virtual void EmitMarkers(class UDungeonBuilder* DungeonBuilder, class ADungeon* Dungeon);

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "EmitMarkers"))
	void ReceiveEmitMarkers(class UDungeonBuilder* DungeonBuilder, class ADungeon* Dungeon);
};
