#include "AssetEditor_DungeonGraph.h"
#include "Framework/Docking/TabManager.h"

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

}

TSharedRef<SDockTab> FAssetEditor_DungeonGraph::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(FText::FromString("Dungeon Graph"))
		.TabColorScale(GetTabColorScale());

	/*

		[
			GraphEditor.ToSharedRef()
		]
	*/
}

