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

	virtual const FSlateBrush* GetNameIcon() const;	
protected:
	virtual void CreateNodeWidget();
	virtual void AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox);

	UVisualGraphNode* VisualNode;

	TSharedPtr<SHorizontalBox> TopNodeBox;
	TSharedPtr<SOverlay> NodeWiget;
};