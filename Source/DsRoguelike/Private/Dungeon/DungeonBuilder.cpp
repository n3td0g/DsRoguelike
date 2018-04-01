// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonBuilder.h"
#include "Dungeon.h"

UDungeonBuilder::UDungeonBuilder()
{
	DungeonGrid = nullptr;
}

void UDungeonBuilder::GenerateDungeon(ADungeon* ParentDungeon)
{
	if (!ParentDungeon) {
		return;
	}

	EmptyDungeon();

	Dungeon = ParentDungeon;
	BuilderConfig = Dungeon->BuilderConfig;	

	DungeonGrid = new int32*[BuilderConfig.DungeonWidth];
	for (int32 I = 0; I < BuilderConfig.DungeonWidth; ++I) {
		DungeonGrid[I] = new int32[BuilderConfig.DungeonHeight];
		for (int32 J = 0; J < BuilderConfig.DungeonHeight; ++J) {
			DungeonGrid[I][J] = DUNGEON_NOTHING;
		}
	}
}

void UDungeonBuilder::BeginDestroy()
{
	Super::BeginDestroy();
	CleanDungeon();	
}

void UDungeonBuilder::PlaceMarker(const FName& MarkerName, const FTransform& Transform)
{

}

EDungeonCellType UDungeonBuilder::GetCellType(FIntPoint CellPosition)
{
	if (DungeonGrid) {
		if (IsPointValid(CellPosition)) {
			int32 Cell = DungeonGrid[CellPosition.X][CellPosition.Y];
			if (Cell & DUNGEON_ROOM) {
				return EDungeonCellType::CT_Room;
			}
			if (Cell & DUNGEON_CORRIDOR) {
				return EDungeonCellType::CT_Corridor;
			}
		}
	}
	return EDungeonCellType::CT_None;
}

bool UDungeonBuilder::IsPointValid(const FIntPoint& Point) const
{
	return IsCoordsValid(Point.X, Point.Y);
}

bool UDungeonBuilder::IsCoordsValid(int32 X, int32 Y) const
{
	return X >= 0 && Y >= 0 && X < BuilderConfig.DungeonWidth && Y < BuilderConfig.DungeonHeight;
}

void UDungeonBuilder::EmptyDungeon()
{
	CleanDungeon();
	Rooms.Empty();
	Corridors.Empty();
}

void UDungeonBuilder::CleanDungeon()
{
	if (DungeonGrid) {
		for (int32 I = 0; I < BuilderConfig.DungeonWidth; ++I) {
			delete[] DungeonGrid[I];
		}
		delete[] DungeonGrid;
		DungeonGrid = nullptr;
	}
}
