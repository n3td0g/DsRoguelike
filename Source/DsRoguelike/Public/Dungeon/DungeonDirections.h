#pragma once
#include "DungeonDirections.generated.h"

USTRUCT(BlueprintType)
struct FDungeonDirections
{
	GENERATED_BODY()

	static const FIntPoint DirectionUp;
	static const FIntPoint DirectionRight;
	static const FIntPoint DirectionDown;
	static const FIntPoint DirectionLeft;
	static const int32 NumDirections;
	static const FIntPoint AllDirections[4];

	static FORCEINLINE int32 GetRandomDirectionIndex()
	{
		return FMath::RandRange(0, FDungeonDirections::NumDirections - 1);
	}
};