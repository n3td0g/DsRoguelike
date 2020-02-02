// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonEditor/DungeonTemplateFactory.h"
#include "Editor/DungeonTemplate.h"

UDungeonTemplateFactory::UDungeonTemplateFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UDungeonTemplate::StaticClass();
}

UObject* UDungeonTemplateFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UDungeonTemplate* NewDungeonTemplate = NewObject<UDungeonTemplate>(InParent, Class, Name, Flags | RF_Transactional);

	return NewDungeonTemplate;
}
