#include "ModuleScene.h"

#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"

#include "GameObject.h"
#include "Log.h"
#include "CCamera.h"
#include "TimeManager.h"

#include "ModuleFileSystem.h"
#include "PhysfsEncapsule.h"

#include "External/Optick/include/optick.h"

#include "ImporterMesh.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	// UID regenerated = duplication (This will be fixed with scene serialization)

	mRootNode = CreateGameObject("Scene", nullptr);
	mRootNode->UID = Random::Generate();

	gameCameraObject = CreateGameObject("Main Camera", mRootNode);
	gameCameraObject->UID = Random::Generate();

	LOG("Creating ModuleScene");
}

ModuleScene::~ModuleScene()
{
	delete mRootNode;
}

bool ModuleScene::Init()
{
	bool ret = true;

	LOG("Loading scene");

	cameras.push_back(App->camera->editorCamera);

	gameCameraComponent = new CCamera(gameCameraObject);

	// TODO: remove and do with proper constructor
	gameCameraObject->mTransform->SetPosition(float3(-40.0f, 29.0f, 54.0f));
	gameCameraObject->mTransform->SetRotation(float3(180.0f, 40.0f, 180.0f));

	//gameCameraComponent->SetPos(-40.0f, 29.0f, 54.0f);
	//gameCameraComponent->LookAt(float3(0.f, 0.f, 0.f));
	gameCameraComponent->SetAspectRatio(SCREEN_WIDTH / SCREEN_HEIGHT);

	gameCameraObject->AddComponent(gameCameraComponent);
	cameras.push_back(gameCameraComponent);

	// yscene file creation

	// You shouldn't save from default

	//ysceneFile.SetFloat3("Editor Camera Position", App->camera->editorCamera->GetPos());
	//ysceneFile.SetFloat3("Editor Camera Right (X)", App->camera->editorCamera->GetRight());
	//ysceneFile.SetFloat3("Editor Camera Up (Y)", App->camera->editorCamera->GetUp());
	//ysceneFile.SetFloat3("Editor Camera Front (Z)", App->camera->editorCamera->GetFront());
	//ysceneFile.SetHierarchy("Hierarchy", gameObjects);

	//ysceneFile.CreateJSON(External->fileSystem->libraryScenesPath, std::to_string(mRootNode->UID) + ".yscene");

	return ret;
}

bool ModuleScene::Start()
{
	// Hardcoded Scene To test Resource Manager
	// LoadSceneFromAssets("Assets/Scenes/TestScene.yscene"); // Baker House

	return false;
}

update_status ModuleScene::PreUpdate(float dt)
{
	OPTICK_EVENT();

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	OPTICK_EVENT();

	for (auto it = gameObjects.begin(); it != gameObjects.end(); ++it)
	{
		(*it)->Update();

		for (auto jt = (*it)->mComponents.begin(); jt != (*it)->mComponents.end(); ++jt) {

			(*jt)->Update();

		}

	}

	//if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) {

	//	ClearScene();

	//}

	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && TimeManager::gameTimer.GetState() == TimerState::STOPPED) {

		QuickSaveScene();

	}

	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT && TimeManager::gameTimer.GetState() == TimerState::STOPPED) {

		QuickLoadScene();

	}

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{
	OPTICK_EVENT();

	

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	bool ret = true;

	LOG("Deleting scene");

	return ret;
}

GameObject* ModuleScene::CreateGameObject(std::string name, GameObject* parent)
{
	GameObject* tempGameObject = new GameObject(name, parent);

	if (parent != nullptr) {

		parent->AddChild(tempGameObject);

	}

	gameObjects.push_back(tempGameObject);

	return tempGameObject;
}

void ModuleScene::DestroyGameObject(GameObject* toDestroy)
{
	if (toDestroy) {

		toDestroy->DestroyGameObject();

	}

}

void ModuleScene::ClearScene()
{
	//JsonFile::DeleteJSON(External->fileSystem->libraryScenesPath + std::to_string(mRootNode->UID) + ".yscene");

	uint deletedSceneUID = mRootNode->UID;

	App->editor->DestroyHierarchyTree(mRootNode);
	delete mRootNode;
	mRootNode = nullptr;

	gameObjects.clear();

	App->renderer3D->models.clear();
	mRootNode = CreateGameObject("Scene", nullptr); // Recreate scene
	mRootNode->UID = deletedSceneUID;
}

void ModuleScene::QuickSaveScene()
{
	ysceneFile.SetFloat3("Editor Camera Position", App->camera->editorCamera->GetPos());
	ysceneFile.SetFloat3("Editor Camera Right (X)", App->camera->editorCamera->GetRight());
	ysceneFile.SetFloat3("Editor Camera Up (Y)", App->camera->editorCamera->GetUp());
	ysceneFile.SetFloat3("Editor Camera Front (Z)", App->camera->editorCamera->GetFront());

	ysceneFile.SetHierarchy("Hierarchy", gameObjects);

	ysceneFile.CreateJSON(External->fileSystem->libraryScenesPath, std::to_string(mRootNode->UID) + ".yscene");

}

void ModuleScene::QuickLoadScene()
{
	JsonFile* sceneToLoad = JsonFile::GetJSON(External->fileSystem->libraryScenesPath + std::to_string(mRootNode->UID) + ".yscene");

	App->camera->editorCamera->SetPos(sceneToLoad->GetFloat3("Editor Camera Position"));
	App->camera->editorCamera->SetUp(sceneToLoad->GetFloat3("Editor Camera Up (Y)"));
	App->camera->editorCamera->SetFront(sceneToLoad->GetFloat3("Editor Camera Front (Z)"));

	ClearScene();

	gameObjects = sceneToLoad->GetHierarchy("Hierarchy");
	mRootNode = gameObjects[0];

	delete sceneToLoad;
}

void ModuleScene::LoadSceneFromAssets(std::string path)
{
	// 1. Create meta of the scene in assets

	JsonFile* tmpMetaFile = JsonFile::GetJSON(path + ".meta");

	if (tmpMetaFile == nullptr) {

		JsonFile* sceneToLoad = JsonFile::GetJSON(path);

		JsonFile sceneMetaFile;

		sceneMetaFile.SetString("Assets Path", path.c_str());
		sceneMetaFile.SetString("Library Path", (External->fileSystem->libraryScenesPath + std::to_string(sceneToLoad->GetHierarchy("Hierarchy")[0]->UID) + ".yscene").c_str());
		sceneMetaFile.SetInt("UID", sceneToLoad->GetHierarchy("Hierarchy")[0]->UID);
		sceneMetaFile.SetString("Type", "Scene");

		External->fileSystem->CreateMetaFileFromAsset(path, sceneMetaFile);

		tmpMetaFile = JsonFile::GetJSON(path + ".meta");

		delete sceneToLoad;

	}

	// 2. Create the scene in Library from the meta file of assets

	if (!PhysfsEncapsule::FileExists(External->fileSystem->libraryScenesPath + std::to_string(tmpMetaFile->GetInt("UID")) + ".yscene")) {

		std::string filePath;
		PhysfsEncapsule::DuplicateFile(path.c_str(), External->fileSystem->libraryScenesPath.c_str(), filePath);

		// Input string
		std::string input_string = path;

		// Find the position of the last '/' in the string
		size_t last_slash_position = input_string.find_last_of('/');

		// Extract the substring starting from the character after the last '/'
		std::string filename_with_extension = input_string.substr(last_slash_position + 1);

		// Find the position of the '.' in the filename
		size_t dot_position = filename_with_extension.find('.');

		// Extract the substring before the '.'
		std::string filename = filename_with_extension.substr(0, dot_position);

		PhysfsEncapsule::RenameFile((External->fileSystem->libraryScenesPath + filename + ".yscene"), (External->fileSystem->libraryScenesPath + std::to_string(tmpMetaFile->GetInt("UID")) + ".yscene"));

	}

	// 3. Load that scene from Library to the engine

	std::string libraryPath = tmpMetaFile->GetString("Library Path");
	JsonFile* sceneToLoad = JsonFile::GetJSON(libraryPath);

	App->camera->editorCamera->SetPos(sceneToLoad->GetFloat3("Editor Camera Position"));
	App->camera->editorCamera->SetUp(sceneToLoad->GetFloat3("Editor Camera Up (Y)"));
	App->camera->editorCamera->SetFront(sceneToLoad->GetFloat3("Editor Camera Front (Z)"));

	ClearScene();

	gameObjects = sceneToLoad->GetHierarchy("Hierarchy");
	mRootNode = gameObjects[0];

	delete tmpMetaFile;
	delete sceneToLoad;
}

// Function to handle GameObject selection by Mouse Picking
void ModuleScene::HandleGameObjectSelection(const LineSegment& ray)
{
	// Map to store mesh candidates based on their distance to the ray origin.
	std::map<float, Mesh*> meshCandidates;

	// Iterate through all models in the 3D renderer.
	for (auto it = App->renderer3D->models.begin(); it != App->renderer3D->models.end(); ++it) {

		// Iterate through all meshes in the current model.
		for (auto jt = (*it).meshes.begin(); jt != (*it).meshes.end(); ++jt) {

			Mesh* meshToTest = &(*jt);

			// Variables to store the closest and furthest intersection distances.
			float closest;
			float furthest;

			// Check for intersection between the ray and the global axis-aligned bounding box (AABB) of the mesh.
			if (ray.Intersects(meshToTest->globalAABB, closest, furthest)) {

				// Test if the mesh is inside another AABB (avoid Skybox selection).
				if (!IsInsideAABB(ray.a, meshToTest->globalAABB))
				{
					// Store the mesh in the map based on the closest intersection distance.
					meshCandidates[closest] = meshToTest;
				}

			}

		}

	}

	// Vector to store meshes sorted by their distance to the ray origin.
	std::vector<Mesh*> meshesSorted;

	// Populate the sorted vector based on the map.
	for (auto& candidate : meshCandidates) {

		meshesSorted.push_back(candidate.second);

	}

	// Set all meshes to unselected initially.
	for (Mesh* mesh : meshesSorted) {

		if (mesh != nullptr && mesh->meshGO != nullptr) {

			mesh->meshGO->selected = false;

		}

	}

	// Iterate through the sorted meshes to find the first intersection with the ray.
	for (Mesh* mesh : meshesSorted) {

		if (mesh != nullptr) {

			// Create a local copy of the ray to transform based on the mesh's transform.
			LineSegment localRay = ray;

			// Transform the ray using the mesh's transform.
			localRay.Transform(mesh->meshGO->mTransform->mGlobalMatrix.Inverted());

			// Iterate over triangles in the mesh.
			for (uint j = 0; j < mesh->indices.size(); j += 3) {

				uint triangle_indices[3] = { mesh->indices[j], mesh->indices[j + 1], mesh->indices[j + 2] };

				// Access mesh vertices.
				float3 point_a(mesh->vertices[triangle_indices[0]].position);
				float3 point_b(mesh->vertices[triangle_indices[1]].position);
				float3 point_c(mesh->vertices[triangle_indices[2]].position);

				// Create a triangle from the vertices.
				Triangle triangle(point_a, point_b, point_c);

				// Check for intersection between the transformed ray and the triangle.
				if (localRay.Intersects(triangle, nullptr, nullptr)) {

					// Intersection found, set the selected object.
					if (mesh->meshGO != nullptr) {

						mesh->meshGO->selected = true;

						// Iterate through all game objects in the scene.
						for (auto it = App->scene->gameObjects.begin(); it != App->scene->gameObjects.end(); ++it) {

							// Unselect other game objects.
							if ((*it) != mesh->meshGO) {

								(*it)->selected = false;

							}

						}

					}

					// Exit the function after the first intersection is found.
					return;

				}

			}

		}

	}

	// No intersection found, clear the selection for all meshes.
	for (auto it = App->scene->gameObjects.begin(); it != App->scene->gameObjects.end(); ++it) {

		(*it)->selected = false;

	}

}

bool ModuleScene::IsInsideAABB(const float3& point, const AABB& aabb)
{
	return point.x >= aabb.minPoint.x
		&& point.x <= aabb.maxPoint.x
		&& point.y >= aabb.minPoint.y
		&& point.y <= aabb.maxPoint.y
		&& point.z >= aabb.minPoint.z
		&& point.z <= aabb.maxPoint.z;
}