#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IHL2Editor.h"
#include "LevelEditor.h"
#include "Interfaces/IPluginManager.h"
#include "HL2EditorDeveloperSettings.h"

#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1)
#include "AssetRegistry/AssetRegistryModule.h"
#else
#include "AssetRegistryModule.h"
#endif

DECLARE_LOG_CATEGORY_EXTERN(LogHL2Editor, Log, All);

class UTexture;
class UVMTMaterial;
class UMaterial;

class HL2EditorImpl : public IHL2Editor
{
public:

	/** Begin IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();
	/** End IModuleInterface implementation */

	/** Begin IHL2Editor implementation */
	virtual UHL2EditorDeveloperSettings* GetConfig() const override;
	/** End IHL2Editor implementation */

private:

	TSharedPtr<FUICommandList> utilMenuCommandList;
	TSharedPtr<FExtender> myExtender;

	UHL2EditorDeveloperSettings* Settings = GetMutableDefault<UHL2EditorDeveloperSettings>();

	void* vtfLibDllHandle = nullptr;
	void* mpg123DllHandle = nullptr;
	void* lzmaDllHandle = nullptr;

	static inline FString GetVTFLibDllPath();
	static inline FString GetMPG123LibDllPath();
	static inline FString GetLZMALibDllPath();

	void AddToolbarExtension(FToolBarBuilder& builder) const;
	static TSharedRef<SWidget> GenerateUtilityMenu(TSharedRef<FUICommandList> InCommandList);

	void BulkImportTexturesClicked();
	void BulkImportMaterialsClicked();
	void BulkImportModelsClicked();
	void BulkImportSoundsClicked();
	void ImportScriptsClicked();
	void ConvertSkyboxes();
	void ImportBSPClicked();
	void ImportBSPOnlyClicked();
	void TraceTerrainClicked();

	void SaveImportedAssets(TArrayView<UObject*> importedObjects) const;

	static void GroupFileListByDirectory(const TArray<FString>& files, TMap<FString, TArray<FString>>& outMap);

};

inline FString HL2EditorImpl::GetVTFLibDllPath()
{
	const FString pluginBaseDir = IPluginManager::Get().FindPlugin("HL2AssetImporter")->GetBaseDir();
	const FString Win64(TEXT("Win64"));
	const FString Win32(TEXT("Win32"));
	// Note: if you get a compile error here, you're probably trying to build for linux or mac - we need to add support for these platforms here
	return pluginBaseDir / TEXT("Source/VTFLib/bin") / UBT_COMPILED_PLATFORM / TEXT("VTFLib.dll");
}

inline FString HL2EditorImpl::GetMPG123LibDllPath()
{
	const FString pluginBaseDir = IPluginManager::Get().FindPlugin("HL2AssetImporter")->GetBaseDir();
	const FString Win64(TEXT("Win64"));
	const FString Win32(TEXT("Win32"));
	// Note: if you get a compile error here, you're probably trying to build for linux or mac - we need to add support for these platforms here
	return pluginBaseDir / TEXT("Source/MPG123/bin") / UBT_COMPILED_PLATFORM / TEXT("libmpg123-0.dll");
}

inline FString HL2EditorImpl::GetLZMALibDllPath()
{
	const FString pluginBaseDir = IPluginManager::Get().FindPlugin("HL2AssetImporter")->GetBaseDir();
	const FString Win64(TEXT("Win64"));
	const FString Win32(TEXT("Win32"));
	// Note: if you get a compile error here, you're probably trying to build for linux or mac - we need to add support for these platforms here
	return pluginBaseDir / TEXT("Source/LZMA/bin") / UBT_COMPILED_PLATFORM / TEXT("LZMA.dll");
}