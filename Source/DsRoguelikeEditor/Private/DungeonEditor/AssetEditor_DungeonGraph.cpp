#include "AssetEditor_DungeonGraph.h"
#include "IDetailsView.h"
#include "GenericCommands.h"
#include "GraphEditor.h"
#include "PropertyEditorModule.h"
#include "EdGraph_DungeonTemplate.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "Framework/Docking/TabManager.h"
#include "GraphSchema_DungeonTemplate.h"

#include "MarkerGraphNode.h"
#include "VisualGraphNode.h"
#include "EmitterGraphNode.h"

#include "MarkerNode.h"
#include "VisualNode.h"

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

	FGenericCommands::Register();

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

void FAssetEditor_DungeonGraph::DeleteSelectedNodes()
{
	TArray<UEdGraphNode*> NodesToDelete;
	TSharedPtr<SGraphEditor> FocusedGraphEd = ViewportWidget;

	if (FocusedGraphEd.IsValid())
	{
		FocusedGraphEd->GetCurrentGraph()->Modify();

		const FGraphPanelSelectionSet SelectedNodes = FocusedGraphEd->GetSelectedNodes();
		FocusedGraphEd->ClearSelectionSet();

		for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
		{
			UEdGraphNode* EdNode = Cast<UEdGraphNode>(*NodeIt);
			if (EdNode == nullptr || !EdNode->CanUserDeleteNode())
				continue;			

			auto MarkerNode = Cast<UMarkerGraphNode>(*NodeIt);
			if (MarkerNode) {
				DeleteMarkerNode(MarkerNode);
				continue;
			}

			DeleteNode(EdNode);
		}
	}
}

bool FAssetEditor_DungeonGraph::CanDeleteNodes()
{
	return true;
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

	TabManager->RegisterTabSpawner(FDungeonTemplateAssetEditorTabs::DungeonTemplatePropertyID, FOnSpawnTab::CreateSP(this, &FAssetEditor_DungeonGraph::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("DetailsTab", "Property"))
		.SetGroup(WorkspaceMenuCategory)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
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
	/*if (DungeonTemplate != nullptr)
	{
		RebuildGenericGraph();
	}*/

	FAssetEditorToolkit::SaveAsset_Execute();
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

TSharedRef<SDockTab> FAssetEditor_DungeonGraph::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FDungeonTemplateAssetEditorTabs::DungeonTemplatePropertyID);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("Details_Title", "Property"))
		[
			PropertyWidget.ToSharedRef()
		];
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

	CreateCommandList();

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FAssetEditor_DungeonGraph::OnSelectedNodesChanged);
	//InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FAssetEditor_GenericGraph::OnNodeDoubleClicked);

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
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

void FAssetEditor_DungeonGraph::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}

	GraphEditorCommands = MakeShareable(new FUICommandList);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FAssetEditor_DungeonGraph::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_DungeonGraph::CanDeleteNodes)
	);
}

void FAssetEditor_DungeonGraph::DeleteMarkerNode(class UMarkerGraphNode* MarkerNode)
{
	if (MarkerNode->MarkerNode) {
		if (DungeonTemplate) {
			if (DungeonTemplate->MarkerNodes.Contains(MarkerNode->MarkerNode->MarkerName)) {
				if (DungeonTemplate->EdGraph) {
					TArray<UEmitterGraphNode*> EmitterNodes;
					DungeonTemplate->EdGraph->GetNodesOfClass<UEmitterGraphNode>(EmitterNodes);
					for (auto EmitterNode : EmitterNodes) {
						if (EmitterNode->MarkerNode == MarkerNode->MarkerNode) {
							DeleteNode(EmitterNode);
						}
					}
				}

				DungeonTemplate->Modify();
				DungeonTemplate->MarkerNodes.Remove(MarkerNode->MarkerNode->MarkerName);
			}
		}
	}
	DeleteNode(MarkerNode);
}

void FAssetEditor_DungeonGraph::DeleteNode(class UEdGraphNode* Node)
{
	Node->Modify();

	const UEdGraphSchema* Schema = Node->GetSchema();
	if (Schema != nullptr)
	{
		Schema->BreakNodeLinks(*Node);
	}

	FBlueprintEditorUtils::RemoveNode(NULL, Node, true);
}

void FAssetEditor_DungeonGraph::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	for (UObject* SelectionEntry : NewSelection)
	{
		Selection.Add(SelectionEntry);
	}

	if (Selection.Num() == 0)
	{
		PropertyWidget->SetObject(DungeonTemplate);
	}
	else
	{
		PropertyWidget->SetObjects(Selection);
	}
}

void FAssetEditor_DungeonGraph::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (DungeonTemplate == nullptr)
		return;

	DungeonTemplate->EdGraph->GetSchema()->ForceVisualizationCacheClear();
}

