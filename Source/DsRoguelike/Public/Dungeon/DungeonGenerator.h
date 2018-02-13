// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonRoomData.h"
#include "DungeonGenerator.generated.h"

struct FDungeonLeaf;

UCLASS()
class DSROGUELIKE_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonGenerator();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool GenerateDungeon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION(BlueprintImplementableEvent)
	void DungeonGenerated();

	UFUNCTION(BlueprintCallable)
	FVector GetRoomCenter(const FDungeonRoomData& Room);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	int32 DungeonWidth = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	int32 DungeonHeight = 60;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	int32 Iterations = 4;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	float VerticalRatio = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	float HorizontalRatio = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	int32 MinRoomWidth = 4;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	int32 MinRoomHeight = 4;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	float MinRoomWidthRatio = 0.6f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	float MinRoomHeightRatio = 0.6f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	float MaxRoomRatio = 1.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	float MaxLeafRatio = 1.25f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	float DoorCreationChance = 0.1f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	int32 BorderSize = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	int32 MaxDoorsCount = 4;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	bool LockCorners = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dungeon")
	float ChangeDirectionChance = 0.1f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dungeon")
	float CellSize = 400.0f;	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon")
	TSubclassOf<class ADungeonRoom> DungeonRoomClass;

	UPROPERTY(BlueprintReadOnly)
	TArray<FDungeonRoomData> Rooms;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UInstancedStaticMeshComponent* FloorInstancedStaticMesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UInstancedStaticMeshComponent* WallsInstancedStaticMesh;

	int32** DungeonData;

	int32** RoomsMatrix;
private:
	bool ProcessLeaf(FDungeonLeaf* Leaf, uint32 Iteration);
	bool SplitLeaf(FDungeonLeaf* Leaf, bool SplitHorizontal);
	void EmptyLeaf(FDungeonLeaf* Leaf);

	void GenerateRooms(FDungeonLeaf* Leaf);

	void GenerateDoors();
	void OpenRoom(const FDungeonRoomData& RoomInfo);
	bool CheckOpen(const FIVector& Cell, const FIVector& Direction);
	void LockCorner(int32 Row, int32 Col, int32& ValidCellsCount);

	void GenerateCorridors();
	void Tunnel(int32 ThisR, int32 ThisC, int32 LastDir = 0);
	bool OpenTunnel(int32 ThisR, int32 ThisC, const FIVector& Direction);
	bool SoundTunnel(int32 ThisR, int32 ThisC, const FIVector& Direction);
	void DelveTunnel(int32 ThisR, int32 ThisC, const FIVector& Direction);

	void RemoveDeadEnds();
	void CollapseTunnel(int32 I, int32 J);

	void CheckRooms();
	bool CheckRoom(FDungeonRoomData& RoomInfo);
	bool CreateDoor(int32 Row, int32 Col, const FIVector& Direction, FDungeonRoomData& RoomInfo);
	void DestoryRoom(const FDungeonRoomData& RoomInfo);
	void FillDirections();
	int32 CellDirections(int32 Row, int32 Col, int32 Cell);
	
	void GenerateMeshes();

	void SetIndices();
	void SetRoomIndex(FDungeonRoomData& RoomInfo, int32 RoomIndex);
	bool SetCorridorIndex(int32 Row, int32 Col, int32 Index, int32 StartRoomIndex);

	void FillConnectedRooms();
	void FindRoom(FDungeonRoomData& StartRoom, int32 Row, int32 Col, int32 Distance);

	void CreateRoomMatrix();

	void PrintDungeonData();
	int8 GetDirectionText(int32 Cell);
	void EmptyDungeonData();

private:
	FDungeonLeaf* RootLeaf = nullptr;

	TArray<AActor*> ChildrenRooms;

	TArray<FIVector> Directions;
};
