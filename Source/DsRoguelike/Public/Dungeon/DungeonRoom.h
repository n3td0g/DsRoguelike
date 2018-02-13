// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonRoom.generated.h"

struct FDungeonRoomData;

UCLASS()
class DSROGUELIKE_API ADungeonRoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonRoom();

	virtual void InitializeRoom(const FDungeonRoomData& RoomData, float CellSize, int32** Dungeon);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UInstancedStaticMeshComponent* FloorInstancedStaticMesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UInstancedStaticMeshComponent* WallsInstancedStaticMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
