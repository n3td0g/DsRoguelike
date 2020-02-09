#pragma once

#include "ActionType.h"

#include "PlayerAction.generated.h"

USTRUCT(BlueprintType)
struct FPlayerAction
{
	GENERATED_BODY()

public:
	FPlayerAction() : ExecuteTime(0.0f), ActionType(EActionType::AT_Idle)
	{
	}
	FPlayerAction(float Time, EActionType Type) : ExecuteTime(Time), ActionType(Type)
	{
	}

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float ExecuteTime;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EActionType ActionType = EActionType::AT_Idle;
};