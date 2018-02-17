// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGenerator2.h"
#include "DungeonLeaf.h"

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
	Initialize();
	Super::BeginPlay();	
}

void ADungeonGenerator2::Initialize()
{
	CleanupDungeon();

	HalfDungeonWidth = FMath::FloorToInt(DungeonWidth * 0.5f);
	HalfDungeonHeight = FMath::FloorToInt(DungeonHeight * 0.5f);

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
	PrintDungeon();
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
				C = '+';
			}
			DungeonRow.Append(FString::Printf(TEXT("%c"), C));
			Index++;
		}
		UE_LOG(LogTemp, Log, TEXT("%s"), *DungeonRow);
	}
}

#pragma optimize("", on)