#include "DungeonDirections.h"

const FIntPoint FDungeonDirections::DirectionUp = FIntPoint(0, 1);
const FIntPoint FDungeonDirections::DirectionDown = FIntPoint(0, -1);
const FIntPoint FDungeonDirections::DirectionLeft = FIntPoint(-1, 0);
const FIntPoint FDungeonDirections::DirectionRight = FIntPoint(1, 0);
const int32 FDungeonDirections::NumDirections = 4;
const FIntPoint FDungeonDirections::AllDirections[4] = {
	FDungeonDirections::DirectionUp,
	FDungeonDirections::DirectionRight,
	FDungeonDirections::DirectionDown,
	FDungeonDirections::DirectionLeft };
