// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "GraphSchema_DungeonTemplate.generated.h"

enum class EVisualNodeType : uint8;

USTRUCT()
struct  FCustomSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_BODY()
	
	class UEdGraphNode* ActorNode;

	FCustomSchemaAction_NewNode()
		: FEdGraphSchemaAction()
	{}

	FCustomSchemaAction_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
	{}

	// FEdGraphSchemaAction interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	// End of FEdGraphSchemaAction interface

	virtual UEdGraphNode* SpawnNode(UEdGraph* ParentGraph) { return nullptr; }
};

USTRUCT()
struct FCustomSchemaAction_NewMarkerNode : public FCustomSchemaAction_NewNode
{
	GENERATED_BODY()
	
	FCustomSchemaAction_NewMarkerNode()
		: FCustomSchemaAction_NewNode()
	{}

	FCustomSchemaAction_NewMarkerNode(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping)
		: FCustomSchemaAction_NewNode(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
	{}
	
	virtual UEdGraphNode* SpawnNode(UEdGraph* ParentGraph) override;
};

USTRUCT()
struct FCustomSchemaAction_NewVisualNode : public FCustomSchemaAction_NewNode
{
	GENERATED_BODY()
	
	EVisualNodeType NodeType;
	
	FCustomSchemaAction_NewVisualNode()
		: FCustomSchemaAction_NewNode()
	{}

	FCustomSchemaAction_NewVisualNode(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping)
		: FCustomSchemaAction_NewNode(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
	{}

	virtual UEdGraphNode* SpawnNode(UEdGraph* ParentGraph) override;
};

USTRUCT()
struct FCustomSchemaAction_NewMarkerEmitterNode : public FCustomSchemaAction_NewNode
{
	GENERATED_BODY()
	
	FCustomSchemaAction_NewMarkerEmitterNode()
		: FCustomSchemaAction_NewNode()
	{}

	FCustomSchemaAction_NewMarkerEmitterNode(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping)
		: FCustomSchemaAction_NewNode(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
	{}
	
	class UMarkerNode* MarkerNode;

	virtual UEdGraphNode* SpawnNode(UEdGraph* ParentGraph) override;
};

/**
 * 
 */
UCLASS()
class DSROGUELIKEEDITOR_API UGraphSchema_DungeonTemplate : public UEdGraphSchema
{
	GENERATED_BODY()
protected:
	// Begin EdGraphSchema interface
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, FMenuBuilder* MenuBuilder, bool bIsDebugging) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual bool ShouldHidePinDefaultValue(UEdGraphPin* Pin) const override;
	// End EdGraphSchema interface

	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
};
