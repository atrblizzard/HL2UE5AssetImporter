#pragma once

#include "CoreMinimal.h"
#include "HL2EditorConfig.h"

#include "Engine/DeveloperSettings.h"
#include "UObject/Object.h"
#include "HL2EditorDeveloperSettings.generated.h"


UCLASS(config = Editor, defaultconfig, meta = (DisplayName = "Half-Life 2 Asset Importer"))
class HL2EDITOR_API UHL2EditorDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static const UHL2EditorDeveloperSettings* Get() { return GetDefault<UHL2EditorDeveloperSettings>(); }

	virtual FName GetContainerName() const override { return FName("Project"); }
	virtual FName GetCategoryName() const override { return FName("Plugins"); }
	virtual FName GetSectionName() const override { return FName("HL2AssetImporter"); }

	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;

	UPROPERTY(Config, EditAnywhere, Category = "Cells", meta = (DisplayName = "Cells"))
	FHL2EditorCellsConfig Cells;

	UPROPERTY(Config, EditAnywhere, Category = "Editor", meta = (DisplayName = "Editor Config"))
	FHL2EditorConfig Config;

	UPROPERTY(Config, EditAnywhere, Category = "Map Importing", meta = (DisplayName = "Verbose debug messages"))
	bool VerboseDebugEnabled;

	UPROPERTY(Config, EditAnywhere, Category = "Map Importing", meta = (DisplayName = "Override Import Root Path"))
	bool OverrideImportRootPath;

	UPROPERTY(Config, EditAnywhere, Category = "Map Importing", meta = (DisplayName = "Import Root Path"))
	FString ImportRootPath;
};