#pragma once

#include "CoreMinimal.h"
#include "EdGraph_DungeonTemplate.generated.h"

class UDungeonTemplate;

UCLASS()
class UEdGraph_DungeonTemplate : public UEdGraph
{
	GENERATED_BODY()

public:
	UEdGraph_DungeonTemplate();
	virtual ~UEdGraph_DungeonTemplate();

	UDungeonTemplate* GetDungeonTemplate() const;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PostEditUndo() override;
};