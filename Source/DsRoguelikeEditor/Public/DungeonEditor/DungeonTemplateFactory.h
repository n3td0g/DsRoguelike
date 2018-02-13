// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnrealEd.h"

#include "DungeonTemplateFactory.generated.h"

UCLASS()
class UDungeonTemplateFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
public:

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
