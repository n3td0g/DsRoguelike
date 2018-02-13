#include "DungeonTemplateAction.h"
#include "Developer/AssetTools/Public/AssetTypeCategories.h"
#include "DungeonTemplate.h"

FText FDungeonTemplateAction::GetName() const
{
	return FText::FromString("DungeonTemplate");
}

FColor FDungeonTemplateAction::GetTypeColor() const
{
	return FColor::Cyan;
}

UClass* FDungeonTemplateAction::GetSupportedClass() const
{
	return UDungeonTemplate::StaticClass();
}

uint32 FDungeonTemplateAction::GetCategories()
{
	return EAssetTypeCategories::Misc | EAssetTypeCategories::Basic;
}

void FDungeonTemplateAction::OpenAssetEditor(const TArray<UObject *>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor /* = TSharedPtr<IToolkitHost>() */)
{
	/*const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		UDungeonTemplate* PropData = Cast<UDungeonTemplate>(*ObjIt);
		if (PropData)
		{
			TSharedRef<FCustAssetEditor> NewCustEditor(new FCustAssetEditor());

			NewCustEditor->InitCustAssetEditor(Mode, EditWithinLevelEditor, PropData);
		}
	}*/
}

void FDungeonTemplateAction::RegistrateCustomPartAssetType()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FDungeonTemplateAction));
}
