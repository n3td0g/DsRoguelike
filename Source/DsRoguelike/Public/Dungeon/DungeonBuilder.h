// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonBuilderConfig.h"
#include "DungeonCorridor.h"
#include "DungeonMarker.h"
#include "DungeonRoom.h"
#include "UObject/NoExportTypes.h"

#include "DungeonBuilder.generated.h"

#define DUNGEON_NOTHING 0x00000000
#define DUNGEON_ROOM 0x00000001
#define DUNGEON_CORRIDOR 0x00000002

UENUM(BlueprintType)
enum class EDungeonCellType : uint8
{
	CT_None,
	CT_Corridor,
	CT_Room
};

class ADungeon;
/**
 *
 */
UCLASS(Blueprintable)
class DSROGUELIKE_API UDungeonBuilder : public UObject
{
	GENERATED_BODY()
public:
	UDungeonBuilder();

public:
	virtual void GenerateDungeon(ADungeon* ParentDungeon);
	// virtual void PlaceMarkers();

	virtual void BeginDestroy();

	UFUNCTION(BlueprintCallable)
	void PlaceMarker(const FName& MarkerName, const FTransform& Transform);

	UFUNCTION(BlueprintPure)
	EDungeonCellType GetCellType(FIntPoint CellPosition);

	UFUNCTION(BlueprintPure)
	bool IsPointValid(const FIntPoint& Point) const;
	UFUNCTION(BlueprintPure)
	bool IsCoordsValid(int32 X, int32 Y) const;

public:
	TMap<FName, TArray<FDungeonMarker>> Markers;

protected:
	virtual void EmptyDungeon();
	void CleanDungeon();

protected:
	UPROPERTY(BlueprintReadOnly)
	ADungeon* Dungeon;

	UPROPERTY(BlueprintReadOnly)
	TArray<FDungeonRoom> Rooms;

	UPROPERTY(BlueprintReadOnly)
	TArray<FDungeonCorridor> Corridors;

	UPROPERTY(BlueprintReadOnly)
	FDungeonBuilderConfig BuilderConfig;

	int32** DungeonGrid;
};
