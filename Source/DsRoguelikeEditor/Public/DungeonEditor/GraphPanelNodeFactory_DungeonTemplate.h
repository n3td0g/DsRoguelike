#pragma once
#include "EdGraphUtilities.h"

class  FGraphPanelNodeFactory_DungeonTemplate : public FGraphPanelNodeFactory
{
public:
	FGraphPanelNodeFactory_DungeonTemplate();

private:
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};