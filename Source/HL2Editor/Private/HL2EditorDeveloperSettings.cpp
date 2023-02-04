#include "HL2EditorDeveloperSettings.h"

#define LOCTEXT_NAMESPACE "HL2EditorDeveloperSettings"

FText UHL2EditorDeveloperSettings::GetSectionText() const
{
	return LOCTEXT("HL2AssetImporterSettingsName", "HL2 Asset Importer");
}

FText UHL2EditorDeveloperSettings::GetSectionDescription() const
{
	return LOCTEXT("HL2AssetImporterDescription", "Configure the HL2 Asset Importer plugin");
}

#undef LOCTEXT_NAMESPACE