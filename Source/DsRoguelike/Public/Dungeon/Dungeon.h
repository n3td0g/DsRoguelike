// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonBuilderConfig.h"
#include "Dungeon.generated.h"

class UDungeonBuilder;
class UMarkerEmitter;

UCLASS()
class DSROGUELIKE_API ADungeon : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ADungeon();

public:
	UFUNCTION(BlueprintCallable)
	void CreateDungeon();

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dungeon Config")
	FDungeonBuilderConfig BuilderConfig;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void CreateDungeonVisual();

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dungeon")
	TSubclassOf<UDungeonBuilder> BuilderClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dungeon")
	TArray<class UDungeonTemplate*> Templates;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dungeon")
	TArray<TSubclassOf<UMarkerEmitter>> MarkerEmitterClasses;

	UPROPERTY(BlueprintReadOnly)
	UDungeonBuilder* Builder;

	UPROPERTY(BlueprintReadOnly)
	TArray<UMarkerEmitter*> MarkerEmitters;
};
