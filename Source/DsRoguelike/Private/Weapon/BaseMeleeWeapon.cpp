// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseMeleeWeapon.h"
#include "TraceComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABaseMeleeWeapon::ABaseMeleeWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseMeleeWeapon::StartAttack()
{
	PrevLocations.Empty();
	for (auto Component : ComponentsForTrace) {
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
	
	auto Components = GetComponentsByClass(UTraceComponent::StaticClass());
	for (auto Component : Components) {
		if (auto SceneComponent = Cast<USceneComponent>(Component)) {
			ComponentsForTrace.Push(SceneComponent);
		}		
	}
}

void ABaseMeleeWeapon::Attack()
{
	for (int32 I = 0; I < ComponentsForTrace.Num(); ++I) {
		auto Component = ComponentsForTrace[I];
		const FVector& EndLocation = Component->GetComponentLocation();
		const FVector& StartLocation = PrevLocations[I];

		TArray<FHitResult> HitResults;
		UKismetSystemLibrary::LineTraceMulti(GetWorld(), StartLocation, EndLocation, TraceType, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResults, true);
		
		for (const auto& Hit : HitResults) {
			if (!Hit.bBlockingHit) {
				ActorsToIgnore.Add(Hit.GetActor());
			}
		}

		PrevLocations[I] = EndLocation;
	}
}

