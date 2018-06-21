#pragma once

#include "ActionType.generated.h"

UENUM(BlueprintType)
enum class EActionType : uint8
{
	AT_Idle					UMETA(DisplayName = "Idle"),
	AT_AttackOnRun			UMETA(DisplayName = "AttackOnRun"),
	AT_Attack				UMETA(DisplayName = "Attack"),
	AT_Roll					UMETA(DisplayName = "Roll"),
	AT_Block				UMETA(DisplayName = "Block"),
	AT_Run					UMETA(DisplayName = "Run"),
	AT_Jump					UMETA(DisplayName = "Jump"),
	AT_Bounce				UMETA(DisplayName = "Bounce"),
	AT_Use					UMETA(DisplayName = "Use"),
	AT_Interact				UMETA(DisplayName = "Interact"),
	AT_BackstabAttack		UMETA(DisplayName = "BackstabAttack"),
	AT_Backstab				UMETA(DisplayName = "Backstab")
};