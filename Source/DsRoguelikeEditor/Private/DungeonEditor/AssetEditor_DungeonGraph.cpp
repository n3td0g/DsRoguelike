#include "AssetEditor_DungeonGraph.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "EdGraph_DungeonTemplate.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "Framework/Docking/TabManager.h"
#include "GraphSchema_DungeonTemplate.h"

#define LOCTEXT_NAMESPACE "FAssetEditor_DungeonGraph"

const FName DungeonTemplateEditorAppName = FName(TEXT("DungeonTemplateEditorApp"));

//////////////////////////////////////////////////////////////////////////

struct FDungeonTemplateAssetEditorTabs
{
	// Tab identifiers
	static const FName DungeonTemplatePropertyID;
	static const FName ViewportID;
	static const FName DungeonTemplateEditorSettingsID;
};

//////////////////////////////////////////////////////////////////////////

const FName FDungeonTemplateAssetEditorTabs::DungeonTemplatePropertyID(TEXT("DungeonTemplateProperty"));
const FName FDungeonTemplateAssetEditorTabs::ViewportID(TEXT("Viewport"));
const FName FDungeonTemplateAssetEditorTabs::DungeonTemplateEditorSettingsID(TEXT("DungeonTemplateEditorSettings"));

FAssetEditor_DungeonGraph::FAssetEditor_DungeonGraph()
{
	DungeonTemplate = nullptr;
}

FAssetEditor_DungeonGraph::~FAssetEditor_DungeonGraph()
{

}

void FAssetEditor_DungeonGraph::InitDungeonGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UDungeonTemplate* Graph)
{
	DungeonTemplate = Graph;

	CreateEdGraph();
	CreateInternalWidgets();

	// Layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("DungeonGraph_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.2f)
				->Split
				(

					FTabManager::NewStack()
					->SetSizeCoefficient(0.8f)
					->SetHideTabWell(true)
					->AddTab(FDungeonTemplateAssetEditorTabs::ViewportID, ETabState::OpenedTab)

				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->SetHideTabWell(true)
					->AddTab(FDungeonTemplateAssetEditorTabs::DungeonTemplatePropertyID, ETabState::OpenedTab)
				)


			)

		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, DungeonTemplateEditorAppName, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, Graph, false);

	RegenerateMenusAndToolbars();
}

void FAssetEditor_DungeonGraph::RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager)
{
	auto WorkspaceMenuCategory = TabManager->AddLocalWorkspaceMenuCategory(FText::FromString("Custom Editor"));
	//auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(TabManager);

	TabManager->RegisterTabSpawner(FDungeonTemplateAssetEditorTabs::ViewportID, FOnSpawnTab::CreateSP(this, &FAssetEditor_DungeonGraph::SpawnTab_Viewport))
		.SetDisplayName(FText::FromString("Viewport"))
		.SetGroup(WorkspaceMenuCategory)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));
}

void FAssetEditor_DungeonGraph::UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(TabManager);

	TabManager->UnregisterTabSpawner(FDungeonTemplateAssetEditorTabs::ViewportID);
	TabManager->UnregisterTabSpawner(FDungeonTemplateAssetEditorTabs::DungeonTemplatePropertyID);
	TabManager->UnregisterTabSpawner(FDungeonTemplateAssetEditorTabs::DungeonTemplateEditorSettingsID);
}

FName FAssetEditor_DungeonGraph::GetToolkitFName() const
{
	return FName("FDungeonTemplateEditor");
}

FText FAssetEditor_DungeonGraph::GetBaseToolkitName() const
{
	return FText::FromString("Dungeon Template Editor");
}

FText FAssetEditor_DungeonGraph::GetToolkitName() const
{
	const bool bDirtyState = DungeonTemplate->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("DungeonTemplateName"), FText::FromString(DungeonTemplate->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(FText::FromString("{DungeonTemplateName}{DirtyState}"), Args);
}

FText FAssetEditor_DungeonGraph::GetToolkitToolTipText() const
{
	return FAssetEditorToolkit::GetToolTipTextForObject(DungeonTemplate);
}

FLinearColor FAssetEditor_DungeonGraph::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FAssetEditor_DungeonGraph::GetWorldCentricTabPrefix() const
{
	return TEXT("DungeonTemplateEditor");
}

FString FAssetEditor_DungeonGraph::GetDocumentationLink() const
{
	return TEXT("");
}

void FAssetEditor_DungeonGraph::SaveAsset_Execute()
{

}

void FAssetEditor_DungeonGraph::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(DungeonTemplate);
}

TSharedRef<SDockTab> FAssetEditor_DungeonGraph::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FDungeonTemplateAssetEditorTabs::ViewportID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("ViewportTab_Title", "Viewport"));

	if (ViewportWidget.IsValid())
	{
		SpawnedTab->SetContent(ViewportWidget.ToSharedRef());
	}

	return SpawnedTab;
}

void FAssetEditor_DungeonGraph::CreateInternalWidgets()
{
	ViewportWidget = CreateViewportWidget();

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.NotifyHook = this;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyWidget = PropertyModule.CreateDetailView(Args);
	PropertyWidget->SetObject(DungeonTemplate);
	//PropertyWidget->OnFinishedChangingProperties().AddSP(this, &FAssetEditor_GenericGraph::OnFinishedChangingProperties);

	EditorSettingsWidget = PropertyModule.CreateDetailView(Args);
	//EditorSettingsWidget->SetObject(GenricGraphEditorSettings);
}

TSharedRef<SGraphEditor> FAssetEditor_DungeonGraph::CreateViewportWidget()
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_DungeonTemplate", "Dungeon Template");

	//CreateCommandList();

	SGraphEditor::FGraphEditorEvents InEvents;
	//InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FAssetEditor_GenericGraph::OnSelectedNodesChanged);
	//InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FAssetEditor_GenericGraph::OnNodeDoubleClicked);

	return SNew(SGraphEditor)
		//.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(DungeonTemplate->EdGraph)
		.GraphEvents(InEvents);
		/*.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(false);*/
}

void FAssetEditor_DungeonGraph::CreateEdGraph()
{
	if (DungeonTemplate->EdGraph == nullptr)
	{
		DungeonTemplate->EdGraph = CastChecked<UEdGraph_DungeonTemplate>(FBlueprintEditorUtils::CreateNewGraph(DungeonTemplate, NAME_None, UEdGraph_DungeonTemplate::StaticClass(), UGraphSchema_DungeonTemplate::StaticClass()));
		DungeonTemplate->EdGraph->bAllowDeletion = false;

		// Give the schema a chance to fill out any required nodes (like the results node)
		const UEdGraphSchema* Schema = DungeonTemplate->EdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*DungeonTemplate->EdGraph);
	}

}

