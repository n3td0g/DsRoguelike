#pragma once

#include "DungeonMarker.generated.h"

USTRUCT(BlueprintType)
struct FDungeonMarker
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FTransform Transform;
};