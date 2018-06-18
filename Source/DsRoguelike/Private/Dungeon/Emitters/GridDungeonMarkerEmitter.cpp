// Fill out your copyright notice in the Description page of Project Settings.

#include "GridDungeonMarkerEmitter.h"
#include "Dungeon.h"
#include "GridDungeonBuilder.h"
#include "DungeonDirections.h"
#include "DungeonMarker.h"

void UGridDungeonMarkerEmitter::EmitMarkers(UDungeonBuilder* Builder, ADungeon* Dungeon)
{
	Super::EmitMarkers(Builder, Dungeon);
	this->DungeonBuilder = Cast<UGridDungeonBuilder>(Builder);

	if (!DungeonBuilder) {
		return;
	}
	if (!Dungeon) {
		return;
	}

	FDungeonMarker FloorMarker;
	FDungeonMarker WallMarker;
	FDungeonMarker DoorMarker;
	FDungeonMarker SeparatorMarker;
	FDungeonMarker WindowMarker;

	FloorMarker.Transform = FTransform::Identity;
	WallMarker.Transform = FTransform::Identity;
	DoorMarker.Transform = FTransform::Identity;
	SeparatorMarker.Transform = FTransform::Identity;

	const auto& BuilderConfig = Dungeon->BuilderConfig;

	auto& FloorMarkers = DungeonBuilder->Markers.Add(TEXT("Floor"));
	auto& WallMarkers = DungeonBuilder->Markers.Add(TEXT("Wall"));
	auto& EntranceMarkers = DungeonBuilder->Markers.Add(TEXT("Entrance"));
	auto& DoorMarkers = DungeonBuilder->Markers.Add(TEXT("Door"));
	auto& WallTorchMarkers = DungeonBuilder->Markers.Add(TEXT("WallTorch"));
	auto& FloorTorchMarkers = DungeonBuilder->Markers.Add(TEXT("FloorTorch"));
	auto& WindowMarkers = DungeonBuilder->Markers.Add(TEXT("Window"));
	auto& SeparatorMarkers = DungeonBuilder->Markers.Add(TEXT("Separator"));

	float HalfCellSize = BuilderConfig.CellSize * 0.5f;
	float HalfWallThickness = BuilderConfig.WallThickness * 0.5f;
	float SizeWithWall = HalfCellSize - HalfWallThickness;

	const FVector SeparatorOffsets[4] = {
		FVector(HalfCellSize, HalfCellSize, 0.0f),
		FVector(HalfCellSize, -HalfCellSize, 0.0f),
		FVector(-HalfCellSize, -HalfCellSize, 0.0f),
		FVector(-HalfCellSize, HalfCellSize, 0.0f) };

	for (int32 J = 0; J < BuilderConfig.DungeonHeight; ++J) {
		for (int32 I = 0; I < BuilderConfig.DungeonWidth; ++I) {
			const int32& Data = DungeonBuilder->GetCell(I, J);
			int32 CellType = Data & DUNGEON_FLOOR;
			if (CellType) {
				const FVector& CellCenter = FVector((I + 0.5f) * BuilderConfig.CellSize, (J + 0.5f) * BuilderConfig.CellSize, 0.0f);
				FloorMarker.Transform.SetLocation(CellCenter);
				FloorMarkers.Push(FloorMarker);
				FRotator Rotation = FRotator::ZeroRotator;
				int32 SeparatorIndex = 0;
				auto& TorchMarkers = Data & DUNGEON_ROOM ? FloorTorchMarkers : WallTorchMarkers;

				for (const auto& Direction : FDungeonDirections::AllDirections) {				

					int32 Row = I + Direction.X;
					int32 Col = J + Direction.Y;

					WallMarker.Transform.SetLocation(CellCenter + FVector(SizeWithWall * Direction.X, SizeWithWall * Direction.Y, 0.0f));
					WallMarker.Transform.SetRotation(Rotation.Quaternion());

					SeparatorMarker.Transform.SetLocation(CellCenter + SeparatorOffsets[SeparatorIndex]);

					if (DungeonBuilder->IsCoordsValid(Row, Col)) {
						const int32& Cell = DungeonBuilder->GetCell(Row, Col);
						if (!(Cell & CellType)) {
							bool IsRoomEntrance = (Data & ROOM_DOOR) && (Cell & ROOM_ENTRANCE);
							bool IsCorridorEntrance = (Data & ROOM_ENTRANCE) && (Cell & ROOM_DOOR);
							if (IsRoomEntrance || IsCorridorEntrance) {
								EntranceMarkers.Push(WallMarker);
								if (Data & DUNGEON_ROOM) {
									DoorMarker.Transform.SetLocation(CellCenter + FVector(HalfCellSize * Direction.X, HalfCellSize * Direction.Y, 0.0f));
									DoorMarker.Transform.SetRotation(Rotation.Quaternion());
									DoorMarkers.Push(DoorMarker);
								}
							}
							else {
								bool IsWindow = (Data & ROOM_WINDOW) && (Cell & ROOM_WINDOW);								
								if (IsWindow) {
									if (Data & DUNGEON_ROOM) {
										WindowMarker.Transform.SetLocation(CellCenter + FVector(HalfCellSize * Direction.X, HalfCellSize * Direction.Y, 0.0f));
										WindowMarker.Transform.SetRotation(Rotation.Quaternion());
										WindowMarkers.Push(WindowMarker);
									}									
								}
								else {
									WallMarkers.Push(WallMarker);
									TryToPlaceTorch(TorchMarkers, WallMarker.Transform);
								}								
							}
							PlaceSeparatorMarker(SeparatorMarker, I, J, SeparatorIndex, HalfWallThickness);
							SeparatorMarkers.Push(SeparatorMarker);
						}
					}
					else {
						PlaceSeparatorMarker(SeparatorMarker, I, J, SeparatorIndex, HalfWallThickness);
						SeparatorMarkers.Push(SeparatorMarker);
						WallMarkers.Push(WallMarker);
						TryToPlaceTorch(TorchMarkers, WallMarker.Transform);
					}

					Rotation.Yaw -= 90.0f;
					++SeparatorIndex;
				}
			}
		}
	}
}

void UGridDungeonMarkerEmitter::PlaceSeparatorMarker(FDungeonMarker& Marker, int32 I, int32 J, int32 DirectionIndex, const float& HalfWallThickness)
{
	int32 NextDirectionIndex = (DirectionIndex + 1) % FDungeonDirections::NumDirections;
	const FIntPoint& NextDirection = FDungeonDirections::AllDirections[NextDirectionIndex];
	const FIntPoint& CurrentDirection = FDungeonDirections::AllDirections[DirectionIndex];
	FIntPoint DiagonalDirection = NextDirection + CurrentDirection;

	const int32& CurrentCell = DungeonBuilder->GetCell(I, J);
	int32 CellType = CurrentCell & DUNGEON_FLOOR;

	FVector MarkerLocation = Marker.Transform.GetLocation();

	int32 NextRow = I + NextDirection.X;
	int32 NextCol = J + NextDirection.Y;

	int32 DiagonalRow = I + DiagonalDirection.X;
	int32 DiagonalCol = J + DiagonalDirection.Y;

	if (DungeonBuilder->IsCoordsValid(NextRow, NextCol)) {
		const int32& NextCell = DungeonBuilder->GetCell(NextRow, NextCol);
		if (NextCell & CellType) {
			if (DungeonBuilder->IsCoordsValid(DiagonalRow, DiagonalCol)) {
				const int32& DiagonalCell = DungeonBuilder->GetCell(DiagonalRow, DiagonalCol);
				if (!(DiagonalCell & CellType)) {
					MarkerLocation.X -= CurrentDirection.X * HalfWallThickness;
					MarkerLocation.Y -= CurrentDirection.Y * HalfWallThickness;
					Marker.Transform.SetLocation(MarkerLocation);
					return;
				}

				FIntPoint OffsetDirection = NextDirection - CurrentDirection;

				MarkerLocation.X += OffsetDirection.X * HalfWallThickness;
				MarkerLocation.Y += OffsetDirection.Y * HalfWallThickness;
				Marker.Transform.SetLocation(MarkerLocation);
				return;
			}
		}
	}

	MarkerLocation.X -= DiagonalDirection.X * HalfWallThickness;
	MarkerLocation.Y -= DiagonalDirection.Y * HalfWallThickness;
	Marker.Transform.SetLocation(MarkerLocation);
}

void UGridDungeonMarkerEmitter::TryToPlaceTorch(TArray<FDungeonMarker>& TorchMarkers, const FTransform& MarkerTransform)
{
	if (FMath::FRand() > ChanceToPlaceTorch) {
		return;
	}
	TorchMarkers.Add(FDungeonMarker(MarkerTransform));
}
