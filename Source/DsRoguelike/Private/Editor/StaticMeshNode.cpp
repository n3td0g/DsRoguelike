// Fill out your copyright notice in the Description page of Project Settings.

#include "Editor/StaticMeshNode.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

void UStaticMeshNode::Process(const FTransform& MarkerTransform, UWorld* World)
{
	if (!StaticMesh)
	{
		return;
	}

	FTransform WorldTransform = Transform * MarkerTransform;

	auto StaticMeshActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), WorldTransform, SpawnParameters);
	if (StaticMeshActor)
	{
		UStaticMeshComponent* StaticMeshComponent = StaticMeshActor->GetStaticMeshComponent();
		StaticMeshComponent->SetStaticMesh(StaticMesh);
	}
}

UObject* UStaticMeshNode::GetObject()
{
	if (StaticMesh)
	{
		return StaticMesh;
	}
	return Super::GetObject();
}
