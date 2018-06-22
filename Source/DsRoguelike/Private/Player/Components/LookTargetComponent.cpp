// Fill out your copyright notice in the Description page of Project Settings.

#include "LookTargetComponent.h"
//#include "Kismet/KismetMathLibrary.h"

ULookTargetComponent::ULookTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
#if WITH_EDITOR
	bVisualizeComponent = true;
#endif
	bHiddenInGame = true;
}

void ULookTargetComponent::ActivateTarget(USceneComponent* Component)
{
	SetHiddenInGame(false, true);
	ComponentLookAt = Component;
}

void ULookTargetComponent::DeactivateTarget()
{
	SetHiddenInGame(true, true);
	ComponentLookAt = nullptr;
}

void ULookTargetComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ComponentLookAt) {
		/*const FVector& RightVector = ComponentLookAt->GetRightVector();
		const FVector& ForwardVector = ComponentLookAt->GetForwardVector();
		const FVector& UpVector = ComponentLookAt->GetUpVector();
		const FRotator& NewRotation = UKismetMathLibrary::MakeRotationFromAxes(RightVector, ForwardVector, UpVector);*/
		SetWorldRotation(ComponentLookAt->GetComponentRotation());

		float Distance = (GetComponentLocation() - ComponentLookAt->GetComponentLocation()).Size();
		float TargetScale = FMath::GetMappedRangeValueClamped(DistanceRage, ScaleRange, Distance);
		SetWorldScale3D(FVector(TargetScale));
	}
}
