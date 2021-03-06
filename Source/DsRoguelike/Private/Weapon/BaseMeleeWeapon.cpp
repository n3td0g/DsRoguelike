// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/BaseMeleeWeapon.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Weapon/DamageType/MeleeDamageEvent.h"
#include "Weapon/DamageType/MeleeDamageType.h"
#include "Weapon/TraceComponent.h"

// Sets default values
ABaseMeleeWeapon::ABaseMeleeWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseMeleeWeapon::StartAttack()
{
	PrevLocations.Empty();
	for (auto Component : ComponentsForTrace)
	{
		PrevLocations.Push(Component->GetComponentLocation());
	}

	ActorsToIgnore.Empty();
	ActorsToIgnore.Add(GetOwner());

	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ABaseMeleeWeapon::Attack, AttackUpdateRate, true);
}

void ABaseMeleeWeapon::StopAttack()
{
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
}

// Called when the game starts or when spawned
void ABaseMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(UTraceComponent::StaticClass(), Components);

	for (auto Component : Components)
	{
		if (auto SceneComponent = Cast<USceneComponent>(Component))
		{
			ComponentsForTrace.Push(SceneComponent);
		}
	}
}

void ABaseMeleeWeapon::Attack()
{
	for (int32 I = 0; I < ComponentsForTrace.Num(); ++I)
	{
		auto Component = ComponentsForTrace[I];
		const FVector& EndLocation = Component->GetComponentLocation();
		const FVector& StartLocation = PrevLocations[I];

		TArray<FHitResult> HitResults;
		EDrawDebugTrace::Type DrawDebugTrace = bDebugDraw ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
		UKismetSystemLibrary::LineTraceMulti(
			GetWorld(), StartLocation, EndLocation, TraceType, false, ActorsToIgnore, DrawDebugTrace, HitResults, true);

		for (const auto& HitResult : HitResults)
		{
			auto ActorToHit = HitResult.GetActor();
			if (ActorsToIgnore.Contains(ActorToHit))
			{
				continue;
			}
			if (ActorToHit)
			{
				FVector HitSource = GetOwner() ? GetOwner()->GetActorLocation() : GetActorLocation();
				FVector HitDirection = ActorToHit->GetActorLocation() - HitSource;
				HitDirection.Normalize();
				ActorToHit->TakeDamage(HealthDamage,
					FMeleeDamageEvent(
						HealthDamage, StaminaDamage, PoiseDamage, HitResult, HitDirection, UMeleeDamageType::StaticClass()),
					nullptr, this);
			}
			ActorsToIgnore.Add(ActorToHit);
		}

		PrevLocations[I] = EndLocation;
	}
}
