#pragma once

#include "CoreMinimal.h"
#include "DungeonTemplate.h"

class FAssetEditor_DungeonGraph : public FAssetEditorToolkit, public FNotifyHook, public FGCObject
{
public:
	FAssetEditor_DungeonGraph();
	virtual ~FAssetEditor_DungeonGraph();

	void InitDungeonGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UDungeonTemplate* Graph);

	// Delegates for graph editor commands
	void DeleteSelectedNodes();
	bool CanDeleteNodes();

	// IToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	// End of IToolkit interface

	// FAssetEditorToolkit
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FString GetDocumentationLink() const override;
	virtual void SaveAsset_Execute() override;
	// End of FAssetEditorToolkit

	// FSerializableObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	// End of FSerializableObject interface

protected:
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);

	void CreateInternalWidgets();
	TSharedRef<SGraphEditor> CreateViewportWidget();

	void CreateEdGraph();
	void CreateCommandList();

	virtual void DeleteMarkerNode(class UMarkerGraphNode* MarkerNode);
	virtual void DeleteNode(class UEdGraphNode* Node);

	// Delegates for graph editor commands

	// editor event
	void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);
	void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);

private:
	UDungeonTemplate* DungeonTemplate;

	TSharedPtr<SGraphEditor> ViewportWidget;
	TSharedPtr<class IDetailsView> PropertyWidget;
	TSharedPtr<class IDetailsView> EditorSettingsWidget;

	TSharedPtr<FUICommandList> GraphEditorCommands;
};