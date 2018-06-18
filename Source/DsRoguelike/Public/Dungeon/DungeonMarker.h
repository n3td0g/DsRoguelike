#pragma once

#include "DungeonMarker.generated.h"

USTRUCT(BlueprintType)
struct FDungeonMarker
{
	GENERATED_BODY()
	
	FDungeonMarker() {}

	FDungeonMarker(const FTransform& MarkerTransform) : Transform(MarkerTransform) {}
	
	UPROPERTY(BlueprintReadOnly)
	FTransform Transform;
};