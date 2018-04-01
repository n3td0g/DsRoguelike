#include "GraphNode_VisualNode.h"
#include "Math/Color.h"
#include "VisualGraphNode.h"
#include "DungeonTemplatePin.h"

void SGraphNode_VisualNode::Construct(const FArguments& InArgs, UVisualGraphNode* InNode)
{
	GraphNode = InNode;
	VisualNode = InNode;
	UpdateGraphNode();
}

void SGraphNode_VisualNode::UpdateGraphNode()
{
	const FMargin NodePadding = FMargin(2.0f);

	RightNodeBox.Reset();
	LeftNodeBox.Reset();
	TopNodeBox.Reset();

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

					//TOP INPUT PIN AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					SAssignNew(TopNodeBox, SHorizontalBox)
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

void SGraphNode_VisualNode::CreatePinWidgets()
{
	for (auto Pin : VisualNode->Pins) {
		TSharedPtr<SGraphPin> NewPin = SNew(SDungeonTemplatePin, Pin);
		NewPin->SetIsEditable(IsEditable);
		AddPin(NewPin.ToSharedRef());

		if (Pin->Direction == EEdGraphPinDirection::EGPD_Input)
		{
			InputPins.Add(NewPin.ToSharedRef());
		}
	}
}

void SGraphNode_VisualNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}

	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		TopNodeBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(1.0f)
			.Padding(20.0f, 0.0f)
			[
				PinToAdd
			];
		OutputPins.Add(PinToAdd);
	}
}

bool SGraphNode_VisualNode::IsNameReadOnly() const
{
	return true;
}

const FSlateBrush* SGraphNode_VisualNode::GetNameIcon() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Icon"));
}

void SGraphNode_VisualNode::CreateNodeWidget()
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

void SGraphNode_VisualNode::AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox)
{
	const FSlateBrush* NodeTypeIcon = GetNameIcon();

	const int32 ThumbnailSize = 48;
	auto MyAsset = VisualNode->GetObject();

	TSharedPtr<FAssetThumbnail> Thumbnail = MakeShareable(new FAssetThumbnail(MyAsset, ThumbnailSize, ThumbnailSize, nullptr));
	FAssetThumbnailConfig ThumbnailConfig;

	NodeBox->AddSlot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(4.0f, 4.0f)
		[
			SNew(SBox)
			.WidthOverride(ThumbnailSize)
		.HeightOverride(ThumbnailSize)
		[
			Thumbnail->MakeThumbnailWidget(ThumbnailConfig)
		]
		]

		/*+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SImage)
			.Image(NodeTypeIcon)
		]*/
		/*+ SHorizontalBox::Slot()
		.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
		[
			SNew(SImage)
			.Image(NodeIcon)
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Test")))
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16))
		]*/

	];
}

