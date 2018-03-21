#pragma once
#include "DungeonRoom.generated.h"

USTRUCT(BlueprintType)
struct FDoorInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FIntPoint Location;

	UPROPERTY(BlueprintReadOnly)
	int32 DirectionIndex;
};

USTRUCT(BlueprintType)
struct FDungeonRoom
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Room")
	FIntPoint RoomPosition;

	UPROPERTY(BlueprintReadWrite, Category = "Room")
	FIntPoint RoomSize;

	UPROPERTY(BlueprintReadWrite, Category = "Room")
	FBox Bounds;

	UPROPERTY(BlueprintReadWrite, Category = "Doors")
	TArray<FDoorInfo> Doors;
};