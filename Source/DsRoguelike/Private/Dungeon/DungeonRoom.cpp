// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonRoom.h"
#include "DungeonRoomData.h"
#include "Components/InstancedStaticMeshComponent.h"


// Sets default values
ADungeonRoom::ADungeonRoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	FloorInstancedStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("FloorInstancedStaticMesh");
	FloorInstancedStaticMesh->SetupAttachment(RootComponent);

	WallsInstancedStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("WallsInstancedStaticMesh");
	WallsInstancedStaticMesh->SetupAttachment(RootComponent);
}

void ADungeonRoom::InitializeRoom(const FDungeonRoomData& RoomData, float CellSize, int32** Dungeon)
{
	FTransform FloorTransform;
	FTransform WallTransform;
	for (int32 I = 0; I < RoomData.Size.X; I++) {
		for (int32 J = 0; J < RoomData.Size.Y; J++) {
			FVector CellPosition = FVector(I * CellSize, J * CellSize, 0.0f);
			FloorTransform.SetLocation(CellPosition + FVector(0.0f, 0.0f, 10.0f));
			FloorInstancedStaticMesh->AddInstance(FloorTransform);
			int32 Row = RoomData.Position.X + I;
			int32 Col = RoomData.Position.Y + J;
			int32 Cell = Dungeon[Row][Col];
			if (Cell & WALL) {
				float WallAngle = 180.0f;
				if (Cell & DOWN) {
					FVector WallPosition = CellPosition;
					WallPosition.X += CellSize;
					WallTransform = FTransform(FRotator(0.0f, WallAngle, 0.0f), WallPosition, FVector::OneVector);
					WallsInstancedStaticMesh->AddInstance(WallTransform);
				}
				if (Cell & RIGHT) {
					WallAngle = -90.0f;
					FVector WallPosition = CellPosition;
					WallPosition.X += CellSize;
					WallPosition.Y += CellSize;
					WallTransform = FTransform(FRotator(0.0f, WallAngle, 0.0f), WallPosition, FVector::OneVector);
					WallsInstancedStaticMesh->AddInstance(WallTransform);
				}
				if (Cell & LEFT) {
					WallAngle = 90.0f;
					WallTransform = FTransform(FRotator(0.0f, WallAngle, 0.0f), CellPosition, FVector::OneVector);
					WallsInstancedStaticMesh->AddInstance(WallTransform);
				}
				if (Cell & UP) {
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

// Called when the game starts or when spawned
void ADungeonRoom::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ADungeonRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

