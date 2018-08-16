// Fill out your copyright notice in the Description page of Project Settings.

#include "SprintAction.h"
#include "GameFramework/CharacterMovementComponent.h"



bool USprintAction::StartAction()
{
	if (!MovementComponent) {
		auto Owner = GetOwner();
		if (Owner) {
			MovementComponent = Owner->FindComponentByClass<UCharacterMovementComponent>();
		}
	}

	if (MovementComponent) {

		return true;
	}

	return false;
}

bool USprintAction::ContinueAction()
{
	return false;
}

bool USprintAction::StopAction(bool Force)
{
	return false;
}

void USprintAction::ActionTick(float DeltaTime)
{

}
