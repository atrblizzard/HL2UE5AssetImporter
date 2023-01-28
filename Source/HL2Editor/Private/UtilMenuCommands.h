#pragma once

#include "SlateBasics.h"

class FUtilMenuCommands : public TCommands<FUtilMenuCommands>
{
public:

	FUtilMenuCommands();

#if WITH_EDITOR
	virtual void RegisterCommands() override;
#endif

	TSharedPtr<FUICommandInfo> BulkImportTextures;
	TSharedPtr<FUICommandInfo> BulkImportMaterials;
	TSharedPtr<FUICommandInfo> BulkImportModels;
	TSharedPtr<FUICommandInfo> BulkImportSounds;
	TSharedPtr<FUICommandInfo> ImportScripts;
	TSharedPtr<FUICommandInfo> ConvertSkyboxes;

	TSharedPtr<FUICommandInfo> ImportBSP;
	TSharedPtr<FUICommandInfo> ImportBSPOnly;

	TSharedPtr<FUICommandInfo> TraceTerrain;
};