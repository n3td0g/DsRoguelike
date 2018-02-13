#pragma once

#include "DungeonLeaf.generated.h"

USTRUCT()
struct FDungeonLeaf
{
	GENERATED_BODY()

	UPROPERTY()
	int32 X;

	UPROPERTY()
	int32 Y;

	UPROPERTY()
	int32 Height;

	UPROPERTY()
	int32 Width;

	FDungeonLeaf* LeftLeaf;
	FDungeonLeaf* RightLeaf;

	FDungeonLeaf() {
		LeftLeaf = nullptr;
		RightLeaf = nullptr;
	}

	FDungeonLeaf(int32 LeafX, int32 LeafY, int32 LeafWidth, int32 LeafHeight) :
		X(LeafX), Y(LeafY), Width(LeafWidth), Height(LeafHeight)
	{
		LeftLeaf = nullptr;
		RightLeaf = nullptr;
	}
};