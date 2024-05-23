#include "ModuleScene.h"

#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleResourceManager.h"
#include "ResourceMesh.h"

#include "GameObject.h"
#include "Log.h"
#include "CCamera.h"
#include "TimeManager.h"

#include "ModuleFileSystem.h"
#include "PhysfsEncapsule.h"
#include "ModuleMonoManager.h"
#include "ModulePathfinding.h"
#include "CScript.h"

#include "External/Optick/include/optick.h"
#include "G_UI.h"

#include "ImporterMesh.h"

#include "External/mmgr/mmgr.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	LOG("Creating ModuleScene");

	mRootNode = CreateGameObject("Scene", nullptr);

	gameCameraObject = CreateGameObject("Main Camera", mRootNode);
	tags = { "Untagged" };

	isLocked = false;

	gameCameraComponent = nullptr;
	canvas = nullptr;
}

ModuleScene::~ModuleScene()
{

}

bool ModuleScene::Init()
{
	OPTICK_EVENT();

	bool ret = true;

	LOG("Loading scene");

	CAudioListener* audioListenerComponent = new CAudioListener(gameCameraObject);
	audioListenerComponent->SetAsDefaultListener();
	gameCameraObject->AddComponent(audioListenerComponent);

	CAudioSource* audioSourceComponent = new CAudioSource(gameCameraObject);
	gameCameraObject->AddComponent(audioSourceComponent);

	selectedGO = nullptr;
	godMode = false;

	onHoverUI = 0;
	selectedUIGO = nullptr;
	focusedUIGO = nullptr;
	canTab = true;
	canNav = true;

	return ret;
}

bool ModuleScene::Start()
{
	OPTICK_EVENT();

	currentSceneDir = "Assets";
#ifdef _RELEASE
	
	//LoadScene("Assets/Main Character", "Alpha3 test");
	//LoadScene("Assets/BASE_FINAL", "LVL_BASE_COLLIDERS");
	//LoadScene("Assets", "Enemigo player");
	//LoadScene("Assets/UI/Inventory", "InventoryScene");
	//LoadScene("Assets", "Enemigo player");
	//LoadScene("Assets/Test_Francesc", "TestPrefabs");
	//LoadScene("Assets", "Prueba enemigo lvl2");
	//LoadScene("Assets", "Pollo Loco");
	//LoadScene("Assets", "ParticleTest");
	//LoadScene("Assets/Prefabs", "Prueba de Pruebas");
	//LoadScene("Assets/UI/Scenes", "StartScene");
	//LoadScene("Assets/Camera", "CameraTesting");
	//LoadScene("Assets/CutScenes", "CutScenes");
	//LoadScene("Assets/Particles", "PlayerParticlesScene");

	// -------------------- FINAL MAPS -------------------- \\
	
	LoadScene("Assets/UI/Scenes", "StartScene");
	//LoadScene("Assets/BASE_FINAL", "LVL_BASE_COLLIDERS");
	//LoadScene("Assets/LVL1_FINAL", "LVL1_FINAL_COLLIDERS");
	//LoadScene("Assets/LVL2_LAB_PART1_FINAL", "LVL2_LAB_PART1_COLLIDERS");
	//LoadScene("Assets/LVL2_LAB_PART2_FINAL", "LVL2_LAB_PART2_COLLIDERS");
	//LoadScene("Assets/LVL3_BlockOut", "LVL3_PART1_COLLIDERS");
	//LoadScene("Assets/LVL3_BlockOut", "LVL3_BOSS_COLLDIERS");

	// ----------------- END OF FINAL MAPS ---------------- \\

#endif // _RELEASE

#ifdef _STANDALONE

	//LoadScene("Assets", "Alpha1_Level");
	//LoadScene("Assets", "ParticleTest");

	//LoadScene("Assets", "Prueba enemigo lvl2");
	//LoadScene("Assets", "Pollo Loco");
	//LoadScene("Assets/UI/Scenes", "StartScene");
	//LoadScene("Assets/BASE_FINAL", "LVL_BASE_COLLIDERS");
	//LoadScene("Assets/LVL1_FINAL", "LVL1_FINAL_COLLIDERS");
	//LoadScene("Assets/UI/Scenes", "StartScene");

	// -------------------- FINAL MAPS -------------------- \\
	
	LoadScene("Assets/UI/Scenes", "StartScene");
	//LoadScene("Assets/BASE_FINAL", "LVL_BASE_COLLIDERS");
	//LoadScene("Assets/LVL1_FINAL", "LVL1_FINAL_COLLIDERS");
	//LoadScene("Assets/LVL2_LAB_PART1_FINAL", "LVL2_LAB_PART1_COLLIDERS");
	//LoadScene("Assets/LVL2_LAB_PART2_FINAL", "LVL2_LAB_PART2_COLLIDERS");
	//LoadScene("Assets/LVL3_BlockOut", "LVL3_PART1_COLLIDERS");
	//LoadScene("Assets/LVL3_BlockOut", "LVL3_BOSS_COLLDIERS");

	// ----------------- END OF FINAL MAPS ---------------- \\

#endif // _STANDALONE

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
		if ((*it)->pendingToDelete) {
			destroyList.push_back((*it));
			continue;
		}

		if ((*it)->active)
		{
			(*it)->Update(dt);

			for (auto jt = (*it)->mComponents.begin(); jt != (*it)->mComponents.end(); ++jt) {

				if ((*jt)->active)(*jt)->Update();

			}
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		godMode = !godMode;
	}

	//if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) {

	//	ClearScene();

	//}

	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT) &&
		App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT &&
		TimeManager::gameTimer.GetState() == TimerState::STOPPED) {

		SaveScene(currentSceneDir, currentSceneFile);

	}

	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT) &&
		App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT &&
		TimeManager::gameTimer.GetState() == TimerState::STOPPED) {

		if (currentSceneFile != "")
		{
			//LoadScene(currentSceneDir, currentSceneFile);
			pendingToAddScene = currentSceneDir + "/" + currentSceneFile + ".yscene";
		}

	}

	// Delete GameObject
	if ((App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) &&
		!ImGui::GetIO().WantTextInput/* && !App->input->GetInputActive()*/)
	{
		// TODO: Sara --> multiple selection
		//node->mParent->DeleteChild(node);
	}

	// UI navigation
	//HandleUINavigation();

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{
	OPTICK_EVENT();

	if (!pendingToAddScene.empty())
	{
		// Obtener el nombre del archivo sin la extensi�n
		std::string name, path;
		PhysfsEncapsule::SplitFilePath(pendingToAddScene.c_str(), &path, &name, nullptr);

		//// Encontrar la posici�n del �ltimo separador de directorio
		//size_t lastSlashPos = pendingToAddScene.find_last_of("/\\");

		//// Si se encontr� el separador de directorio
		//if (lastSlashPos != std::string::npos) {
		//	// Eliminar el nombre del archivo y su extensi�n
		//	pendingToAddScene = pendingToAddScene.substr(0, lastSlashPos);
		//}

		//// Ahora path contiene el directorio sin el nombre del archivo y su extensi�n
		//std::string sceneFileName = name + ".yscene";
		//// Eliminar el nombre del archivo de la ruta completa
		//pendingToAddScene = pendingToAddScene.substr(0, pendingToAddScene.length() - sceneFileName.length());

		LoadScene(path, name);

		pendingToAddScene.clear();
	}

	if (!pendingToAddPrefab.empty())
	{
		for (const auto& tuple : pendingToAddPrefab)
		{
			GameObject* go = LoadPrefab(std::get<0>(tuple), std::get<1>(tuple));
			go->mTransform->SetPosition(std::get<2>(tuple));
		}
		pendingToAddPrefab.clear();
	}

	gameObjects.insert(gameObjects.end(), pendingToAdd.begin(), pendingToAdd.end());
	pendingToAdd.clear();

	/*Destroy gameobjects inside the destroy queue*/
	if (destroyList.size() > 0)
	{
		isLocked = false;
		SetSelected();
		for (size_t i = 0; i < destroyList.size(); ++i)
		{
			Destroy(destroyList[i]);

		}
		destroyList.clear();
	}

	/*swap gameobjects inside the swap queue*/
	if (swapList.size() > 0)
	{
		for (std::map<GameObject*, GameObject*>::iterator it = swapList.begin(); it != swapList.end(); ++it)
		{
			it->first->SwapChildren(it->second);
		}
		swapList.clear();
	}
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	OPTICK_EVENT();

	bool ret = true;

	LOG("Deleting scene");

	for (auto it = mFonts.rbegin(); it != mFonts.rend(); ++it)
	{
		RELEASE((*it));
	}
	
	RELEASE(mRootNode);
	ClearScene();

	return ret;
}

GameObject* ModuleScene::CreateGameObject(std::string name, GameObject* parent)
{
	std::string newName = GetUniqueName(name);

	// FRANCESC: MEMORY LEAK
	GameObject* tempGameObject = new GameObject(newName, parent);

	if (parent != nullptr) {

		parent->AddChild(tempGameObject);

	}

	gameObjects.push_back(tempGameObject);

	return tempGameObject;
}

std::string ModuleScene::GetUniqueName(std::string name)
{
	// Check if a Game Object with the same name exists
	bool exists = false;
	int counter = 0;

	// Iterate through existing game object names
	for (auto gameObject : gameObjects) {

		if (name == gameObject->name) {

			exists = true;

			break;

		}

	}

	// If the name already exists, rename it
	if (exists) {

		do {

			counter++;

			name = ReName(name, counter);

			exists = false;

			// Check if the new name already exists

			for (auto gameObject : gameObjects) {

				if (name == gameObject->name) {

					exists = true;

					break;

				}

			}

		} while (exists);

	}

	return name;
}

std::string ModuleScene::ReName(std::string name, uint counter)
{
	std::string uniqueName = name + " (" + std::to_string(counter) + ")";
	std::string newName;

	size_t first = uniqueName.find_first_of("(");

	newName = uniqueName.erase(first - 1);
	newName = uniqueName + " (" + std::to_string(counter) + ")";

	return newName;
}

GameObject* ModuleScene::PostUpdateCreateGameObject(std::string name, GameObject* parent)
{
	GameObject* tempGameObject = new GameObject(name, parent);

	if (parent != nullptr) {

		parent->AddChild(tempGameObject);

	}

	//Creo otro vector de game objects i en el postupdate del scene le meto un push en la lista
	pendingToAdd.push_back(tempGameObject);

	return tempGameObject;
}

void ModuleScene::PostUpdateCreateGameObject_UI(GameObject* go)
{
	pendingToAdd.push_back(go);
}

G_UI* ModuleScene::CreateGUI(UI_TYPE t, GameObject* pParent, int x, int y)
{
	G_UI* tempGameObject = new G_UI(t, pParent == nullptr ? App->scene->mRootNode : pParent, x, y);
	gameObjects.push_back(tempGameObject);

	return tempGameObject;
}

void ModuleScene::ClearScene()
{
	isLocked = false;
	SetSelected();

	focusedUIGO = nullptr;
	selectedUIGO = nullptr;

	External->physics->currentCollisions.clear();
	External->physics->previousCollisions.clear();

	External->lightManager->ClearLights(); // Done Correctly

	ClearVec(App->renderer3D->models); // Done Correctly

	RELEASE(mRootNode); // Done Correctly
	ClearVec(gameObjects); // Done Correctly
	ClearVec(destroyList); // Done Correctly
	ClearVec(tags); // Done Correctly

	// Recreate Physics World
	External->physics->DeleteWorld(); // Done Correctly
	External->physics->CreateWorld(); // Done Correctly

	ClearVec(vTempComponents); // Done Correctly
	ClearVec(vCanvas); // Done Correctly

	External->pathFinding->ClearNavMeshes(); // Done Correctly
}

void ModuleScene::SaveScene(const std::string& dir, const std::string& fileName)
{
	//char str[20];
	//sprintf(str, "%u", App->pathFinding->Save((dir + "/" + fileName).c_str()));
	std::string str = App->pathFinding->Save((dir + "/" + fileName).c_str());
	ysceneFile.SetString("NavMesh", str.c_str());
	ysceneFile.SetFloat3("Editor Camera Position", App->camera->editorCamera->GetPos());
	ysceneFile.SetFloat3("Editor Camera Right (X)", App->camera->editorCamera->GetRight());
	ysceneFile.SetFloat3("Editor Camera Up (Y)", App->camera->editorCamera->GetUp());
	ysceneFile.SetFloat3("Editor Camera Front (Z)", App->camera->editorCamera->GetFront());

	ysceneFile.SetHierarchy("Hierarchy", gameObjects);

	if (dir != External->fileSystem->libraryScenesPath)
	{
		App->scene->currentSceneDir = dir;
		App->scene->currentSceneFile = (fileName == "" ? std::to_string(mRootNode->UID) : fileName);

		ysceneFile.CreateJSON(dir + "/", App->scene->currentSceneFile + ".yscene");

		LOG("Scene '%s' saved to %s", App->scene->currentSceneFile.c_str(), App->scene->currentSceneDir.c_str());
	}
	else
	{
		ysceneFile.CreateJSON(dir, std::to_string(mRootNode->UID) + ".yscene");
	}

}

void ModuleScene::LoadScene(const std::string& dir, const std::string& fileName)
{
	OPTICK_EVENT();

	godMode = false; // Reset God Mode when scene changes

	if (dir != External->fileSystem->libraryScenesPath)
	{
		App->scene->currentSceneDir = dir;
		App->scene->currentSceneFile = (fileName == "" ? std::to_string(mRootNode->UID) : fileName);

		LOG("Scene '%s' loaded", App->scene->currentSceneFile.c_str(), App->scene->currentSceneDir.c_str());
	}

	CheckCurrentMap((dir + "/" + fileName + ".yscene").c_str());

	std::unique_ptr<JsonFile> sceneToLoad = JsonFile::GetJSON(dir + "/" + (fileName == "" ? std::to_string(mRootNode->UID) : fileName) + ".yscene");

	App->camera->editorCamera->SetPos(sceneToLoad->GetFloat3("Editor Camera Position"));
	App->camera->editorCamera->SetUp(sceneToLoad->GetFloat3("Editor Camera Up (Y)"));
	App->camera->editorCamera->SetFront(sceneToLoad->GetFloat3("Editor Camera Front (Z)"));

	uint deletedSceneUID = mRootNode->UID;

	ClearScene();

	mRootNode = CreateGameObject("Scene", nullptr); // Recreate scene
	mRootNode->UID = deletedSceneUID;

	sceneToLoad->GetHierarchyNoMemoryLeaks("Hierarchy");

	mRootNode = gameObjects[0];

	//for (int i = 0; i < gameObjects.size(); ++i) 
	//{
	//	gameObjects[i]->mTransform->UpdateGlobalMatrix();
	//}

	LoadScriptsData();

	const char* navMeshPath = sceneToLoad->GetNavMeshPath("NavMesh");

	if (navMeshPath != "")
	{
		External->pathFinding->Load(navMeshPath);
	}

}

void ModuleScene::SavePrefab(GameObject* prefab, const std::string& dir, const std::string& fileName)
{
	JsonFile* prefabFile = new JsonFile;

	prefabFile->SetPrefab("Prefab", *prefab);

	prefabFile->CreateJSON(dir + "/", fileName + ".yfab");

	LOG("Prefab '%s' saved to %s", fileName.c_str(), dir.c_str());

	delete prefabFile;
}




GameObject* ModuleScene::LoadPrefab(const std::string& dir, const std::string& fileName)
{
	TimeManager::gameTimer.Pause();
	if (!vTempComponents.empty())
	{
		ClearVec(vTempComponents);
	}
	
	std::unique_ptr<JsonFile> prefabToLoad = JsonFile::GetJSON(dir + "/" + fileName + ".yfab");

	std::vector<GameObject*> prefab = prefabToLoad->GetHierarchy("Prefab");

	// Add the loaded prefab objects to the existing gameObjects vector
	gameObjects.insert(gameObjects.begin(), prefab.begin(), prefab.end());

	LoadScriptsData();

	GameObject* rootObject = nullptr;

	for (GameObject* obj : gameObjects) {
		if (obj == prefab[0]) {
			// Se encontró el GameObject raíz del prefab
			rootObject = obj;
		}
	}


	LOG("Prefab '%s' loaded", fileName.c_str());

	ClearVec(prefab);
	TimeManager::gameTimer.Resume();
		
	return rootObject;
}


GameObject* ModuleScene::LoadPrefab( const char* path)
{
	ClearVec(vTempComponents);

	std::unique_ptr<JsonFile> prefabToLoad = JsonFile::GetJSON( path);

	// FRANCESC: Bug Hierarchy reimported GO when loading in Case 2
	std::vector<GameObject*> prefab = prefabToLoad->GetHierarchy("Prefab");

	// Add the loaded prefab objects to the existing gameObjects vector
	gameObjects.insert(gameObjects.begin(), prefab.begin(), prefab.end());

	LoadScriptsData();

	GameObject* rootObject = nullptr;

	for (GameObject* obj : gameObjects) {
		if (obj == prefab[0]) {
			// Se encontró el GameObject raíz del prefab
			rootObject = obj;
		}
	}


	LOG("Prefab '%s' loaded", path);

	ClearVec(prefab);

	return rootObject;
}

void ModuleScene::Destroy(GameObject* gm)
{
	for (std::vector<GameObject*>::iterator i = gm->mParent->mChildren.begin(); i != gm->mParent->mChildren.end(); ++i)
	{
		(*i._Ptr)->ClearReferences();

		if (*i._Ptr == gm)
		{
			gm->mParent->mChildren.erase(i);
			break;
		}
	}

	gm->mParent->mChildren.shrink_to_fit();

	RELEASE(gm);
}

//
std::vector<GameObject*>& ModuleScene::GetSelectedGOs()
{
	return vSelectedGOs;
}

void ModuleScene::SetSelected(GameObject* go)
{
	if (!isLocked)
	{
		if (go != nullptr)
		{
			// If ctrl not pressed, set everything to false clear and the selected go's vector 
			if (!ImGui::GetIO().KeyCtrl)
			{
				for (auto i = 0; i < vSelectedGOs.size(); i++)
				{
					SetSelectedState(vSelectedGOs[i], false);
				}
				ClearVec(vSelectedGOs);
			}

			// On click select or deselect item
			go->selected = !go->selected;

			// If the item was selected, add it to the vec, otherwise remove it
			if (go->selected)
			{
				selectedGO = go;

				vSelectedGOs.push_back(go);

				// Set selected go children to the same state as the clicked item
				SetSelectedState(go, go->selected);
			}
			else if (!vSelectedGOs.empty())
			{
				SetSelectedState(go, false);
				vSelectedGOs.erase(std::find(vSelectedGOs.begin(), vSelectedGOs.end(), go));
			}
		}
		else
		{
			selectedGO = nullptr;

			for (auto i = 0; i < vSelectedGOs.size(); i++)
			{
				SetSelectedState(vSelectedGOs[i], false);
			}

			ClearVec(vSelectedGOs);
		}
	}
}

void ModuleScene::SetSelectedState(GameObject* go, bool selected)
{
	// Must change go value manually. In "active" not necessary since it changes from the toggle

	if (go != nullptr)
	{
		go->selected = selected;

		for (auto i = 0; i < go->mChildren.size(); i++)
		{
			if (!go->mChildren.empty())
			{
				SetSelectedState(go->mChildren[i], selected);
			}

			go->mChildren[i]->selected = selected;

			if (go->mChildren[i]->selected)
			{
				vSelectedGOs.push_back(go->mChildren[i]);
			}
		}
	}
}

void ModuleScene::SetActiveRecursively(GameObject* gameObject, bool active)
{
	gameObject->active = active;
	if (!gameObject->active) {
		active = false;
		CScript* aux = static_cast<CScript*>(gameObject->GetComponent(ComponentType::SCRIPT));

		if (aux) {
			aux->isStarting = true;
		}

	}

	for (auto& child : gameObject->mChildren) {

		SetActiveRecursively(child, active);

	}
}

// Function to handle GameObject selection by Mouse Picking
void ModuleScene::HandleGameObjectSelection(const LineSegment& ray)
{
	// Map to store mesh candidates based on their distance to the ray origin.
	std::map<float, CMesh*> meshCandidates;

	// Iterate through all models in the 3D renderer.
	for (auto it = App->scene->gameObjects.begin(); it != App->scene->gameObjects.end(); ++it)
	{
		CMesh* meshToTest = (CMesh*)(*it)->GetComponent(ComponentType::MESH);

		// Variables to store the closest and furthest intersection distances.
		float closest;
		float furthest;

		if (meshToTest != nullptr)
		{
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
	std::vector<CMesh*> meshesSorted;

	// Populate the sorted vector based on the map.
	for (auto& candidate : meshCandidates) {

		meshesSorted.push_back(candidate.second);

	}

	// Set all meshes to unselected initially.
	/*for (CMesh* mesh : meshesSorted) {

		if (mesh != nullptr && mesh->mOwner != nullptr) {

			SetSelected();

		}

	}*/

	// Iterate through the sorted meshes to find the first intersection with the ray.
	for (CMesh* mesh : meshesSorted) {

		if (mesh != nullptr) {

			// Create a local copy of the ray to transform based on the mesh's transform.
			LineSegment localRay = ray;

			// Transform the ray using the mesh's transform.
			localRay.Transform(mesh->mOwner->mTransform->mGlobalMatrix.Inverted());

			// Iterate over triangles in the mesh.
			for (uint j = 0; j < mesh->rMeshReference->indices.size(); j += 3) {

				uint triangle_indices[3] = { mesh->rMeshReference->indices[j], mesh->rMeshReference->indices[j + 1], mesh->rMeshReference->indices[j + 2] };

				// Access mesh vertices.
				float3 point_a(mesh->rMeshReference->vertices[triangle_indices[0]].position);
				float3 point_b(mesh->rMeshReference->vertices[triangle_indices[1]].position);
				float3 point_c(mesh->rMeshReference->vertices[triangle_indices[2]].position);

				// Create a triangle from the vertices.
				Triangle triangle(point_a, point_b, point_c);

				// Check for intersection between the transformed ray and the triangle.
				if (localRay.Intersects(triangle, nullptr, nullptr)) {

					// Intersection found, set the selected object.
					if (mesh->mOwner != nullptr) {

						App->scene->SetSelected(mesh->mOwner);

						//// Iterate through all game objects in the scene.
						//for (auto it = App->scene->gameObjects.begin(); it != App->scene->gameObjects.end(); ++it) {

						//	// Unselect other game objects.
						//	if ((*it) != mesh->mOwner) {

						//		(*it)->selected = false;

						//	}

						//}

					}

					// Exit the function after the first intersection is found.
					return;

				}

			}

		}

	}

	SetSelected();

	//// No intersection found, clear the selection for all meshes.
	//for (auto it = App->scene->gameObjects.begin(); it != App->scene->gameObjects.end(); ++it) {

	//	(*it)->selected = false;

	//}

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

// GUI
void ModuleScene::SetCanvas(G_UI* newCanvas)
{
	canvas = newCanvas;
}

G_UI* ModuleScene::GetCanvas()
{
	return canvas;
}

//
GameObject* ModuleScene::GetGOFromUID(GameObject* n, uint sUID)
{
	if (n->UID == sUID)
		return n;

	GameObject* ret = nullptr;
	for (size_t i = 0; i < n->mChildren.size(); i++)
	{
		ret = GetGOFromUID(n->mChildren[i], sUID);
		if (ret != nullptr)
			return ret;
	}

	return nullptr;
}

void ModuleScene::ReplaceScriptsReferences(uint oldUID, uint newUID)
{
	std::multimap<uint, SerializedField*>::iterator referenceIt = referenceMap.find(oldUID);

	if (referenceIt != referenceMap.end())
	{
		AddToReferenceMap(newUID, referenceIt->second);
		referenceMap.erase(oldUID);
	}
}

void ModuleScene::AddToReferenceMap(uint UID, SerializedField* fieldToAdd)
{
	referenceMap.emplace(UID, fieldToAdd);
}

void ModuleScene::LoadScriptsData(GameObject* rootObject)
{
	for (auto i = referenceMap.begin(); i != referenceMap.end(); ++i)
	{
		// Get the range of the current key
		auto range = referenceMap.equal_range(i->first);

		// Now render out that whole range
		for (auto d = range.first; d != range.second; ++d)
		{
			//if (d->second->fiValue.goValue != nullptr)
				//continue;

			if (rootObject != nullptr)
			{
				GameObject* gameObject = GetGOFromUID(rootObject, d->first);

				if (gameObject != nullptr)
					d->second->fiValue.goValue = gameObject;
				else
					d->second->fiValue.goValue = GetGOFromUID(External->scene->mRootNode, d->first);
			}
			else
			{
				d->second->fiValue.goValue = GetGOFromUID(External->scene->mRootNode, d->first);
			}

			if (d->second->fiValue.goValue != nullptr)
			{
				//d->second->goUID = d->first;

				if (std::find(d->second->fiValue.goValue->csReferences.begin(), d->second->fiValue.goValue->csReferences.end(), d->second) == d->second->fiValue.goValue->csReferences.end())
					d->second->fiValue.goValue->csReferences.push_back(d->second);

				d->second->parentSC->SetField(d->second->field, d->second->fiValue.goValue);

				//d->second = nullptr;
			}
		}
	}

	//for (auto i = referenceMap.begin(); i != referenceMap.end(); ++i)
	//{
	//	if (i->second != nullptr)
	//	{
	//		referenceMapCopy.emplace(i->first, i->second);
	//	}
	//}

	//referenceMap = referenceMapCopy;

	referenceMap.clear();
}

void ModuleScene::GetUINavigate(GameObject* go, std::vector<C_UI*>& listgo)
{
	if (go->active)
	{
		for (auto i = 0; i < static_cast<G_UI*>(go)->mComponents.size(); i++)
		{
			if (static_cast<G_UI*>(go)->mComponents[i]->ctype == ComponentType::UI && static_cast<C_UI*>(static_cast<G_UI*>(go)->mComponents[i])->tabNav_ &&
				static_cast<C_UI*>(static_cast<G_UI*>(go)->mComponents[i])->state != UI_STATE::DISABLED)
			{
				listgo.push_back((C_UI*)static_cast<G_UI*>(go)->mComponents[i]);
			}
		}
	}

	if (!go->mChildren.empty())
	{
		for (auto i = 0; i < go->mChildren.size(); i++)
		{
			GetUINavigate(go->mChildren[i], listgo);
		}
	}
}

GameObject* ModuleScene::GetUISelected(GameObject* go)
{
	if (go->active)
	{
		if (!go->mChildren.empty())
		{
			for (auto i = 0; i < go->mChildren.size(); i++)
			{
				for (auto i = 0; i < static_cast<G_UI*>(go->mChildren[i])->mComponents.size(); i++)
				{
					if (static_cast<C_UI*>(static_cast<G_UI*>(go->mChildren[i])->mComponents[i])->state == UI_STATE::SELECTED)
					{
						return go;
					}
				}
			}
		}
	}

	return nullptr;
}

void ModuleScene::ResetSelected()
{
	// Get UI elements to navigate
	std::vector<C_UI*> listUI;

	for (int i = 0; i < vCanvas.size(); ++i)
	{
		GetUINavigate(vCanvas[i], listUI);
	}

	for (auto i = 0; i < listUI.size(); i++)
	{
		if (listUI[i]->state == UI_STATE::SELECTED)
		{
			listUI[i]->state = UI_STATE::NORMAL;
		}

	}
}


void ModuleScene::TabNavigate(bool isForward)
{
	if (TimeManager::gameTimer.GetState() == TimerState::RUNNING)
	{
		// Get UI elements to navigate
		std::vector<C_UI*> listUI;

		for (int i = 0; i < vCanvas.size(); ++i)
		{
			GetUINavigate(vCanvas[i], listUI);
		}

		if (!listUI.empty())
		{
			if (isForward)
			{
				if (onHoverUI == listUI.size() - 1)
				{
					SetSelected(listUI[0]->mOwner);

					focusedUIGO = listUI[0]->mOwner;

					if (listUI[onHoverUI]->state != UI_STATE::SELECTED)
					{
						listUI[onHoverUI]->SetState(UI_STATE::NORMAL);
					}

					if (listUI[0]->state != UI_STATE::SELECTED)
					{
						listUI[0]->SetState(UI_STATE::FOCUSED);
					}

					onHoverUI = 0;
				}

				else
				{
					SetSelected(listUI[onHoverUI + 1]->mOwner);

					focusedUIGO = listUI[onHoverUI + 1]->mOwner;

					if (listUI[onHoverUI]->state != UI_STATE::SELECTED)
					{
						listUI[onHoverUI]->SetState(UI_STATE::NORMAL);
					}

					if (listUI[onHoverUI + 1]->state != UI_STATE::SELECTED)
					{
						listUI[onHoverUI + 1]->SetState(UI_STATE::FOCUSED);
					}

					onHoverUI += 1;
				}
			}

			else
			{

				if (onHoverUI == 0)
				{
					SetSelected(listUI[listUI.size() - 1]->mOwner);
					focusedUIGO = listUI[listUI.size() - 1]->mOwner;

					if (listUI[onHoverUI]->state != UI_STATE::SELECTED)
					{
						listUI[onHoverUI]->SetState(UI_STATE::NORMAL);
					}

					if (listUI[listUI.size() - 1]->state != UI_STATE::SELECTED)
					{
						listUI[listUI.size() - 1]->SetState(UI_STATE::FOCUSED);
					}

					onHoverUI = listUI.size() - 1;
				}

				else
				{
					SetSelected(listUI[onHoverUI - 1]->mOwner);
					focusedUIGO = listUI[onHoverUI - 1]->mOwner;

					if (listUI[onHoverUI]->state != UI_STATE::SELECTED)
					{
						listUI[onHoverUI]->SetState(UI_STATE::NORMAL);
					}

					if (listUI[onHoverUI - 1]->state != UI_STATE::SELECTED)
					{
						listUI[onHoverUI - 1]->SetState(UI_STATE::FOCUSED);
					}

					onHoverUI -= 1;
				}
			}
		}
	}
}

void ModuleScene::HandleUINavigation()
{
	if (canNav)
	{
		if (!canTab && App->input->GetGamepadLeftJoystickPositionValueY() == 0)
		{
			canTab = true;
		}

		if ((App->input->GetGamepadLeftJoystickPositionValueY() < 0 && canTab) || App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
		{
			canTab = false;
			TabNavigate(false);
		}

		else if ((App->input->GetGamepadLeftJoystickPositionValueY() > 0 && canTab) || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
		{
			canTab = false;
			TabNavigate(true);
		}
	}
}

std::string ModuleScene::ComponentTypeToString(ComponentType type)
{
	switch (type) {
	case NONE: return "NONE";
	case TRANSFORM: return "TRANSFORM";
	case MESH: return "MESH";
	case MATERIAL: return "MATERIAL";
	case SCRIPT: return "SCRIPT";
	case AUDIO_SOURCE: return "AUDIO_SOURCE";
	case AUDIO_LISTENER: return "AUDIO_LISTENER";
	case CAMERA: return "CAMERA";
	case RIGIDBODY: return "RIGIDBODY";
	case PHYSICS: return "PHYSICS";
	case ANIMATION: return "ANIMATION";
	case PARTICLE: return "PARTICLE";
	case UI_TRAMSFORM: return "UI_TRAMSFORM";
	case UI: return "UI";
	case LIGHT: return "LIGHT";
	case NAVMESHAGENT: return "NAVMESHAGENT";
	case ALL_TYPES: return "ALL_TYPES";
	default: return "UNKNOWN";
	}
}

ComponentType ModuleScene::StringToComponentType(const std::string& typeName) {
	static std::unordered_map<std::string, ComponentType> typeMap = {
		{"NONE", NONE},
		{"TRANSFORM", TRANSFORM},
		{"MESH", MESH},
		{"MATERIAL", MATERIAL},
		{"SCRIPT", SCRIPT},
		{"AUDIO_SOURCE", AUDIO_SOURCE},
		{"AUDIO_LISTENER", AUDIO_LISTENER},
		{"CAMERA", CAMERA},
		{"RIGIDBODY", RIGIDBODY},
		{"PHYSICS", PHYSICS},
		{"ANIMATION", ANIMATION},
		{"PARTICLE", PARTICLE},
		{"UI_TRAMSFORM", UI_TRAMSFORM},
		{"UI", UI},
		{"LIGHT", LIGHT},
		{"NAVMESHAGENT", NAVMESHAGENT},
		{"ALL_TYPES", ALL_TYPES}
	};

	auto it = typeMap.find(typeName);
	if (it != typeMap.end()) {
		return it->second;
	}
	else {
		return NONE;
	}
}

void ModuleScene::CheckCurrentMap(const char* mapPath)
{
	// Hardcodeada para saber en que mapa estás 

	if (strcmp(mapPath, "Assets/BASE_FINAL/LVL_BASE_COLLIDERS.yscene") == 0 || strcmp(mapPath, "Assets/BASE_FINAL//LVL_BASE_COLLIDERS.yscene") == 0)
	{
		External->scene->currentMap = MAP::LVL_BASE;
	}
	else if (strcmp(mapPath, "Assets/LVL1_FINAL/LVL1_FINAL_COLLIDERS.yscene") == 0 || strcmp(mapPath, "Assets/LVL1_FINAL//LVL1_FINAL_COLLIDERS.yscene") == 0)
	{
		External->scene->currentMap = MAP::LVL_1;
	}
	else if (strcmp(mapPath, "Assets/LVL2_LAB_PART1_FINAL/LVL2_LAB_PART1_COLLIDERS.yscene") == 0 || strcmp(mapPath, "Assets/LVL2_LAB_PART1_FINAL//LVL2_LAB_PART1_COLLIDERS.yscene") == 0)
	{
		External->scene->currentMap = MAP::LVL_2_PART_1;
	}
	else if (strcmp(mapPath, "Assets/LVL2_LAB_PART2_FINAL/LVL2_LAB_PART2_COLLIDERS.yscene") == 0 || strcmp(mapPath, "Assets/LVL2_LAB_PART2_FINAL//LVL2_LAB_PART2_COLLIDERS.yscene") == 0)
	{
		External->scene->currentMap = MAP::LVL_2_PART_2;
	}
	else if (strcmp(mapPath, "Assets/LVL3_BlockOut/LVL3_PART1_COLLIDERS.yscene") == 0 || strcmp(mapPath, "Assets/LVL3_BlockOut//LVL3_PART1_COLLIDERS.yscene") == 0)
	{
		External->scene->currentMap = MAP::LVL_3_PART_1;
	}
	else if (strcmp(mapPath, "Assets/LVL3_BlockOut/LVL3_BOSS_COLLDIERS.yscene") == 0 || strcmp(mapPath, "Assets/LVL3_BlockOut//LVL3_BOSS_COLLDIERS.yscene") == 0)
	{
		External->scene->currentMap = MAP::LVL_3_PART_2;
	}
	else 
	{
		External->scene->currentMap = MAP::NO_MAP;
	}
}
