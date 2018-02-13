#pragma once

#include "DungeonRoomData.generated.h"

#define NOTHING		0x00000000

#define BLOCKED		0x00000001
#define ROOM		0x00000002
#define CORRIDOR	0x00000004
#define PERIMETER	0x00000008
#define ENTRANCE	0x00000010
#define DOOR		0x00000020
#define LOCKED		0x00000040
#define WALL		0x00000080

#define UP			0x00000100
#define RIGHT		0x00000200
#define DOWN		0x00000400
#define LEFT		0x00000800

#define VISITED		0x00001000

#define INDEX		0xFFFF0000

#define ALL_DIRECTIONS	(UP | RIGHT | DOWN | LEFT)

#define UP_RIGHT_DOWN	(UP | RIGHT | DOWN)
#define UP_DOWN_LEFT	(UP | DOWN | LEFT)
#define UP_RIGHT_LEFT	(UP | RIGHT | LEFT)
#define RIGHT_DOWN_LEFT	(RIGHT | DOWN | LEFT)

#define RIGHT_LEFT		(RIGHT | LEFT)
#define UP_DOWN			(UP | DOWN)
#define UP_RIGHT		(UP | RIGHT)
#define UP_LEFT			(UP | LEFT)
#define DOWN_RIGHT		(DOWN | RIGHT)
#define DOWN_LEFT		(DOWN | LEFT)

#define OPENSPACE	(ROOM | CORRIDOR)

#define BLOCK_ROOM	(BLOCKED | ROOM)
#define BLOCK_CORR	(BLOCKED | PERIMETER | CORRIDOR | ROOM)
#define BLOCK_DOOR	(BLOCKED | LOCKED | ENTRANCE)
#define BLOCK_ENTRANCE	(PERIMETER | BLOCKED | ENTRANCE | ROOM)
#define ROOM_INSIDE	(PERIMETER | ENTRANCE | ROOM | WALL)
#define CORRIDOR_ENTRANCE	(CORRIDOR | ENTRANCE)

USTRUCT(BlueprintType)
struct FIVector
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	int32 X;

	UPROPERTY(BlueprintReadOnly)
	int32 Y;

	FIVector(): X(0), Y(0) {}

	FIVector(int32 NewX, int32 NewY) : X(NewX), Y(NewY) {}
};

USTRUCT(BlueprintType)
struct FDoorInfo 
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FIVector Location;

	UPROPERTY(BlueprintReadOnly)
	FIVector Direction;
};

USTRUCT(BlueprintType)
struct FDungeonRoomData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	int32 Index;

	UPROPERTY(BlueprintReadOnly)
	FIVector Position;

	UPROPERTY(BlueprintReadOnly)
	FIVector Size;

	UPROPERTY(BlueprintReadOnly)
	TArray<FDoorInfo> Doors;

	UPROPERTY(BlueprintReadOnly)
	TMap<int32, int32> ConnectedRooms;

	FDungeonRoomData(int32 RoomX, int32 RoomY, int32 RoomWidth, int32 RoomHheight) :
		Position(RoomX, RoomY), Size(RoomWidth, RoomHheight) {}

	FDungeonRoomData() {}
};
