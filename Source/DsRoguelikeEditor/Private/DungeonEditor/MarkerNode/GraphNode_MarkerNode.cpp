#include "DungeonEditor/MarkerNode/GraphNode_MarkerNode.h"
#include "Math/Color.h"
#include "DungeonEditor/MarkerNode/MarkerGraphNode.h"
#include "Widgets/SBoxPanel.h"
#include "DungeonEditor/DungeonTemplatePin.h"
#include "DungeonEditor/EdGraph_DungeonTemplate.h"
#include "Editor/MarkerNode.h"
#include "Editor/DungeonTemplate.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

void SGraphNode_MarkerNode::Construct(const FArguments& InArgs, UMarkerGraphNode* InNode)
{
	GraphNode = InNode;
	MarkerNode = InNode;
	UpdateGraphNode();


}

void SGraphNode_MarkerNode::UpdateGraphNode()
{
	const FMargin NodePadding = FMargin(2.0f);

	InputPins.Empty();
	OutputPins.Empty();

	RightNodeBox.Reset();
	LeftNodeBox.Reset();
	BottomNodeBox.Reset();

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0.0f)
			.BorderBackgroundColor(FLinearColor::Gray)
			[
				SNew(SOverlay)

				//BOTTOM OUTPUT PIN AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Bottom)
				[
					SAssignNew(BottomNodeBox, SHorizontalBox)
				]

				
				//NodeWidget
				+SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(10.0f)
				[
					SAssignNew(NodeWiget, SOverlay)
				]				
			]
		];

	CreateNodeWidget();
	CreatePinWidgets();
}

void SGraphNode_MarkerNode::CreatePinWidgets()
{
	for (auto Pin : MarkerNode->Pins)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SDungeonTemplatePin, Pin);
		NewPin->SetIsEditable(IsEditable);
		AddPin(NewPin.ToSharedRef());

		if (Pin->Direction == EEdGraphPinDirection::EGPD_Output)
		{
			OutputPins.Add(NewPin.ToSharedRef());
		}
	}	
}

void SGraphNode_MarkerNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}

	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Output)
	{
		BottomNodeBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(1.0f)
			.Padding(20.0f, 0.0f)
			[
				PinToAdd
			];
		InputPins.Add(PinToAdd);
	}
}

bool SGraphNode_MarkerNode::IsNameReadOnly() const
{
	return false;
}

const FSlateBrush* SGraphNode_MarkerNode::GetNameIcon() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Icon"));
}

void SGraphNode_MarkerNode::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	SGraphNode::OnNameTextCommited(InText, CommitInfo);

	auto EdNode_Node = Cast<UMarkerGraphNode>(GraphNode);
	if (!EdNode_Node) 
	{
		return;
	}

	auto DungeonGraph = Cast<UEdGraph_DungeonTemplate>(EdNode_Node->GetGraph());
	if (!DungeonGraph)
	{
		return;
	}

	auto DungeonTemplate = DungeonGraph->GetDungeonTemplate();
	if (!DungeonTemplate)
	{
		return;
	}

	const FName NewName = *(InText.ToString());

	if (DungeonTemplate->IsNameOccupied(NewName))
	{
		return;
	}
	
	if (MarkerNode && MarkerNode->MarkerNode)
	{
		MarkerNode->Modify();
		MarkerNode->MarkerNode->Modify();
		DungeonTemplate->Modify();

		DungeonTemplate->MarkerNodes.Remove(MarkerNode->MarkerNode->MarkerName);
		MarkerNode->MarkerNode->MarkerName = NewName;
		DungeonTemplate->MarkerNodes.Add(NewName, MarkerNode->MarkerNode);

		UpdateGraphNode();
	}
}

void SGraphNode_MarkerNode::CreateNodeWidget()
{
	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);
	TSharedPtr<SVerticalBox> NodeBox;

	NodeWiget->AddSlot()
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.ColorSpill"))
		.BorderBackgroundColor(TitleShadowColor)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Visibility(EVisibility::SelfHitTestInvisible)
		[
			SAssignNew(NodeBox, SVerticalBox)
		]
		];


	AddNodeStrings(NodeBox);
}

void SGraphNode_MarkerNode::AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox)
{
	const FSlateBrush* NodeTypeIcon = GetNameIcon();

	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	NodeBox->AddSlot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SImage)
			.Image(NodeTypeIcon)
		]
		+ SHorizontalBox::Slot()
		.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(InlineEditableText, SInlineEditableTextBlock)
				.Style(FEditorStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
				.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
				.OnVerifyTextChanged(this, &SGraphNode_MarkerNode::OnVerifyNameTextChanged)
				.OnTextCommitted(this, &SGraphNode_MarkerNode::OnNameTextCommited)
				.IsSelected(this, &SGraphNode_MarkerNode::IsSelectedExclusively)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				NodeTitle.ToSharedRef()
			]
		]
	];
}

