// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeon.h"
#include "DungeonTemplate.h"
#include "DungeonBuilder.h"
#include "MarkerNode.h"
#include "VisualNode.h"
#include "DungeonMarker.h"

// Sets default values
ADungeon::ADungeon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BuilderClass = UDungeonBuilder::StaticClass();

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
#ifdef WITH_EDITOR
	RootComponent->bVisualizeComponent = true;
#endif
}

// Called when the game starts or when spawned
void ADungeon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADungeon::CreateDungeonVisual()
{
	if (Templates.Num() == 0) {
		return;
	}
	auto Template = Templates[0];
	if (!Template) {
		return;
	}

	for (auto& Marker : Builder->Markers) {
		auto MarkerNodeRef = Template->MarkerNodes.Find(Marker.Key);
		if (MarkerNodeRef) {
			auto MarkerNode = (*MarkerNodeRef);
			if (MarkerNode) {
				for (const auto& DungeonMarker : Marker.Value) {
					for (auto VisualNode : MarkerNode->VisualNodes) {
						if (VisualNode) {
							VisualNode->Process(DungeonMarker.Transform, GWorld);
						}
					}
				}
			}
		}
	}
}

void ADungeon::CreateDungeon()
{
	if (!BuilderClass) {
		UE_LOG(LogTemp, Warning, TEXT("CreateDungeon: BuilderClass is null"));
	}

	Builder = BuilderClass->GetDefaultObject<UDungeonBuilder>();
	Builder->GenerateDungeon(this);
	Builder->PlaceMarkers();

	CreateDungeonVisual();
}
