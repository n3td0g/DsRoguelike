#pragma once

#include "SGraphNode.h"
#include "AssetThumbnail.h"
#include "SGraphPin.h"

class UEmitterGraphNode;

class SGraphNode_EmitterNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_EmitterNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEmitterGraphNode* InNode);

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual bool IsNameReadOnly() const override;

	virtual const FSlateBrush* GetNameIcon() const;	
protected:
	virtual void CreateNodeWidget();
	virtual void AddNodeStrings(TSharedPtr<SVerticalBox> NodeBox);

	UEmitterGraphNode* EmitterNode;

	TSharedPtr<SHorizontalBox> TopNodeBox;
	TSharedPtr<SHorizontalBox> BottomNodeBox;
	TSharedPtr<SHorizontalBox> OutputPinBox;
	TSharedPtr<SOverlay> NodeWiget;
};