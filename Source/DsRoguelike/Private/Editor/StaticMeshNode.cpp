// Fill out your copyright notice in the Description page of Project Settings.

#include "StaticMeshNode.h"
#include "Engine/StaticMeshActor.h"

void UStaticMeshNode::Process(const FTransform& MarkerTransform, UWorld* World)
{
	if (!StaticMesh) {
		return;
	}

	FTransform WorldTransform = MarkerTransform;
	WorldTransform.Accumulate(Transform);

	auto StaticMeshActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), WorldTransform, SpawnParameters);
	if (StaticMeshActor) {
		auto StaticMeshComponent = StaticMeshActor->GetStaticMeshComponent();
		StaticMeshComponent->SetStaticMesh(StaticMesh);
	}
}

UObject* UStaticMeshNode::GetObject()
{
	if (StaticMesh) {
		return StaticMesh;
	}
	return Super::GetObject();
}
