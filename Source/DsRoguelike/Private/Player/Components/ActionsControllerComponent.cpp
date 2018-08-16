#include "ActionsControllerComponent.h"
#include "BaseAction.h"

UActionsControllerComponent::UActionsControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UActionsControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentAction != nullptr) {
		CurrentAction->ActionTick(DeltaTime);
	}
}

void UActionsControllerComponent::StartAction(EActionType Type)
{
	if (auto NewAction = *Actions.Find(Type)) {
		if (CurrentAction) {
			if (CurrentAction->Priority < NewAction->Priority) {

			}		
		}

		auto World = GetWorld();
		check(World);

		ActionsMemory.Add(FPlayerAction(World->TimeSeconds, Type));
	}	
}

void UActionsControllerComponent::StopAction()
{

}

void UActionsControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	check(GetOwner());

	auto ActionComponents = GetOwner()->GetComponentsByClass(UBaseAction::StaticClass());

	for (auto ActionClass : ActionComponents) {
		auto Action = Cast<UBaseAction>(ActionClass);
		if (!Actions.Contains(Action->Type)) {
			Actions.Add(Action->Type, Action);
		}		
	}
}
