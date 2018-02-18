// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGenerator2.generated.h"

#define DUNGEON_NOTHING		0x00000000
#define DUNGEON_BLOCKED		0x00000001
#define DUNGEON_ROOM		0x00000002
#define DUNGEON_CORRIDOR	0x00000004

#define ROOM_PERIMETER		0x00000008
#define ROOM_ENTRANCE		0x00000010
#define ROOM_DOOR			0x00000020

#define DOOR_BLOCKED		(ROOM_DOOR | DUNGEON_BLOCKED)

struct FDungeonLeaf;

USTRUCT(BlueprintType)
struct FPoint2D
{
	GENERATED_BODY()	

	UPROPERTY(BlueprintReadOnly)
	int32 X;

	UPROPERTY(BlueprintReadOnly)
	int32 Y;

	FPoint2D() : X(0), Y(0) {}

	FPoint2D(int32 NewX, int32 NewY) : X(NewX), Y(NewY) {}

	static const FPoint2D Up;
	static const FPoint2D Down;
	static const FPoint2D Left;
	static const FPoint2D Right;
};

USTRUCT(BlueprintType)
struct FRoomDungeon
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FPoint2D Size;

	UPROPERTY(BlueprintReadOnly)
	FPoint2D Position;

	FRoomDungeon(int32 RoomX, int32 RoomY, int32 RoomWidth, int32 RoomHheight) :
		Position(RoomX, RoomY), Size(RoomWidth, RoomHheight) {}

	FRoomDungeon() {}
};

UCLASS()
class DSROGUELIKE_API ADungeonGenerator2 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonGenerator2();

public:
	UFUNCTION(BlueprintCallable)
	void GenerateDungeon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void Initialize();

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options", meta = (ClampMin = 10))
	int32 DungeonWidth = 50;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options", meta = (ClampMin = 10))
	int32 DungeonHeight = 50;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options", meta = (ClampMin = 1))
	int32 Iterations = 4;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options")
	float MinLeafRatio = 0.5f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options|Rooms", meta = (ClampMin = 1))
	int32 MinRoomSize = 4;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options|Rooms", meta = (ClampMin = 1))
	int32 MaxRoomSize = 8;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options|Rooms")
	float MinRoomRatio = 0.6f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options|Rooms", meta = (ClampMin = 1))
	int32 MaxDoorsCount = 4;

	UPROPERTY(BlueprintReadOnly)
	TArray<FRoomDungeon> Rooms;
private:
	//Cleanup
	UFUNCTION()
	void CleanupDungeon();
	void CleanupLeaf(FDungeonLeaf* Leaf);

	//BSP
	void ProcessLeaf(FDungeonLeaf* Leaf, int32 Iteration);
	bool SplitLeaf(FDungeonLeaf* Leaf, bool SplitHorizontal);

	//Rooms
	void CreateRoom(const FDungeonLeaf* Leaf);
	void OpenRooms();
	void OpenRoom(FRoomDungeon& Room);
	bool CheckSill(int32 Row, int32 Col, const FPoint2D CheckDirection);

	//Printing
	UFUNCTION()
	void PrintDungeon();

private:
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

	

	FDungeonLeaf* RootLeaf = nullptr;

	int32** DungeonData;	
};
