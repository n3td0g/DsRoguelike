// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TraceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DSROGUELIKE_API UTraceComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UTraceComponent();
	
};
