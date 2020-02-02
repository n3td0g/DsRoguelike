#include "DungeonEditor/EmitterNode/GraphNode_EmitterNode.h"
#include "Math/Color.h"
#include "DungeonEditor/EmitterNode/EmitterGraphNode.h"
#include "Widgets/SBoxPanel.h"

void SGraphNode_EmitterNode::Construct(const FArguments& InArgs, UEmitterGraphNode* InNode)
{
	GraphNode = InNode;
	EmitterNode = InNode;
	UpdateGraphNode();
}

void SGraphNode_EmitterNode::UpdateGraphNode()
{
	const FMargin NodePadding = FMargin(2.0f);

	RightNodeBox.Reset();
	LeftNodeBox.Reset();
	TopNodeBox.Reset();
	BottomNodeBox.Reset();

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0.0f)
			.BorderBackgroundColor(FLinearColor::Blue)
			[
				SNew(SOverlay)

					//LEFT INPUT PIN AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Fill)
				.Padding(0.0f, 10.0f)
				[
					SAssignNew(LeftNodeBox, SVerticalBox)
				]

				//RIGHT OUTPUT PIN AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Fill)
				.Padding(0.0f, 10.0f)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
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

void SGraphNode_EmitterNode::CreatePinWidgets()
{

}

void SGraphNode_EmitterNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{

}

bool SGraphNode_EmitterNode::IsNameReadOnly() const
{
	return true;
}

const FSlateBrush* SGraphNode_EmitterNode::GetNameIcon() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Icon"));
}

void SGraphNode_EmitterNode::CreateNodeWidget()
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

void SGraphNode_EmitterNode::AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox)
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
			.Text(EmitterNode->GetNodeTitle(ENodeTitleType::FullTitle))
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16))
		]
	];
}

