// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "StatsComponent.generated.h"

UENUM(BlueprintType)
enum class EStatsParameterType : uint8
{
	SP_Health UMETA(DisplayName = "Health"),
	SP_Stamina UMETA(DisplayName = "Stamina"),
	SP_Poise UMETA(DisplayName = "Poise")
};

USTRUCT(BlueprintType)
struct FStatsParameter
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MaxValue = 100.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MinValue = -100.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Value = 100.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float RegenerationSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float RegenerationDelay = 1.0f;

	UPROPERTY(BlueprintReadOnly)
	float LastDamageTime = 0.0f;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DSROGUELIKE_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatsComponent();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure)
	void GetStatsParameter(EStatsParameterType Type, FStatsParameter& Parameter);

	UFUNCTION(BlueprintCallable)
	void ApplyDamage(EStatsParameterType Type, float Damage);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Stats)
	TMap<EStatsParameterType, FStatsParameter> StatsParameters;
};
