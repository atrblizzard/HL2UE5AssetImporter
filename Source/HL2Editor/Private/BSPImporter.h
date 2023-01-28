#pragma once

#include "CoreMinimal.h"
#include "ValveBSP/BSPFile.hpp"
#include "MeshDescription.h"
#include "HL2EntityData.h"
#include "EntityParser.h"
#include "BaseEntity.h"
#include "VBSPInfo.h"
#include "HL2EditorConfig.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHL2BSPImporter, Log, All);

class FBSPImporter
{
private:

	FString bspFileName;
	bool bspLoaded;
	Valve::BSPFile bspFile;

	FString mapName;
	UWorld* world;
	AVBSPInfo* vbspInfo;

public:

	FBSPImporter(const FString& fileName);

	/* Loads the BSP file and parses the data. */
	bool Load();

	/* Imports the entire BSP map into the currently opened level. */
	bool ImportToCurrentLevel();

	bool ImportGeometryToCurrentLevel();

	/* Imports the entire BSP map into the target world. */
	bool ImportAllToWorld(UWorld* targetWorld);

	bool ImportOnlyGeometryToWorld(UWorld* targetWorld);

	/* Imports geometry only into the target world. */
	bool ImportGeometryToWorld(UWorld* targetWorld);

	/* Imports entities only into the target world. */
	bool ImportEntitiesToWorld(UWorld* targetWorld);

private:

	void GatherBrushes(uint32 nodeIndex, TArray<uint16>& out);
	
	void GatherFaces(uint32 nodeIndex, TArray<uint16>& out);
	
	void GatherDisplacements(const TArray<uint16>& faceIndices, TArray<uint16>& out);
	
	FPlane4f ValveToUnrealPlane(const Valve::BSP::cplane_t& plane);
	
	void RenderModelToActors(TArray<AStaticMeshActor*>& out, uint32 modelIndex);

	void RenderCellsToActors(const FMeshDescription& meshDesc, const FString& prefix, const FHL2EditorCellsConfig& config, bool enableLumen, bool enableCollisions, TArray<AStaticMeshActor*>& outActors);
	
	UStaticMesh* RenderMeshToStaticMesh(const FMeshDescription& meshDesc, const FString& assetName, bool enableLumen, bool enableCollisions);

	AStaticMeshActor* RenderMeshToActor(const FMeshDescription& meshDesc, const FString& assetName, bool enableLumen, bool enableCollisions);
	
	void RenderFacesToMesh(const TArray<uint16>& faceIndices, FMeshDescription& meshDesc, bool skyboxFilter);

	void RenderBrushesToMesh(const TArray<uint16>& brushIndices, FMeshDescription& meshDesc, FName overrideMaterial = NAME_None, bool alwaysEmitFaces = false);
	
	void RenderDisplacementsToMesh(const TArray<uint16>& displacements, FMeshDescription& meshDesc);

	void RenderDetailPropsToMesh(FMeshDescription& meshDesc);
	
	void RenderTreeToVBSPInfo(uint32 nodeIndex);

	UStaticMesh* RenderBrushesToCollisionStaticMesh(const TArray<uint16>& brushIndices, const FString& assetName, FName editorMaterial);

	AStaticMeshActor* RenderBrushesToCollisionActor(const TArray<uint16>& brushIndices, const FString& assetName, FName editorMaterial);

	void ProcessBrush(uint16 brushIndex, bool closeGeometry, int rejectedSurfFlags, FBSPBrush& outBSPBrush);

	float FindFaceArea(const Valve::BSP::dface_t& bspFace, bool unrealCoordSpace = true);

	static FBox3f GetModelBounds(const Valve::BSP::dmodel_t& model, bool unrealCoordSpace = true);

	static FBox3f GetNodeBounds(const Valve::BSP::snode_t& node, bool unrealCoordSpace = true);

	static FString ParseMaterialName(const char* bspMaterialName);
	
	static bool SharesSmoothingGroup(uint16 groupA, uint16 groupB);

};