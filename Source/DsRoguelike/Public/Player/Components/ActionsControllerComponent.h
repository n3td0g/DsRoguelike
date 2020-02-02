#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/ActionType.h"
#include "Player/PlayerAction.h"
#include "ActionsControllerComponent.generated.h"

class UBaseAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DSROGUELIKE_API UActionsControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UActionsControllerComponent();

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void StartAction(EActionType Type);

	UFUNCTION(BlueprintCallable)
	void StopAction();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(BlueprintReadOnly)
	TMap<EActionType, UBaseAction*> Actions;
	
private:
	UPROPERTY(Transient)
	TArray<FPlayerAction> ActionsMemory;

	UPROPERTY(Transient)
	UBaseAction* CurrentAction;
};
