// Fill out your copyright notice in the Description page of Project Settings.

#include "Editor/ActorNode.h"
#include "Engine/World.h"
#include "UObject/Package.h"
#include "UObject/PropertyPortFlags.h"
#include "Engine/Engine.h"
#include "UObject/UObjectHash.h"

#pragma optimize("", off)

void UActorNode::Process(const FTransform& MarkerTransform, UWorld* World)
{
	if (ActorClass) {
		FTransform WorldTransform = Transform * MarkerTransform;

		if (ActorTemplate && ActorTemplate->GetClass() == ActorClass)
		{
			SpawnParameters.Template = ActorTemplate;
		}
		auto Actor = World->SpawnActor<AActor>(ActorClass, WorldTransform, SpawnParameters);
	}
}

#if WITH_EDITOR
void UActorNode::PostEditImport()
{
	Super::PostEditImport();

	TArray<UObject*> Children;
	GetObjectsWithOuter(this, Children, false);

	for (UObject* Child : Children)
	{
		if (Child->GetClass() == ActorClass)
		{
			ActorTemplate = CastChecked<AActor>(Child);
			break;
		}
	}
}

void UActorNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UActorNode, ActorClass)) {
			
			SetChildActorClass(ActorClass);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UActorNode::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property) {
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UActorNode, ActorClass)) {
			SetChildActorClass(ActorClass);
		}
	}

	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}
#endif

void UActorNode::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (Ar.HasAllPortFlags(PPF_DuplicateForPIE))
	{
		// PIE duplication should just work normally
		Ar << ActorTemplate;
	}
	else if (Ar.HasAllPortFlags(PPF_Duplicate))
	{
		if (GIsEditor && Ar.IsLoading() && !IsTemplate())
		{
			// If we're not a template then we do not want the duplicate so serialize manually and destroy the template that was created for us
			Ar.Serialize(&ActorTemplate, sizeof(UObject*));

			if (AActor* UnwantedDuplicate = static_cast<AActor*>(FindObjectWithOuter(this, AActor::StaticClass())))
			{
				UnwantedDuplicate->MarkPendingKill();
			}
		}
		else if (!GIsEditor && !Ar.IsLoading() && !GIsDuplicatingClassForReinstancing)
		{
			// Avoid the archiver in the duplicate writer case because we want to avoid the duplicate being created
			Ar.Serialize(&ActorTemplate, sizeof(UObject*));
		}
		else
		{
			// When we're loading outside of the editor we won't have created the duplicate, so its fine to just use the normal path
			// When we're loading a template then we want the duplicate, so it is fine to use normal archiver
			// When we're saving in the editor we'll create the duplicate, but on loading decide whether to take it or not
			Ar << ActorTemplate;
		}
	}

#if WITH_EDITOR
	if (ActorClass == nullptr)
	{
		// It is unknown how this state can come to be, so for now we'll simply correct the issue and record that it occurs and 
		// and if it is occurring frequently, then investigate how the state comes to pass
		if (!ensureAlwaysMsgf(ActorTemplate == nullptr, TEXT("Found unexpected ActorTemplate %s when ActorClass is null"), *ActorTemplate->GetFullName()))
		{
			ActorTemplate = nullptr;
		}
	}
	// Since we sometimes serialize properties in instead of using duplication and we can end up pointing at the wrong template
	else if (!Ar.IsPersistent() && ActorTemplate)
	{
		if (IsTemplate())
		{
			// If we are a template and are not pointing at a component we own we'll need to fix that
			if (ActorTemplate->GetOuter() != this)
			{
				const FString TemplateName = FString::Printf(TEXT("%s_%s_CAT"), *GetName(), *ActorClass->GetName());
				ActorTemplate = CastChecked<AActor>(StaticDuplicateObject(ActorTemplate, this, *TemplateName));
			}
		}
		else {
			//ActorTemplate = CastChecked<UActorNode>(GetArchetype())->ActorTemplate;
			SetChildActorClass(ActorClass);
		}
	}
#endif
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
			//const FString TemplateName = FString::Printf(TEXT("%s_%s_CAT"), *GetName(), *ActorClass->GetName());

			//ActorTemplate->Rename(*TemplateName, this, REN_DoNotDirty | REN_DontCreateRedirectors | REN_ForceNoResetLoaders);
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