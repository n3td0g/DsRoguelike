// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Actions/BaseAction.h"
#include "SprintAction.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Actions), meta = (BlueprintSpawnableComponent))
class DSROGUELIKE_API USprintAction : public UBaseAction
{
	GENERATED_BODY()
	
public:

	virtual bool StartAction() override;

	virtual bool ContinueAction() override;

	virtual bool StopAction(bool Force) override;

	virtual void ActionTick(float DeltaTime) override;
	
protected:

private:
	UPROPERTY(Transient)
	class UCharacterMovementComponent* MovementComponent;
	
};
