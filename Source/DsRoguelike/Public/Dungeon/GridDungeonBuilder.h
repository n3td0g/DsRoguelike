// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dungeon/DungeonBuilder.h"
#include "GridDungeonBuilder.generated.h"

#define ROOM_ENTRANCE		0x00000004
#define ROOM_DOOR			0x00000008
#define ROOM_VISITED		0x00000010

#define BLOCK_DOOR		( ROOM_DOOR )
#define BLOCK_CORRIDOR	( DUNGEON_ROOM | DUNGEON_CORRIDOR )
#define DUNGEON_FLOOR	( DUNGEON_ROOM | DUNGEON_CORRIDOR )

USTRUCT()
struct FDungeonLeaf
{
	GENERATED_BODY()

	UPROPERTY()
	FIntPoint LeafPosition;

	UPROPERTY()
	FIntPoint LeafSize;


	FDungeonLeaf* LeftLeaf;
	FDungeonLeaf* RightLeaf;

	FDungeonLeaf() {
		LeftLeaf = nullptr;
		RightLeaf = nullptr;
	}

	FDungeonLeaf(FIntPoint Position, FIntPoint Size) :
		LeafPosition(Position), LeafSize(Size)
	{
		LeftLeaf = nullptr;
		RightLeaf = nullptr;
	}
};

/**
 * 
 */
UCLASS()
class DSROGUELIKE_API UGridDungeonBuilder : public UDungeonBuilder
{
	GENERATED_BODY()
public:
	UGridDungeonBuilder();
	
public:
	virtual void GenerateDungeon(ADungeon* ParentDungeon) override;
	virtual void PlaceMarkers() override;

protected:
	void Initialize();

	//Cleanup
	void CleanupLeafs();
	void CleanupLeaf(FDungeonLeaf* Leaf);

	//BSP
	void ProcessLeaf(FDungeonLeaf* Leaf, int32 Iteration);
	bool SplitLeaf(FDungeonLeaf* Leaf, bool SplitHorizontal);

	//Rooms
	void CreateRoom(const FDungeonLeaf* Leaf);
	void OpenRooms();
	void OpenRoom(FDungeonRoom& Room);
	bool CheckSill(int32 Row, int32 Col, const FIntPoint& CheckDirection);
	void CheckRooms();
	bool CheckRoom(FDungeonRoom& Room);

	//Corridor
	void CreateCorridors();
	void CreateCorridor(int32 I, int32 J, int32 LastDirection);
	bool CheckCorridor(int32 CorridorX, int32 CorridorY, const FIntPoint& Direction);
	void RemoveDeadEnds();
	void CollapseCorridor(int32 I, int32 J);

	//Printing
	void PrintDungeon();

	//Markers temp
	void PlaceFloorMarkers();

	FORCEINLINE const int32 GetRandomDirectionIndex();
protected:
	static const FIntPoint DirectionUp;
	static const FIntPoint DirectionRight;
	static const FIntPoint DirectionDown;
	static const FIntPoint DirectionLeft;
	static const int32 NumDirections;
	static const FIntPoint AllDirections[4];

	FDungeonLeaf* RootLeaf;

	UPROPERTY(Transient)
	int32 HalfDungeonWidth;
	UPROPERTY(Transient)
	int32 HalfDungeonHeight;

	UPROPERTY(Transient)
	int32 HalfMinRoomSize;
	UPROPERTY(Transient)
	int32 HalfMaxRoomSize;

	UPROPERTY(Transient)
	int32 LastRow;
	UPROPERTY(Transient)
	int32 LastCol;
	
};
