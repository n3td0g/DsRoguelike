// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeon/Emitters/MarkerEmitter.h"

#include "Dungeon/Dungeon.h"
#include "Dungeon/DungeonBuilder.h"

void UMarkerEmitter::EmitMarkers(UDungeonBuilder* DungeonBuilder, ADungeon* Dungeon)
{
	ReceiveEmitMarkers(DungeonBuilder, Dungeon);
}
