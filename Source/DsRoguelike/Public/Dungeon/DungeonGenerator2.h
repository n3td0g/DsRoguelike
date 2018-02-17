// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGenerator2.generated.h"

UCLASS()
class DSROGUELIKE_API ADungeonGenerator2 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonGenerator2();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void GenerateDungeon();
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options", meta = (ClampMin = 10))
	int32 DungeonWidth = 50;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Options", meta = (ClampMin = 10))
	int32 DungeonHeight = 50;

private:
	UFUNCTION()
	void CleanupDungeon();

private:
	UPROPERTY(Transient)
	int32 HalfDungeonWidth;
	UPROPERTY(Transient)
	int32 HalfDungeonHeight;

	int32** DungeonData;	
};
