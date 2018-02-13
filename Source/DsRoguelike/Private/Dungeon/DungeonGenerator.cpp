// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGenerator.h"
#include "DungeonLeaf.h"
#include "DungeonRoomData.h"
#include "DungeonRoom.h"
#include "Components/InstancedStaticMeshComponent.h"


// Sets default values
ADungeonGenerator::ADungeonGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DungeonData = nullptr;

	Directions.Push(FIVector(0, 1)); // up
	Directions.Push(FIVector(1, 0)); // right
	Directions.Push(FIVector(0, -1)); // down
	Directions.Push(FIVector(-1, 0)); // left

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	FloorInstancedStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("FloorInstancedStaticMesh");
	FloorInstancedStaticMesh->SetupAttachment(RootComponent);

	WallsInstancedStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("WallsInstancedStaticMesh");
	WallsInstancedStaticMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADungeonGenerator::Destroyed()
{
	EmptyDungeonData();
}

FVector ADungeonGenerator::GetRoomCenter(const FDungeonRoomData& Room)
{
	int32 HalfWidth = Room.Size.X / 2;
	int32 HalfHeight = Room.Size.Y / 2;

	int32 CenterX = Room.Position.X + HalfWidth;
	int32 CenterY = Room.Position.Y + HalfHeight;

	return FVector(CenterX * CellSize, CenterY * CellSize, 200.0f);
}

// Called every frame
void ADungeonGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#pragma optimize("", off)

bool ADungeonGenerator::GenerateDungeon()
{
	EmptyLeaf(RootLeaf);
	EmptyDungeonData();

	RootLeaf = new FDungeonLeaf(0, 0, DungeonWidth, DungeonHeight);	
	DungeonData = new int32*[DungeonWidth];
	for (int32 I = 0; I < DungeonWidth; ++I) {
		DungeonData[I] = new int32[DungeonHeight];
		for (int J = 0; J < DungeonHeight; ++J) {
			if (I == 0 || J == 0 || I == DungeonWidth - 1 || J == DungeonHeight - 1) {
				DungeonData[I][J] = BLOCKED;
			}
			else {
				DungeonData[I][J] = 0;
			}
		}
	}
	
	ProcessLeaf(RootLeaf, Iterations);

	if(!RootLeaf) {
		return false;
	}

	GenerateRooms(RootLeaf);
	GenerateDoors();
	GenerateCorridors();
	RemoveDeadEnds();
	CheckRooms();
	FillDirections();

	GenerateMeshes();

	SetIndices();
	FillConnectedRooms();
	CreateRoomMatrix();

	PrintDungeonData();

	DungeonGenerated();
	return true;
}

bool ADungeonGenerator::ProcessLeaf(FDungeonLeaf * Leaf, uint32 Iteration)
{
	if (!Leaf) {
		return false;
	}

	if (Leaf->RightLeaf != nullptr || Leaf->LeftLeaf != nullptr) {
		return false;
	}

	if (Iteration == 0) {
		return false;
	}

	bool SplitHorizontal = FMath::RandBool();
	float Ratio = Leaf->Width / Leaf->Height;
	if (Ratio > MaxLeafRatio) {
		SplitHorizontal = false;
	}
	else {
		if (Ratio < (1.0f / MaxLeafRatio)) {
			SplitHorizontal = true;
		}
	}

	if (!SplitLeaf(Leaf, SplitHorizontal)) {
		if (!SplitLeaf(Leaf, !SplitHorizontal)) {
			return false;
		}
	}

	ProcessLeaf(Leaf->RightLeaf, Iteration - 1);
	ProcessLeaf(Leaf->LeftLeaf, Iteration - 1);
	
	return true;	
}

bool ADungeonGenerator::SplitLeaf(FDungeonLeaf* Leaf, bool SplitHorizontal)
{
	if (SplitHorizontal) {
		if (Leaf->Height < (MinRoomHeight + BorderSize) * 2) {
			UE_LOG(LogTemp, Log, TEXT("Leaf height is too small -> Width: %d, Height: %d"), Leaf->Width, Leaf->Height);
			return false;
		}

		int32 MinHeight = Leaf->Width * VerticalRatio;
		if (Leaf->Height < MinHeight * 2) {
			return false;
		}

		float SplitY = MinHeight + FMath::RandRange(0, Leaf->Height - MinHeight * 2);

		Leaf->LeftLeaf = new FDungeonLeaf(Leaf->X, Leaf->Y, Leaf->Width, SplitY);
		Leaf->RightLeaf = new FDungeonLeaf(Leaf->X, Leaf->Y + SplitY, Leaf->Width, Leaf->Height - SplitY);

		UE_LOG(LogTemp, Log, TEXT("SplitHorizontal -> Width: %d, Height: %d, X: %d, Y: %d"), Leaf->Width, Leaf->Height, Leaf->X, Leaf->Y);
	}
	else {
		if (Leaf->Width < (MinRoomWidth + BorderSize) * 2) {
			UE_LOG(LogTemp, Log, TEXT("Leaf width is too small -> Width: %d, Height: %d"), Leaf->Width, Leaf->Height);
			return false;
		}

		int32 MinWidth = Leaf->Height * HorizontalRatio;
		if (Leaf->Width < MinWidth * 2) {
			return false;
		}

		float SplitX = MinWidth + FMath::RandRange(0, Leaf->Width - MinWidth * 2);

		Leaf->LeftLeaf = new FDungeonLeaf(Leaf->X, Leaf->Y, SplitX, Leaf->Height);
		Leaf->RightLeaf = new FDungeonLeaf(Leaf->X + SplitX, Leaf->Y, Leaf->Width - SplitX, Leaf->Height);

		UE_LOG(LogTemp, Log, TEXT("SplitVertical -> Width: %d, Height: %d, X: %d, Y: %d"), Leaf->Width, Leaf->Height, Leaf->X, Leaf->Y);
	}

	return true;
}

void ADungeonGenerator::EmptyLeaf(FDungeonLeaf* Leaf)
{
	for (auto Room : ChildrenRooms) {
		if (Room && !Room->IsPendingKill()) {
			Room->Destroy();
		}
	}

	if (Leaf) {
		EmptyLeaf(Leaf->LeftLeaf);
		EmptyLeaf(Leaf->RightLeaf);
		delete Leaf;
	}
}

void ADungeonGenerator::GenerateRooms(FDungeonLeaf* Leaf)
{
	if (!Leaf) {
		return;
	}

	if (Leaf->RightLeaf != nullptr || Leaf->LeftLeaf != nullptr) {
		GenerateRooms(Leaf->RightLeaf);
		GenerateRooms(Leaf->LeftLeaf);
		return;
	}

	int32 MaxRoomWidth = Leaf->Width - BorderSize * 2;
	int32 MaxRoomHeight = Leaf->Height - BorderSize * 2;

	if (MaxRoomWidth > MaxRoomHeight) {
		MaxRoomWidth = FMath::Min((int32)(MaxRoomHeight * MaxRoomRatio), MaxRoomWidth);		
	}
	else {
		MaxRoomHeight = FMath::Min((int32)(MaxRoomWidth * MaxRoomRatio), MaxRoomHeight);
	}

	int32 MinRoomW = FMath::Max(MinRoomWidth, (int32) (MaxRoomWidth * MinRoomWidthRatio));
	int32 MinRoomH = FMath::Max(MinRoomHeight, (int32) (MaxRoomHeight * MinRoomHeightRatio));

	MinRoomW = FMath::Max(MinRoomW, FMath::CeilToInt(MinRoomH / MaxRoomRatio));

	if (MinRoomW > MaxRoomWidth) {
		UE_LOG(LogTemp, Log, TEXT("Incorrect room size"));
		return;
	}

	int32 RoomWidth = FMath::RandRange(MinRoomW, MaxRoomWidth);

	MinRoomH = FMath::Max(MinRoomH, FMath::CeilToInt(RoomWidth / MaxRoomRatio));
	MaxRoomHeight = FMath::Min(MaxRoomHeight, FMath::FloorToInt(RoomWidth * MaxRoomRatio));

	if (MinRoomH > MaxRoomHeight) {
		UE_LOG(LogTemp, Log, TEXT("Incorrect room size"));
		return;
	}

	int32 RoomHeight = FMath::RandRange(MinRoomH, MaxRoomHeight);

	int32 RoomX = Leaf->X + BorderSize + FMath::RandRange(0, MaxRoomWidth - RoomWidth);
	int32 RoomY = Leaf->Y + BorderSize + FMath::RandRange(0, MaxRoomHeight - RoomHeight);

	FDungeonRoomData Room(RoomX, RoomY, RoomWidth, RoomHeight);

	int32 RoomRight = RoomX + RoomWidth;
	int32 RooomTop = RoomY + RoomHeight;

	for (int32 Row = RoomX; Row < RoomRight; ++Row) {
		for (int32 Col = RoomY; Col < RooomTop; ++Col) {
			DungeonData[Row][Col] |= ROOM;
		}
	}

	for (int32 Row = RoomX; Row < RoomRight; ++Row) {
		DungeonData[Row][RoomY] |= WALL;
		DungeonData[Row][RooomTop - 1] |= WALL;
	}

	for (int32 Col = RoomY; Col < RooomTop; ++Col) {
		DungeonData[RoomX][Col] |= WALL;
		DungeonData[RoomRight - 1][Col] |= WALL;
	}

	for (int32 Row = RoomX - 1; Row <= RoomRight; ++Row) {
		DungeonData[Row][RoomY - 1] |= PERIMETER;
		DungeonData[Row][RooomTop] |= PERIMETER;
	}
	for (int32 Col = RoomY- 1; Col <= RooomTop; ++Col) {
		DungeonData[RoomX - 1][Col] |= PERIMETER;
		DungeonData[RoomRight][Col] |= PERIMETER;
	}

	Rooms.Push(Room);
}

void ADungeonGenerator::GenerateDoors()
{
	for (auto& Room : Rooms) {
		OpenRoom(Room);		
	}
}

void ADungeonGenerator::OpenRoom(const FDungeonRoomData& RoomInfo)
{
	FIVector CurrentPosition(RoomInfo.Position.X - 1, RoomInfo.Position.Y);

	int32 PerimeterWidth = RoomInfo.Size.X + 2;
	int32 PerimeterHeight = RoomInfo.Size.Y + 2;

	int32 PerimeterSize = PerimeterWidth * 2 + PerimeterHeight * 2;

	int32 DirectionIndex = 0;
	FIVector Direction = Directions[DirectionIndex];
	FIVector CheckDir = Directions[(DirectionIndex + 3) % 4];

	int32 ValidCellsCount = 0;
	FIVector EndPosition(CurrentPosition.X, CurrentPosition.Y + PerimeterHeight * Direction.Y - 2);

	for (int I = 0; I < PerimeterSize; ++I) {
		bool DoorLocked = false;
		if (CurrentPosition.X == EndPosition.X && CurrentPosition.Y == EndPosition.Y) {
			DirectionIndex = (DirectionIndex + 1) % 4;
			Direction = Directions[DirectionIndex];
			CheckDir = Directions[(DirectionIndex + 3) % 4];
			EndPosition = FIVector(CurrentPosition.X + PerimeterWidth * Direction.X - Direction.X, CurrentPosition.Y + PerimeterHeight * Direction.Y - Direction.Y);
			DoorLocked = true;
		}
		else {
			DoorLocked = !CheckOpen(CurrentPosition, CheckDir);
		}

		if (DoorLocked) {
			int32 DoorCell = DungeonData[CurrentPosition.X][CurrentPosition.Y];
			DungeonData[CurrentPosition.X][CurrentPosition.Y] = DoorCell | LOCKED;
		}
		else {
			++ValidCellsCount;
		}

		CurrentPosition.X += Direction.X;
		CurrentPosition.Y += Direction.Y;
	}

	if (LockCorners) {
		int32 Left = RoomInfo.Position.X - 1;
		int32 Bottom = RoomInfo.Position.Y - 1;
		int32 Right = RoomInfo.Position.X + RoomInfo.Size.X;
		int32 Top = RoomInfo.Position.Y + RoomInfo.Size.Y;

		LockCorner(Left, Top - 1, ValidCellsCount);
		LockCorner(Left, Bottom + 1, ValidCellsCount);

		LockCorner(Right, Top - 1, ValidCellsCount);
		LockCorner(Right, Bottom + 1, ValidCellsCount);

		LockCorner(Right - 1, Top, ValidCellsCount);
		LockCorner(Left + 1, Top, ValidCellsCount);

		LockCorner(Right - 1, Bottom, ValidCellsCount);
		LockCorner(Left + 1, Bottom, ValidCellsCount);
	}

	if (ValidCellsCount < 2) {
		return;
	}

	CurrentPosition = FIVector(RoomInfo.Position.X - 1, RoomInfo.Position.Y);
	DirectionIndex = 0;
	Direction = Directions[DirectionIndex];
	EndPosition = FIVector(CurrentPosition.X, CurrentPosition.Y + PerimeterHeight * Direction.Y - 2);

	int32 NumDoors = FMath::RandRange(1, FMath::Min(ValidCellsCount / 2, MaxDoorsCount));
	int32 DoorsCount = NumDoors - 1;
	int32 StartIndex = 0;
	for (int32 I = 0; I < NumDoors; ++I) {
		int32 LastIndex = ValidCellsCount - DoorsCount - 1;
		int32 DoorIndex = FMath::RandRange(StartIndex, LastIndex);
		ValidCellsCount -= (DoorIndex + 1);
		for(int32 Index = 0; Index < PerimeterSize; ++Index) {
			int32 DoorCell = DungeonData[CurrentPosition.X][CurrentPosition.Y];			
			if (!(DoorCell & (BLOCK_DOOR))) {
				int32 PrevCell = DungeonData[CurrentPosition.X - Direction.X][CurrentPosition.Y - Direction.Y];
				int32 NextCell = DungeonData[CurrentPosition.X + Direction.X][CurrentPosition.Y + Direction.Y];

				if (!(PrevCell & ENTRANCE)) {
					if (!(NextCell & ENTRANCE)) {
						if (DoorIndex == 0) {
							DungeonData[CurrentPosition.X][CurrentPosition.Y] = DoorCell | ENTRANCE;
							break;
						}
						else {
							--DoorIndex;
						}
					}
				}										
			}

			if (CurrentPosition.X == EndPosition.X && CurrentPosition.Y == EndPosition.Y) {
				DirectionIndex = (DirectionIndex + 1) % 4;
				Direction = Directions[DirectionIndex];
				EndPosition = FIVector(CurrentPosition.X + PerimeterWidth * Direction.X - Direction.X, CurrentPosition.Y + PerimeterHeight * Direction.Y - Direction.Y);
			}
			CurrentPosition.X += Direction.X;
			CurrentPosition.Y += Direction.Y;
		}
		--DoorsCount;
	}
}

bool ADungeonGenerator::CheckOpen(const FIVector& Cell, const FIVector& Direction)
{
	FIVector CheckedCell(Cell.X + Direction.X, Cell.Y + Direction.Y);
	if (CheckedCell.X > 0 && CheckedCell.Y > 0 && CheckedCell.X < DungeonWidth - 1 && CheckedCell.Y < DungeonHeight - 1) {
		int32 CheckCell = DungeonData[CheckedCell.X][CheckedCell.Y];
		if (CheckCell & BLOCK_ENTRANCE) {
			return false;
		}
		return true;
	}
	return false;
}

void ADungeonGenerator::LockCorner(int32 Row, int32 Col, int32& ValidCellsCount)
{
	int32& Cell = DungeonData[Row][Col];
	if (Cell & LOCKED) {
		return;
	}
	Cell |= LOCKED;
	--ValidCellsCount;
}

void ADungeonGenerator::GenerateCorridors()
{
	for (auto& Room : Rooms) {
		int32 RoomRight = Room.Position.X + Room.Size.X - 1;
		int32 RoomTop = Room.Position.Y + Room.Size.Y - 1;

		for (int32 Row = Room.Position.X; Row <= RoomRight; ++Row) {
			for (int32 Col = Room.Position.Y; Col <= RoomTop; ++Col) {
				if (DungeonData[Row][Col] & WALL) {
					Tunnel(Row, Col);
				}
			}
		}
	}
}

void ADungeonGenerator::Tunnel(int32 ThisR, int32 ThisC, int32 LastDir /*= 0*/)
{
	int32 StartDirection = FMath::FRand() < ChangeDirectionChance ? FMath::RandRange(0, Directions.Num() - 1) : LastDir;

	for (int32 Index = 0; Index < 4; ++Index) {
		int32 DirectionIndex = (StartDirection + Index) % Directions.Num();
		const FIVector& Direction = Directions[DirectionIndex];
		if (OpenTunnel(ThisR, ThisC, Direction)) {
			int32 NextI = ThisR + Direction.X * 2;
			int32 NextJ = ThisC + Direction.Y * 2;
			Tunnel(NextI, NextJ, DirectionIndex);
		}
	}
}

bool ADungeonGenerator::OpenTunnel(int32 ThisR, int32 ThisC, const FIVector& Direction)
{
	int32 NextR = ThisR + Direction.X;
	int32 NextC = ThisC + Direction.Y;

	if (SoundTunnel(ThisR, ThisC, Direction)) {
		DelveTunnel(ThisR, ThisC, Direction);
		return true;
	}
	return false;
}

bool ADungeonGenerator::SoundTunnel(int32 ThisR, int32 ThisC, const FIVector& Direction)
{
	int32 NextR = ThisR + Direction.X;
	int32 NextC = ThisC + Direction.Y;

	if (NextR < 0 || NextR >= DungeonWidth) {
		return false;
	}
	if (NextC < 0 || NextC >= DungeonHeight) {
		return false;
	}

	for (int32 I = 1; I < 3; ++I) {
		int32 R = ThisR + Direction.X * I;
		int32 C = ThisC + Direction.Y * I;

		if (!(DungeonData[R][C] & ENTRANCE)) {
			if (DungeonData[R][C] & (BLOCK_CORR)) {
				return false;
			}
		}
	}

	int32 CorridorCount = 0;
	FIVector DirectionRight(Direction.Y, Direction.X);
	for (int32 I = 0; I < 2; ++I) {
		int32 R = ThisR + Direction.X * I + DirectionRight.X;
		int32 C = ThisC + Direction.Y * I + DirectionRight.Y;
		if (DungeonData[R][C] & CORRIDOR) {
			++CorridorCount;
		}
	}

	if (CorridorCount == 2) {
		return false;
	}

	CorridorCount = 0;

	FIVector DirectionLeft(-Direction.Y, -Direction.X);
	for (int32 I = 0; I < 2; ++I) {
		int32 R = ThisR + Direction.X * I + DirectionLeft.X;
		int32 C = ThisC + Direction.Y * I + DirectionLeft.Y;
		if (DungeonData[R][C] & CORRIDOR) {
			++CorridorCount;
		}
	}

	if (CorridorCount == 2) {
		return false;
	}

	return true;
}

void ADungeonGenerator::DelveTunnel(int32 ThisR, int32 ThisC, const FIVector& Direction)
{
	for (int32 I = 0; I < 3; ++I) {
		int32 R = ThisR + Direction.X * I;
		int32 C = ThisC + Direction.Y * I;

		int32 &ThisCell = DungeonData[R][C];
		ThisCell |= CORRIDOR;
	}
}

void ADungeonGenerator::RemoveDeadEnds()
{
	for (int32 I = 0; I < DungeonWidth; ++I) {
		for (int32 J = 0; J < DungeonHeight; ++J) {
			int32& Cell = DungeonData[I][J];
			if ((Cell & CORRIDOR) && !(Cell & ENTRANCE)) {
				CollapseTunnel(I, J);
			}
		}
	}
}

void ADungeonGenerator::CollapseTunnel(int32 I, int32 J)
{
	int32 NumWays = 0;
	FIVector Way;
	for (auto& Direction : Directions) {
		int32 Row = I + Direction.X;
		int32 Col = J + Direction.Y;
		if (Row > 0 && Row < DungeonWidth && Col > 0 && Col < DungeonHeight) {
			int32 &Cell = DungeonData[Row][Col];
			if (Cell & (CORRIDOR | ENTRANCE)) {
				++NumWays;
				Way.X = Row;
				Way.Y = Col;
			}
		}
	}

	if (NumWays < 2) {
		int32 &Cell = DungeonData[I][J];
		Cell &= ~(CORRIDOR | ENTRANCE);
		if (NumWays == 1) {
			CollapseTunnel(Way.X, Way.Y);
		}		
	}
}

void ADungeonGenerator::CheckRooms()
{
	for (int32 I = Rooms.Num() - 1; I >= 0; --I) {
		FDungeonRoomData& Room = Rooms[I];
		if (!CheckRoom(Room)) {
			DestoryRoom(Room);
			Rooms.RemoveAt(I);
		}
	}
}

bool ADungeonGenerator::CheckRoom(FDungeonRoomData& RoomInfo)
{
	int32 DoorCount = 0;

	int32 RoomRight = RoomInfo.Position.X + RoomInfo.Size.X - 1;
	int32 RooomTop = RoomInfo.Position.Y + RoomInfo.Size.Y - 1;
	FIVector Direction;
	FDoorInfo DoorInfo;
	for (int32 Row = RoomInfo.Position.X; Row <= RoomRight; ++Row) {
		if (CreateDoor(Row, RoomInfo.Position.Y, FIVector(0, -1), RoomInfo)) {
			++DoorCount;
		}
		if (CreateDoor(Row, RooomTop, FIVector(0, 1), RoomInfo)) {
			++DoorCount;
		}
	}

	for (int32 Col = RoomInfo.Position.Y; Col <= RooomTop; ++Col) {
		if (CreateDoor(RoomInfo.Position.X, Col, FIVector(-1, 0), RoomInfo)) {
			++DoorCount;
		}
		if (CreateDoor(RoomRight, Col, FIVector(1, 0), RoomInfo)) {
			++DoorCount;
		}
	}

	return DoorCount > 0;
}

bool ADungeonGenerator::CreateDoor(int32 Row, int32 Col, const FIVector& Direction, FDungeonRoomData& RoomInfo)
{
	int32 NextR = Row + Direction.X;
	int32 NextC = Col + Direction.Y;

	if (!(DungeonData[NextR][NextC] & ENTRANCE)) {
		return false;
	}

	DungeonData[NextR][NextC] &= ~CORRIDOR;

	int32 CorridorR = NextR + Direction.X;
	int32 CorridorC = NextC + Direction.Y;

	if (CorridorR < 0 || CorridorR >= DungeonWidth) {
		return false;
	}

	if (CorridorC < 0 || CorridorC >= DungeonHeight) {
		return false;
	}

	if (!(DungeonData[CorridorR][CorridorC] & CORRIDOR)) {
		DungeonData[NextR][NextC] &= ~ENTRANCE;
		return false;
	}

	int32& Cell = DungeonData[Row][Col];
	Cell &= ~WALL;
	Cell |= DOOR;

	FDoorInfo Door;
	Door.Location = FIVector(Row, Col);
	Door.Direction = Direction;
	RoomInfo.Doors.Push(Door);

	return true;
}

void ADungeonGenerator::DestoryRoom(const FDungeonRoomData& RoomInfo)
{
	UE_LOG(LogTemp, Log, TEXT("DestoryRoom"));
	int32 PerimeterWidth = RoomInfo.Size.X + 2;
	int32 PerimeterHeight = RoomInfo.Size.Y + 2;

	int32 X = RoomInfo.Position.X - 1;
	int32 Y = RoomInfo.Position.Y - 1;
	for (int32 I = 0; I < PerimeterWidth; ++I) {
		for (int32 J = 0; J < PerimeterHeight; ++J) {
			int32 Row = I + X;
			int32 Col = J + Y;

			int32& Cell = DungeonData[Row][Col];
			if (Cell & ROOM_INSIDE) {
				Cell &= ~ROOM_INSIDE;
			}
		}
	}
}

void ADungeonGenerator::FillDirections()
{
	for (int32 Row = 1; Row < DungeonWidth - 1; ++Row) {
		for (int32 Col = 1; Col < DungeonHeight - 1; ++Col) {
			int32& Cell = DungeonData[Row][Col];
			if (Cell & WALL) {
				Cell |= CellDirections(Row, Col, PERIMETER);
			}
			else if (Cell & CORRIDOR) {
				Cell |= CellDirections(Row, Col, CORRIDOR | ENTRANCE);
			}
			else if (Cell & ENTRANCE) {
				Cell |= CellDirections(Row, Col, CORRIDOR | ROOM);
			}			
		}
	}
}

int32 ADungeonGenerator::CellDirections(int32 Row, int32 Col, int32 Cell)
{
	int32 Dirs = 0;
	const int32 DirectionBits[4] = {UP, RIGHT, DOWN, LEFT};
	for (int32 I = 0; I < 4; ++I) {
		auto& Direction = Directions[I];
		int32 DirCell = DungeonData[Row + Direction.X][Col + Direction.Y];
		if (DirCell & Cell) {
			Dirs |= DirectionBits[I];
		}		
	}

	return Dirs;
}

void ADungeonGenerator::GenerateMeshes()
{
	if (!DungeonRoomClass) {
		return;
	}
	SetActorLocation(FVector::ZeroVector);
	FTransform RoomTransform;
	for (auto& RoomData : Rooms) {
		RoomTransform.SetLocation(FVector(RoomData.Position.X * CellSize, RoomData.Position.Y * CellSize, 0.0f));
		auto Room = GetWorld()->SpawnActor<ADungeonRoom>(DungeonRoomClass, RoomTransform);
		if (Room) {
			Room->InitializeRoom(RoomData, CellSize, DungeonData);
		}
	}

	FTransform FloorTransform;
	FTransform WallTransform;
	for (int32 I = 0; I < DungeonWidth; ++I)  {
		for (int32 J = DungeonHeight - 1; J >= 0; --J) {
			int32 Cell = DungeonData[I][J];
			if (Cell & (CORRIDOR | ENTRANCE)) {
				FVector CellPosition = FVector(I * CellSize, J * CellSize, 0.0f);
				FloorTransform.SetLocation(CellPosition + FVector(0.0f, 0.0f, 10.0f));
				FloorInstancedStaticMesh->AddInstance(FloorTransform);

				int32 WallDirections = ALL_DIRECTIONS & ~Cell;

				float WallAngle = 180.0f;
				if (WallDirections & DOWN) {
					FVector WallPosition = CellPosition;
					WallPosition.X += CellSize;
					WallTransform = FTransform(FRotator(0.0f, WallAngle, 0.0f), WallPosition, FVector::OneVector);
					WallsInstancedStaticMesh->AddInstance(WallTransform);
				}
				if (WallDirections & RIGHT) {
					WallAngle = -90.0f;
					FVector WallPosition = CellPosition;
					WallPosition.X += CellSize;
					WallPosition.Y += CellSize;
					WallTransform = FTransform(FRotator(0.0f, WallAngle, 0.0f), WallPosition, FVector::OneVector);
					WallsInstancedStaticMesh->AddInstance(WallTransform);
				}
				if (WallDirections & LEFT) {
					WallAngle = 90.0f;
					WallTransform = FTransform(FRotator(0.0f, WallAngle, 0.0f), CellPosition, FVector::OneVector);
					WallsInstancedStaticMesh->AddInstance(WallTransform);
				}
				if (WallDirections & UP) {
					FVector WallPosition = CellPosition;
					WallAngle = 0.0f;
					WallPosition.Y += CellSize;
					WallTransform = FTransform(FRotator(0.0f, WallAngle, 0.0f), WallPosition, FVector::OneVector);
					WallsInstancedStaticMesh->AddInstance(WallTransform);
				}
			}			
		}
	}
}

void ADungeonGenerator::SetIndices()
{
	int32 CurrentIndex = 0;
	for (auto& Room : Rooms) {
		SetRoomIndex(Room, CurrentIndex++);
	}

	int32 CorridorIndex = 1;
	for (auto& Room : Rooms) {
		for (auto& Door : Room.Doors) {
			int32 Row = Door.Location.X + Door.Direction.X;
			int32 Col = Door.Location.Y + Door.Direction.Y;
			if (SetCorridorIndex(Row, Col, CorridorIndex, Room.Index)) {
				++CorridorIndex;
			}
		}
	}
}

void ADungeonGenerator::SetRoomIndex(FDungeonRoomData& RoomInfo, int32 RoomIndex)
{
	int32 RoomRight = RoomInfo.Position.X + RoomInfo.Size.X;
	int32 RooomTop = RoomInfo.Position.Y + RoomInfo.Size.Y;
	RoomInfo.Index = RoomIndex;

	for (int32 Row = RoomInfo.Position.X; Row < RoomRight; ++Row) {
		for (int32 Col = RoomInfo.Position.Y; Col < RooomTop; ++Col) {
			int32 &Data = DungeonData[Row][Col];
			Data |= (RoomIndex << 16);
		}
	}
}

bool ADungeonGenerator::SetCorridorIndex(int32 Row, int32 Col, int32 Index, int32 StartRoomIndex)
{
	int32 &CellData = DungeonData[Row][Col];
	int32 CellIndex = (CellData & INDEX) >> 16;
	if (CellData & CORRIDOR_ENTRANCE) {
		if (CellIndex > 0) {
			return false;
		}

		CellData |= (Index << 16);
		for (auto& Direction : Directions) {
			int32 R = Row + Direction.X;
			int32 C = Col + Direction.Y;
			SetCorridorIndex(R, C, Index, StartRoomIndex);
		}
		return true;
	}
	return false;
}

void ADungeonGenerator::FillConnectedRooms()
{
	for (auto& Room : Rooms) {
		for (auto& Door : Room.Doors) {
			int32 Row = Door.Location.X + Door.Direction.X;
			int32 Col = Door.Location.Y + Door.Direction.Y;
			FindRoom(Room, Row, Col, 0);

			for (int32 I = 1; I < DungeonWidth - 1; ++I) {
				for (int32 J = 1; J < DungeonHeight - 1; ++J) {
					DungeonData[I][J] &= ~VISITED;
				}
			}
		}
	}
}

void ADungeonGenerator::FindRoom(FDungeonRoomData& StartRoom, int32 Row, int32 Col, int32 Distance)
{
	int32 &CellData = DungeonData[Row][Col];
	if (CellData & CORRIDOR_ENTRANCE) {
		if (CellData & VISITED) {
			return;
		}

		CellData |= VISITED;

		for (auto& Direction : Directions) {
			int32 R = Row + Direction.X;
			int32 C = Col + Direction.Y;
			FindRoom(StartRoom, R, C, Distance + 1);
		}
		return;
	}	

	if (CellData & ROOM) {
		int32 CellIndex = (CellData & INDEX) >> 16;
		if (CellIndex == StartRoom.Index) {
			return;
		}
		if (!Rooms.IsValidIndex(CellIndex)) {
			return;
		}
		FDungeonRoomData& EndRoom = Rooms[CellIndex];
		const int32* CurrDistance = StartRoom.ConnectedRooms.Find(CellIndex);
		if (CurrDistance && (*CurrDistance < Distance)) {
			return;
		}
		StartRoom.ConnectedRooms.Add(CellIndex, Distance);
		EndRoom.ConnectedRooms.Add(StartRoom.Index, Distance);
	}
}

void ADungeonGenerator::CreateRoomMatrix()
{
	const int32 Inf = MAX_int32;
	RoomsMatrix = new int32*[Rooms.Num()];
	for (int32 I = 0; I < Rooms.Num(); ++I) {
		RoomsMatrix[I] = new int32[Rooms.Num()];
		for (int32 J = 0; J < Rooms.Num(); ++J) {
			RoomsMatrix[I][J] = I == J ? 0 : Inf;
		}
	}

	for (auto& Room : Rooms) {
		for (auto& ConnectedRoom : Room.ConnectedRooms) {
			RoomsMatrix[Room.Index][ConnectedRoom.Key] = ConnectedRoom.Value;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Room Distance: "));
	for (int32 I = 0; I < Rooms.Num(); ++I) {
		FString DungeonRow;
		for (int32 J = 0; J < Rooms.Num(); ++J) {
			DungeonRow.Append(FString::Printf(TEXT("%d - "), RoomsMatrix[I][J]));
		}
		UE_LOG(LogTemp, Log, TEXT("%s"), *DungeonRow);
	}

	for (int32 K = 0; K < Rooms.Num(); ++K) {
		for (int32 I = 0; I < Rooms.Num(); ++I) {
			for (int32 J = 0; J < Rooms.Num(); ++J) {
				RoomsMatrix[I][J] = FMath::Min(RoomsMatrix[I][J], RoomsMatrix[I][K] + RoomsMatrix[K][J]);
			}
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("RoomMatrix: "));
	for (int32 I = 0; I < Rooms.Num(); ++I) {
		FString DungeonRow;
		for (int32 J = 0; J < Rooms.Num(); ++J) {
			DungeonRow.Append(FString::Printf(TEXT("%d - "), RoomsMatrix[I][J]));
		}
		UE_LOG(LogTemp, Log, TEXT("%s"), *DungeonRow);
	}
}

void ADungeonGenerator::PrintDungeonData()
{
	int32 Index = 0;
	for (int32 J = DungeonHeight - 1; J >= 0; --J) {
		FString DungeonRow;
		for (int32 I = 0; I < DungeonWidth; ++I) {

			int32 Data = DungeonData[I][J];
			int8 C = ' ';

			if (Data & BLOCKED) {
				C = '+';
			}
			else if (Data & (WALL | CORRIDOR | ENTRANCE)) {
				C = GetDirectionText(Data);
			}
			else if (Data & DOOR) {
				C = '/';
			}
			else if (Data & ROOM) {
				C = '.';
			}
			if (Data & PERIMETER) {
				if (Data & LOCKED) {
					C = '~';
				}
			}
			/*else if (Data & CORRIDOR) {
				C = GetDirectionText(Data);
			}
			else if (Data & PERIMETER) {
				if (Data & LOCKED) {
					C = 'L';
				}
				else if (Data & ENTRANCE) {
					C = '=';
				}
				else {
					C = '*';
				}
			}*/	

			DungeonRow.Append(FString::Printf(TEXT("%c"), C));
			Index++;
		}
		UE_LOG(LogTemp, Log, TEXT("%s"), *DungeonRow);
	}
}

int8 ADungeonGenerator::GetDirectionText(int32 Cell)
{
	int32 CellValue = (ALL_DIRECTIONS) & Cell;
	switch (CellValue)
	{
	case ALL_DIRECTIONS: return 'A';

	case UP_RIGHT_DOWN: return '0';
	case UP_DOWN_LEFT: return '1';
	case UP_RIGHT_LEFT: return '2';
	case RIGHT_DOWN_LEFT: return '3';

	case RIGHT_LEFT: return '4';
	case UP_DOWN: return '5';
	case UP_RIGHT: return '6';
	case UP_LEFT: return '7';
	case DOWN_RIGHT: return '8';
	case DOWN_LEFT: return '9';

	case UP: return 'U';
	case DOWN: return 'D';
	case RIGHT: return 'R';
	case LEFT: return 'L';
	default:
		break;
	}
	/*if (CellValue == ALL_DIRECTIONS) {
		return 'A';
	}
	if (CellValue == UP_RIGHT_DOWN) {
		return '0';
	}
	if (CellValue == UP_DOWN_LEFT) {
		return '1';
	}
	if (CellValue == UP_RIGHT_LEFT) {
		return '2';
	}
	if (CellValue == RIGHT_DOWN_LEFT) {
		return '3';
	}
	if (CellValue == RIGHT_LEFT) {
		return '4';
	}
	if (CellValue == UP_DOWN) {
		return '5';
	}
	if (CellValue == UP_RIGHT) {
		return '6';
	}
	if (CellValue == UP_LEFT) {
		return '7';
	}
	if (CellValue == DOWN_RIGHT) {
		return '8';
	}
	if (CellValue == DOWN_LEFT) {
		return '9';
	}
	if (Cell & UP) {
		return 'U';
	}
	if (Cell & DOWN) {
		return 'D';
	}
	if (Cell & RIGHT) {
		return 'R';
	}
	if (Cell & LEFT) {
		return 'L';
	}*/
	return 'B';
}

void ADungeonGenerator::EmptyDungeonData()
{
	if (DungeonData) {
		for (int I = 0; I < DungeonWidth; ++I) {
			delete[] DungeonData[I];
		}
		delete[] DungeonData;
	}

	FloorInstancedStaticMesh->ClearInstances();
	WallsInstancedStaticMesh->ClearInstances();

	Rooms.Empty();
}

#pragma optimize("", on)