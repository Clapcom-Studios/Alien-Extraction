#pragma once

#include <string>

#include "Module.h"
#include "Mesh.h"
#include "Texture.h"

#include "JsonFile.h"

class ModuleFileSystem : public Module
{
public:

	ModuleFileSystem(Application* app, bool start_enabled = true);

	virtual ~ModuleFileSystem();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void CreateMetaFileFromAsset(std::string assetPath, JsonFile& jsonFile);
	void CreateLibraryFolder();

	// Save
	bool SaveMeshToFile(const Mesh* ourMesh, const std::string& filename);
	bool SaveTextureToFile(const Texture* ourTexture, const std::string& filename);

	// Load
	bool LoadMeshToFile(const std::string filename, Mesh* ourMesh);
	bool LoadTextureToFile(const Texture* ourTexture, const std::string& filename);

public:

	std::string workingDirectory;
	std::string libraryPath;

	std::string libraryScenesPath;
	std::string libraryModelsPath;
	std::string libraryMeshesPath;
	std::string libraryMaterialsPath;
	std::string libraryShadersPath;
	std::string libraryTexturesPath;
	std::string librarySettingsPath;

	JsonFile outputFile;

};