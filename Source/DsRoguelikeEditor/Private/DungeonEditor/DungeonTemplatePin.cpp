#include "DungeonEditor/DungeonTemplatePin.h"
#include "Math/Color.h"

//PinNode..........................................................................
void SDungeonTemplatePin::Construct(const FArguments& InArgs, UEdGraphPin* InPin, int PinNumber)
{
	this->SetCursor(EMouseCursor::Default);

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != nullptr);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SDungeonTemplatePin::GetPinBorder)
		.BorderBackgroundColor(this, &SDungeonTemplatePin::GetPinColor)
		.OnMouseButtonDown(this, &SDungeonTemplatePin::OnPinMouseDown)
		.Cursor(this, &SDungeonTemplatePin::GetPinCursor)
		.Padding(FMargin(10.0f))
	);
}

TSharedRef<SWidget>	SDungeonTemplatePin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SDungeonTemplatePin::GetPinBorder() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Body"));
}

FSlateColor SDungeonTemplatePin::GetPinColor() const
{
	static const FLinearColor PinColor(0.02f, 0.02f, 0.02f);

	return PinColor;
}