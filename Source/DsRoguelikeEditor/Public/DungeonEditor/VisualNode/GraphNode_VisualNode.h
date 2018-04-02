#pragma once

#include "SGraphNode.h"
#include "AssetThumbnail.h"
#include "SGraphPin.h"

class UVisualGraphNode;

class SGraphNode_VisualNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_VisualNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UVisualGraphNode* InNode);

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual bool IsNameReadOnly() const override;

	virtual TArray<FOverlayWidgetInfo> GetOverlayWidgets(bool bSelected, const FVector2D& WidgetSize) const override;

	virtual const FSlateBrush* GetNameIcon() const;	
protected:
	virtual void CreateNodeWidget();
	virtual void AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox);

	/** Get the visibility of the index overlay */
	EVisibility GetIndexVisibility() const;

	/** Get the text to display in the index overlay */
	FText GetIndexText() const;

	/** Get the tooltip for the index overlay */
	FText GetIndexTooltipText() const;

	/** Get the color to display for the index overlay. This changes on hover state of sibling nodes */
	FSlateColor GetIndexColor(bool bHovered) const;

	/** Handle hover state changing for the index widget - we use this to highlight sibling nodes */
	void OnIndexHoverStateChanged(bool bHovered);

protected:

	UVisualGraphNode* VisualNode;

	TSharedPtr<SHorizontalBox> TopNodeBox;
	TSharedPtr<SOverlay> NodeWiget;
	TSharedPtr<SWidget> IndexOverlay;
};