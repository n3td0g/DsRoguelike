// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonBuilderConfig.h"
#include "Dungeon.generated.h"

class UDungeonBuilder;

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

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dungeon")
	TSubclassOf<UDungeonBuilder> BuilderClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Themes")
	TArray<class UDungeonTemplate*> Templates;

private:
	UPROPERTY(Transient)
	UDungeonBuilder* Builder;
	
};
