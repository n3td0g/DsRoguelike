// Fill out your copyright notice in the Description page of Project Settings.

#include "GridDungeonBuilder.h"

UGridDungeonBuilder::UGridDungeonBuilder()
{
	RootLeaf = nullptr;
}

void UGridDungeonBuilder::GenerateDungeon(ADungeon* ParentDungeon)
{
	Super::GenerateDungeon(ParentDungeon);
	//Init
	Initialize();

	//Rooms
	ProcessLeaf(RootLeaf, BuilderConfig.Iterations);
	CreateRoom(RootLeaf);
	CleanupLeafs();

	//Doors

	//Corridors

	//Debug print
	PrintDungeon();	
}

void UGridDungeonBuilder::PlaceMarkers()
{
	Super::PlaceMarkers();

	//Markers
	PlaceFloorMarkers();
}

void UGridDungeonBuilder::Initialize()
{
	CleanupLeafs();	

	HalfDungeonWidth = FMath::FloorToInt(BuilderConfig.DungeonWidth * 0.5f);
	HalfDungeonHeight = FMath::FloorToInt(BuilderConfig.DungeonHeight * 0.5f);

	HalfMinRoomSize = 1 + BuilderConfig.MinRoomSize / 2;
	HalfMaxRoomSize = (BuilderConfig.MaxRoomSize + 1) / 2;
	if (HalfMaxRoomSize < HalfMinRoomSize) {
		Swap(HalfMinRoomSize, HalfMaxRoomSize);
	}

	BuilderConfig.DungeonWidth = HalfDungeonWidth * 2;
	BuilderConfig.DungeonHeight = HalfDungeonHeight * 2;

	LastRow = BuilderConfig.DungeonWidth - 1;
	LastCol = BuilderConfig.DungeonHeight - 1;

	RootLeaf = new FDungeonLeaf(FIntPoint(0, 0), FIntPoint(HalfDungeonWidth - 1, HalfDungeonHeight - 1));
}

void UGridDungeonBuilder::CleanupLeafs()
{
	CleanupLeaf(RootLeaf);
	RootLeaf = nullptr;
}

void UGridDungeonBuilder::CleanupLeaf(FDungeonLeaf* Leaf)
{
	if (!Leaf) {
		return;
	}
	CleanupLeaf(Leaf->LeftLeaf);
	CleanupLeaf(Leaf->RightLeaf);
	delete Leaf;
}

void UGridDungeonBuilder::ProcessLeaf(FDungeonLeaf* Leaf, int32 Iteration)
{
	if (!Leaf) {
		return;
	}

	if (Leaf->RightLeaf != nullptr || Leaf->LeftLeaf != nullptr) {
		return;
	}

	if (!Iteration) {
		return;
	}

	bool SplitHorizontal = FMath::RandBool();
	float Ratio = (Leaf->LeafSize.Y * 1.0f) / Leaf->LeafSize.X;
	if (Ratio < BuilderConfig.MinLeafRatio) {
		SplitHorizontal = true;
	}
	else {
		if ((1.0f / Ratio) < BuilderConfig.MinLeafRatio) {
			SplitHorizontal = false;
		}
	}

	if (!SplitLeaf(Leaf, SplitHorizontal)) {
		if (!SplitLeaf(Leaf, !SplitHorizontal)) {
			return;
		}
	}

	ProcessLeaf(Leaf->RightLeaf, Iteration - 1);
	ProcessLeaf(Leaf->LeftLeaf, Iteration - 1);
}

bool UGridDungeonBuilder::SplitLeaf(FDungeonLeaf* Leaf, bool SplitHorizontal)
{
	if (!Leaf) {
		return false;
	}

	if (SplitHorizontal) {
		int32 MinWidth = FMath::Max(int32(Leaf->LeafSize.Y * BuilderConfig.MinLeafRatio), HalfMinRoomSize);
		if (Leaf->LeafSize.X < MinWidth * 2) {
			UE_LOG(LogTemp, Log, TEXT("Leaf width is too small -> Width: %d, Height: %d"), Leaf->LeafSize.X, Leaf->LeafSize.Y);
			return false;
		}

		float SplitX = MinWidth + FMath::RandRange(0, Leaf->LeafSize.X - MinWidth * 2);

		Leaf->LeftLeaf = new FDungeonLeaf(Leaf->LeafPosition, FIntPoint(SplitX, Leaf->LeafSize.Y));
		Leaf->RightLeaf = new FDungeonLeaf(FIntPoint(Leaf->LeafPosition.X + SplitX, Leaf->LeafPosition.Y), FIntPoint(Leaf->LeafSize.X - SplitX, Leaf->LeafSize.Y));
	}
	else {
		int32 MinHeight = FMath::Max(int32(Leaf->LeafSize.X * BuilderConfig.MinLeafRatio), HalfMinRoomSize);
		if (Leaf->LeafSize.Y < MinHeight * 2) {
			UE_LOG(LogTemp, Log, TEXT("Leaf height is too small -> Width: %d, Height: %d"), Leaf->LeafSize.X, Leaf->LeafSize.Y);
			return false;
		}

		float SplitY = MinHeight + FMath::RandRange(0, Leaf->LeafSize.Y - MinHeight * 2);

		Leaf->LeftLeaf = new FDungeonLeaf(Leaf->LeafPosition, FIntPoint(Leaf->LeafSize.X, SplitY));
		Leaf->RightLeaf = new FDungeonLeaf(FIntPoint(Leaf->LeafPosition.X, Leaf->LeafPosition.Y + SplitY), FIntPoint(Leaf->LeafSize.X, Leaf->LeafSize.Y - SplitY));
	}

	return true;
}

void UGridDungeonBuilder::CreateRoom(const FDungeonLeaf* Leaf)
{
	if (!Leaf) {
		return;
	}

	if (Leaf->RightLeaf != nullptr || Leaf->LeftLeaf != nullptr) {
		CreateRoom(Leaf->RightLeaf);
		CreateRoom(Leaf->LeftLeaf);
		return;
	}

	int32 MaxRoomWidth = FMath::Min(Leaf->LeafSize.X, HalfMaxRoomSize);
	int32 MaxRoomHeight = FMath::Min(Leaf->LeafSize.Y, HalfMaxRoomSize);

	int32 Width(0);
	int32 Height(0);

	if (MaxRoomWidth > MaxRoomHeight) {
		Height = FMath::RandRange(HalfMinRoomSize, MaxRoomHeight);
		MaxRoomWidth = FMath::Min(MaxRoomWidth, (int32)(Height / BuilderConfig.MinRoomRatio));
		int32 MinRoomWidth = FMath::Max(HalfMinRoomSize, (int32)(Height * BuilderConfig.MinRoomRatio));
		if (MaxRoomWidth < MinRoomWidth) {
			Swap(MinRoomWidth, MaxRoomWidth);
		}
		Width = FMath::RandRange(MinRoomWidth, MaxRoomWidth);
	}
	else {
		Width = FMath::RandRange(HalfMinRoomSize, MaxRoomWidth);
		MaxRoomHeight = FMath::Min(MaxRoomHeight, (int32)(Width / BuilderConfig.MinRoomRatio));
		int32 MinRoomHeight = FMath::Max(HalfMinRoomSize, (int32)(Width * BuilderConfig.MinRoomRatio));
		if (MaxRoomHeight < MinRoomHeight) {
			Swap(MinRoomHeight, MaxRoomHeight);
		}
		Height = FMath::RandRange(MinRoomHeight, MaxRoomHeight);
	}

	int32 Row = Leaf->LeafPosition.X + FMath::RandRange(0, Leaf->LeafSize.X - Width);
	int32 Col = Leaf->LeafPosition.Y + FMath::RandRange(0, Leaf->LeafSize.Y - Height);

	FDungeonRoom Room;
	Room.RoomPosition = FIntPoint(Row * 2 + 1, Col * 2 + 1);

	int32 RoomRight = (Row + Width) * 2 - 1;
	int32 RoomTop = (Col + Height) * 2 - 1;	

	for (int32 R = Room.RoomPosition.X; R <= RoomRight; ++R) {
		for (int32 C = Room.RoomPosition.Y; C <= RoomTop; ++C) {
			int32& Data = DungeonGrid[R][C];
			Data |= DUNGEON_ROOM;
		}
	}

	Room.RoomSize = FIntPoint((RoomRight - Room.RoomPosition.X) + 1, (RoomTop - Room.RoomPosition.Y) + 1);

	Rooms.Push(Room);
}

void UGridDungeonBuilder::OpenRooms()
{
	for (auto& Room : Rooms) {
		OpenRoom(Room);
	}
}

void UGridDungeonBuilder::OpenRoom(FDungeonRoom& Room)
{
	
}

bool UGridDungeonBuilder::CheckSill(int32 Row, int32 Col, const FIntPoint& CheckDirection)
{
	return false;
}

void UGridDungeonBuilder::CheckRooms()
{

}

bool UGridDungeonBuilder::CheckRoom(FDungeonRoom& Room)
{
	return false;
}

void UGridDungeonBuilder::CreateCorridors()
{

}

void UGridDungeonBuilder::CreateCorridor(int32 I, int32 J, int32 LastDirection)
{

}

bool UGridDungeonBuilder::CheckCorridor(int32 CorridorX, int32 CorridorY, const FIntPoint& Direction)
{
	return false;
}

void UGridDungeonBuilder::RemoveDeadEnds()
{

}

void UGridDungeonBuilder::CollapseCorridor(int32 I, int32 J)
{

}

void UGridDungeonBuilder::PrintDungeon()
{
	int32 Index = 0;
	for (int32 J = BuilderConfig.DungeonHeight - 1; J >= 0; --J) {
		FString DungeonRow;
		for (int32 I = 0; I < BuilderConfig.DungeonWidth; ++I) {
			int32 Data = DungeonGrid[I][J];
			int8 C = ' ';

			if (Data & DUNGEON_CORRIDOR) {
				C = '+';
			}
			else if (Data & ROOM_DOOR) {
				C = 'D';
			}
			else if (Data & ROOM_ENTRANCE) {
				C = 'E';
			}
			else if (Data & DUNGEON_ROOM) {
				C = '-';
			}
			DungeonRow.Append(FString::Printf(TEXT("%c"), C));
			Index++;
		}
		UE_LOG(LogTemp, Log, TEXT("%s"), *DungeonRow);
	}
}

void UGridDungeonBuilder::PlaceFloorMarkers()
{
	FDungeonMarker FloorMarker;
	FDungeonMarker WallMarker;

	FloorMarker.Transform = FTransform::Identity;
	FloorMarker.Transform = FTransform::Identity;

	auto& FloorMarkers = Markers.Add(TEXT("Floor"));
	auto& WallMarkers = Markers.Add(TEXT("Wall"));

	for (int32 J = 0; J < BuilderConfig.DungeonHeight; ++J) {
		for (int32 I = 0; I < BuilderConfig.DungeonWidth; ++I) {
			int32& Data = DungeonGrid[I][J];
			if (Data & DUNGEON_ROOM) {
				FloorMarker.Transform.SetLocation(FVector((I + 0.5f) * BuilderConfig.CellSize, (J + 0.5f) * BuilderConfig.CellSize, 0.0f));
				FloorMarkers.Push(FloorMarker);

				
			}
		}
	}
}