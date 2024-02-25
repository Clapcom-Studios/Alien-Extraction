#include "Model.h"
#include "Log.h"

#include "GameObject.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"

#include "ModuleFileSystem.h"
#include "PhysfsEncapsule.h"

#define STB_IMAGE_IMPLEMENTATION
#include "External/stb_image/stb_image.h"

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcessPreset_TargetRealtime_MaxQuality)

Model::Model()
{
	if (modelGO->active && External->scene->mRootNode && External->scene->mRootNode->active) {

		for (auto it = meshes.begin(); it != meshes.end(); ++it) {

			if ((*it).meshGO->active) {

				(*it).DrawMesh();

			}

		}

	}
}

Model::Model(const std::string& path, const std::string& shaderPath)
{
	LoadModel(path, shaderPath);
}

Model::~Model()
{

}

void Model::DrawModel()
{
	if (modelGO->active && External->scene->mRootNode && External->scene->mRootNode->active) {

		for (auto it = meshes.begin(); it != meshes.end(); ++it) {

			if ((*it).meshGO->active && External->renderer3D->IsInsideFrustum(External->scene->gameCameraComponent, (*it).globalAABB)) {

				(*it).DrawMesh();

			}
			
		}

	}
	
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
		ProcessNode(scene->mRootNode, scene, nullptr, shaderPath, -1);

		LOG("Model created: %s", name.c_str());

		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene %s", path.c_str());
	}

}

void Model::ProcessNode(aiNode* node, const aiScene* scene, GameObject* parentGO, const std::string& shaderPath, int iteration)
{
	// Retrieve transformation from Assimp

	aiVector3D translation, scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);

	NodeTransform tmpNodeTransform;

	tmpNodeTransform.translation = { translation.x, translation.y, translation.z };

	Quat rotQ(rotation.x, rotation.y, rotation.z, rotation.w);
	tmpNodeTransform.rotation = rotQ.ToEulerXYZ();

	tmpNodeTransform.scale = { scaling.x, scaling.y, scaling.z };

	// Link Assimp to GameObjects Hierarchy

	GameObject* currentNodeGO;

	if (parentGO == nullptr) {

		// If the current node is the root node, create here the model GameObject, parented to scene GameObject
		currentNodeGO = External->scene->CreateGameObject(name, External->scene->mRootNode);
		modelGO = currentNodeGO;

		JsonFile* tmpMetaFile = JsonFile::GetJSON(path + ".meta");

		if (tmpMetaFile) {

			// The meta file exists; it's not the first time we load the texture.
			modelGO->UID = tmpMetaFile->GetInt("UID");

			delete tmpMetaFile;

		}
		else {

			// The meta file doesn't exists; first time loading the texture.
			modelGO->UID = Random::Generate();

		}

	}
	else {

		// Create a GameObject for the current node and set it as a child of the parent GameObject
		currentNodeGO = External->scene->CreateGameObject(node->mName.C_Str(), parentGO);

		// Model Meta File and Library File Creation

		//JsonFile* tmpMetaFile = JsonFile::GetJSON(path + ".meta");

		//if (tmpMetaFile) {

			// The meta file exists; it's not the first time we load the texture.
			//currentNodeGO->UID = tmpMetaFile->GetIntArray("Meshes Embedded UID")[iteration];
			
			//delete tmpMetaFile;

		//}
		//else {

			// The meta file doesn't exists; first time loading the texture.
			currentNodeGO->UID = Random::Generate();

		//}

		embeddedMeshesUID.push_back(currentNodeGO->UID);

		GenerateModelMetaFile();

		GenerateYmodelFile(tmpNodeTransform.translation, tmpNodeTransform.rotation, tmpNodeTransform.scale);

	}

	// Process all the node's meshes (if any)

	for (uint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		meshes.push_back(ProcessMesh(mesh, scene, currentNodeGO, &tmpNodeTransform, shaderPath));
	}

	// Then do the same for each of its children

	for (uint i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, currentNodeGO, shaderPath, iteration);
	}

}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, GameObject* linkGO, NodeTransform* transform, const std::string& shaderPath)
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

	// Process Texture Maps

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		if (material->GetTextureCount(aiTextureType_DIFFUSE)) {

			aiString aiPath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath);

			std::string path = directory + aiPath.C_Str();

			Texture tmpTexture;

			tmpTexture.path = path;
			tmpTexture.type = TextureTypes::DIFFUSE;

			textures.push_back(tmpTexture);

		}

		if (material->GetTextureCount(aiTextureType_SPECULAR)) {

			aiString aiPath;
			material->GetTexture(aiTextureType_SPECULAR, 0, &aiPath);

			std::string path = directory + aiPath.C_Str();

			Texture tmpTexture;

			tmpTexture.path = path;
			tmpTexture.type = TextureTypes::SPECULAR;

			textures.push_back(tmpTexture);

		}

		if (material->GetTextureCount(aiTextureType_NORMALS)) {
			 
			aiString aiPath;
			material->GetTexture(aiTextureType_NORMALS, 0, &aiPath);

			std::string path = directory + aiPath.C_Str();

			Texture tmpTexture;

			tmpTexture.path = path;
			tmpTexture.type = TextureTypes::NORMALS;

			textures.push_back(tmpTexture);

		}

		if (material->GetTextureCount(aiTextureType_HEIGHT)) {

			aiString aiPath;
			material->GetTexture(aiTextureType_HEIGHT, 0, &aiPath);

			std::string path = directory + aiPath.C_Str();

			Texture tmpTexture;

			tmpTexture.path = path;
			tmpTexture.type = TextureTypes::HEIGHT;

			textures.push_back(tmpTexture);

		}

		if (material->GetTextureCount(aiTextureType_AMBIENT)) {

			aiString aiPath;
			material->GetTexture(aiTextureType_AMBIENT, 0, &aiPath);

			std::string path = directory + aiPath.C_Str();

			Texture tmpTexture;

			tmpTexture.path = path;
			tmpTexture.type = TextureTypes::AMBIENT;

			textures.push_back(tmpTexture);

		}

		if (material->GetTextureCount(aiTextureType_EMISSIVE)) {

			aiString aiPath;
			material->GetTexture(aiTextureType_EMISSIVE, 0, &aiPath);

			std::string path = directory + aiPath.C_Str();

			Texture tmpTexture;

			tmpTexture.path = path;
			tmpTexture.type = TextureTypes::EMISSIVE;

			textures.push_back(tmpTexture);

		}

	}

	// Create the mesh

	Mesh tmpMesh(vertices, indices, textures, linkGO, transform, shaderPath);

	CMesh* cmesh = new CMesh(linkGO);

	cmesh->meshReference = &tmpMesh;
	cmesh->nVertices = vertices.size();
	cmesh->nIndices = indices.size();

	linkGO->AddComponent(cmesh);

	JsonFile ymeshFile(External->fileSystem->libraryMeshesPath, std::to_string(linkGO->UID) + ".ymesh");

	External->fileSystem->SaveMeshToFile(&tmpMesh, External->fileSystem->libraryMeshesPath + std::to_string(linkGO->UID) + ".ymesh");

	return tmpMesh; // Retrieve the Mesh with all the necessary data to draw
}

void Model::GenerateModelMetaFile()
{
	JsonFile modelMetaFile;

	modelMetaFile.SetString("Assets Path", path.c_str());
	modelMetaFile.SetString("Library Path", (External->fileSystem->libraryModelsPath + std::to_string(modelGO->UID) + ".ymodel").c_str());
	modelMetaFile.SetInt("UID", modelGO->UID);
	modelMetaFile.SetString("Type", "Model");
	modelMetaFile.SetIntArray("Meshes Embedded UID", embeddedMeshesUID.data(), embeddedMeshesUID.size());

	External->fileSystem->CreateMetaFileFromAsset(path, modelMetaFile);
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
		vertex.boneIDs[i] = 1;
		vertex.weights[i] = 0.0f;
	}
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
		if (vertex.boneIDs[i] < 0) {
			vertex.boneIDs[i] = boneID;
			vertex.weights[i] = weight;
		}
	}
}

void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
		int boneID = -1; 
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
			BoneInfo newBoneInfo; 
			newBoneInfo.id = boneCounter; 

			newBoneInfo.offset.SetCol(0, float4(mesh->mBones[boneIndex]->mOffsetMatrix.a1,
				mesh->mBones[boneIndex]->mOffsetMatrix.b1,
				mesh->mBones[boneIndex]->mOffsetMatrix.c1,
				mesh->mBones[boneIndex]->mOffsetMatrix.d1));

			newBoneInfo.offset.SetCol(1, float4(mesh->mBones[boneIndex]->mOffsetMatrix.a2,
				mesh->mBones[boneIndex]->mOffsetMatrix.b2,
				mesh->mBones[boneIndex]->mOffsetMatrix.c2,
				mesh->mBones[boneIndex]->mOffsetMatrix.d2));

			newBoneInfo.offset.SetCol(2, float4(mesh->mBones[boneIndex]->mOffsetMatrix.a3,
				mesh->mBones[boneIndex]->mOffsetMatrix.b3,
				mesh->mBones[boneIndex]->mOffsetMatrix.c3,
				mesh->mBones[boneIndex]->mOffsetMatrix.d3));

			newBoneInfo.offset.SetCol(3, float4(mesh->mBones[boneIndex]->mOffsetMatrix.a4,
				mesh->mBones[boneIndex]->mOffsetMatrix.b4,
				mesh->mBones[boneIndex]->mOffsetMatrix.c4,
				mesh->mBones[boneIndex]->mOffsetMatrix.d4));

			boneInfoMap[boneName] = newBoneInfo;
			boneID = boneCounter;  
			boneCounter++;
		}
		else {
			boneID = boneInfoMap[boneName].id;
		}
		assert(boneID != -1);

		aiVertexWeight* weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;
		
		for (int weightIndex = 0; weightIndex < numWeights; weightIndex++) {
			int vertexID = weights->mVertexId;
			float weight = weights->mWeight; 
			assert(vertexID <= vertices.size());
			SetVertexBoneData(vertices[vertexID], boneID, weight);
		}
	}
}
