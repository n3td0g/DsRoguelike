// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "Editor/VisualNode.h"
#include "ActorNode.generated.h"

/**
 * 
 */
UCLASS()
class DSROGUELIKE_API UActorNode : public UVisualNode
{
	GENERATED_BODY()
	
public:
	virtual void Process(const FTransform& MarkerTransform, UWorld* World) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

	AActor* GetActorTemplate() const { return ActorTemplate; }

public:
	TSubclassOf<AActor> GetActorClass() const { return ActorClass; }

protected:
	void SetChildActorClass(TSubclassOf<AActor> InClass);

protected:
	/** The class of Actor to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ActorNode, meta = (OnlyPlaceable, AllowPrivateAccess = "true", ForceRebuildProperty = "ActorTemplate"))
	TSubclassOf<AActor>	ActorClass;

	/** Property to point to the template child actor for details panel purposes */
	UPROPERTY(VisibleDefaultsOnly, DuplicateTransient, Category = ActorNode, meta = (ShowInnerProperties))
	AActor* ActorTemplate;
};
