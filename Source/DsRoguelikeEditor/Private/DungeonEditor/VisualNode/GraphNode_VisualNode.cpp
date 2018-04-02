#include "GraphNode_VisualNode.h"
#include "Math/Color.h"
#include "VisualGraphNode.h"
#include "SGraphPanel.h"
#include "DungeonTemplatePin.h"

/** Widget for overlaying an execution-order index onto a node */
class SVisualNodeIndex : public SCompoundWidget
{
public:
	/** Delegate event fired when the hover state of this widget changes */
	DECLARE_DELEGATE_OneParam(FOnHoverStateChanged, bool /* bHovered */);

	/** Delegate used to receive the color of the node, depending on hover state and state of other siblings */
	DECLARE_DELEGATE_RetVal_OneParam(FSlateColor, FOnGetIndexColor, bool /* bHovered */);

	SLATE_BEGIN_ARGS(SVisualNodeIndex) {}
	SLATE_ATTRIBUTE(FText, Text)
		SLATE_EVENT(FOnHoverStateChanged, OnHoverStateChanged)
		SLATE_EVENT(FOnGetIndexColor, OnGetIndexColor)
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs)
	{
		OnHoverStateChangedEvent = InArgs._OnHoverStateChanged;
		OnGetIndexColorEvent = InArgs._OnGetIndexColor;

		const FSlateBrush* IndexBrush = FEditorStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Index"));

		ChildSlot
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				// Add a dummy box here to make sure the widget doesnt get smaller than the brush
				SNew(SBox)
				.WidthOverride(IndexBrush->ImageSize.X)
			.HeightOverride(IndexBrush->ImageSize.Y)
			]
		+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBorder)
				.BorderImage(IndexBrush)
			.BorderBackgroundColor(this, &SVisualNodeIndex::GetColor)
			.Padding(FMargin(4.0f, 0.0f, 4.0f, 1.0f))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(InArgs._Text)
			.Font(FEditorStyle::GetFontStyle("BTEditor.Graph.BTNode.IndexText"))
			]
			]
			];
	}

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		OnHoverStateChangedEvent.ExecuteIfBound(true);
		SCompoundWidget::OnMouseEnter(MyGeometry, MouseEvent);
	}

	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override
	{
		OnHoverStateChangedEvent.ExecuteIfBound(false);
		SCompoundWidget::OnMouseLeave(MouseEvent);
	}

	/** Get the color we use to display the rounded border */
	FSlateColor GetColor() const
	{
		if (OnGetIndexColorEvent.IsBound())
		{
			return OnGetIndexColorEvent.Execute(IsHovered());
		}

		return FSlateColor::UseForeground();
	}

private:
	/** Delegate event fired when the hover state of this widget changes */
	FOnHoverStateChanged OnHoverStateChangedEvent;

	/** Delegate used to receive the color of the node, depending on hover state and state of other siblings */
	FOnGetIndexColor OnGetIndexColorEvent;
};

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
	+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(10.0f)
		[
			SAssignNew(NodeWiget, SOverlay)
		]
		]
		];


	IndexOverlay = SNew(SVisualNodeIndex)
		.ToolTipText(this, &SGraphNode_VisualNode::GetIndexTooltipText)
		.Visibility(this, &SGraphNode_VisualNode::GetIndexVisibility)
		.Text(this, &SGraphNode_VisualNode::GetIndexText)
		.OnHoverStateChanged(this, &SGraphNode_VisualNode::OnIndexHoverStateChanged)
		.OnGetIndexColor(this, &SGraphNode_VisualNode::GetIndexColor);

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

TArray<FOverlayWidgetInfo> SGraphNode_VisualNode::GetOverlayWidgets(bool bSelected, const FVector2D& WidgetSize) const
{
	check(IndexOverlay.IsValid());

	TArray<FOverlayWidgetInfo> Widgets;
	FVector2D Origin(0.0f, 0.0f);

	FOverlayWidgetInfo Overlay(IndexOverlay);
	Overlay.OverlayOffset = FVector2D(WidgetSize.X - (IndexOverlay->GetDesiredSize().X * 0.5f), Origin.Y);
	Widgets.Add(Overlay);

	return Widgets;
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
		];
}

EVisibility SGraphNode_VisualNode::GetIndexVisibility() const
{
	TSharedPtr<SGraphPanel> MyOwnerPanel = GetOwnerPanel();
	return (!MyOwnerPanel.IsValid() || MyOwnerPanel->GetCurrentLOD() > EGraphRenderingLOD::LowDetail) ? EVisibility::Visible : EVisibility::Collapsed;
}

FText SGraphNode_VisualNode::GetIndexText() const
{
	UVisualGraphNode* StateNode = CastChecked<UVisualGraphNode>(GraphNode);
	UEdGraphPin* MyInputPin = StateNode->GetInputPin();
	UEdGraphPin* MyParentOutputPin = NULL;
	if (MyInputPin != NULL && MyInputPin->LinkedTo.Num() > 0)
	{
		MyParentOutputPin = MyInputPin->LinkedTo[0];
	}

	int32 Index = 0;

	if (MyParentOutputPin != NULL)
	{
		for (Index = 0; Index < MyParentOutputPin->LinkedTo.Num(); ++Index)
		{
			if (MyParentOutputPin->LinkedTo[Index] == MyInputPin)
			{
				break;
			}
		}
	}

	return FText::AsNumber(Index);
}

FText SGraphNode_VisualNode::GetIndexTooltipText() const
{
	return FText::FromString("Visual node index");
}

FSlateColor SGraphNode_VisualNode::GetIndexColor(bool bHovered) const
{
	static const FName DefaultColor("BTEditor.Graph.BTNode.Index.Color");
	return FEditorStyle::Get().GetSlateColor(DefaultColor);
}

void SGraphNode_VisualNode::OnIndexHoverStateChanged(bool bHovered)
{

}

