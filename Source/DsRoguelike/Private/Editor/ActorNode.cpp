// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorNode.h"
#include "Engine/World.h"
#include "UObject/Package.h"
#include "Engine/Engine.h"

#pragma optimize("", off)

void UActorNode::Process(const FTransform& MarkerTransform, UWorld* World)
{
	if (ActorClass) {
		FTransform WorldTransform = MarkerTransform;
		WorldTransform.Accumulate(Transform);

		if (ActorTemplate && ActorTemplate->GetClass() == ActorClass)
		{
			SpawnParameters.Template = ActorTemplate;
		}
		auto Actor = World->SpawnActor<AActor>(ActorClass, WorldTransform, SpawnParameters);
	}
}

void UActorNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UActorNode, ActorClass))
	{
		SetChildActorClass(ActorClass);
		/*if (IsTemplate())
		{
			SetChildActorClass(ActorClass);
		}
		else
		{
			UActorNode* Archetype = CastChecked<UActorNode>(GetArchetype());
			ActorTemplate = (Archetype->ActorClass == ActorClass ? Archetype->ActorTemplate : nullptr);
		}*/
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UActorNode::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UActorNode, ActorClass))
	{
		SetChildActorClass(ActorClass);
		/*if (IsTemplate())
		{
			SetChildActorClass(ActorClass);
		}
		else
		{
			ActorTemplate = CastChecked<UActorNode>(GetArchetype())->ActorTemplate;
		}*/
	}

	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

UObject* UActorNode::GetObject()
{
	if (ActorTemplate) {
		return ActorTemplate;
	}
	return Super::GetObject();
}

void UActorNode::SetChildActorClass(TSubclassOf<AActor> InClass)
{
	ActorClass = InClass;

	if (ActorClass)
	{
		if (ActorTemplate == nullptr || (ActorTemplate->GetClass() != ActorClass))
		{
			Modify();

			AActor* NewActorTemplate = NewObject<AActor>(GetTransientPackage(), ActorClass, NAME_None, RF_ArchetypeObject | RF_Transactional | RF_Public);

			if (ActorTemplate)
			{
				UEngine::CopyPropertiesForUnrelatedObjects(ActorTemplate, NewActorTemplate);
				ActorTemplate->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors);
			}

			ActorTemplate = NewActorTemplate;
			ActorTemplate->Modify();

			// Now set the actual name and outer to the BPGC.
			const FString TemplateName = FString::Printf(TEXT("%s_%s_CAT"), *GetName(), *ActorClass->GetName());

			ActorTemplate->Rename(*TemplateName, this, REN_DoNotDirty | REN_DontCreateRedirectors | REN_ForceNoResetLoaders);
		}
	}
	else if (ActorTemplate)
	{
		Modify();

		ActorTemplate->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors);
		ActorTemplate = nullptr;
	}
}


#pragma optimize("", on)