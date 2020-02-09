#pragma once
#include "CoreMinimal.h"

#include "MeleeDamageEvent.generated.h"

USTRUCT()
struct DSROGUELIKE_API FMeleeDamageEvent : public FPointDamageEvent
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	float StaminaDamage;

	UPROPERTY()
	float PoiseDamage;

	FMeleeDamageEvent() : FPointDamageEvent()
	{
	}
	FMeleeDamageEvent(float InDamage, float InStaminaDamage, float InPoiseDamage, struct FHitResult const& InHitInfo,
		FVector const& InShotDirection, TSubclassOf<class UDamageType> InDamageTypeClass)
		: FPointDamageEvent(InDamage, InHitInfo, InShotDirection, InDamageTypeClass)
	{
		StaminaDamage = InStaminaDamage;
		PoiseDamage = InPoiseDamage;
	}

	virtual int32 GetTypeID() const override
	{
		return FMeleeDamageEvent::ClassID;
	};
	virtual bool IsOfType(int32 InID) const override
	{
		return (FMeleeDamageEvent::ClassID == InID) || FDamageEvent::IsOfType(InID);
	};

	/** Simple API for common cases where we are happy to assume a single hit is expected, even though damage event may have
	 * multiple hits. */
	virtual void GetBestHitInfo(
		AActor const* HitActor, AActor const* HitInstigator, struct FHitResult& OutHitInfo, FVector& OutImpulseDir) const override
	{
		// assume the actor got hit in the center of his root component
		OutHitInfo = HitInfo;
		OutImpulseDir = ShotDirection;
	};
};