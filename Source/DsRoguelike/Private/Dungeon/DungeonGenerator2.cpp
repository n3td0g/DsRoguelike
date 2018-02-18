// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGenerator2.h"
#include "DungeonLeaf.h"

const FPoint2D FPoint2D::Up = FPoint2D(0, 1);
const FPoint2D FPoint2D::Down = FPoint2D(0, -1);
const FPoint2D FPoint2D::Left = FPoint2D(-1, 0);
const FPoint2D FPoint2D::Right = FPoint2D(1, 0);

#pragma optimize("", off)
// Sets default values
ADungeonGenerator2::ADungeonGenerator2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	DungeonData = nullptr;
}

// Called when the game starts or when spawned
void ADungeonGenerator2::BeginPlay()
{
	Super::BeginPlay();	
}

void ADungeonGenerator2::Initialize()
{
	CleanupDungeon();

	HalfDungeonWidth = FMath::FloorToInt(DungeonWidth * 0.5f);
	HalfDungeonHeight = FMath::FloorToInt(DungeonHeight * 0.5f);

	HalfMinRoomSize = 1 + MinRoomSize / 2;
	HalfMaxRoomSize = (MaxRoomSize + 1) / 2;
	if (HalfMaxRoomSize < HalfMinRoomSize) {
		Swap(HalfMinRoomSize, HalfMaxRoomSize);
	}

	DungeonWidth = HalfDungeonWidth * 2;
	DungeonHeight = HalfDungeonHeight * 2;

	LastRow = DungeonWidth - 1;
	LastCol = DungeonHeight - 1;

	DungeonData = new int32*[DungeonWidth];
	for (int32 Row = 0; Row < DungeonWidth; ++Row) {
		DungeonData[Row] = new int32[DungeonHeight];
		for (int32 Col = 0; Col < DungeonHeight; ++Col) {
			DungeonData[Row][Col] = DUNGEON_NOTHING;
		}
	}

	for (int32 Row = 0; Row < DungeonWidth; ++Row) {
		DungeonData[Row][0] = DUNGEON_BLOCKED;
		DungeonData[Row][LastCol] = DUNGEON_BLOCKED;
	}

	for (int32 Col = 0; Col < DungeonHeight; ++Col) {
		DungeonData[0][Col] = DUNGEON_BLOCKED;
		DungeonData[LastRow][Col] = DUNGEON_BLOCKED;
	}
}

void ADungeonGenerator2::GenerateDungeon()
{
	Initialize();
	RootLeaf = new FDungeonLeaf(0, 0, HalfDungeonWidth - 1, HalfDungeonHeight - 1);

	//Rooms
	ProcessLeaf(RootLeaf, Iterations);
	CreateRoom(RootLeaf);
	CleanupLeaf(RootLeaf);

	//Doors
	OpenRooms();

	//Print
	PrintDungeon();
}

void ADungeonGenerator2::CleanupDungeon()
{
	if (DungeonData) {
		for (int32 Row = 0; Row < DungeonWidth; ++Row) {
			delete[] DungeonData[Row];
		}
		delete[] DungeonData;
	}
	CleanupLeaf(RootLeaf);
	RootLeaf = nullptr;
	Rooms.Empty();
}

void ADungeonGenerator2::CleanupLeaf(FDungeonLeaf* Leaf)
{
	if (!Leaf) {
		return;
	}
	CleanupLeaf(Leaf->LeftLeaf);
	CleanupLeaf(Leaf->RightLeaf);
	delete Leaf;
}

void ADungeonGenerator2::ProcessLeaf(FDungeonLeaf* Leaf, int32 Iteration)
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
	float Ratio = (Leaf->Height * 1.0f) / Leaf->Width;
	if (Ratio < MinLeafRatio) {
		SplitHorizontal = true;
	}
	else {
		if ((1.0f / Ratio) < MinLeafRatio) {
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

bool ADungeonGenerator2::SplitLeaf(FDungeonLeaf* Leaf, bool SplitHorizontal)
{
	if (!Leaf) {
		return false;
	}

	if (SplitHorizontal) {
		int32 MinWidth = FMath::Max(int32(Leaf->Height * MinLeafRatio), HalfMinRoomSize);
		if (Leaf->Width < MinWidth * 2) {
			UE_LOG(LogTemp, Log, TEXT("Leaf width is too small -> Width: %d, Height: %d"), Leaf->Width, Leaf->Height);
			return false;
		}

		float SplitX = MinWidth + FMath::RandRange(0, Leaf->Width - MinWidth * 2);

		Leaf->LeftLeaf = new FDungeonLeaf(Leaf->X, Leaf->Y, SplitX, Leaf->Height);
		Leaf->RightLeaf = new FDungeonLeaf(Leaf->X + SplitX, Leaf->Y, Leaf->Width - SplitX, Leaf->Height);

		UE_LOG(LogTemp, Log, TEXT("SplitVertical -> Width: %d, Height: %d, X: %d, Y: %d"), Leaf->Width, Leaf->Height, Leaf->X, Leaf->Y);
	}
	else {
		int32 MinHeight = FMath::Max(int32(Leaf->Width * MinLeafRatio), HalfMinRoomSize);
		if (Leaf->Height < MinHeight * 2) {
			UE_LOG(LogTemp, Log, TEXT("Leaf height is too small -> Width: %d, Height: %d"), Leaf->Width, Leaf->Height);
			return false;
		}

		float SplitY = MinHeight + FMath::RandRange(0, Leaf->Height - MinHeight * 2);

		Leaf->LeftLeaf = new FDungeonLeaf(Leaf->X, Leaf->Y, Leaf->Width, SplitY);
		Leaf->RightLeaf = new FDungeonLeaf(Leaf->X, Leaf->Y + SplitY, Leaf->Width, Leaf->Height - SplitY);

		UE_LOG(LogTemp, Log, TEXT("SplitHorizontal -> Width: %d, Height: %d, X: %d, Y: %d"), Leaf->Width, Leaf->Height, Leaf->X, Leaf->Y); 
	}

	return true;
}

void ADungeonGenerator2::CreateRoom(const FDungeonLeaf* Leaf)
{
	if (!Leaf) {
		return;
	}

	if (Leaf->RightLeaf != nullptr || Leaf->LeftLeaf != nullptr) {
		CreateRoom(Leaf->RightLeaf);
		CreateRoom(Leaf->LeftLeaf);
		return;
	}

	int32 MaxRoomWidth = FMath::Min(Leaf->Width, HalfMaxRoomSize);
	int32 MaxRoomHeight = FMath::Min(Leaf->Height, HalfMaxRoomSize);

	int32 Width(0);
	int32 Height(0);	

	if (MaxRoomWidth > MaxRoomHeight) {
		Height = FMath::RandRange(HalfMinRoomSize, MaxRoomHeight);
		MaxRoomWidth = FMath::Min(MaxRoomWidth, (int32)(Height / MinRoomRatio));
		int32 MinRoomWidth = FMath::Max(HalfMinRoomSize, (int32)(Height * MinRoomRatio));
		if (MaxRoomWidth < MinRoomWidth) {
			Swap(MinRoomWidth, MaxRoomWidth);
		}
		Width = FMath::RandRange(MinRoomWidth, MaxRoomWidth);
	}
	else {
		Width = FMath::RandRange(HalfMinRoomSize, MaxRoomWidth);
		MaxRoomHeight = FMath::Min(MaxRoomHeight, (int32)(Width / MinRoomRatio));
		int32 MinRoomHeight = FMath::Max(HalfMinRoomSize, (int32)(Width * MinRoomRatio));
		if (MaxRoomHeight < MinRoomHeight) {
			Swap(MinRoomHeight, MaxRoomHeight);
		}
		Height = FMath::RandRange(MinRoomHeight, MaxRoomHeight);
	}

	int32 Row = Leaf->X + FMath::RandRange(0, Leaf->Width - Width);
	int32 Col = Leaf->Y + FMath::RandRange(0, Leaf->Height - Height);

	int32 RoomX = Row * 2 + 1;
	int32 RoomY = Col * 2 + 1;
	int32 RoomRight = (Row + Width) * 2 - 1;
	int32 RoomTop = (Col + Height) * 2 - 1;

	for (int32 R = RoomX; R <= RoomRight; ++R) {
		for (int32 C = RoomY; C <= RoomTop; ++C) {
			int32& Data = DungeonData[R][C];
			Data |= DUNGEON_ROOM;
		}
	}

	Rooms.Push(FRoomDungeon(RoomX, RoomY, (RoomRight - RoomX) + 1, (RoomTop - RoomY) + 1));
}

void ADungeonGenerator2::OpenRooms()
{
	for (auto& Room : Rooms) {
		OpenRoom(Room);
	}
}

void ADungeonGenerator2::OpenRoom(FRoomDungeon& Room)
{
	TArray<FPoint2D> DoorPlaces;
	int32 RoomRight = Room.Position.X + Room.Size.X - 1;
	int32 RoomTop = Room.Position.Y + Room.Size.Y - 1;
	for (int32 Row = Room.Position.X + 1; Row <= RoomRight; Row += 2) {
		if (CheckSill(Row, Room.Position.Y, FPoint2D::Down)) {
			DoorPlaces.Push(FPoint2D(Row, Room.Position.Y));
		}
		if (CheckSill(Row, RoomTop, FPoint2D::Up)) {
			DoorPlaces.Push(FPoint2D(Row, RoomTop));
		}
	}
	for (int32 Col = Room.Position.Y + 1; Col <= RoomTop; Col += 2) {
		if (CheckSill(Room.Position.X, Col, FPoint2D::Left)) {
			DoorPlaces.Push(FPoint2D(Room.Position.X, Col));
		}
		if (CheckSill(RoomRight, Col, FPoint2D::Right)) {
			DoorPlaces.Push(FPoint2D(RoomRight, Col));
		}
	}

	for (auto& Door : DoorPlaces) {
		DungeonData[Door.X][Door.Y] |= ROOM_DOOR;
	}
}

bool ADungeonGenerator2::CheckSill(int32 Row, int32 Col, const FPoint2D CheckDirection)
{
	if (Row < 3 || Col < 3 || Row >= DungeonWidth - 3 || Col >= DungeonHeight - 3) {
		return false;
	}

	int32 DoorR = Row + CheckDirection.X;
	int32 DoorC = Col + CheckDirection.Y;

	if (DungeonData[DoorR][DoorC] & DOOR_BLOCKED) {
		return false;
	}

	int32 OpenR = DoorR + CheckDirection.X;
	int32 OpenC = DoorC + CheckDirection.Y;

	if (DungeonData[OpenR][OpenC] & DUNGEON_BLOCKED) {
		return false;
	}

	return true;
}

void ADungeonGenerator2::PrintDungeon()
{
	int32 Index = 0;
	for (int32 J = DungeonHeight - 1; J >= 0; --J) {
		FString DungeonRow;
		for (int32 I = 0; I < DungeonWidth; ++I) {
			int32 Data = DungeonData[I][J];
			int8 C = ' ';

			if (Data & DUNGEON_BLOCKED) {
				C = '+';
			}
			else if (Data & ROOM_DOOR) {
				C = 'D';
			}
			else if (Data & DUNGEON_ROOM) {
				C = 'H';
			}
			else if (Data & ROOM_PERIMETER) {
				C = 'X';
			}
			DungeonRow.Append(FString::Printf(TEXT("%c"), C));
			Index++;
		}
		UE_LOG(LogTemp, Log, TEXT("%s"), *DungeonRow);
	}
}

#pragma optimize("", on)