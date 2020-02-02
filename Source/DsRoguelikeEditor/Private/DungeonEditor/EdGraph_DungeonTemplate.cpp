#include "DungeonEditor/EdGraph_DungeonTemplate.h"
#include "Editor/DungeonTemplate.h"

UEdGraph_DungeonTemplate::UEdGraph_DungeonTemplate()
{

}

UEdGraph_DungeonTemplate::~UEdGraph_DungeonTemplate()
{

}

UDungeonTemplate* UEdGraph_DungeonTemplate::GetDungeonTemplate() const
{
	return CastChecked<UDungeonTemplate>(GetOuter());
}

bool UEdGraph_DungeonTemplate::Modify(bool bAlwaysMarkDirty /*= true*/)
{
	bool Rtn = Super::Modify(bAlwaysMarkDirty);

	GetDungeonTemplate()->Modify();

	return Rtn;
}

void UEdGraph_DungeonTemplate::PostEditUndo()
{
	Super::PostEditUndo();

	NotifyGraphChanged();
}

