#pragma once

#include "AssetThumbnail.h"
#include "SGraphPin.h"

//PineNode....................................................................................
class  SDungeonTemplatePin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SDungeonTemplatePin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin, int PinNumber = 0);
protected:
	// Begin SGraphPin interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	// End SGraphPin interface

	/** @return The color that we should use to draw this pin */
	virtual FSlateColor GetPinColor() const override;

	// End SGraphPin interface

	const FSlateBrush* GetPinBorder() const;

private:

	bool IsPinNumbered;
};