#pragma once

#include "SGraphNode.h"
#include "AssetThumbnail.h"
#include "SGraphPin.h"

class UMarkerGraphNode;

class SGraphNode_MarkerNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_MarkerNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UMarkerGraphNode* InNode);

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual bool IsNameReadOnly() const override;

	virtual const FSlateBrush* GetNameIcon() const;	
protected:
	virtual void CreateNodeWidget();
	virtual void AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox);

	UMarkerGraphNode* MarkerNode;

	TSharedPtr<SHorizontalBox> BottomNodeBox;
	TSharedPtr<SOverlay> NodeWiget;
};