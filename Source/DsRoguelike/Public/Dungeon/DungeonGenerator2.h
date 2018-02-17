// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGenerator2.generated.h"

#define DUNGEON_NOTHING		0x00000000
#define DUNGEON_BLOCKED		0x00000001

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

private:
	//Cleanup
	UFUNCTION()
	void CleanupDungeon();
	void CleanupLeaf(FDungeonLeaf* Leaf);

	//BSP
	void ProcessLeaf(FDungeonLeaf* Leaf, int32 Iteration);	

	//Printing
	UFUNCTION()
	void PrintDungeon();

private:
	UPROPERTY(Transient)
	int32 HalfDungeonWidth;
	UPROPERTY(Transient)
	int32 HalfDungeonHeight;

	UPROPERTY(Transient)
	int32 LastRow;
	UPROPERTY(Transient)
	int32 LastCol;

	FDungeonLeaf* RootLeaf = nullptr;

	int32** DungeonData;	
};
