// Fill out your copyright notice in the Description page of Project Settings.

#include "GridDungeonBuilder.h"

const FIntPoint UGridDungeonBuilder::DirectionUp = FIntPoint(0, 1);
const FIntPoint UGridDungeonBuilder::DirectionDown = FIntPoint(0, -1);
const FIntPoint UGridDungeonBuilder::DirectionLeft = FIntPoint(-1, 0);
const FIntPoint UGridDungeonBuilder::DirectionRight = FIntPoint(1, 0);
const int32 UGridDungeonBuilder::NumDirections = 4;
const FIntPoint UGridDungeonBuilder::AllDirections[4] = { 
															UGridDungeonBuilder::DirectionUp,
															UGridDungeonBuilder::DirectionRight,
															UGridDungeonBuilder::DirectionDown,
															UGridDungeonBuilder::DirectionLeft };

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
	OpenRooms();

	//Corridors
	CreateCorridors();
	if (BuilderConfig.bRemoveDeadEnds) {
		RemoveDeadEnds();
	}

	CheckRooms();

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
	TArray<FIntPoint> DoorPlaces;
	int32 RoomRight = Room.RoomPosition.X + Room.RoomSize.X - 1;
	int32 RoomTop = Room.RoomPosition.Y + Room.RoomSize.Y - 1;
	for (int32 Row = Room.RoomPosition.X + 1; Row <= RoomRight; Row += 2) {
		if (CheckSill(Row, Room.RoomPosition.Y, UGridDungeonBuilder::DirectionDown)) {
			DoorPlaces.Push(FIntPoint(Row, Room.RoomPosition.Y));
		}
		if (CheckSill(Row, RoomTop, UGridDungeonBuilder::DirectionUp)) {
			DoorPlaces.Push(FIntPoint(Row, RoomTop));
		}
	}
	for (int32 Col = Room.RoomPosition.Y + 1; Col <= RoomTop; Col += 2) {
		if (CheckSill(Room.RoomPosition.X, Col, UGridDungeonBuilder::DirectionLeft)) {
			DoorPlaces.Push(FIntPoint(Room.RoomPosition.X, Col));
		}
		if (CheckSill(RoomRight, Col, UGridDungeonBuilder::DirectionRight)) {
			DoorPlaces.Push(FIntPoint(RoomRight, Col));
		}
	}

	int32 NumDoors = FMath::RandRange(BuilderConfig.MinDoorsCount, BuilderConfig.MaxDoorsCount);
	for (int32 I = 0; I < NumDoors; ++I) {
		if (DoorPlaces.Num() == 0) {
			break;
		}
		int32 DoorIndex = FMath::RandRange(0, DoorPlaces.Num() - 1);
		const FIntPoint& DoorLocation = DoorPlaces[DoorIndex];

		FDoorInfo DoorInfo;
		DoorInfo.Location = DoorLocation;

		FIntPoint EntranceLocation;
		int32 DirectionIndex = 0;
		for (auto& Direction : UGridDungeonBuilder::AllDirections) {
			EntranceLocation.X = DoorLocation.X + Direction.X;
			EntranceLocation.Y = DoorLocation.Y + Direction.Y;
			if (!IsPointValid(EntranceLocation)) {
				UE_LOG(LogTemp, Warning, TEXT("Ivalid location: %d; %d"), EntranceLocation.X, EntranceLocation.Y);
				continue;
			}
			int32& Data = DungeonGrid[EntranceLocation.X][EntranceLocation.Y];
			if (Data & DUNGEON_ROOM) {
				++DirectionIndex;
				continue;
			}
			DoorInfo.DirectionIndex = DirectionIndex;
			Data |= ROOM_ENTRANCE;
		}

		Room.Doors.Push(DoorInfo);
		DoorPlaces.RemoveAt(DoorIndex);
	}
}

bool UGridDungeonBuilder::CheckSill(int32 Row, int32 Col, const FIntPoint& CheckDirection)
{
	int32 DoorR = Row + CheckDirection.X;
	int32 DoorC = Col + CheckDirection.Y;

	if (DungeonGrid[DoorR][DoorC] & BLOCK_DOOR) {
		return false;
	}

	int32 OpenR = DoorR + CheckDirection.X;
	int32 OpenC = DoorC + CheckDirection.Y;

	if (OpenR == 0 || OpenC == 0) {
		return false;
	}
	if (OpenR == LastRow || OpenC == LastCol) {
		return false;
	}

	return true;
}

void UGridDungeonBuilder::CheckRooms()
{
	for (int32 I = Rooms.Num() - 1; I >= 0; --I) {
		FDungeonRoom& Room = Rooms[I];
		if (!CheckRoom(Room)) {
			for (int32 Row = Room.RoomPosition.X; Row < Room.RoomPosition.X + Room.RoomSize.X; ++Row) {
				for (int32 Col = Room.RoomPosition.Y; Col < Room.RoomPosition.Y + Room.RoomSize.Y; ++Col) {
					DungeonGrid[Row][Col] = DUNGEON_NOTHING;
				}
			}
			Rooms.RemoveAt(I);
		}
	}

}

bool UGridDungeonBuilder::CheckRoom(FDungeonRoom& Room)
{
	for (int32 I = Room.Doors.Num() - 1; I >= 0; --I) {
		const FDoorInfo& Door = Room.Doors[I];
		const FIntPoint& Direction = UGridDungeonBuilder::AllDirections[Door.DirectionIndex];
		int32& Data = DungeonGrid[Door.Location.X + Direction.X][Door.Location.Y + Direction.Y];
		if (Data & ROOM_ENTRANCE) {
			int32 Row = Door.Location.X + Direction.X * 2;
			int32 Col = Door.Location.Y + Direction.Y * 2;
			int32& NextData = DungeonGrid[Row][Col];
			if (IsCoordsValid(Row, Col)) {
				if (NextData & (DUNGEON_ROOM | DUNGEON_CORRIDOR | ROOM_ENTRANCE)) {
					Data |= DUNGEON_CORRIDOR;
					continue;
				}
				continue;
			}			
		}

		Room.Doors.RemoveAt(I);
	}
	return Room.Doors.Num() > 0;
}

void UGridDungeonBuilder::CreateCorridors()
{
	for (auto& Room : Rooms) {
		for (auto& Door : Room.Doors) {
			const FIntPoint& Direction = UGridDungeonBuilder::AllDirections[Door.DirectionIndex];
			int32 CorridorX = Door.Location.X + Direction.X;
			int32 CorridorY = Door.Location.Y + Direction.Y;
			CreateCorridor(CorridorX, CorridorY, Door.DirectionIndex);
		}
	}
}

void UGridDungeonBuilder::CreateCorridor(int32 I, int32 J, int32 LastDirection)
{
	int32 CorridorX = I;
	int32 CorridorY = J;
	if (!IsCoordsValid(CorridorX, CorridorY)) {
		return;
	}

	int32& StartCell = DungeonGrid[CorridorX][CorridorY];
	if (StartCell & DUNGEON_CORRIDOR) {
		return;
	}

	int32 DirectionIndex = FMath::FRand() < BuilderConfig.ChangeDirectionChance ? UGridDungeonBuilder::GetRandomDirectionIndex() : LastDirection;

	for (int32 Row = 0; Row < UGridDungeonBuilder::NumDirections; ++Row, ++DirectionIndex) {
		DirectionIndex %= UGridDungeonBuilder::NumDirections;
		const FIntPoint& Direction = UGridDungeonBuilder::AllDirections[DirectionIndex];
		if (!CheckCorridor(CorridorX, CorridorY, Direction)) {
			continue;
		}
		for (int32 N = 0; N < 2; ++N) {
			int32& Data = DungeonGrid[CorridorX][CorridorY];
			Data |= DUNGEON_CORRIDOR;
			CorridorX += Direction.X;
			CorridorY += Direction.Y;
		}
		CreateCorridor(CorridorX, CorridorY, DirectionIndex);
	}

	if (!(StartCell & DUNGEON_CORRIDOR)) {
		StartCell &= ~ROOM_ENTRANCE;
	}
}

bool UGridDungeonBuilder::CheckCorridor(int32 CorridorX, int32 CorridorY, const FIntPoint& Direction)
{
	for (int32 I = 1; I < 3; ++I) {
		int32 X = CorridorX + Direction.X * I;
		int32 Y = CorridorY + Direction.Y * I;
		if (!IsCoordsValid(X, Y)) {
			return false;
		}
		int32& Data = DungeonGrid[X][Y];
		if (Data & BLOCK_CORRIDOR) {
			return false;
		}
	}
	return true;
}

void UGridDungeonBuilder::RemoveDeadEnds()
{
	for (int32 I = 0; I < BuilderConfig.DungeonWidth; ++I) {
		for (int32 J = 0; J < BuilderConfig.DungeonHeight; ++J) {
			CollapseCorridor(I, J);			
		}
	}
}

void UGridDungeonBuilder::CollapseCorridor(int32 I, int32 J)
{
	int32& CurrentCell = DungeonGrid[I][J];
	if (!(CurrentCell & DUNGEON_CORRIDOR)) {
		return;
	}

	int32 NumWays = 0;
	FIntPoint Way(0, 0);
	if (CurrentCell & ROOM_ENTRANCE) {
		++NumWays;
	}

	for (auto& Direction : UGridDungeonBuilder::AllDirections) {
		int32 Row = I + Direction.X;
		int32 Col = J + Direction.Y;
		if (Row >= 0 && Row < BuilderConfig.DungeonWidth && Col >= 0 && Col < BuilderConfig.DungeonHeight) {
			int32 &Cell = DungeonGrid[Row][Col];
			if (Cell & DUNGEON_CORRIDOR) {
				++NumWays;
				Way.X = Row;
				Way.Y = Col;
			}
		}
	}

	if (NumWays < 2) {
		int32 &Cell = DungeonGrid[I][J];
		Cell &= ~(DUNGEON_CORRIDOR | ROOM_ENTRANCE);
		if (NumWays == 1) {
			CollapseCorridor(Way.X, Way.Y);
		}
	}
}

void UGridDungeonBuilder::PrintDungeon()
{
	int32 Index = 0;
	for (int32 J = BuilderConfig.DungeonHeight - 1; J >= 0; --J) {
		FString DungeonRow;
		for (int32 I = 0; I < BuilderConfig.DungeonWidth; ++I) {
			int32 Data = DungeonGrid[I][J];
			int8 C = ' ';

			if (Data & ROOM_ENTRANCE) {
				C = 'E';
			}
			else if (Data & DUNGEON_CORRIDOR) {
				C = '+';
			}
			else if (Data & ROOM_DOOR) {
				C = 'D';
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
	FDungeonMarker DoorMarker;
	FDungeonMarker SeparatorMarker;

	FloorMarker.Transform = FTransform::Identity;
	WallMarker.Transform = FTransform::Identity;
	DoorMarker.Transform = FTransform::Identity;
	SeparatorMarker.Transform = FTransform::Identity;

	auto& FloorMarkers = Markers.Add(TEXT("Floor"));
	auto& WallMarkers = Markers.Add(TEXT("Wall"));
	auto& EntranceMarkers = Markers.Add(TEXT("Entrance"));
	auto& DoorMarkers = Markers.Add(TEXT("Door"));
	auto& SeparatorMarkers = Markers.Add(TEXT("Separator"));

	float HalfCellSize = BuilderConfig.CellSize * 0.5f;
	float SizeWithWall = HalfCellSize - BuilderConfig.WallThickness * 0.5f;

	const FVector SeparatorOffsets[4] = {
		FVector(HalfCellSize, HalfCellSize, 0.0f),
		FVector(HalfCellSize, -HalfCellSize, 0.0f),
		FVector(-HalfCellSize, -HalfCellSize, 0.0f),
		FVector(-HalfCellSize, HalfCellSize, 0.0f) };

	for (int32 J = 0; J < BuilderConfig.DungeonHeight; ++J) {
		for (int32 I = 0; I < BuilderConfig.DungeonWidth; ++I) {
			const int32& Data = DungeonGrid[I][J];
			int32 CellType = Data & DUNGEON_FLOOR;
			if (CellType) {
				const FVector& CellCenter = FVector((I + 0.5f) * BuilderConfig.CellSize, (J + 0.5f) * BuilderConfig.CellSize, 0.0f);
				FloorMarker.Transform.SetLocation(CellCenter);
				FloorMarkers.Push(FloorMarker);
				FRotator Rotation = FRotator::ZeroRotator;
				int32 SeparatorIndex = 0;

				for (const auto& Direction : UGridDungeonBuilder::AllDirections) {
					int32 Row = I + Direction.X;
					int32 Col = J + Direction.Y;

					WallMarker.Transform.SetLocation(CellCenter + FVector(SizeWithWall * Direction.X, SizeWithWall * Direction.Y, 0.0f));
					WallMarker.Transform.SetRotation(Rotation.Quaternion());

					SeparatorMarker.Transform.SetLocation(CellCenter + SeparatorOffsets[SeparatorIndex]);

					if (IsCoordsValid(Row, Col)) {
						const int32& Cell = DungeonGrid[Row][Col];
						if (!(Cell & CellType)) {							
							bool IsRoomEntrance = (Cell & ROOM_ENTRANCE) == ROOM_ENTRANCE;
							bool IsCorridorEntrance = (Data & ROOM_ENTRANCE) && (Cell & DUNGEON_ROOM);
							if (IsRoomEntrance || IsCorridorEntrance) {
								EntranceMarkers.Push(WallMarker);
								if (Data & DUNGEON_ROOM) {
									DoorMarker.Transform.SetLocation(CellCenter + FVector(HalfCellSize * Direction.X, HalfCellSize * Direction.Y, 0.0f));
									DoorMarker.Transform.SetRotation(Rotation.Quaternion());
									DoorMarkers.Push(DoorMarker);
								}
							}
							else {
								WallMarkers.Push(WallMarker);
								SeparatorMarkers.Push(SeparatorMarker);
							}
						}
					}
					else {
						SeparatorMarkers.Push(SeparatorMarker);
						WallMarkers.Push(WallMarker);
					}
					
					Rotation.Yaw -= 90.0f;
					++SeparatorIndex;
				}
			}
		}
	}
}

FORCEINLINE const int32 UGridDungeonBuilder::GetRandomDirectionIndex()
{
	return FMath::RandRange(0, UGridDungeonBuilder::NumDirections - 1);
}
