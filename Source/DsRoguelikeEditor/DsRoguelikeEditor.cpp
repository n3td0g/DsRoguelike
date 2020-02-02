// Fill out your copyright notice in the Description page of Project Settings.

#include "DsRoguelikeEditor.h"
#include "Modules/ModuleManager.h"
#include "DungeonEditor/DungeonTemplateAction.h"
#include "EdGraphUtilities.h"
#include "DungeonEditor/GraphPanelNodeFactory_DungeonTemplate.h"

IMPLEMENT_GAME_MODULE(FDsRoguelikeEditorModule, DsRoguelikeEditor);

DEFINE_LOG_CATEGORY(DsRoguelikeEditor)

#define LOCTEXT_NAMESPACE "DsRoguelikeEditor"

void FDsRoguelikeEditorModule::StartupModule()
{
	UE_LOG(DsRoguelikeEditor, Warning, TEXT("MyGameEditor: Log Started"));
//#if WITH_EDITORONLY_DATA
	//Registrate asset actions for DungeonTemplate
	FDungeonTemplateAction::RegistrateCustomPartAssetType();

	// Register custom graph nodes
	TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory = MakeShareable(new FGraphPanelNodeFactory_DungeonTemplate);
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory);
//#endif
}

void FDsRoguelikeEditorModule::ShutdownModule()
{
	UE_LOG(DsRoguelikeEditor, Warning, TEXT("MyGameEditor: Log Ended"));
}

#undef LOCTEXT_NAMESPACE