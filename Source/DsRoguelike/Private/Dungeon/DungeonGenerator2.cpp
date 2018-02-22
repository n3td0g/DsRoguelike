// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGenerator2.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/PointLight.h"
#include "DungeonLeaf.h"

const FPoint2D FPoint2D::Up = FPoint2D(0, 1);
const FPoint2D FPoint2D::Down = FPoint2D(0, -1);
const FPoint2D FPoint2D::Left = FPoint2D(-1, 0);
const FPoint2D FPoint2D::Right = FPoint2D(1, 0);
const int32 FPoint2D::NumDirections = 4;
const FPoint2D FPoint2D::AllDirections[4] = { FPoint2D::Up, FPoint2D::Right, FPoint2D::Down, FPoint2D::Left };

int32 FPoint2D::GetRandomDirectionIndex()
{
	return FMath::RandRange(0, FPoint2D::NumDirections - 1);
}

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

	//Corridors
	CreateCorridors();	
	if (bRemoveDeadEnds) {
		RemoveDeadEnds();
	}	
	CheckRooms();

	//Print
	PrintDungeon();

	//Meshes
	GenerateMeshes();

	//Events
	DungeonGenerated();
}

bool ADungeonGenerator2::IsLocationValid(const FPoint2D& Location) const
{
	return IsPointValid(Location.X, Location.Y);
}

bool ADungeonGenerator2::IsPointValid(int32 Row, int32 Col) const
{
	return Row >= 0 && Col >= 0 && Row < DungeonWidth && Col < DungeonHeight;
}

FVector ADungeonGenerator2::GetRoomCenter(int32 RoomIndex)
{
	if (Rooms.IsValidIndex(RoomIndex)) {
		const auto& Room = Rooms[RoomIndex];

		int32 HalfWidth = Room.Size.X / 2;
		int32 HalfHeight = Room.Size.Y / 2;

		int32 CenterX = Room.Position.X + HalfWidth;
		int32 CenterY = Room.Position.Y + HalfHeight;

		return FVector(CenterX * CellSize, CenterY * CellSize, 200.0f);
	}
	return FVector::ZeroVector;
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

	for (int32 R = RoomX - 1; R <= RoomRight + 1; ++R) {
		int32& DataTop = DungeonData[R][RoomTop + 1];
		DataTop |= ROOM_PERIMETER;
		int32& DataBottom = DungeonData[R][RoomY - 1];
		DataBottom |= ROOM_PERIMETER;
	}

	for (int32 C = RoomY - 1; C <= RoomTop + 1; ++C) {
		int32& DataLeft = DungeonData[RoomX - 1][C];
		DataLeft |= ROOM_PERIMETER;
		int32& DataRight = DungeonData[RoomRight + 1][C];
		DataRight |= ROOM_PERIMETER;
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

	int32 NumDoors = FMath::RandRange(MinDoorsCount, MaxDoorsCount);
	for (int32 I = 0; I < NumDoors; ++I) {
		if (DoorPlaces.Num() == 0) {
			break;
		}
		int32 DoorIndex = FMath::RandRange(0, DoorPlaces.Num() - 1);
		const FPoint2D& DoorLocation = DoorPlaces[DoorIndex];		
		DungeonData[DoorLocation.X][DoorLocation.Y] |= ROOM_DOOR;	

		FRoomDoorInfo DoorInfo;
		DoorInfo.Location = DoorLocation;

		FPoint2D EntranceLocation;
		int32 DirectionIndex = 0;
		for (auto& Direction : FPoint2D::AllDirections) {
			EntranceLocation.X = DoorLocation.X + Direction.X;
			EntranceLocation.Y = DoorLocation.Y + Direction.Y;
			if (!IsLocationValid(EntranceLocation)) {
				UE_LOG(LogTemp, Warning, TEXT("Ivalid location: %d; %d"), EntranceLocation.X, EntranceLocation.Y);
				continue;
			}
			int32& Data = DungeonData[EntranceLocation.X][EntranceLocation.Y];
			if (Data & ROOM_PERIMETER) {
				DoorInfo.Direction = Direction;
				DoorInfo.DirectionIndex = DirectionIndex;
				Data |= ROOM_ENTRANCE;
				Data &= ~ROOM_PERIMETER;
				break;
			}
			++DirectionIndex;
		}

		Room.Doors.Push(DoorInfo);
		DoorPlaces.RemoveAt(DoorIndex);
	}
}

bool ADungeonGenerator2::CheckSill(int32 Row, int32 Col, const FPoint2D CheckDirection)
{
	int32 DoorR = Row + CheckDirection.X;
	int32 DoorC = Col + CheckDirection.Y;

	if (DungeonData[DoorR][DoorC] & BLOCK_DOOR) {
		return false;
	}

	int32 OpenR = DoorR + CheckDirection.X;
	int32 OpenC = DoorC + CheckDirection.Y;

	if (DungeonData[OpenR][OpenC] & DUNGEON_BLOCKED) {
		return false;
	}

	return true;
}

void ADungeonGenerator2::CheckRooms()
{
	for (int32 I = Rooms.Num() - 1; I >= 0; --I) {
		FRoomDungeon& Room = Rooms[I];
		if (!CheckRoom(Room)) {
			for (int32 Row = Room.Position.X; Row < Room.Position.X + Room.Size.X; ++Row) {
				for (int32 Col = Room.Position.Y; Col < Room.Position.Y + Room.Size.Y; ++Col) {
					DungeonData[Row][Col] = 0x00000000;
				}
			}
			Rooms.RemoveAt(I);
		}
	}
}

bool ADungeonGenerator2::CheckRoom(FRoomDungeon& Room)
{
	for (int32 I = Room.Doors.Num() - 1; I >= 0; --I) {
		const FRoomDoorInfo& Door = Room.Doors[I];
		int32& Data = DungeonData[Door.Location.X + Door.Direction.X][Door.Location.Y + Door.Direction.Y];
		if (Data & DUNGEON_CORRIDOR) {
			continue;
		}
		if (Data & ROOM_ENTRANCE) {
			int32& NextData = DungeonData[Door.Location.X + Door.Direction.X * 2][Door.Location.Y + Door.Direction.Y * 2];
			if (NextData & (DUNGEON_ROOM | DUNGEON_CORRIDOR | ROOM_ENTRANCE | ROOM_DOOR)) {
				Data &= ~ROOM_ENTRANCE;
				Data |= DUNGEON_CORRIDOR;
				if (NextData & DUNGEON_ROOM) {
					NextData |= ROOM_DOOR;
				}
				continue;
			}
			Data &= ~ROOM_ENTRANCE;
			DungeonData[Door.Location.X][Door.Location.Y] &= ~ROOM_DOOR;
		}

		Room.Doors.RemoveAt(I);
	}
	return Room.Doors.Num() > 0;
}

void ADungeonGenerator2::CreateCorridors()
{
	for (auto& Room : Rooms) {
		for (auto& Door : Room.Doors) {
			int32 CorridorX = Door.Location.X + Door.Direction.X;
			int32 CorridorY = Door.Location.Y + Door.Direction.Y;
			CreateCorridor(CorridorX, CorridorY, Door.DirectionIndex);
		}
	}
}

void ADungeonGenerator2::CreateCorridor(int32 Row, int32 Col, int32 LastDirection)
{	
	int32 CorridorX = Row;
	int32 CorridorY = Col;
	if (!IsPointValid(CorridorX, CorridorY)) {
		return;
	}
	int32 DirectionIndex = FMath::FRand() < ChangeDirectionChance ? FPoint2D::GetRandomDirectionIndex() : LastDirection;

	int32& StartCell = DungeonData[CorridorX][CorridorY];
	if (StartCell & DUNGEON_CORRIDOR) {
		return;
	}

	for (int32 I = 0; I < FPoint2D::NumDirections; ++I, ++DirectionIndex) {
		DirectionIndex %= FPoint2D::NumDirections;
		const FPoint2D& Direction = FPoint2D::AllDirections[DirectionIndex];
		if (!CheckCorridor(CorridorX, CorridorY, Direction)) {
			continue;
		}
		for (int32 I = 0; I < 2; ++I) {			
			int32& Data = DungeonData[CorridorX][CorridorY];
			Data |= DUNGEON_CORRIDOR;
			CorridorX += Direction.X;
			CorridorY += Direction.Y;
		}
		CreateCorridor(CorridorX, CorridorY, DirectionIndex);
	}
}

bool ADungeonGenerator2::CheckCorridor(int32 CorridorX, int32 CorridorY, const FPoint2D& Direction)
{
	for (int32 I = 1; I < 3; ++I) {
		int32 X = CorridorX + Direction.X * I;
		int32 Y = CorridorY + Direction.Y * I;
		if (!IsPointValid(X, Y)) {
			return false;
		}
		int32& Data = DungeonData[X][Y];
		if (Data & BLOCK_CORRIDOR) {
			return false;			
		}
	}
	return true;
}

void ADungeonGenerator2::RemoveDeadEnds()
{
	for (int32 I = 0; I < DungeonWidth; ++I) {
		for (int32 J = 0; J < DungeonHeight; ++J) {
			int32& Cell = DungeonData[I][J];
			if (Cell & ROOM_ENTRANCE) {
				continue;
			}
			if (Cell & DUNGEON_CORRIDOR) {
				CollapseCorridor(I, J);
			}
		}
	}
}

void ADungeonGenerator2::CollapseCorridor(int32 I, int32 J)
{
	int32 NumWays = 0;
	FPoint2D Way;
	for (auto& Direction : FPoint2D::AllDirections) {
		int32 Row = I + Direction.X;
		int32 Col = J + Direction.Y;
		if (Row > 0 && Row < DungeonWidth && Col > 0 && Col < DungeonHeight) {
			int32 &Cell = DungeonData[Row][Col];
			if (Cell & (DUNGEON_CORRIDOR | ROOM_ENTRANCE | ROOM_DOOR)) {
				++NumWays;
				Way.X = Row;
				Way.Y = Col;
			}
		}
	}

	if (NumWays < 2) {
		int32 &Cell = DungeonData[I][J];
		Cell &= ~(DUNGEON_CORRIDOR | ROOM_ENTRANCE);
		if (NumWays == 1) {
			CollapseCorridor(Way.X, Way.Y);
		}
	}
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
				C = '@';
			}
			else if (Data & DUNGEON_CORRIDOR) {
				C = '+';
			}
			else if (Data & ROOM_DOOR) {
				C = 'D';
			}
			else if (Data & ROOM_ENTRANCE) {
				C = 'E';
			}
			else if (Data & ROOM_PERIMETER) {
				C = ' ';
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

void ADungeonGenerator2::GenerateMeshes()
{
	if (!FloorMesh) {
		UE_LOG(LogTemp, Warning, TEXT("FloorMesh is null"));
		return;
	}
	if (!WallMesh) {
		UE_LOG(LogTemp, Warning, TEXT("WallMesh is null"));
		return;
	}
	if (!WindowMesh) {
		UE_LOG(LogTemp, Warning, TEXT("WindowMesh is null"));
		return;
	}
	if (!SeparatorMesh) {
		UE_LOG(LogTemp, Warning, TEXT("SeparatorMesh is null"));
		return;
	}
	if (!DoorMesh) {
		UE_LOG(LogTemp, Warning, TEXT("DoorMesh is null"));
		return;
	}

	FRotator WallRotation = FRotator::ZeroRotator;
	float HalfCellSize = CellSize * 0.5f;
	for (int32 I = 1; I < DungeonWidth - 1; ++I) {
		for (int32 J = 1; J < DungeonHeight - 1; ++J) {
			FVector CellPosition = FVector(I * CellSize, J * CellSize, 0.0f);
			const int32& Data = DungeonData[I][J];
			if (Data & DUNGEON_CORRIDOR) {
				WallRotation.Yaw = 0.0f;
				for (const auto& Direction : FPoint2D::AllDirections) {
					int32 X = I + Direction.X;
					int32 Y = J + Direction.Y;
					if (!(DungeonData[X][Y] & (DUNGEON_CORRIDOR | DUNGEON_ROOM))) {
						FVector WallPosition = CellPosition + FVector(Direction.X * HalfCellSize, Direction.Y * HalfCellSize, 200.0f);
						auto Wall = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), WallPosition, WallRotation);
						Wall->GetStaticMeshComponent()->SetStaticMesh(WallMesh);						
					}
					WallRotation.Yaw = UKismetMathLibrary::NormalizeAxis(WallRotation.Yaw + 90.0f);
				}
			}
			else if (Data & DUNGEON_ROOM) {
				WallRotation.Yaw = 0.0f;
				FPoint2D PrevDirection = FPoint2D::Left;
				for (const auto& Direction : FPoint2D::AllDirections) {
					int32 X = I + Direction.X;
					int32 Y = J + Direction.Y;
					int32& NextData = DungeonData[X][Y];
					if (!(NextData & DUNGEON_ROOM)) {
						FVector WallPosition = CellPosition + FVector(Direction.X * HalfCellSize, Direction.Y * HalfCellSize, 200.0f);
						auto Wall = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), WallPosition, WallRotation);
						UStaticMesh* StaticMesh;
						if (Data & ROOM_DOOR) {
							StaticMesh = DoorMesh;
						}
						else {
							StaticMesh = (NextData & DUNGEON_CORRIDOR) && FMath::RandBool() ? WindowMesh : WallMesh;
						}
						Wall->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
						FVector SeparatorPosition = WallPosition + FVector(PrevDirection.X * HalfCellSize, PrevDirection.Y * HalfCellSize, 0.0f);
						auto Separator = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), SeparatorPosition, FRotator::ZeroRotator);
						Separator->GetStaticMeshComponent()->SetStaticMesh(SeparatorMesh);
					}
					WallRotation.Yaw = UKismetMathLibrary::NormalizeAxis(WallRotation.Yaw + 90.0f);
					PrevDirection = Direction;
				}
			}
			else {
				continue;
			}
			auto Floor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), CellPosition, FRotator::ZeroRotator);
			Floor->GetStaticMeshComponent()->SetStaticMesh(FloorMesh);

			auto Ceiling = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), CellPosition + FVector(0.0f, 0.0f, 400.0f), FRotator(180.0f,0.0f, 0.0f));
			Ceiling->GetStaticMeshComponent()->SetStaticMesh(FloorMesh);
		}
	}

	for (int32 I = 0; I < Rooms.Num(); ++I) {
		FVector RoomCenter = GetRoomCenter(I) + FVector(0.0f, 0.0f, 100.0f);
		auto Ceiling = GetWorld()->SpawnActor<APointLight>(APointLight::StaticClass(), RoomCenter, FRotator::ZeroRotator);
	}
}

#pragma optimize("", on)