// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dungeon/Emitters/MarkerEmitter.h"
#include "GridDungeonMarkerEmitter.generated.h"

/**
 * 
 */
UCLASS()
class DSROGUELIKE_API UGridDungeonMarkerEmitter : public UMarkerEmitter
{
	GENERATED_BODY()
	
protected:
	virtual void EmitMarkers(class UDungeonBuilder* DungeonBuilder, class ADungeon* Dungeon) override;

	void UGridDungeonMarkerEmitter::PlaceSeparatorMarker(struct FDungeonMarker& Marker, int32 I, int32 J, int32 DirectionIndex, const float& HalfWallThickness);
protected:
	class UGridDungeonBuilder* DungeonBuilder;
};
