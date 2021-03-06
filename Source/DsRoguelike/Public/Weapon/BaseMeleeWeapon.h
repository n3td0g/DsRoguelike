// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseMeleeWeapon.generated.h"

UCLASS()
class DSROGUELIKE_API ABaseMeleeWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseMeleeWeapon();

	UFUNCTION(BlueprintCallable)
	void StartAttack();

	UFUNCTION(BlueprintCallable)
	void StopAttack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void Attack();

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Trace)
	TEnumAsByte<ETraceTypeQuery> TraceType;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Trace)
	float AttackUpdateRate = 0.05f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Damage)
	float HealthDamage = 25.0f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Damage)
	float StaminaDamage = 30.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Damage)
	float PoiseDamage = 25.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Debug)
	bool bDebugDraw = false;

private:
	UPROPERTY(Transient)
	FTimerHandle AttackTimerHandle;

	UPROPERTY(Transient)
	TArray<USceneComponent*> ComponentsForTrace;

	UPROPERTY(Transient)
	TArray<FVector> PrevLocations;

	UPROPERTY(Transient)
	TArray<AActor*> ActorsToIgnore;	
};
