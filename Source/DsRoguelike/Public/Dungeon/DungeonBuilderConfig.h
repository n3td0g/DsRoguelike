#pragma once
#include "DungeonBuilderConfig.generated.h"

USTRUCT(BlueprintType)
struct FDungeonBuilderConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options")
	float CellSize = 400.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options", meta = (ClampMin = 10))
	int32 DungeonWidth = 50;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options", meta = (ClampMin = 10))
	int32 DungeonHeight = 50;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options", meta = (ClampMin = 1))
	int32 Iterations = 4;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options")
	float MinLeafRatio = 0.5f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options|Rooms", meta = (ClampMin = 1))
	int32 MinRoomSize = 4;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options|Rooms", meta = (ClampMin = 1))
	int32 MaxRoomSize = 8;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options|Rooms")
	float MinRoomRatio = 0.6f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options|Rooms", meta = (ClampMin = 1))
	int32 MinDoorsCount = 2;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options|Rooms", meta = (ClampMin = 1))
	int32 MaxDoorsCount = 4;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options|Corridor")
	float ChangeDirectionChance = 0.1f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options|Corridor")
	bool bRemoveDeadEnds = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options|Walls")
	float WallThickness = 50.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options|Widnows")
	float ChangeToPlaceWindow = 0.1f;
};