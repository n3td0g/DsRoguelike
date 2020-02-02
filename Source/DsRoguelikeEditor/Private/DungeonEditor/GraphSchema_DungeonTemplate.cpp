// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonEditor/GraphSchema_DungeonTemplate.h"
#include "ConnectionDrawingPolicy.h"
#include "GraphEditorActions.h"
#include "ToolMenus.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "DungeonEditor/MarkerNode/MarkerGraphNode.h"
#include "DungeonEditor/EdGraph_DungeonTemplate.h"
#include "Editor/DungeonTemplate.h"
#include "DungeonEditor/VisualNode/VisualGraphNode.h"
#include "Editor/VisualNode.h"
#include "DungeonEditor/EmitterNode/EmitterGraphNode.h"
#include "DungeonEditor/ConnectionDrawingPolicy_DungeonGraph.h"
#include "Framework/Commands/GenericCommands.h"
#include "Editor/MarkerNode.h"

UEdGraphNode* FCustomSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	auto Node = SpawnNode(ParentGraph);
	if (!Node) {
		return Node;
	}

	const FScopedTransaction Transaction(FText::FromString(TEXT("DungeonTemplate: New Node")));

	ParentGraph->Modify();
	ParentGraph->AddNode(Node, true, bSelectNewNode);

	Node->SetFlags(RF_Transactional);
	Node->CreateNewGuid();
	Node->PostPlacedNewNode();
	Node->AllocateDefaultPins();
	Node->NodePosX = Location.X;
	Node->NodePosY = Location.Y;
	Node->SnapToGrid(16);	

	ActorNode = Node;
	return Node;
}

void FCustomSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(ActorNode);
}

UEdGraphNode* FCustomSchemaAction_NewMarkerNode::SpawnNode(UEdGraph* ParentGraph)
{
	UMarkerGraphNode* MarkerNode = NewObject<UMarkerGraphNode>((UObject *)ParentGraph, UMarkerGraphNode::StaticClass());

	const UEdGraph_DungeonTemplate* Graph = Cast<UEdGraph_DungeonTemplate>(ParentGraph);
	if (!Graph) {
		return nullptr;
	}

	UDungeonTemplate* DungeonTemplate = Graph->GetDungeonTemplate();
	if (!DungeonTemplate) {
		return nullptr;
	}

	MarkerNode->MarkerNode = DungeonTemplate->CreateMarkerNode();

	return MarkerNode;
}

UEdGraphNode* FCustomSchemaAction_NewVisualNode::SpawnNode(UEdGraph* ParentGraph)
{
	UVisualGraphNode* VisualGraphNode = NewObject<UVisualGraphNode>((UObject *)ParentGraph, UVisualGraphNode::StaticClass());

	const UEdGraph_DungeonTemplate* Graph = Cast<UEdGraph_DungeonTemplate>(ParentGraph);
	if (!Graph) {
		return nullptr;
	}

	UDungeonTemplate* DungeonTemplate = Graph->GetDungeonTemplate();
	if (!DungeonTemplate) {
		return nullptr;
	}

	UVisualNode* VisualNode = NewObject<UVisualNode>(DungeonTemplate, UVisualNode::GetNodeClass(NodeType));	

	VisualGraphNode->VisualNode = VisualNode;

	return VisualGraphNode;
}

UEdGraphNode* FCustomSchemaAction_NewMarkerEmitterNode::SpawnNode(UEdGraph* ParentGraph)
{
	UEmitterGraphNode* EmitterNode = NewObject<UEmitterGraphNode>((UObject *)ParentGraph, UEmitterGraphNode::StaticClass());
	EmitterNode->MarkerNode = MarkerNode;
	return EmitterNode;
}

//-------------------------------GraphSchema--------------------------

void UGraphSchema_DungeonTemplate::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const UEdGraph_DungeonTemplate* Graph = Cast<UEdGraph_DungeonTemplate>(ContextMenuBuilder.CurrentGraph);
	if (!Graph) {
		return;
	}

	const UDungeonTemplate* DungeonTemplate = Graph->GetDungeonTemplate();
	if (!DungeonTemplate) {
		return;
	}

	FText MarkerNodeDescription = FText::FromString(TEXT("Add Marker Node"));
	FText MarkerNodeToolTip = FText::FromString(TEXT("ToolTip"));
	FText MerkerNodeCategory = FText::FromString(TEXT("Markers"));
	auto NewMarkerNodeAction = TSharedPtr<FCustomSchemaAction_NewMarkerNode>(new FCustomSchemaAction_NewMarkerNode(MerkerNodeCategory, MarkerNodeDescription, MarkerNodeToolTip, 0));
	ContextMenuBuilder.AddAction(NewMarkerNodeAction);
	
	const FString VisualNodeDescription = TEXT("Add ");
	FText VisualNodeToolTip = FText::FromString(TEXT("ToolTip"));
	FText VisualNodeCategory = FText::FromString(TEXT("Visual Nodes"));
	uint8 Count = (uint8)EVisualNodeType::NumEnums;
	for (uint8 I = 0; I < Count; ++I) {
		EVisualNodeType NodeType = (EVisualNodeType)(I);
		FString Description = VisualNodeDescription + UVisualNode::GetNodeName(NodeType);
		auto VisualNodeAction = new FCustomSchemaAction_NewVisualNode(VisualNodeCategory, FText::FromString(Description), VisualNodeToolTip, 1);
		auto VisualNodeNodeAction = TSharedPtr<FCustomSchemaAction_NewVisualNode>(VisualNodeAction);
		VisualNodeNodeAction->NodeType = NodeType;
		ContextMenuBuilder.AddAction(VisualNodeNodeAction);
	}

	FString EmitterNodeDescription = TEXT("Add Marker Emitter");
	FText EmitterNodeToolTip = FText::FromString(TEXT("ToolTip"));
	FText EmitterNodeCategory = FText::FromString(TEXT("Marker Emitters"));
	for (auto& MarkerNode : DungeonTemplate->MarkerNodes) {
		if (MarkerNode.Value) {
			FString Description = EmitterNodeDescription + ": " + MarkerNode.Value->MarkerName.ToString();
			auto MarkerEmitterAction = new FCustomSchemaAction_NewMarkerEmitterNode(EmitterNodeCategory, FText::FromString(Description), EmitterNodeToolTip, 1);
			auto MarkerEmitterNodeAction = TSharedPtr<FCustomSchemaAction_NewMarkerEmitterNode>(MarkerEmitterAction);
			MarkerEmitterNodeAction->MarkerNode = MarkerNode.Value;
			ContextMenuBuilder.AddAction(MarkerEmitterNodeAction);
		}
	}
}

void UGraphSchema_DungeonTemplate::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (Context && Context->Node)
	{
		FToolMenuSection& Section = Menu->AddSection("GenericGraphAssetGraphSchemaNodeActions", FText::FromString(TEXT("Node Actions")));
		{
			Section.AddMenuEntry(FGenericCommands::Get().Delete);
			Section.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
		}
	}

	Super::GetContextMenuActions(Menu, Context);
}

const FPinConnectionResponse UGraphSchema_DungeonTemplate::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (A->GetOwningNode() == B->GetOwningNode()) {
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Same node"));
	}

	// Compare the directions
	if ((A->Direction == EGPD_Input) && (B->Direction == EGPD_Input))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Can't connect input node to input node"));
	}
	else if ((A->Direction == EGPD_Output) && (B->Direction == EGPD_Output))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Can't connect output node to output node"));
	}


	UEdGraphNode* OutputNode = A->Direction == EGPD_Output ? A->GetOwningNode() : B->GetOwningNode();
	UEdGraphNode* InputNode = A->Direction == EGPD_Input ? A->GetOwningNode() : B->GetOwningNode();

	auto MarkerNode = Cast<UMarkerGraphNode>(OutputNode);
	if (MarkerNode) {
		auto VisualNode = Cast<UVisualGraphNode>(InputNode);
		if (!VisualNode) {
			return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Wrong input node"));
		}
	}
	else {
		auto VisualNode = Cast<UVisualGraphNode>(OutputNode);
		if (VisualNode) {
			auto EmitterNode = Cast<UEmitterGraphNode>(InputNode);
			if (!EmitterNode) {
				return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Wrong input node"));
			}
		}
		else {
			return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Wrong output node"));
		}
	}

	//Check for cicles
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

FLinearColor UGraphSchema_DungeonTemplate::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FColor::Yellow;
}

bool UGraphSchema_DungeonTemplate::ShouldHidePinDefaultValue(UEdGraphPin* Pin) const
{
	return true;
}

void UGraphSchema_DungeonTemplate::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FScopedTransaction Transaction(FText::FromString(TEXT("DungeonTemplate: BreakNodeLinks")));

	Super::BreakNodeLinks(TargetNode);
}

void UGraphSchema_DungeonTemplate::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(FText::FromString(TEXT("DungeonTemplate: BreakPinLinks")));

	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UGraphSchema_DungeonTemplate::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(FText::FromString(TEXT("DungeonTemplate: BreakSinglePinLink")));

	Super::BreakSinglePinLink(SourcePin, TargetPin);
}

class FConnectionDrawingPolicy* UGraphSchema_DungeonTemplate::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	return new FConnectionDrawingPolicy_DungeonGraph(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements);
}
