// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h"
#include "LookTargetComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DSROGUELIKE_API ULookTargetComponent : public USceneComponent
{
	GENERATED_BODY()
	

public:
	ULookTargetComponent();

	UFUNCTION(BlueprintCallable)
	void ActivateTarget(USceneComponent* Component);
	
	UFUNCTION(BlueprintCallable)
	void DeactivateTarget();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Scale)
	FVector2D ScaleRange = FVector2D(0.01f, 1.0f);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Scale)
	FVector2D DistanceRage = FVector2D(10.0f, 1000.0f);
private:
	UPROPERTY(Transient)
	USceneComponent* ComponentLookAt;
};
