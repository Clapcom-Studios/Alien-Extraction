#pragma once

#include "Globals.h"
#include "Application.h"

#include "GameObject.h"
#include "ResourceMesh.h"

#include "CMesh.h"
#include "CScript.h"
#include "CTransform.h"

#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleResourceManager.h" 
#include "ModuleMonoManager.h"
#include "Resources.h"
#include "PhysfsEncapsule.h"
#include "MathGeoLib/include/Math/float3.h"

#include "External/mmgr/mmgr.h"

template<typename T>
T CS_CompToComp(MonoObject* obj)
{
	uintptr_t ptr = 0;
	MonoClass* goClass = mono_object_get_class(obj);
	const char* name = mono_class_get_name(goClass);

	mono_field_get_value(obj, mono_class_get_field_from_name(goClass, "pointer"), &ptr);

	return reinterpret_cast<T>(ptr);
}

//------//
MonoObject* Ymir_Box_Vector(MonoObject* obj, const char* type, bool global)	//Retorna la nueva posici�n del objeto
{
	//TODO: Quitar esto mas adelante, cuando est� arreglado el Transform

	if (External == nullptr)
		return nullptr;

	const char* name = mono_class_get_name(mono_object_get_class(obj));

	float3 value;
	CTransform* workTrans = CS_CompToComp<CTransform*>(obj);

	if (strcmp(type, "POSITION") == 0)
	{
		(global == true) ? value = workTrans->mGlobalMatrix.TranslatePart() : value = workTrans->translation;
	}
	else
	{
		(global == true) ? value = workTrans->mGlobalMatrix.GetScale() : value = workTrans->scale;
	}

	return External->moduleMono->Float3ToCS(value);
}

MonoObject* Ymir_Box_Quat(MonoObject* obj, bool global)	//Retorna la nueva rotaci�n del objeto
{
	//TODO: Quitar esto mas adelante, cuando est� arreglado el Transform

	if (External == nullptr)
		return nullptr;

	const char* name = mono_class_get_name(mono_object_get_class(obj));

	Quat value;
	GameObject* workGO = External->moduleMono->GameObject_From_CSGO(obj);

	CTransform* workTrans = CS_CompToComp<CTransform*>(obj);
	if (global = true) {
		float3 pos, scale;
		Quat globalRot;
		workTrans->mGlobalMatrix.Decompose(pos, globalRot, scale);
		value = globalRot;
	}
	else
	{
		value = workTrans->rotation;
	}


	return External->moduleMono->QuatToCS(value);
}

#pragma region Internals
//-------------------------------------------- Internals -----------------------------------------------//
void CSLog(MonoString* x)
{
	if (x == NULL)
		return;

	char* msg = mono_string_to_utf8(x);
	LOG(msg);
	mono_free(msg);
}

int GetKey(MonoObject* x)
{
	if (External != nullptr)
		return External->input->GetKey(*(int*)mono_object_unbox(x));

	return 0;
}

//Hardcoded para que solo sea KEY_DOWN y A
bool IsGamepadButtonAPressedCS()
{
	return External->input->IsGamepadButtonPressed(SDL_CONTROLLER_BUTTON_A, KEY_DOWN);
}

//Hardcoded para que solo sea KEY_DOWN y B
bool IsGamepadButtonBPressedCS()
{
	return External->input->IsGamepadButtonPressed(SDL_CONTROLLER_BUTTON_B, KEY_DOWN);
}

int GetMouseClick(MonoObject* x)
{
	if (External != nullptr)
		return External->input->GetMouseButton(*(int*)mono_object_unbox(x));

	return 0;
}

int MouseX()
{
	if (External != nullptr)
		return External->input->GetMouseXMotion();

	return 0;
}

int MouseY()
{
	if (External != nullptr)
		return External->input->GetMouseYMotion();

	return 0;
}

MonoObject* CreatePrefab(MonoString* prefabPath, MonoObject* position, MonoObject* rotation, MonoObject* scale)
{
	if (prefabPath == nullptr)
		return nullptr;

	 char* library_path = mono_string_to_utf8(prefabPath);
	GameObject* prefab_object =  External->scene->LoadPrefab(library_path);
	mono_free(library_path);

	if (prefab_object != nullptr)
	{
		CTransform* object_transform = dynamic_cast<CTransform*>(prefab_object->GetComponent(ComponentType::TRANSFORM));

		float3 posVector = ModuleMonoManager::UnboxVector(position);
		Quat rotQuat = ModuleMonoManager::UnboxQuat(rotation);

		float3 scaleVector;
		if (scale != nullptr)
			scaleVector = ModuleMonoManager::UnboxVector(scale);
		else
			scaleVector = prefab_object->mTransform->scale;

		prefab_object->mTransform->SetPosition(posVector);

		prefab_object->mTransform->SetRotation(rotQuat);
		
	}

	return External->moduleMono->GoToCSGO(prefab_object);
}

void CSCreateGameObject(MonoObject* name, MonoObject* position)
{
	if (External == nullptr)
		return;

	char* p = mono_string_to_utf8(mono_object_to_string(name, NULL));
	GameObject* go = External->scene->PostUpdateCreateGameObject(p, External->scene->mRootNode);
	mono_free(p);

	float3 posVector = ModuleMonoManager::UnboxVector(position);

	go->mTransform->SetPosition(posVector);
	//go->mTransform->updateTransform = true;	//TODO: No tenemos la variable esta "updateTransform"
}

MonoObject* CS_GetComponent(MonoObject* ref, MonoString* type, int inputType)
{
	ComponentType sType = static_cast<ComponentType>(inputType);

	char* name = mono_string_to_utf8(type);
	Component* component = External->moduleMono->GameObject_From_CSGO(ref)->GetComponent(sType, name);
	mono_free(name);

	//assert(component != nullptr, "Trying to get a null component");
	if (component == nullptr)
		return nullptr;

	if (sType == ComponentType::SCRIPT)
		return mono_gchandle_get_target(dynamic_cast<CScript*>(component)->noGCobject);

	MonoClass* cmpClass = mono_object_get_class(ref);
	MonoObject* ret = mono_object_new(External->moduleMono->domain, cmpClass);

	//Get type from unity

	//Get type
	MonoClassField* field = mono_class_get_field_from_name(cmpClass, "pointer");

	uintptr_t goPtr = reinterpret_cast<uintptr_t>(component);
	mono_field_set_value(ret, field, &goPtr);

	return ret;
}

GameObject* CS_Comp_To_GameObject(MonoObject* component)
{
	uintptr_t ptr = 0;
	MonoClass* goClass = mono_object_get_class(component);

	mono_field_get_value(component, mono_class_get_field_from_name(goClass, "pointer"), &ptr);

	return reinterpret_cast<Component*>(ptr)->mOwner;
}

MonoObject* CS_Component_Get_GO(MonoObject* thisRef)
{
	return External->moduleMono->GoToCSGO(CS_Comp_To_GameObject(thisRef));
}

MonoString* Get_GO_Name(MonoObject* go)
{
	if (External == nullptr)
		return nullptr;

	return mono_string_new(External->moduleMono->domain, External->moduleMono->GameObject_From_CSGO(go)->name.c_str());
}

MonoObject* FindObjectWithUID(int id)
{
	return External->moduleMono->GoToCSGO(External->scene->mRootNode->FindChild(id));
}

MonoObject* FindObjectWithName(MonoString* name) {

	std::vector<GameObject*> gameObjectVec;
	External->scene->mRootNode->CollectChilds(gameObjectVec);

	if (name == NULL) {
		assert("The name you passed is null. >:/");
		return nullptr;
	}

	char* _name = mono_string_to_utf8(name);


	for (int i = 0; i < gameObjectVec.size(); i++) {

		if (strcmp(gameObjectVec[i]->name.c_str(), _name) == 0) {

			return External->moduleMono->GoToCSGO(gameObjectVec[i]);

		}

	}
	mono_free(_name);

	assert("The object you searched for doesn't exist. :/");

	return nullptr;
}

MonoObject* FindChildrenWithName(MonoObject* obj, MonoString* name) {

	std::vector<GameObject*> gameObjectVec;

	GameObject* GO = External->moduleMono->GameObject_From_CSGO(obj);
	GO->CollectChilds(gameObjectVec);

	if (name == NULL) {
		assert("The name you passed is null. >:/");
		return nullptr;
	}

	char* _name = mono_string_to_utf8(name);
	std::string _nameString = _name;

	for (int i = 0; i < gameObjectVec.size(); i++) {

		//size_t found = gameObjectVec[i]->name.find(_name);

		if (gameObjectVec[i]->name.find(_name) != std::string::npos) {

			mono_free(_name);
			return External->moduleMono->GoToCSGO(gameObjectVec[i]);
		}

	}

	mono_free(_name);

	assert("The object you searched for doesn't exist. :/");

	return nullptr;
}

MonoObject* FindEnemyGameObject(MonoObject* obj) {

	std::vector<GameObject*> gameObjectVec;

	GameObject* GO = External->moduleMono->GameObject_From_CSGO(obj);
	GameObject* parent = GO->mParent;
	parent->CollectChilds(gameObjectVec);

	GameObject* child = gameObjectVec[1];

	gameObjectVec.clear();

	if(child != nullptr) return External->moduleMono->GoToCSGO(child);

	assert("The object you searched for doesn't exist. :/");

	return nullptr;
}

MonoObject* FindHealthBarObject(MonoObject* obj) {

	std::vector<GameObject*> gameObjectVec;

	GameObject* GO = External->moduleMono->GameObject_From_CSGO(obj);
	GameObject* parent = GO->mParent;
	parent->CollectChilds(gameObjectVec);

	GameObject* go = gameObjectVec[2];
	gameObjectVec.clear();
	go->CollectChilds(gameObjectVec);
	GameObject* child = gameObjectVec[1];

	gameObjectVec.clear();

	if (child != nullptr) return External->moduleMono->GoToCSGO(child);

	assert("The object you searched for doesn't exist. :/");


	return nullptr;
}

MonoObject* CS_GetParent(MonoObject* obj)
{
	GameObject* go = External->moduleMono->GameObject_From_CSGO(obj);

	return External->moduleMono->GoToCSGO(go->mParent);
}

MonoObject* CS_GetChild(MonoObject* obj, int numberChild)
{
	GameObject* go = External->moduleMono->GameObject_From_CSGO(obj);

	return External->moduleMono->GoToCSGO(go->mChildren[numberChild]);
}

int CS_GetChildrenSize(MonoObject* obj)
{
	return External->moduleMono->GameObject_From_CSGO(obj)->mChildren.size();
}

bool CompareGameObjectsByUID(MonoObject* obj1, MonoObject* obj2)
{
	GameObject* go1 = External->moduleMono->GameObject_From_CSGO(obj1);
	GameObject* go2 = External->moduleMono->GameObject_From_CSGO(obj2);

	return go1->UID == go2->UID;
}

void SetImpulse(MonoObject* obj, MonoObject* vel) {

	if (External == nullptr)
		return;

	float3 omgItWorks = External->moduleMono->UnboxVector(vel);
	GameObject* cpp_gameObject = External->moduleMono->GameObject_From_CSGO(obj);
	CCollider* rigidbody = dynamic_cast<CCollider*>(cpp_gameObject->GetComponent(ComponentType::PHYSICS));

	if (rigidbody)
	{
		rigidbody->physBody->body->activate(true);
		rigidbody->physBody->body->applyCentralImpulse({ omgItWorks.x, omgItWorks.y,omgItWorks.z });
	}
}

void SetVelocity(MonoObject* obj, MonoObject* vel) {

	if (External == nullptr)
		return;

	float3 omgItWorks = External->moduleMono->UnboxVector(vel);
	GameObject* cpp_gameObject = External->moduleMono->GameObject_From_CSGO(obj);
	CCollider* rigidbody = dynamic_cast<CCollider*>(cpp_gameObject->GetComponent(ComponentType::PHYSICS));

	if (rigidbody)
	{
		rigidbody->physBody->body->activate(true);
		rigidbody->physBody->body->setLinearVelocity({ omgItWorks.x, omgItWorks.y,omgItWorks.z });
	}
}

void SetRotation(MonoObject* obj, MonoObject* vel) {

	if (External == nullptr)
		return;

	Quat omgItWorks = External->moduleMono->UnboxQuat(vel);
	GameObject* cpp_gameObject = External->moduleMono->GameObject_From_CSGO(obj);
	CCollider* rigidbody = dynamic_cast<CCollider*>(cpp_gameObject->GetComponent(ComponentType::PHYSICS));

	if (rigidbody)
	{
		rigidbody->physBody->SetRotation(omgItWorks);
	}
}

void SetPosition(MonoObject* obj, MonoObject* pos) {
	if (External == nullptr)
		return;

	float3 omgItWorks = External->moduleMono->UnboxVector(pos);
	GameObject* cpp_gameObject = External->moduleMono->GameObject_From_CSGO(obj);
	CCollider* rigidbody = dynamic_cast<CCollider*>(cpp_gameObject->GetComponent(ComponentType::PHYSICS));

	if (rigidbody)
	{
		rigidbody->physBody->SetPosition(omgItWorks);
	}
}

void SetColliderSize(MonoObject* obj, MonoObject* scale) {
	if (External == nullptr)
		return;

	float3 hopeItWorks = External->moduleMono->UnboxVector(scale);
	GameObject* cpp_gameObject = External->moduleMono->GameObject_From_CSGO(obj);
	CCollider* rigidbody = dynamic_cast<CCollider*>(cpp_gameObject->GetComponent(ComponentType::PHYSICS));

	if (rigidbody)
	{
		// ** Descomentar para escalar todo el game object
		//rigidbody->mOwner->mTransform->SetScale(hopeItWorks);
		rigidbody->GetShape()->setLocalScaling(btVector3(hopeItWorks.x, hopeItWorks.y, hopeItWorks.z));
	}
}

float3 GetColliderSize(MonoObject* obj) {

	float3 size = float3(0, 0, 0);

	if (External == nullptr)
		return size;

	GameObject* cpp_gameObject = External->moduleMono->GameObject_From_CSGO(obj);
	CCollider* rigidbody = dynamic_cast<CCollider*>(cpp_gameObject->GetComponent(ComponentType::PHYSICS));

	if (rigidbody)
		size = float3(rigidbody->shape->getLocalScaling());

	return size;
}


void ClearForces(MonoObject* obj) {

	if (External == nullptr)
		return;

	GameObject* cpp_gameObject = External->moduleMono->GameObject_From_CSGO(obj);
	CCollider* rigidbody = dynamic_cast<CCollider*>(cpp_gameObject->GetComponent(ComponentType::PHYSICS));

	if (rigidbody)
	{
		rigidbody->physBody->body->clearForces();
		rigidbody->physBody->body->getTotalTorque();
		rigidbody->physBody->body->setLinearVelocity(btVector3(0, 0, 0));
		rigidbody->physBody->body->setAngularVelocity(btVector3(0, 0, 0));
	}

}

//bool Raycast(MonoObject* obj, MonoObject* origin, MonoObject* direction, float rayLenght, std::vector<float3> hitPoints, std::vector<MonoObject*> hits) {
//
//	if (External == nullptr)
//		return;
//
//	GameObject* cpp_gameObject = External->moduleMono->GameObject_From_CSGO(obj);
//
//	float3 pOrigin = External->moduleMono->UnboxVector(origin);
//
//	btVector3 fOrigin;
//
//	External->physics->Raycast(origin);
//
//}

MonoObject* RaycastHit(MonoObject* obj, MonoObject* origin, MonoObject* direction, float rayLenght) {

	if (External == nullptr)
		return false;

	GameObject* cpp_gameObject = External->moduleMono->GameObject_From_CSGO(obj);

	float3 pOrigin = External->moduleMono->UnboxVector(origin);

	btVector3 fOrigin;

	fOrigin.setX(pOrigin.x);
	fOrigin.setY(pOrigin.y);
	fOrigin.setZ(pOrigin.z);

	float3 pdirection = External->moduleMono->UnboxVector(direction);

	btVector3 fdirection;

	fdirection.setX(pdirection.x);
	fdirection.setY(pdirection.y);
	fdirection.setZ(pdirection.z);

	return External->moduleMono->GoToCSGO(External->physics->RaycastHit(fOrigin, fdirection, rayLenght));
}

bool RaycastTest(MonoObject* obj, MonoObject* origin, MonoObject* direction, float rayLenght) {

	if (External == nullptr)
		return false;

	GameObject* cpp_gameObject = External->moduleMono->GameObject_From_CSGO(obj);

	float3 pOrigin = External->moduleMono->UnboxVector(origin);

	btVector3 fOrigin;

	fOrigin.setX(pOrigin.x);
	fOrigin.setY(pOrigin.y);
	fOrigin.setZ(pOrigin.z);

	float3 pdirection = External->moduleMono->UnboxVector(direction);

	btVector3 fdirection;

	fdirection.setX(pdirection.x);
	fdirection.setY(pdirection.y);
	fdirection.setZ(pdirection.z);

	return External->physics->RaycastTest(fOrigin, fdirection, rayLenght);
}

void SetColliderActive(MonoObject* obj, bool isActive)
{
	if (External == nullptr)
		return;

	GameObject* cpp_gameObject = External->moduleMono->GameObject_From_CSGO(obj);
	CCollider* rigidbody = dynamic_cast<CCollider*>(cpp_gameObject->GetComponent(ComponentType::PHYSICS));

	rigidbody->isActive = isActive;
}

MonoObject* SendPosition(MonoObject* obj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	//return mono_value_box(External->moduleMono->domain, vecClass, External->moduleMono->Float3ToCS(C_Script::runningScript->GetGO()->transform->position)); //Use this method to send "object" types
	return Ymir_Box_Vector(obj, "POSITION", false); //Use this method to send class types
}

void RecievePosition(MonoObject* obj, MonoObject* secObj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	if (External == nullptr)
		return;

	float3 omgItWorks = External->moduleMono->UnboxVector(secObj);
	CTransform* workTrans = CS_CompToComp<CTransform*>(obj); //TODO IMPORTANT: First parameter is the object reference, use that to find UID
	if (workTrans)
	{
		workTrans->SetPosition(omgItWorks);
	}
}

MonoObject* GetForward(MonoObject* go)
{
	if (External == nullptr)
		return nullptr;

	CTransform* trans = CS_CompToComp<CTransform*>(go);

	MonoClass* vecClass = mono_class_from_name(External->moduleMono->image, YMIR_SCRIPTS_NAMESPACE, "Vector3");
	return External->moduleMono->Float3ToCS(trans->GetForward());
}

MonoObject* GetRight(MonoObject* go)
{
	if (External == nullptr)
		return nullptr;

	CTransform* trans = CS_CompToComp<CTransform*>(go);

	MonoClass* vecClass = mono_class_from_name(External->moduleMono->image, YMIR_SCRIPTS_NAMESPACE, "Vector3");
	return External->moduleMono->Float3ToCS(trans->GetRight());
}

MonoObject* GetUp(MonoObject* go)
{
	if (External == nullptr)
		return nullptr;

	CTransform* trans = CS_CompToComp<CTransform*>(go);

	MonoClass* vecClass = mono_class_from_name(External->moduleMono->image, YMIR_SCRIPTS_NAMESPACE, "Vector3");
	return External->moduleMono->Float3ToCS(trans->GetUp());
}

MonoObject* GetLocalForward(MonoObject* go)
{
	if (External == nullptr)
		return nullptr;

	CTransform* trans = CS_CompToComp<CTransform*>(go);

	MonoClass* vecClass = mono_class_from_name(External->moduleMono->image, YMIR_SCRIPTS_NAMESPACE, "Vector3");
	return External->moduleMono->Float3ToCS(trans->GetLocalForward());
}

MonoObject* GetLocalRight(MonoObject* go)
{
	if (External == nullptr)
		return nullptr;

	CTransform* trans = CS_CompToComp<CTransform*>(go);

	MonoClass* vecClass = mono_class_from_name(External->moduleMono->image, YMIR_SCRIPTS_NAMESPACE, "Vector3");
	return External->moduleMono->Float3ToCS(trans->GetLocalRight());
}

MonoObject* GetLocalUp(MonoObject* go)
{
	if (External == nullptr)
		return nullptr;

	CTransform* trans = CS_CompToComp<CTransform*>(go);

	MonoClass* vecClass = mono_class_from_name(External->moduleMono->image, YMIR_SCRIPTS_NAMESPACE, "Vector3");
	return External->moduleMono->Float3ToCS(trans->GetLocalUp());
}

MonoObject* SendRotation(MonoObject* obj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	return Ymir_Box_Quat(obj, false); //Use this method to send class types
}

void RecieveRotation(MonoObject* obj, MonoObject* secObj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	if (External == nullptr)
		return;

	Quat omgItWorks = External->moduleMono->UnboxQuat(secObj);
	CTransform* transform = CS_CompToComp<CTransform*>(obj); //TODO IMPORTANT: First parameter is the object reference, use that to find UID

	if (transform)
	{
		//workGO->transform->SetTransformMatrix(workGO->transform->position, omgItWorks, workGO->transform->localScale);

		transform->rotation = omgItWorks.Normalized();
		transform->eulerRot = omgItWorks.ToEulerXYZ() * RADTODEG;

		transform->dirty_ = true;
	}
}

MonoObject* SendScale(MonoObject* obj)
{
	return Ymir_Box_Vector(obj, "SCALE", false);
}

void RecieveScale(MonoObject* obj, MonoObject* secObj)
{
	if (External == nullptr)
		return;

	float3 omgItWorks = External->moduleMono->UnboxVector(secObj);
	CTransform* transform = CS_CompToComp<CTransform*>(obj); //TODO IMPORTANT: First parameter is the object reference, use that to find UID

	if (transform)
	{
		transform->SetScale(omgItWorks);
	}
}

void SetActive(MonoObject* obj, bool active)
{
	if (External == nullptr)
		return;

	GameObject* go = External->moduleMono->GameObject_From_CSGO(obj);
	go->active = active;
	if (!go->active) {
		active = false;
		CScript* aux = static_cast<CScript*>(go->GetComponent(ComponentType::SCRIPT));

		if (aux) {
			aux->isStarting = true;
		}

	}
	External->scene->SetActiveRecursively(go, active);
}

void ExitGame()
{
	External->editor->exit = update_status::UPDATE_STOP;

	LOG("Exit game");
}

void LoadSceneCS(MonoString* scenePath)
{
	char* _path = mono_string_to_utf8(scenePath);
	External->scene->pendingToAddScene = _path;

	External->scene->CheckCurrentMap(_path);
}

void Destroy(MonoObject* go)
{
	if (go == NULL)
		return;

	MonoClass* klass = mono_object_get_class(go);
	//const char* name = mono_class_get_name(klass);

	GameObject* workGO = External->moduleMono->GameObject_From_CSGO(go);
	//GameObject* workGO = C_Script::runningScript->GetGO();
	if (workGO == nullptr) {
		LOG("[ERROR] Can't delete GameObject from CS");
		return;
	}

	workGO->DestroyGameObject();
}

float GetDT()
{
	//IDK if this would work
	return External->GetDT();
}

float GetTimeCS()
{
	return TimeManager::gameTimer.ReadSec();
}

void CreateBullet(MonoObject* position, MonoObject* rotation, MonoObject* scale)
{
	//Crea un game object temporal llamado "Bullet"
	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject("Bullet", External->scene->mRootNode);
	go->UID = Random::Generate();

	//Hace unbox de los parametros de transform pasados
	float3 posVector = External->moduleMono->UnboxVector(position);
	Quat rotVector = External->moduleMono->UnboxQuat(rotation);
	float3 scaleVector = External->moduleMono->UnboxVector(scale);

	//Settea el transform a la bullet
	go->mTransform->SetPosition(posVector);
	go->mTransform->rotation = rotVector.Normalized();
	go->mTransform->SetScale(scaleVector);

	uint UID = 1728623793; // UID of Cube.fbx mesh in meta (lo siento)

	std::string libraryPath = External->fileSystem->libraryMeshesPath + std::to_string(UID) + ".ymesh";

	//if (!PhysfsEncapsule::FileExists(libraryPath)) {

	//	External->resourceManager->ImportFile("Assets/Primitives/Cube.fbx", true);

	//}

	//Añade la mesh a la bullet
	//ResourceMesh* rMesh = (ResourceMesh*)(External->resourceManager->CreateResourceFromLibrary(libraryPath, ResourceType::MESH, UID));
	//CMesh* cmesh = new CMesh(go);
	//cmesh->rMeshReference = rMesh;
	//go->AddComponent(cmesh);

	//Añade el material a la Bullet
	CMaterial* cmaterial = new CMaterial(go);
	cmaterial->shaderPath = SHADER_VS_FS;
	cmaterial->shader.LoadShader(cmaterial->shaderPath);
	cmaterial->shaderDirtyFlag = false;
	go->AddComponent(cmaterial);

	//Añade RigidBody a la bala
	CCollider* physBody;
	physBody = new CCollider(go);
	physBody->useGravity = false;
	//physBody->size = scaleVector;
	physBody->size = float3(1, 1, 1);
	physBody->physBody->SetPosition(posVector);
	go->AddComponent(physBody);

	CParticleSystem* partSys = new CParticleSystem(go);
	ParticleEmitter* emitter = new ParticleEmitter(partSys);

	emitter->Init(partSys);
	EmitterPosition* ePos = (EmitterPosition*)emitter->CreateEmitterSettingByType(EmitterType::PAR_POSITION);

	ePos->particleSpeed1 = 5;
	ePos->particleSpeed2 = 10;
	ePos->direction1 = float3(0, 0, 0);
	ePos->direction2 = float3(0, 5, 0);

	go->AddComponent(partSys);

	//Añade el script Bullet al gameObject Bullet
	const char* t = "BH_Bullet";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);
}

void CreateShotgunSensor(MonoObject* position, MonoObject* rotation, float lenght, float width, MonoObject* right) {

	//Crea un game object temporal llamado "ShotgunBullet"
	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject("ShotgunBullet", External->scene->mRootNode);
	go->UID = Random::Generate();
	go->tag = "ShotgunBullet";

	//Hace unbox de los parametros de transform pasados
	float3 posVector = External->moduleMono->UnboxVector(position);

	float3 rightVector = External->moduleMono->UnboxVector(right);

	//Funciona pero es rarete
	Quat rotVector = External->moduleMono->UnboxQuat(rotation);
	//rotVector = rotVector.RotateAxisAngle(rightVector, DegToRad(-90.0f));


	//float3 scaleVector = float3(15, 70, 15);
	float3 scaleVector = float3(width, lenght, width);

	//Añade RigidBody a la bala
	CCollider* physBody;
	physBody = new CCollider(go, CONE);
	physBody->useGravity = false;
	physBody->physBody->SetPosition(posVector);
	physBody->physBody->SetRotation(rotVector.Normalized());
	physBody->SetAsSensor(true);

	go->AddComponent(physBody);
	physBody->physBody->body->activate(true);
	physBody->size = scaleVector;
	physBody->shape->setLocalScaling(btVector3(scaleVector.x, scaleVector.y, scaleVector.z));

	//Añade el script AcidicSpit al gameObject go
	Component* c = nullptr;
	c = new CScript(go, "BH_Shotgun");
	go->AddComponent(c);
}

void CreateTailSensor(MonoObject* position, MonoObject* rotation)
{
	//Crea un game object temporal llamado "Bullet"
	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject("Tail", External->scene->mRootNode);
	go->UID = Random::Generate();
	go->tag = "Tail";

	//Hace unbox de los parametros de transform pasados
	float3 posVector = External->moduleMono->UnboxVector(position);
	//Funciona pero es rarete
	//Quat rotVector = External->moduleMono->UnboxQuat(rotation);
	Quat rotVector = Quat(0,0,0,0);
	float3 scaleVector = float3(50, 50, 50);


	//Añade RigidBody a la bala
	CCollider* physBody;
	physBody = new CCollider(go, CYLINDER);
	//physBody = new CCollider(go);
	physBody->useGravity = false;
	physBody->physBody->SetPosition(posVector);
	physBody->physBody->SetRotation(rotVector.Normalized());
	physBody->SetAsSensor(true);

	go->AddComponent(physBody);
	physBody->physBody->body->activate(true);
	physBody->size = scaleVector;
	physBody->shape->setLocalScaling(btVector3(scaleVector.x, scaleVector.y, scaleVector.z));

	//Añade el script Tail al gameObject Bullet
	const char* t = "BH_Tail";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);
}

void CreateAcidicSpit(MonoObject* name, MonoObject* position)
{
	float3 posVector = External->moduleMono->UnboxVector(position);
	float3 scaleVector = float3(2.0f, 2.0f, 2.0f);
	char* p = mono_string_to_utf8(mono_object_to_string(name, NULL));

	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject(p, External->scene->mRootNode);
	go->UID = Random::Generate();

	//Settea el transform a la bola de acido
	go->mTransform->SetPosition(posVector);
	go->mTransform->SetScale(scaleVector);

	uint UID = 826872905; // UID of Sphere.fbx mesh in meta (lo siento)
	std::string libraryPath = External->fileSystem->libraryMeshesPath + std::to_string(UID) + ".ymesh";

	//Añade la mesh a la bola de acido
	ResourceMesh* rMesh = (ResourceMesh*)(External->resourceManager->CreateResourceFromLibrary(libraryPath, ResourceType::MESH, UID));
	CMesh* cmesh = new CMesh(go);
	cmesh->rMeshReference = rMesh;
	go->AddComponent(cmesh);

	//Añade el material a la bola de acido
	CMaterial* cmaterial = new CMaterial(go);
	cmaterial->shaderPath = WATER_SHADER;
	cmaterial->shader.LoadShader(cmaterial->shaderPath);
	cmaterial->shaderDirtyFlag = false;
	go->AddComponent(cmaterial);

	//Añade RigidBody a la bola de acido

	//Antiguo
	//CCollider* physBody = new CCollider(go);
	//physBody->useGravity = false;
	//physBody->size = scaleVector;
	//physBody->physBody->SetPosition(posVector);
	//go->AddComponent(physBody);
	
	//Nuevo
	CCollider* physBody = new CCollider(go);
	physBody->useGravity = false;
	physBody->physBody->SetPosition(posVector);
	physBody->SetAsSensor(true);

	go->AddComponent(physBody);
	physBody->physBody->body->activate(true);
	physBody->size = scaleVector;
	physBody->shape->setLocalScaling(btVector3(scaleVector.x, scaleVector.y, scaleVector.z));

	//Añade el script AcidicSpit al gameObject go
	const char* t = "AcidicSpit";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);

}

void CreateAcidPuddle(MonoObject* name, MonoObject* position)
{
	float3 posVector = External->moduleMono->UnboxVector(position);
	float3 scaleVector = float3(3.0f, 1.0f, 3.0f);
	char* p = mono_string_to_utf8(mono_object_to_string(name, NULL));

	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject(p, External->scene->mRootNode);
	go->UID = Random::Generate();

	//Settea el transform a la bullet
	go->mTransform->SetPosition(posVector);
	go->mTransform->SetScale(scaleVector);

	//Antiguo
	//Añade RigidBody a la bala
	//CCollider* physBody = new CCollider(go);
	//physBody->useGravity = true;
	//physBody->size = scaleVector;
	//physBody->physBody->SetPosition(posVector);
	//go->AddComponent(physBody);

	CCollider* physBody = new CCollider(go, CYLINDER);
	physBody->useGravity = false;
	physBody->physBody->SetPosition(posVector);
	physBody->SetAsSensor(true);

	go->AddComponent(physBody);
	physBody->physBody->body->activate(true);
	physBody->size = scaleVector;
	physBody->shape->setLocalScaling(btVector3(scaleVector.x, scaleVector.y, scaleVector.z));

	//Añade el script Bullet al gameObject Bullet
	const char* t = "AcidPuddle";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);

}

void CreateSpitterAcidSpit(MonoObject* position, MonoObject* rotation)
{
	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject("SpitterAcidSpit", External->scene->mRootNode);
	go->UID = Random::Generate();
	go->tag = "SpitterAcidSpit";

	float3 posVector = External->moduleMono->UnboxVector(position);
	Quat rotVector = External->moduleMono->UnboxQuat(rotation);
	float3 scaleVector = float3(2.5f, 2.5f, 2.5f);

	go->mTransform->SetPosition(posVector);
	go->mTransform->SetScale(scaleVector);
	go->mTransform->SetRotation(rotVector);

	CCollider* physBody;
	physBody = new CCollider(go, SPHERE);

	physBody->useGravity = true;
	physBody->physBody->SetPosition(posVector);
	physBody->physBody->SetRotation(rotVector);
	physBody->SetAsSensor(true);

	go->AddComponent(physBody);
	physBody->physBody->body->activate(true);
	physBody->size = scaleVector;
	physBody->shape->setLocalScaling(btVector3(scaleVector.x, scaleVector.y, scaleVector.z));

	const char* t = "SpitterAcidSpit";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);
}

void CreateSpitterAcidExplosive(MonoObject* position, MonoObject* rotation)
{
	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject("SpitterAcidExplosion", External->scene->mRootNode);
	go->UID = Random::Generate();
	go->tag = "SpitterAcidExplosion";

	float3 posVector = External->moduleMono->UnboxVector(position);
	Quat rotVector = External->moduleMono->UnboxQuat(rotation);
	float3 scaleVector = float3(2.5f, 2.5f, 2.5f);

	go->mTransform->SetPosition(posVector);
	go->mTransform->SetScale(scaleVector);
	go->mTransform->SetRotation(rotVector);

	CCollider* physBody;
	physBody = new CCollider(go, SPHERE);

	physBody->useGravity = true;
	physBody->physBody->SetPosition(posVector);
	physBody->physBody->SetRotation(rotVector);
	physBody->SetAsSensor(true);

	go->AddComponent(physBody);
	physBody->physBody->body->activate(true);
	physBody->size = scaleVector;
	physBody->shape->setLocalScaling(btVector3(scaleVector.x, scaleVector.y, scaleVector.z));

	const char* t = "SpitterAcidExplosion";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);
}

void CreateSpitterAcidShrapnel(MonoObject* position, MonoObject* rotation)
{
	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject("SpitterAcidShrapnel", External->scene->mRootNode);
	go->UID = Random::Generate();
	go->tag = "SpitterAcidShrapnel";

	float3 posVector = External->moduleMono->UnboxVector(position);
	Quat rotVector = External->moduleMono->UnboxQuat(rotation);
	float3 scaleVector = float3(1.5f, 1.5f, 1.5f);

	go->mTransform->SetPosition(posVector);
	go->mTransform->SetScale(scaleVector);
	go->mTransform->SetRotation(rotVector);

	CCollider* physBody;
	physBody = new CCollider(go, SPHERE);

	physBody->useGravity = true;
	physBody->physBody->SetPosition(posVector);
	physBody->physBody->SetRotation(rotVector);
	physBody->SetAsSensor(true);

	go->AddComponent(physBody);
	physBody->physBody->body->activate(true);
	physBody->size = scaleVector;
	physBody->shape->setLocalScaling(btVector3(scaleVector.x, scaleVector.y, scaleVector.z));

	const char* t = "SpitterAcidShrapnel";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);
}

void CreateFaceHuggerTailAttack(MonoObject* position, MonoObject* rotation)
{
	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject("FaceHuggerTailAttack", External->scene->mRootNode);
	go->UID = Random::Generate();
	go->tag = "FaceHuggerTailAttack";

	float3 posVector = External->moduleMono->UnboxVector(position);
	Quat rotVector = External->moduleMono->UnboxQuat(rotation);
	float3 scaleVector = float3(3.3f, 3.3f, 5.0f);

	go->mTransform->SetPosition(posVector);
	go->mTransform->SetRotation(rotVector);

	CCollider* physBody;
	physBody = new CCollider(go, BOX);

	physBody->useGravity = false;

	physBody->SetAsSensor(true);

	go->AddComponent(physBody);

	physBody->size = scaleVector * 3;

	const char* t = "FaceHuggerTailAttack";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);
}

void CreateDroneClawAttack(MonoObject* position, MonoObject* rotation)
{
	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject("DroneClawAttack", External->scene->mRootNode);
	go->UID = Random::Generate();
	go->tag = "DroneClawAttack";

	float3 posVector = External->moduleMono->UnboxVector(position);
	Quat rotVector = External->moduleMono->UnboxQuat(rotation);
	float3 scaleVector = float3(3.3f, 3.3f, 4.2f);

	go->mTransform->SetPosition(posVector);
	go->mTransform->SetRotation(rotVector);

	CCollider* physBody;
	physBody = new CCollider(go, BOX);

	physBody->useGravity = false;

	physBody->SetAsSensor(true);

	physBody->size = scaleVector * 3;
	go->AddComponent(physBody);

	const char* t = "DroneXenomorphClawAttack";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);
}

void CreateDroneTailAttack(MonoObject* position, MonoObject* rotation)
{
	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject("DroneTailAttack", External->scene->mRootNode);
	go->UID = Random::Generate();
	go->tag = "DroneTailAttack";

	float3 posVector = External->moduleMono->UnboxVector(position);
	Quat rotVector = External->moduleMono->UnboxQuat(rotation);
	float3 scaleVector = float3(3.3f, 3.3f, 6.0f);

	go->mTransform->SetPosition(posVector);
	go->mTransform->SetRotation(rotVector);

	CCollider* physBody;
	physBody = new CCollider(go, BOX);

	physBody->useGravity = false;
	physBody->SetAsSensor(true);

	physBody->size = scaleVector*3;
	go->AddComponent(physBody);

	const char* t = "DroneXenomorphTailAttack";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);
}

void CreateQueenClawAttack(MonoObject* position, MonoObject* rotation)
{
	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject("QueenClawAttack", External->scene->mRootNode);
	go->UID = Random::Generate();
	go->tag = "QueenClawAttack";

	float3 posVector = External->moduleMono->UnboxVector(position);
	Quat rotVector = External->moduleMono->UnboxQuat(rotation);
	float3 scaleVector = float3(7.5f, 5.5f, 7.0f);

	go->mTransform->SetPosition(posVector);
	go->mTransform->SetRotation(rotVector);

	CCollider* physBody;
	physBody = new CCollider(go, BOX);

	physBody->useGravity = false;
	physBody->SetAsSensor(true);
	physBody->size = scaleVector * 3;

	go->AddComponent(physBody);

	const char* t = "QueenXenomorphClawAttack";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);
}

void CreateQueenTailAttack(MonoObject* position, MonoObject* rotation)
{
	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject("QueenTailAttack", External->scene->mRootNode);
	go->UID = Random::Generate();
	go->tag = "QueenTailAttack";

	float3 posVector = External->moduleMono->UnboxVector(position);
	Quat rotVector = External->moduleMono->UnboxQuat(rotation);
	float3 scaleVector = float3(7.5f, 5.5f, 7.0f);

	go->mTransform->SetPosition(posVector);
	go->mTransform->SetRotation(rotVector);

	CCollider* physBody;
	physBody = new CCollider(go, BOX);

	physBody->useGravity = false;
	physBody->SetAsSensor(true);

	physBody->size = scaleVector * 3;
	go->AddComponent(physBody);

	const char* t = "QueenXenomorphTailAttack";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);
}

void CreateQueenSpitAttack(MonoObject* position, MonoObject* rotation)
{
	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject("QueenSpitAttack", External->scene->mRootNode);
	go->UID = Random::Generate();
	go->tag = "QueenSpitAttack";

	float3 posVector = External->moduleMono->UnboxVector(position);
	Quat rotVector = External->moduleMono->UnboxQuat(rotation);
	float3 scaleVector = float3(3.0f, 3.0f, 3.0f);

	go->mTransform->SetPosition(posVector);
	go->mTransform->SetScale(scaleVector);
	go->mTransform->SetRotation(rotVector);

	CCollider* physBody;
	physBody = new CCollider(go, SPHERE);

	physBody->useGravity = true;
	physBody->physBody->SetPosition(posVector);
	physBody->physBody->SetRotation(rotVector);
	physBody->SetAsSensor(true);

	go->AddComponent(physBody);
	physBody->physBody->body->activate(true);
	physBody->size = scaleVector;
	physBody->shape->setLocalScaling(btVector3(scaleVector.x, scaleVector.y, scaleVector.z));

	const char* t = "QueenXenomorphSpitAttack";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);
}

void CreateQueenShrapnel(MonoObject* position, MonoObject* rotation)
{
	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject("QueenShrapnel", External->scene->mRootNode);
	go->UID = Random::Generate();
	go->tag = "QueenShrapnel";

	float3 posVector = External->moduleMono->UnboxVector(position);
	Quat rotVector = External->moduleMono->UnboxQuat(rotation);
	float3 scaleVector = float3(2.0f, 2.0f, 2.0f);

	go->mTransform->SetPosition(posVector);
	go->mTransform->SetScale(scaleVector);
	go->mTransform->SetRotation(rotVector);

	CCollider* physBody;
	physBody = new CCollider(go, SPHERE);

	physBody->useGravity = true;
	physBody->physBody->SetPosition(posVector);
	physBody->physBody->SetRotation(rotVector);
	physBody->SetAsSensor(true);

	go->AddComponent(physBody);
	physBody->physBody->body->activate(true);
	physBody->size = scaleVector;
	physBody->shape->setLocalScaling(btVector3(scaleVector.x, scaleVector.y, scaleVector.z));

	const char* t = "QueenXenomorphShrapnelAttack";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);
}

void CreateQueenPuddle(MonoObject* position, MonoObject* rotation)
{
	if (External == nullptr) return;
	GameObject* go = External->scene->PostUpdateCreateGameObject("QueenPuddle", External->scene->mRootNode);
	go->UID = Random::Generate();
	go->tag = "QueenPuddle";

	float3 posVector = External->moduleMono->UnboxVector(position);
	Quat rotVector = External->moduleMono->UnboxQuat(rotation);
	float3 scaleVector = float3(4.0f, 1.0f, 4.0f);

	go->mTransform->SetPosition(posVector);
	go->mTransform->SetScale(scaleVector);
	go->mTransform->SetRotation(rotVector);

	CCollider* physBody;
	physBody = new CCollider(go, CYLINDER);

	physBody->useGravity = false;
	physBody->physBody->SetPosition(posVector);
	physBody->physBody->SetRotation(rotVector);
	physBody->SetAsSensor(true);

	go->AddComponent(physBody);
	physBody->physBody->body->activate(true);
	physBody->size = scaleVector;
	physBody->shape->setLocalScaling(btVector3(scaleVector.x, scaleVector.y, scaleVector.z));

	const char* t = "QueenXenomorphPuddle";
	Component* c = nullptr;
	c = new CScript(go, t);
	go->AddComponent(c);
}

//---------- GLOBAL GETTERS ----------//
MonoObject* SendGlobalPosition(MonoObject* obj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	//return mono_value_box(External->moduleMono->domain, vecClass, External->moduleMono->Float3ToCS(C_Script::runningScript->GetGO()->transform->position)); //Use this method to send "object" types
	return Ymir_Box_Vector(obj, "POSITION", true); //Use this method to send class types
}

MonoObject* SendGlobalRotation(MonoObject* obj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	//return mono_value_box(External->moduleMono->domain, vecClass, External->moduleMono->Float3ToCS(C_Script::runningScript->GetGO()->transform->position)); //Use this method to send "object" types
	return Ymir_Box_Quat(obj, true); //Use this method to send class types
}

MonoObject* SendGlobalScale(MonoObject* obj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	//return mono_value_box(External->moduleMono->domain, vecClass, External->moduleMono->Float3ToCS(C_Script::runningScript->GetGO()->transform->position)); //Use this method to send "object" types
	return Ymir_Box_Vector(obj, "SCALE", true); //Use this method to send class types
}

MonoString* GetTag(MonoObject* cs_Object)
{
	GameObject* cpp_gameObject = External->moduleMono->GameObject_From_CSGO(cs_Object);

	return mono_string_new(External->moduleMono->domain, cpp_gameObject->tag.c_str());
}

void SetTag(MonoObject* cs_Object, MonoString* string)
{
	GameObject* cpp_gameObject = External->moduleMono->GameObject_From_CSGO(cs_Object);

	std::string newTag = mono_string_to_utf8(string);
	std::vector<std::string> tags = External->scene->tags;

	bool tagAlreadyExists = false;
	for (int t = 0; t < tags.size(); t++)
	{
		if (strcmp(newTag.c_str(), tags[t].c_str()) == 0)
		{
			tagAlreadyExists = true;
			break;
		}
	}
	if (tagAlreadyExists == false)
	{
		External->scene->tags.push_back(newTag);
	}
	cpp_gameObject->tag = newTag.c_str();
}

void Rumble_Controller(int time, int intenisty)
{
	if (External != nullptr) {

		intenisty = intenisty * 6500;
		if (SDL_JoystickRumble(External->input->joystick, intenisty, intenisty, time) == -1) {
			printf("Rumble failed...?\n");
		}
		else {
			printf("Rumble success!\n");
		}
	}
}

MonoString* CSVToString(MonoString* _filePath) {

	std::string filename = mono_string_to_utf8(_filePath); // File name to process

	std::string output = PhysfsEncapsule::ExtractStringFromCSV(filename);

	// Convert the resulting output string back to MonoString
	return mono_string_new(External->moduleMono->domain, output.c_str());
}
MonoString* CSVToStringKeys(MonoString* _filePath, MonoString* _csFields) {


	std::string filename = mono_string_to_utf8(_filePath); // File name to process
	std::string csFields = mono_string_to_utf8(_csFields); // CSV fields to extract
	std::vector<std::string> fields;

	// Process csFields to extract elements separated by commas
	std::string field;
	for (char c : csFields) {
		if (c == ',') {
			// When encountering a comma, add the current field to the vector
			fields.push_back(field);
			field.clear();
		}
		else {
			// Append the character to the current field
			field += c;
		}
	}

	// Add the last field after the last comma (or the only field if no commas are present)
	if (!field.empty()) {
		fields.push_back(field);
	}

	std::string output = PhysfsEncapsule::ExtractStringFromCSV(filename, fields);

	// Convert the resulting output string back to MonoString
	return mono_string_new(External->moduleMono->domain, output.c_str());
}

void CreateGOFromPrefabCS(MonoString* _prefabPath, MonoString* _prefabName, MonoObject* pos)
{
	std::string prefabName = mono_string_to_utf8(_prefabName);
	std::string prefabPath = mono_string_to_utf8(_prefabPath);

	float3 posVector = External->moduleMono->UnboxVector(pos);

	External->scene->pendingToAddPrefab.emplace_back(prefabPath, prefabName, posVector);
}

MonoObject* GetChildrenByTag(MonoObject* go, MonoString* tag)
{
	GameObject* gameObject = External->moduleMono->GameObject_From_CSGO(go);
	std::string nameCompare = mono_string_to_utf8(tag);

	std::vector<GameObject*> gameObjectChilds;
	gameObjectChilds = gameObject->mChildren;

	for (auto i = 0; i < gameObjectChilds.size(); i++)
	{
		if (gameObjectChilds[i]->tag.c_str() == nameCompare)
		{
			//mono_string_new(External->moduleMono->domain, gameObjectChilds[i]->tag.c_str());
			return External->moduleMono->GoToCSGO(gameObjectChilds[i]);
		}
	}

	return nullptr;
}

void DisableComponentCS(MonoObject* go, MonoString* compName, bool includeChildren)
{
	GameObject* gameObject = External->moduleMono->GameObject_From_CSGO(go);
	std::string nameCompare = mono_string_to_utf8(compName);


	ComponentType comp = External->scene->StringToComponentType(nameCompare);

	if (!includeChildren)
	{
		std::vector<Component*> components = gameObject->GetAllComponentsByType(comp);

		for (auto comp : components)
		{
			comp->Disable();
		}
	}
	else
	{
		std::vector<Component*> components = gameObject->GetAllComponentsByType(comp);

		for (Component* comp : components)
		{
			comp->Disable();
		}

		std::vector<GameObject*> children = gameObject->mChildren;

		for (auto child : children)
		{
			std::vector<Component*> childComponents = child->GetAllComponentsByType(comp);

			for (Component* comp : childComponents)
			{
				comp->Disable();
			}
		}
	}

}

void SetGameObjectAsBillboardCS(MonoObject* go) {

	GameObject* gameObject = External->moduleMono->GameObject_From_CSGO(go);

	float3 cameraPosition = External->scene->gameCameraComponent->GetPos();

	float3 directionToCamera = cameraPosition - gameObject->mTransform->GetGlobalPosition();
	directionToCamera.Normalize();

	float3 localUpVector(0.0f, 1.0f, 0.0f); 
	float3 worldUpVector(0.0f, 1.0f, 0.0f); 

	Quat desiredRotation = Quat::LookAt(float3(0, 0, 1), directionToCamera, localUpVector, worldUpVector);

	gameObject->mTransform->SetRotation(desiredRotation);
}

bool IsActiveCS(MonoObject* go)
{
	GameObject* gameObject = External->moduleMono->GameObject_From_CSGO(go);

	return gameObject->active;
}

void SetColliderSizeCS(MonoObject* go, MonoObject* vec)
{
	GameObject* gameObject = External->moduleMono->GameObject_From_CSGO(go);
	float3 vector = External->moduleMono->UnboxVector(vec);

	if (gameObject->GetComponent(ComponentType::PHYSICS) != nullptr)
	{
		CCollider* physBody = (CCollider*)gameObject->GetComponent(ComponentType::PHYSICS);
		physBody->shape->setLocalScaling(btVector3(vector.x, vector.y, vector.z));

		RELEASE(physBody);
	}

}

void SetGameObjectScaleCS(MonoObject* go, MonoObject* vec)
{
	GameObject* gameObject = External->moduleMono->GameObject_From_CSGO(go);
	float3 vector = External->moduleMono->UnboxVector(vec);

	gameObject->mTransform->SetScale(vector);
}

void SpawnItemCS(MonoString* name, MonoObject* pos)
{
	{
		//float3 vectorPos = External->moduleMono->UnboxVector(pos);
		//std::string goName = mono_string_to_utf8(name);

		//GameObject* gameObject = External->scene->PostUpdateCreateGameObject(goName, External->scene->mRootNode);
		//gameObject->UID = Random::Generate();
		////Set the position of the GameObject
		//gameObject->mTransform->SetPosition(vectorPos);

		////Add mesh

		//uint UID = 621813214;//1728623793; // UID of Cube.fbx (Shpere.fbx) mesh in meta (lo siento)

		//std::string libraryPath = External->fileSystem->libraryMeshesPath + std::to_string(UID) + ".ymesh";

		//if (!PhysfsEncapsule::FileExists(libraryPath)) {

		//	External->resourceManager->ImportFile("Assets/Primitives/Shpere.fbx", true);
		//}

		//ResourceMesh* rMesh = (ResourceMesh*)(External->resourceManager->CreateResourceFromLibrary(libraryPath, ResourceType::MESH, UID));
		//CMesh* cmesh = new CMesh(gameObject);
		//cmesh->rMeshReference = rMesh;
		//gameObject->AddComponent(cmesh);


		////TODO: Eric Añadir el switch con los diferentes nombres

		////Add the particle emmiter (Children)
		//GameObject* particleObject = External->scene->PostUpdateCreateGameObject(goName + "Particle System", gameObject);
		//CParticleSystem* particles = new CParticleSystem(particleObject);
		//EmitterRotation* emitterRot = (EmitterRotation*)particles->allEmitters.at(0)->CreateEmitterSettingByType(3);
		//emitterRot->currentAlignmentMode = BillboardType::PAR_LOOK_GAME_CAMERA;

		//EmitterImage* emitterImage = (EmitterImage*)particles->allEmitters.at(0)->CreateEmitterSettingByType(6);
		////emitterImage->SetImage(path);		//TODO: Eric & Xavi Falta añadir la imagen con el nombre y el path a la imagen

		//EmitterBase* emitterBase = (EmitterBase*)particles->allEmitters.at(0)->modules.at(0);
		//emitterBase->inmortal = true;

		//EmitterSpawner* emitterSpawn = (EmitterSpawner*)particles->allEmitters.at(0)->modules.at(1);
		//emitterSpawn->spawnMode = ParticlesSpawnMode::PAR_NUM_PARTICLES_BURST;
		//emitterSpawn->numParticlesToSpawn = 1;

		//particleObject->AddComponent(particles);



		////Add material Part 1 (Meter en el switch)
		//uint mat_UID = 366441211; //TODO Eric: UID of (BakerHouse) in meta (lo siento)

		//std::string mat_libraryPath = External->fileSystem->assetsPath + "Item Textures" + std::to_string(mat_UID) + ".dds";

		//if (!PhysfsEncapsule::FileExists(mat_libraryPath)) {

		//	External->resourceManager->ImportFile("Assets/Item Textures/BakerHouse.png", true);
		//}

		//ResourceTexture* rText = (ResourceTexture*)(External->resourceManager->CreateResourceFromLibrary(mat_libraryPath, ResourceType::TEXTURE, mat_UID, TextureType::DIFFUSE));

		////Add material Part 2 (No meter en el switch)
		//CMaterial* cmaterial = new CMaterial(gameObject);
		//cmaterial->shaderPath = SHADER_VS_FS;
		//cmaterial->shader.LoadShader(cmaterial->shaderPath);
		//cmaterial->shaderDirtyFlag = false;

		//cmaterial->rTextures.emplace_back(rText);
		//gameObject->AddComponent(cmaterial);

		////Add physics
		//CCollider* physBody;
		//physBody = new CCollider(gameObject);
		//physBody->useGravity = false;
		//physBody->SetOBBSize();
		//physBody->offset.y = 0;
		//physBody->physBody->SetPosition(vectorPos);
		//physBody->isSensor = true;
		//physBody->physType = PhysicsType::KINEMATIC;
		//gameObject->AddComponent(physBody);

		////Return the MonoObject
		//return External->moduleMono->GoToCSGO(gameObject);
	}

	//TODO pocho: Hacer un switch con todos los prefabs en relación al nombre

}

void SetColorMaterial(MonoObject* go, MonoObject* vec) {

	GameObject* gameObject = External->moduleMono->GameObject_From_CSGO(go);
	float3 vector = External->moduleMono->UnboxVector(vec);

	CMaterial* mat = (CMaterial*)gameObject->GetComponent(ComponentType::MATERIAL);

	if (mat != nullptr ) {
		mat->shader.SetUniformValue("color", &vector);
	}



}

int GetCurrentMapCS()
{
	return External->scene->currentMap;
}
#pragma endregion