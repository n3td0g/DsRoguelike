#include "GraphNode_MarkerNode.h"
#include "Math/Color.h"
#include "MarkerGraphNode.h"
#include "MarkerNode.h"
#include "DungeonTemplatePin.h"

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
	for (auto Pin : MarkerNode->Pins) {
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
	return true;
}

const FSlateBrush* SGraphNode_MarkerNode::GetNameIcon() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Icon"));
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
			SNew(STextBlock)
			.Text(MarkerNode->GetNodeTitle(ENodeTitleType::FullTitle))
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16))
		]
	];
}

