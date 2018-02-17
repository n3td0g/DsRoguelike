// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGenerator2.h"


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
	HalfDungeonWidth = FMath::FloorToInt(DungeonWidth * 0.5f);
	HalfDungeonHeight = FMath::FloorToInt(DungeonHeight * 0.5f);
}

void ADungeonGenerator2::GenerateDungeon()
{

}

void ADungeonGenerator2::CleanupDungeon()
{
	if (DungeonData) {
		for (int32 Row = 0; Row < DungeonWidth; ++Row) {
			delete[] DungeonData[Row];
		}
		delete[] DungeonData;
	}
}

// Called every frame
void ADungeonGenerator2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

