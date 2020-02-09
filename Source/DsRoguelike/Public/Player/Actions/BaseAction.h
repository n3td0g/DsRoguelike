// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Player/ActionType.h"
#include "UObject/NoExportTypes.h"

#include "BaseAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionFinished);
/**
 *
 */
UCLASS(DefaultToInstanced, BlueprintType, abstract)
class DSROGUELIKE_API UBaseAction : public UActorComponent
{
	GENERATED_BODY()

public:
	UBaseAction();

	virtual bool StartAction()
	{
		return false;
	};

	virtual bool ContinueAction()
	{
		return false;
	};

	virtual bool StopAction(bool Force)
	{
		return true;
	};

	virtual void ActionTick(float DeltaTime){};

	UFUNCTION(BlueprintCallable)
	float GetStartTime()
	{
		return StartTime;
	}

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action|Options")
	EActionType Type = EActionType::AT_Idle;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action|Options")
	class UAnimMontage* AnimMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action|Options")
	int32 Priority;

	UPROPERTY(BlueprintAssignable, Category = "Action")
	FActionFinished OnActionFinished;

protected:
	UPROPERTY(BlueprintReadOnly)
	float StartTime;
};
