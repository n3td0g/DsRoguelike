// Fill out your copyright notice in the Description page of Project Settings.

#include "MarkerEmitter.h"
#include "Dungeon.h"
#include "DungeonBuilder.h"

void UMarkerEmitter::EmitMarkers(UDungeonBuilder* DungeonBuilder, ADungeon* Dungeon)
{
	ReceiveEmitMarkers(DungeonBuilder, Dungeon);
}
