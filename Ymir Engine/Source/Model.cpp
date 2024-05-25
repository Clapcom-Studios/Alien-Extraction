#include "Model.h"
#include "Log.h"

#include "GameObject.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"

#include "ModuleFileSystem.h"
#include "PhysfsEncapsule.h"
#include "ImporterTexture.h"
#include "ImporterAnimation.h"

#include "ModuleResourceManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "External/stb_image/stb_image.h"

#include "External/mmgr/mmgr.h"

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices  | aiProcess_CalcTangentSpace | aiProcessPreset_TargetRealtime_MaxQuality)

Model::Model()
{
	//if (modelGO->active && External->scene->mRootNode && External->scene->mRootNode->active) {

	//	for (auto it = meshes.begin(); it != meshes.end(); ++it) {

	//		if ((*it).meshGO->active) {

	//			(*it).DrawMesh();

	//		}

	//	}

	//}
}

Model::Model(const std::string& path, bool onlyReimport, const std::string& shaderPath)
{
	this->onlyReimport = onlyReimport;
	processedMeshes = 0;
	boneCounter = 0;
	LoadModel(path, shaderPath);
}

Model::~Model()
{

}

void Model::DrawModel()
{
	//if (modelGO->active && External->scene->mRootNode && External->scene->mRootNode->active) {

	//	for (auto it = meshes.begin(); it != meshes.end(); ++it) {

	//		if ((*it).meshGO->active && External->renderer3D->IsInsideFrustum(External->scene->gameCameraComponent, (*it).globalAABB)) {

	//			(*it).DrawMesh();

	//		}
	//		
	//	}

	//}
	
}

void Model::LoadModel(const std::string& path, const std::string& shaderPath)
{
	this->path = path;

	// Retrieve info about Model (directory and name)

	if (path.find('\\') != std::string::npos) {

		directory = path.substr(0, path.find_last_of('\\') + 1);

	}
	else {

		directory = path.substr(0, path.find_last_of('/') + 1);

	}

	size_t lastDot = path.find_last_of('.');

	if (path.find("/") != std::string::npos) {

		size_t lastSlash = path.find_last_of('/') + 1;
		name = path.substr(lastSlash, lastDot - lastSlash);

	}
	else {

		size_t lastSlash = path.find_last_of('\\') + 1;
		name = path.substr(lastSlash, lastDot - lastSlash);

	}

	// Import the model using Assimp

	const aiScene* scene = aiImportFile(path.c_str(), ASSIMP_LOAD_FLAGS);

	if (scene != nullptr && scene->HasMeshes())
	{
		int it = 0;

		ProcessNode(scene->mRootNode, scene, nullptr, shaderPath, it);

		if (!PhysfsEncapsule::FileExists(path + ".meta")) {

			GenerateModelMetaFile();

		}

		LOG("Model created: %s", name.c_str());

		aiReleaseImport(scene);
	}
	else
	{
		LOG("[ERROR] could not load assimp scene %s", path.c_str());
	}

}

void Model::ProcessNode(aiNode* node, const aiScene* scene, GameObject* parentGO, const std::string& shaderPath, int& iteration)
{
	const char* mNameStr = node->mName.C_Str();
	std::string mName(mNameStr);

	// Check if mName contains the substring "$AssimpFbx$" or "mixamorig:"
	bool shouldSkip = (mName.find("$AssimpFbx$") != std::string::npos) ||	
					  (mName.find("mixamorig:") != std::string::npos);

	// Link Assimp to GameObjects Hierarchy
	GameObject* currentNodeGO = nullptr;
	int currentResourceUID = 0;

	// If the current node is not to be skipped, proceed with processing it.
	if (!shouldSkip) {

		// Retrieve transformation from Assimp
		aiVector3D translation, scaling;
		aiQuaternion rotation;

		node->mTransformation.Decompose(scaling, rotation, translation);

		NodeTransform tmpNodeTransform;

		tmpNodeTransform.translation = { translation.x, translation.y, translation.z };
		Quat rotQ(rotation.x, rotation.y, rotation.z, rotation.w);
		tmpNodeTransform.rotation = rotQ.ToEulerXYZ();
		tmpNodeTransform.scale = { scaling.x, scaling.y, scaling.z };

		if (parentGO == nullptr) {

			// If the current node is the root node, create here the model GameObject, parented to scene GameObject
			currentNodeGO = External->scene->CreateGameObject(name, External->scene->mRootNode);
			modelGO = currentNodeGO;

			std::unique_ptr<JsonFile> tmpMetaFile = JsonFile::GetJSON(path + ".meta");

			if (tmpMetaFile) {

				// The meta file exists; it's not the first time we load the model.
				modelGO->UID = tmpMetaFile->GetInt("UID");

			}
			else {

				// The meta file doesn't exists; first time loading the model.
				modelGO->UID = Random::Generate();

			}

			modelGO->type = "Model";
			modelGO->originPath = path;

		}
		else {

			// Create the game object since the substring "$AssimpFbx$" doesn't exist in mName
			currentNodeGO = External->scene->CreateGameObject(mNameStr, parentGO);
			currentNodeGO->type = "Mesh";
			currentNodeGO->originPath = path;

			// Model Meta File and Library File Creation
			std::unique_ptr<JsonFile> tmpMetaFile = JsonFile::GetJSON(path + ".meta");

			if (tmpMetaFile) {

				// The meta file exists; it's not the first time we load the model.
				currentNodeGO->UID = tmpMetaFile->GetIntArray("Meshes Embedded UID")[iteration];
				currentResourceUID = tmpMetaFile->GetIntArray("Resources Embedded UID")[iteration];

				iteration++;

			}
			else {

				// The meta file doesn't exists; first time loading the model.
				currentNodeGO->UID = Random::Generate();
				currentResourceUID = Random::Generate();

			}
			
			embeddedMeshesUID.push_back(currentNodeGO->UID);
			resourcesUID.push_back(currentResourceUID);

		}

		// Process all the node's meshes (if any)
		for (uint i = 0; i < node->mNumMeshes; i++) {

			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			ProcessMesh(mesh, scene, currentNodeGO, currentResourceUID, &tmpNodeTransform, shaderPath);
			processedMeshes++;
		}

		GenerateYmodelFile(tmpNodeTransform.translation, tmpNodeTransform.rotation, tmpNodeTransform.scale);

		// Load Transform From Assimp
		static_cast<CTransform*>(currentNodeGO->GetComponent(ComponentType::TRANSFORM))->SetTransform(tmpNodeTransform.translation, tmpNodeTransform.rotation * RADTODEG, tmpNodeTransform.scale);

		// Check if all the bone meshes have been processed ---> when all meshes processed, process if have animations
		if (scene->mNumMeshes == processedMeshes) {
			if (scene->HasAnimations()) {

				LOG("Model has animations");
				if (modelGO->GetComponent(ANIMATION) == nullptr) {
					CAnimation* animationComponent = new CAnimation(modelGO);
					modelGO->AddComponent(animationComponent);
					CAnimation* cAnim = (CAnimation*)modelGO->GetComponent(ANIMATION);
					cAnim->modelPath = path;

					for (int i = 0; i < scene->mNumAnimations; i++) {

						//Animation* anim = new Animation(path, this, i);
						ResourceAnimation* rAnim = new ResourceAnimation(modelGO->UID);
						ImporterAnimation::Import(path, rAnim, this, i);

						uint UID = Random::Generate();

						std::string filenameUID = std::to_string(UID) + ".yanim";
						std::string libraryPath = External->fileSystem->libraryAnimationsPath + filenameUID;

						if (PhysfsEncapsule::FileExists(libraryPath)) {
							filenameUID = std::to_string(UID) + ".yanim";
							libraryPath = External->fileSystem->libraryAnimationsPath + filenameUID;

						}
						rAnim->SetLibraryFilePath(libraryPath);

						External->fileSystem->SaveAnimationToFile(rAnim, libraryPath);

						std::string filename = rAnim->name + ".yanim";
						std::string assetsPath = External->fileSystem->assetsPath + filename;

						rAnim->SetAssetsFilePath(assetsPath);

						External->fileSystem->SaveAnimationToFile(rAnim, assetsPath);

						RELEASE(rAnim);

						ResourceAnimation* rAnimation = (ResourceAnimation*)External->resourceManager->CreateResourceFromLibrary(assetsPath, ResourceType::ANIMATION, UID);
						cAnim->AddAnimation(rAnimation);
					}
				}
			}
			else {

				//animator = nullptr;

				LOG("Model doesn't have animations");
			}

		}
	}
		

	// Process children of the current node
	for (uint i = 0; i < node->mNumChildren; i++) {

		ProcessNode(node->mChildren[i], scene, shouldSkip ? parentGO : currentNodeGO, shaderPath, iteration);

	}

}

void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, GameObject* linkGO, int rMeshUID, NodeTransform* transform, const std::string& shaderPath)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	// Process Vertices

	for (uint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		
		// Initialize bone data
		SetVertexBoneDataDefault(vertex);

		// Retrieve vertex positions
		
		float3 vPosition;

		vPosition.x = mesh->mVertices[i].x;
		vPosition.y = mesh->mVertices[i].y;
		vPosition.z = mesh->mVertices[i].z;

		vertex.position = vPosition;

		// Retrieve vertex normals

		if (mesh->HasNormals())
		{
			float3 vNormals;

			vNormals.x = mesh->mNormals[i].x;
			vNormals.y = mesh->mNormals[i].y;
			vNormals.z = mesh->mNormals[i].z;

			vertex.normal = vNormals;
		}

		// Retrieve vertex tangents and bitangents

		if (mesh->HasTangentsAndBitangents())
		{
			float3 vTangents;

			vTangents.x = mesh->mTangents[i].x;
			vTangents.y = mesh->mTangents[i].y;
			vTangents.z = mesh->mTangents[i].z;

			vertex.tangents = vTangents;

			float3 vBitangents;

			vBitangents.x = mesh->mBitangents[i].x;
			vBitangents.y = mesh->mBitangents[i].y;
			vBitangents.z = mesh->mBitangents[i].z;

			vertex.bitangents = vBitangents;

		}

		// Retrieve vertex texture coordinates

		if (mesh->HasTextureCoords(0))
		{
			float2 vTextureCoords;

			vTextureCoords.x = mesh->mTextureCoords[0][i].x;
			vTextureCoords.y = mesh->mTextureCoords[0][i].y;

			vertex.textureCoordinates = vTextureCoords;
		}
		else
		{
			vertex.textureCoordinates = float2(0.0f, 0.0f);
		}

		// Create vertex with all the information stored

		vertices.push_back(vertex);
	}

	// Process Indices

	if (mesh->HasFaces()) {

		for (uint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (uint j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}

		}

	}

	// Process Bones

	if (mesh->HasBones())
	{
		ExtractBoneWeightForVertices(vertices, mesh, scene);
		LOG("Mesh with %i bones", mesh->mNumBones);
	}
	else {
		LOG("Mesh with no bones");
	}

	// Create the mesh

	std::string filename = std::to_string(rMeshUID) + ".ymesh";
	std::string libraryPath = External->fileSystem->libraryMeshesPath + filename;

	if (!PhysfsEncapsule::FileExists(libraryPath)) {

		External->fileSystem->SaveMeshToFile(vertices, indices, libraryPath);

	}

	if (!onlyReimport) {

		ResourceMesh* rMesh = nullptr;

		auto itr = External->resourceManager->resources.find(rMeshUID);

		if (itr == External->resourceManager->resources.end())
		{
			rMesh = (ResourceMesh*)External->resourceManager->CreateResourceFromLibrary(libraryPath, ResourceType::MESH, rMeshUID);
		}
		else
		{
			rMesh = static_cast<ResourceMesh*>(itr->second);
			itr->second->IncreaseReferenceCount();
		}

		CMesh* cmesh = new CMesh(linkGO);

		cmesh->rMeshReference = rMesh;

		cmesh->nVertices = vertices.size();
		cmesh->nIndices = indices.size();

		cmesh->InitBoundingBoxes();

		linkGO->AddComponent(cmesh);

	}

	// Process Texture Maps

	CMaterial* cmaterial = new CMaterial(linkGO);

	/*
	if (mesh->mMaterialIndex > 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		if (material->GetTextureCount(aiTextureType_DIFFUSE)) {

			aiString aiPath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath);

			std::string path = directory + aiPath.C_Str();

			std::unique_ptr<JsonFile> metaFile = JsonFile::GetJSON(path + ".meta");

			if (metaFile == nullptr) {

				ResourceTexture* rTexTemp = new ResourceTexture();

				ImporterTexture::Import(path, rTexTemp);

				RELEASE(rTexTemp);

				// Get meta

				std::unique_ptr<JsonFile> metaFile = JsonFile::GetJSON(path + ".meta");

				std::string libraryPath = metaFile->GetString("Library Path");
				uint UID = metaFile->GetInt("UID");
				TextureType type = ResourceTexture::GetTextureTypeFromName(metaFile->GetString("TextureType"));

				ResourceTexture* rTex = (ResourceTexture*)External->resourceManager->CreateResourceFromLibrary(libraryPath, ResourceType::TEXTURE, UID, type);

				switch (rTex->type)
				{
				case TextureType::DIFFUSE:
					cmaterial->diffuse_UID = UID;
					cmaterial->diffuse_ID = rTex->ID;
					cmaterial->diffuse_path = libraryPath;
					break;

				case TextureType::SPECULAR:
					cmaterial->specular_UID = UID;
					cmaterial->specular_ID = rTex->ID;
					cmaterial->specular_path = libraryPath;
					break;

				case TextureType::AMBIENT:
					cmaterial->ambient_UID = UID;
					cmaterial->ambient_ID = rTex->ID;
					cmaterial->ambient_path = libraryPath;
					break;

				case TextureType::EMISSIVE:
					cmaterial->emissive_UID = UID;
					cmaterial->emissive_ID = rTex->ID;
					cmaterial->emissive_path = libraryPath;
					break;

				case TextureType::HEIGHT:
					cmaterial->height_UID = UID;
					cmaterial->height_ID = rTex->ID;
					cmaterial->height_path = libraryPath;
					break;

				case TextureType::NORMAL:
					cmaterial->normal_UID = UID;
					cmaterial->normal_ID = rTex->ID;
					cmaterial->normal_path = libraryPath;
					break;

				default:
					cmaterial->diffuse_UID = UID;
					cmaterial->diffuse_ID = rTex->ID;
					cmaterial->diffuse_path = libraryPath;
					break;

				}

				cmaterial->rTextures.push_back(rTex);

			}
			else {

				// Get meta

				std::string libraryPath = metaFile->GetString("Library Path");
				uint UID = metaFile->GetInt("UID");
				TextureType type = ResourceTexture::GetTextureTypeFromName(metaFile->GetString("TextureType"));

				// Need to reimport?

				if (!PhysfsEncapsule::FileExists(libraryPath)) {

					ResourceTexture* rTexTemp = new ResourceTexture();
					ImporterTexture::Import(path, rTexTemp);
					RELEASE(rTexTemp);

				}

				auto itr = External->resourceManager->resources.find(UID);

				if (itr == External->resourceManager->resources.end())
				{
					ResourceTexture* rTexTemp = static_cast<ResourceTexture*>
						(External->resourceManager->CreateResourceFromLibrary(libraryPath.c_str(), ResourceType::TEXTURE, UID, type));

					switch (rTexTemp->type)
					{
					case TextureType::DIFFUSE:
						cmaterial->diffuse_UID = UID;
						cmaterial->diffuse_path = libraryPath;
						break;

					case TextureType::SPECULAR:
						cmaterial->specular_UID = UID;
						cmaterial->specular_path = libraryPath;
						break;

					case TextureType::AMBIENT:
						cmaterial->ambient_UID = UID;
						cmaterial->ambient_path = libraryPath;
						break;

					case TextureType::EMISSIVE:
						cmaterial->emissive_UID = UID;
						cmaterial->emissive_path = libraryPath;
						break;

					case TextureType::HEIGHT:
						cmaterial->height_UID = UID;
						cmaterial->height_path = libraryPath;
						break;

					case TextureType::NORMAL:
						cmaterial->normal_UID = UID;
						cmaterial->normal_path = libraryPath;
						break;

					default:
						cmaterial->diffuse_UID = UID;
						cmaterial->diffuse_path = libraryPath;
						break;

					}

					cmaterial->rTextures.push_back(rTexTemp);

				}
				else
				{
					ResourceTexture* rTexTemp = static_cast<ResourceTexture*>(itr->second);
					rTexTemp->type = type;
					itr->second->IncreaseReferenceCount();

					switch (rTexTemp->type)
					{
					case TextureType::DIFFUSE:
						cmaterial->diffuse_UID = UID;
						cmaterial->diffuse_path = libraryPath;
						break;

					case TextureType::SPECULAR:
						cmaterial->specular_UID = UID;
						cmaterial->specular_path = libraryPath;
						break;

					case TextureType::AMBIENT:
						cmaterial->ambient_UID = UID;
						cmaterial->ambient_path = libraryPath;
						break;

					case TextureType::EMISSIVE:
						cmaterial->emissive_UID = UID;
						cmaterial->emissive_path = libraryPath;
						break;

					case TextureType::HEIGHT:
						cmaterial->height_UID = UID;
						cmaterial->height_path = libraryPath;
						break;

					case TextureType::NORMAL:
						cmaterial->normal_UID = UID;
						cmaterial->normal_path = libraryPath;
						break;

					default:
						cmaterial->diffuse_UID = UID;
						cmaterial->diffuse_path = libraryPath;
						break;

					}

					cmaterial->rTextures.push_back(rTexTemp);

				}

			}
			
		}
		else {

			// Apply Checker Texture 

			ResourceTexture* rTex = new ResourceTexture();

			rTex->LoadCheckerImage();

			rTex->type = TextureType::DIFFUSE;

			cmaterial->diffuse_UID = rTex->UID;
			cmaterial->diffuse_path = "Checker Image";
			cmaterial->rTextures.push_back(rTex);

		}

		if (material->GetTextureCount(aiTextureType_SPECULAR)) {

			aiString aiPath;
			material->GetTexture(aiTextureType_SPECULAR, 0, &aiPath);

			std::string path = directory + aiPath.C_Str();

			//Texture tmpTexture;

			//tmpTexture.path = path;
			//tmpTexture.type = TextureTypes::SPECULAR;

			//textures.push_back(tmpTexture);

		}

		if (material->GetTextureCount(aiTextureType_NORMALS)) {
			 
			aiString aiPath;
			material->GetTexture(aiTextureType_NORMALS, 0, &aiPath);

			std::string path = directory + aiPath.C_Str();

			//Texture tmpTexture;

			//tmpTexture.path = path;
			//tmpTexture.type = TextureTypes::NORMALS;

			//textures.push_back(tmpTexture);

		}

		if (material->GetTextureCount(aiTextureType_HEIGHT)) {

			aiString aiPath;
			material->GetTexture(aiTextureType_HEIGHT, 0, &aiPath);

			std::string path = directory + aiPath.C_Str();

			//Texture tmpTexture;

			//tmpTexture.path = path;
			//tmpTexture.type = TextureTypes::HEIGHT;

			//textures.push_back(tmpTexture);

		}

		if (material->GetTextureCount(aiTextureType_AMBIENT)) {

			aiString aiPath;
			material->GetTexture(aiTextureType_AMBIENT, 0, &aiPath);

			std::string path = directory + aiPath.C_Str();

			//Texture tmpTexture;

			//tmpTexture.path = path;
			//tmpTexture.type = TextureTypes::AMBIENT;

			//textures.push_back(tmpTexture);

		}

		if (material->GetTextureCount(aiTextureType_EMISSIVE)) {

			aiString aiPath;
			material->GetTexture(aiTextureType_EMISSIVE, 0, &aiPath);

			std::string path = directory + aiPath.C_Str();

			//Texture tmpTexture;

			//tmpTexture.path = path;
			//tmpTexture.type = TextureTypes::EMISSIVE;

			//textures.push_back(tmpTexture);

		}

	}
	*/
	linkGO->AddComponent(cmaterial);
	
}

void Model::GenerateModelMetaFile()
{
	JsonFile modelMetaFile;

	modelMetaFile.SetString("Name", name.c_str());
	modelMetaFile.SetString("Assets Path", path.c_str());
	modelMetaFile.SetString("Library Path", (External->fileSystem->libraryModelsPath + std::to_string(modelGO->UID) + ".ymodel").c_str());
	modelMetaFile.SetInt("UID", modelGO->UID);
	modelMetaFile.SetString("Type", "Model");
	modelMetaFile.SetInt("Meshes num", embeddedMeshesUID.size());
	modelMetaFile.SetIntArray("Meshes Embedded UID", embeddedMeshesUID.data(), embeddedMeshesUID.size());
	modelMetaFile.SetInt("Resources num", resourcesUID.size());
	modelMetaFile.SetIntArray("Resources Embedded UID", resourcesUID.data(), resourcesUID.size());

	External->fileSystem->CreateMetaFileFromAsset(path, modelMetaFile);
}

void Model::GenerateNavMetaFile(const char* aPath, const char* lPath, unsigned int uid) {

	JsonFile modelMetaFile;

	

	modelMetaFile.SetString("Name", name.c_str());
	modelMetaFile.SetString("Assets Path", aPath);
	modelMetaFile.SetString("Library Path", (External->fileSystem->libraryModelsPath + std::to_string(modelGO->UID) + ".ymodel").c_str());
	modelMetaFile.SetInt("UID", modelGO->UID);
	modelMetaFile.SetString("Type", "Model");
	modelMetaFile.SetInt("Meshes num", embeddedMeshesUID.size());
	modelMetaFile.SetIntArray("Meshes Embedded UID", embeddedMeshesUID.data(), embeddedMeshesUID.size());



	External->fileSystem->CreateMetaFileFromAsset(aPath, modelMetaFile);

}

void Model::GenerateYmodelFile(const float3& translation, const float3& rotation, const float3& scale)
{
	JsonFile ymodelFile;

	ymodelFile.SetString("Name", name.c_str());
	ymodelFile.SetFloat3("Position", translation);
	ymodelFile.SetFloat3("Rotation", rotation);
	ymodelFile.SetFloat3("Scale", scale);
	ymodelFile.SetInt("UID", modelGO->UID);
	ymodelFile.SetInt("Parent UID", modelGO->mParent->UID);
	ymodelFile.SetIntArray("Children UID", embeddedMeshesUID.data(), embeddedMeshesUID.size());

	ymodelFile.CreateJSON(External->fileSystem->libraryModelsPath, std::to_string(modelGO->UID) + ".ymodel");
}

void Model::SetVertexBoneDataDefault(Vertex& vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
		vertex.boneIDs[i] = -1;
		vertex.weights[i] = 0.0f;
	}
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	// In theory if both boneIds and weights array macht each other, so we only need to check for available space in either or.
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
		if (vertex.boneIDs[i] == -1) {
			vertex.boneIDs[i] = boneID;
			vertex.weights[i] = weight;
			break;
		}
	}
}

void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
		int boneID = -1; 
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
			/*LOG("===== Bone with ID %i =====", boneCounter + 1);
			LOG("Name: %s", boneName.c_str());*/
			BoneInfo newBoneInfo; 
			newBoneInfo.id = boneCounter; 

			// Copy Transform Matrix
			float4x4 m;

			m.At(0, 0) = mesh->mBones[boneIndex]->mOffsetMatrix.a1;	m.At(0, 1) = mesh->mBones[boneIndex]->mOffsetMatrix.a2;	m.At(0, 2) = mesh->mBones[boneIndex]->mOffsetMatrix.a3;	m.At(0, 3) = mesh->mBones[boneIndex]->mOffsetMatrix.a4;
			m.At(1, 0) = mesh->mBones[boneIndex]->mOffsetMatrix.b1;	m.At(1, 1) = mesh->mBones[boneIndex]->mOffsetMatrix.b2;	m.At(1, 2) = mesh->mBones[boneIndex]->mOffsetMatrix.b3;	m.At(1, 3) = mesh->mBones[boneIndex]->mOffsetMatrix.b4;
			m.At(2, 0) = mesh->mBones[boneIndex]->mOffsetMatrix.c1;	m.At(2, 1) = mesh->mBones[boneIndex]->mOffsetMatrix.c2;	m.At(2, 2) = mesh->mBones[boneIndex]->mOffsetMatrix.c3;	m.At(2, 3) = mesh->mBones[boneIndex]->mOffsetMatrix.c4;
			m.At(3, 0) = mesh->mBones[boneIndex]->mOffsetMatrix.d1;	m.At(3, 1) = mesh->mBones[boneIndex]->mOffsetMatrix.d2;	m.At(3, 2) = mesh->mBones[boneIndex]->mOffsetMatrix.d3;	m.At(3, 3) = mesh->mBones[boneIndex]->mOffsetMatrix.d4;

			newBoneInfo.offset = m;

			/*LOG("Offset matrix:");
			LOG("%f %f %f %f", newBoneInfo.offset.At(0, 0), newBoneInfo.offset.At(0, 1), newBoneInfo.offset.At(0, 2), newBoneInfo.offset.At(0, 3));
			LOG("%f %f %f %f", newBoneInfo.offset.At(1, 0), newBoneInfo.offset.At(1, 1), newBoneInfo.offset.At(1, 2), newBoneInfo.offset.At(1, 3));
			LOG("%f %f %f %f", newBoneInfo.offset.At(2, 0), newBoneInfo.offset.At(2, 1), newBoneInfo.offset.At(2, 2), newBoneInfo.offset.At(2, 3));
			LOG("%f %f %f %f", newBoneInfo.offset.At(3, 0), newBoneInfo.offset.At(3, 1), newBoneInfo.offset.At(3, 2), newBoneInfo.offset.At(3, 3));*/

			boneInfoMap[boneName] = newBoneInfo;
			boneID = boneCounter;
			boneCounter++;
		}
		else {
			boneID = boneInfoMap[boneName].id;
		}
		assert(boneID != -1);

		int numWeights = mesh->mBones[boneIndex]->mNumWeights;
		
		/*LOG("Vertices that influences:");*/

		for (int weightIndex = 0; weightIndex < numWeights; weightIndex++) {
			int vertexID = mesh->mBones[boneIndex]->mWeights[weightIndex].mVertexId;
			float weight = mesh->mBones[boneIndex]->mWeights[weightIndex].mWeight;
			assert(vertexID <= vertices.size());
			SetVertexBoneData(vertices[vertexID], boneID, weight);

			/*LOG("-Vertex ID %i, %f influence", vertexID, weight);*/
			
		}
		///*LOG("=======================\n");*/
	}
}
