#include "JsonFile.h"
#include "GameObject.h"
#include "Component.h"
#include "UI_Transform.h"

#include "UI_Canvas.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_InputBox.h"
#include "UI_CheckBox.h"
#include "UI_Slider.h"

#include <vector>
#include <filesystem>

#include "PhysfsEncapsule.h"
#include "Log.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ResourceTexture.h"
#include "ModuleRenderer3D.h"

#include "Animator.h"
#include "ModuleAudio.h"

#include "ImporterMesh.h"
#include "ModuleResourceManager.h"

#include "CScript.h"
#include "CNavMeshAgent.h"
#include "ImporterTexture.h"

#include "External/mmgr/mmgr.h"

JsonFile::JsonFile()
{
	InitializeJSON();
}

JsonFile::JsonFile(std::string route, std::string fileName)
{
	InitializeJSON();
	CreateJSON(route, fileName);
}

JsonFile::~JsonFile()
{
	DeinitializeJSON();
}

void JsonFile::InitializeJSON()
{
	rootValue = json_value_init_object();
	rootObject = json_value_get_object(rootValue);
}

void JsonFile::DeinitializeJSON()
{
	if (rootValue != nullptr)
	{
		json_value_free(rootValue);
		rootValue = nullptr;
		rootObject = nullptr;
	}
}

void JsonFile::CreateJSON(std::string route, std::string fileName)
{
	std::string fullRoute = route + fileName;

	FILE* file;
	errno_t err = fopen_s(&file, fullRoute.c_str(), "w");

	if (err == 0 && file != nullptr) {

		char* serialized_json = json_serialize_to_string_pretty(rootValue);

		if (serialized_json) {

			fprintf(file, "%s\n", serialized_json);
			json_free_serialized_string(serialized_json);

		}

		fclose(file);

	}
	else {

		LOG("[ERROR] [JsonFile] Failed to open file.");

	}

}

void JsonFile::ModifyJSON(std::string route, std::string fileName)
{

}

void JsonFile::DeleteJSON(const std::string& route)
{
	// Check if the file exists before attempting to delete
	if (PhysfsEncapsule::FileExists(route))
	{
		// Delete the file
		PhysfsEncapsule::DeleteFS(route);

	}
}

std::unique_ptr<JsonFile> JsonFile::GetJSON(const std::string& route) {

	std::unique_ptr<JsonFile> jsonFile = std::make_unique<JsonFile>();

	// Load the existing JSON file

	jsonFile->rootValue = json_parse_file(route.c_str());

	// Error handling
	if (!jsonFile->rootValue) {

		LOG("[ERROR] Unable to load JSON file from %s", route.c_str());

		return nullptr;
	}

	// Get the JSON object from the root value
	jsonFile->rootObject = json_value_get_object(jsonFile->rootValue);

	return jsonFile;
}

// -------------------------- Support functions --------------------------------
void JsonFile::UpdateJSON_File(const char* key)
{
	char* serialized_string = NULL;
	serialized_string = json_serialize_to_string_pretty(rootValue);
	puts(serialized_string);

	json_serialize_to_file(rootValue, key);
	json_free_serialized_string(serialized_string);
}

void JsonFile::SetInt(const char* key, int value) {

	json_object_set_number(rootObject, key, (int)value);

}

int JsonFile::GetInt(const char* key) const
{
	return (int)json_object_get_number(rootObject, key);
}

void JsonFile::SetFloat(const char* key, float value) {

	json_object_set_number(rootObject, key, (float)value);

}

float JsonFile::GetFloat(const char* key) const
{
	return (float)json_object_get_number(rootObject, key);
}

void JsonFile::SetDouble(const char* key, double value)
{
	json_object_set_number(rootObject, key, value);
}

double JsonFile::GetDouble(const char* key) const
{
	return json_object_get_number(rootObject, key);
}

void JsonFile::SetBoolean(const char* key, bool value) {

	json_object_set_boolean(rootObject, key, (bool)value);

}

bool JsonFile::GetBoolean(const char* key) const
{
	return (bool)json_object_get_boolean(rootObject, key);
}

void JsonFile::SetString(const char* key, const char* string) {

	json_object_set_string(rootObject, key, string);

}

std::string JsonFile::GetString(const char* key) const
{
	return json_object_get_string(rootObject, key);
}

void JsonFile::SetIntArray(const char* key, const int* array, size_t size) {

	JSON_Value* jsonArrayValue = json_value_init_array();
	JSON_Array* jsonArrayObject = json_value_get_array(jsonArrayValue);

	for (unsigned int i = 0; i < size; i++)
	{
		json_array_append_number(jsonArrayObject, array[i]);
	}

	json_object_set_value(rootObject, key, jsonArrayValue);

}

std::unique_ptr<int[]> JsonFile::GetIntArray(const char* key) const {
	
	JSON_Value* jsonArrayValue = json_object_get_value(rootObject, key);

	if (jsonArrayValue == nullptr || json_value_get_type(jsonArrayValue) != JSONArray)
	{
		return nullptr;
	}

	JSON_Array* jsonArrayObject = json_value_get_array(jsonArrayValue);
	size_t size = json_array_get_count(jsonArrayObject);

	// TODO FRANCESC: Need a smart pointer to solve this memory leak
	std::unique_ptr<int[]> resultArray = std::make_unique<int[]>(size + 1);

	for (size_t i = 0; i < size; i++)
	{
		resultArray[i] = static_cast<int>(json_array_get_number(jsonArrayObject, i));
	}

	resultArray[size] = -1;

	return resultArray;
}

void JsonFile::SetFloatArray(const char* key, const float* array, size_t size) {

	JSON_Value* jsonArrayValue = json_value_init_array();
	JSON_Array* jsonArrayObject = json_value_get_array(jsonArrayValue);

	for (unsigned int i = 0; i < size; i++)
	{
		json_array_append_number(jsonArrayObject, array[i]);
	}

	json_object_set_value(rootObject, key, jsonArrayValue);

}

float* JsonFile::GetFloatArray(const char* key) const {

	JSON_Value* jsonArrayValue = json_object_get_value(rootObject, key);

	if (jsonArrayValue == nullptr || json_value_get_type(jsonArrayValue) != JSONArray)
	{
		return nullptr;
	}

	JSON_Array* jsonArrayObject = json_value_get_array(jsonArrayValue);
	size_t size = json_array_get_count(jsonArrayObject);

	float* resultArray = new float[size + 1];

	for (size_t i = 0; i < size; i++)
	{
		resultArray[i] = static_cast<float>(json_array_get_number(jsonArrayObject, i));
	}

	resultArray[size] = -1;

	return resultArray;
}

void JsonFile::SetDoubleArray(const char* key, const double* array, size_t size)
{
	JSON_Value* jsonArrayValue = json_value_init_array();
	JSON_Array* jsonArrayObject = json_value_get_array(jsonArrayValue);

	for (unsigned int i = 0; i < size; i++)
	{
		json_array_append_number(jsonArrayObject, array[i]);
	}

	json_object_set_value(rootObject, key, jsonArrayValue);
}

double* JsonFile::GetDoubleArray(const char* key) const
{
	JSON_Value* jsonArrayValue = json_object_get_value(rootObject, key);

	if (jsonArrayValue == nullptr || json_value_get_type(jsonArrayValue) != JSONArray)
	{
		return nullptr;
	}

	JSON_Array* jsonArrayObject = json_value_get_array(jsonArrayValue);
	size_t size = json_array_get_count(jsonArrayObject);

	double* resultArray = new double[size + 1];

	for (size_t i = 0; i < size; i++)
	{
		resultArray[i] = static_cast<double>(json_array_get_number(jsonArrayObject, i));
	}

	resultArray[size] = -1;

	return resultArray;
}

// -------------------------- MathGeoLib functions --------------------------------

void JsonFile::SetFloat2(const char* key, const float2& value)
{
	JSON_Value* jsonFloat2Value = json_value_init_array();
	JSON_Array* jsonFloat2Array = json_value_get_array(jsonFloat2Value);

	json_array_append_number(jsonFloat2Array, value.x);
	json_array_append_number(jsonFloat2Array, value.y);

	json_object_set_value(rootObject, key, jsonFloat2Value);
}

float2 JsonFile::GetFloat2(const char* key) const
{
	JSON_Value* jsonFloat2Value = json_object_get_value(rootObject, key);

	if (jsonFloat2Value == nullptr || json_value_get_type(jsonFloat2Value) != JSONArray) {

		return float2();
	}

	JSON_Array* jsonFloat2Array = json_value_get_array(jsonFloat2Value);

	if (json_array_get_count(jsonFloat2Array) != 2) {

		return float2();
	}

	float2 resultFloat2;
	resultFloat2.x = static_cast<float>(json_array_get_number(jsonFloat2Array, 0));
	resultFloat2.y = static_cast<float>(json_array_get_number(jsonFloat2Array, 1));

	return resultFloat2;
}

void JsonFile::SetFloat3(const char* key, const float3& value)
{
	JSON_Value* jsonFloat3Value = json_value_init_array();
	JSON_Array* jsonFloat3Array = json_value_get_array(jsonFloat3Value);

	json_array_append_number(jsonFloat3Array, value.x);
	json_array_append_number(jsonFloat3Array, value.y);
	json_array_append_number(jsonFloat3Array, value.z);

	json_object_set_value(rootObject, key, jsonFloat3Value);
}

float3 JsonFile::GetFloat3(const char* key) const
{
	JSON_Value* jsonFloat3Value = json_object_get_value(rootObject, key);

	if (jsonFloat3Value == nullptr || json_value_get_type(jsonFloat3Value) != JSONArray) {

		return float3();
	}

	JSON_Array* jsonFloat3Array = json_value_get_array(jsonFloat3Value);

	if (json_array_get_count(jsonFloat3Array) != 3) {

		return float3();
	}

	float3 resultFloat3;
	resultFloat3.x = static_cast<float>(json_array_get_number(jsonFloat3Array, 0));
	resultFloat3.y = static_cast<float>(json_array_get_number(jsonFloat3Array, 1));
	resultFloat3.z = static_cast<float>(json_array_get_number(jsonFloat3Array, 2));

	return resultFloat3;
}

void JsonFile::SetFloat4(const char* key, const float4& value)
{
	JSON_Value* jsonFloat4Value = json_value_init_array();
	JSON_Array* jsonFloat4Array = json_value_get_array(jsonFloat4Value);

	json_array_append_number(jsonFloat4Array, value.x);
	json_array_append_number(jsonFloat4Array, value.y);
	json_array_append_number(jsonFloat4Array, value.z);
	json_array_append_number(jsonFloat4Array, value.w);

	json_object_set_value(rootObject, key, jsonFloat4Value);
}

float4 JsonFile::GetFloat4(const char* key) const
{
	JSON_Value* jsonFloat4Value = json_object_get_value(rootObject, key);

	if (jsonFloat4Value == nullptr || json_value_get_type(jsonFloat4Value) != JSONArray) {

		return float4();
	}

	JSON_Array* jsonFloat4Array = json_value_get_array(jsonFloat4Value);

	if (json_array_get_count(jsonFloat4Array) != 4) {

		return float4();
	}

	float4 resultFloat4;
	resultFloat4.x = static_cast<float>(json_array_get_number(jsonFloat4Array, 0));
	resultFloat4.y = static_cast<float>(json_array_get_number(jsonFloat4Array, 1));
	resultFloat4.z = static_cast<float>(json_array_get_number(jsonFloat4Array, 2));
	resultFloat4.w = static_cast<float>(json_array_get_number(jsonFloat4Array, 3));

	return resultFloat4;
}

void JsonFile::SetMatrix3x3(const char* key, const float3x3& matrix)
{
	JSON_Value* jsonMatrixValue = json_value_init_array();
	JSON_Array* jsonMatrixArray = json_value_get_array(jsonMatrixValue);

	for (unsigned int i = 0; i < 3; i++) {

		JSON_Value* jsonRowValue = json_value_init_array();
		JSON_Array* jsonRowArray = json_value_get_array(jsonRowValue);

		for (unsigned int j = 0; j < 3; j++) {

			json_array_append_number(jsonRowArray, matrix[i][j]);

		}

		json_array_append_value(jsonMatrixArray, jsonRowValue);

	}

	json_object_set_value(rootObject, key, jsonMatrixValue);
}

float3x3 JsonFile::GetMatrix3x3(const char* key) const
{
	JSON_Value* jsonMatrixValue = json_object_get_value(rootObject, key);

	if (jsonMatrixValue == nullptr || json_value_get_type(jsonMatrixValue) != JSONArray) {

		return float3x3();
	}

	JSON_Array* jsonMatrixArray = json_value_get_array(jsonMatrixValue);

	if (json_array_get_count(jsonMatrixArray) != 3) {

		return float3x3();
	}

	float3x3 resultMatrix;

	for (size_t i = 0; i < 3; i++) {

		JSON_Array* jsonRowArray = json_array_get_array(jsonMatrixArray, i);

		if (json_array_get_count(jsonRowArray) != 3) {

			return float3x3();
		}

		for (size_t j = 0; j < 3; j++) {

			resultMatrix[i][j] = static_cast<float>(json_array_get_number(jsonRowArray, j));

		}

	}

	return resultMatrix;
}

void JsonFile::SetMatrix4x4(const char* key, const float4x4& matrix)
{
	JSON_Value* jsonMatrixValue = json_value_init_array();
	JSON_Array* jsonMatrixArray = json_value_get_array(jsonMatrixValue);

	for (unsigned int i = 0; i < 4; i++) {

		JSON_Value* jsonRowValue = json_value_init_array();
		JSON_Array* jsonRowArray = json_value_get_array(jsonRowValue);

		for (unsigned int j = 0; j < 4; j++) {

			json_array_append_number(jsonRowArray, matrix[i][j]);

		}

		json_array_append_value(jsonMatrixArray, jsonRowValue);

	}

	json_object_set_value(rootObject, key, jsonMatrixValue);
}

float4x4 JsonFile::GetMatrix4x4(const char* key) const
{
	JSON_Value* jsonMatrixValue = json_object_get_value(rootObject, key);

	if (jsonMatrixValue == nullptr || json_value_get_type(jsonMatrixValue) != JSONArray) {

		return float4x4();
	}

	JSON_Array* jsonMatrixArray = json_value_get_array(jsonMatrixValue);

	if (json_array_get_count(jsonMatrixArray) != 4) {

		return float4x4();
	}

	float4x4 resultMatrix;

	for (size_t i = 0; i < 4; i++) {

		JSON_Array* jsonRowArray = json_array_get_array(jsonMatrixArray, i);

		if (json_array_get_count(jsonRowArray) != 4) {

			return float4x4();
		}

		for (size_t j = 0; j < 4; j++) {

			resultMatrix[i][j] = static_cast<float>(json_array_get_number(jsonRowArray, j));

		}
	}

	return resultMatrix;
}

void JsonFile::SetQuat(const char* key, const Quat& quaternion)
{
	JSON_Value* jsonQuatValue = json_value_init_array();
	JSON_Array* jsonQuatArray = json_value_get_array(jsonQuatValue);

	json_array_append_number(jsonQuatArray, quaternion.w);
	json_array_append_number(jsonQuatArray, quaternion.x);
	json_array_append_number(jsonQuatArray, quaternion.y);
	json_array_append_number(jsonQuatArray, quaternion.z);

	json_object_set_value(rootObject, key, jsonQuatValue);
}

Quat JsonFile::GetQuat(const char* key) const
{
	JSON_Value* jsonQuatValue = json_object_get_value(rootObject, key);

	if (jsonQuatValue == nullptr || json_value_get_type(jsonQuatValue) != JSONArray) {

		return Quat();
	}

	JSON_Array* jsonQuatArray = json_value_get_array(jsonQuatValue);

	if (json_array_get_count(jsonQuatArray) != 4) {

		return Quat();
	}

	Quat resultQuat;
	resultQuat.w = static_cast<float>(json_array_get_number(jsonQuatArray, 0));
	resultQuat.x = static_cast<float>(json_array_get_number(jsonQuatArray, 1));
	resultQuat.y = static_cast<float>(json_array_get_number(jsonQuatArray, 2));
	resultQuat.z = static_cast<float>(json_array_get_number(jsonQuatArray, 3));

	return resultQuat;
}

// -------------------------- Scene Serialization functions --------------------------------

// ---------- Individual Elements

void JsonFile::SetComponent(const char* key, const Component& component)
{
	JSON_Value* componentValue = json_value_init_object();
	JSON_Object* componentObject = json_value_get_object(componentValue);

	switch (component.ctype)
	{
	case NONE:
		// Handle NONE case (if needed)
		break;

	case TRANSFORM:
		json_object_set_string(componentObject, "Type", "Transform");
		// Additional properties specific to the Transform component can be added here
		break;

	case MESH:
		json_object_set_string(componentObject, "Type", "Mesh");
		// Additional properties specific to the Mesh component can be added here
		break;

	case MATERIAL:
		json_object_set_string(componentObject, "Type", "Material");
		// Additional properties specific to the Material component can be added here
		break;

	case CAMERA:
		json_object_set_string(componentObject, "Type", "Camera");
		// Additional properties specific to the Camera component can be added here
		break;

	case ANIMATION:
		json_object_set_string(componentObject, "Type", "Animation");
		// Additional properties specific to the Camera component can be added here
		break;
	case PARTICLE:
		json_object_set_string(componentObject, "Type", "ParticleSystem");
		// Additional properties specific to the Camera component can be added here
		break;
	}

	// Add the component object to the main object
	json_object_set_value(rootObject, key, componentValue);
}

Component* JsonFile::GetComponent(const char* key) const
{
	JSON_Value* componentValue = json_object_get_value(rootObject, key);

	if (componentValue != nullptr && json_value_get_type(componentValue) == JSONObject) {

		JSON_Object* componentObject = json_value_get_object(componentValue);

		// Create a new Component
		Component* component = new Component();

		// Get common properties
		std::string type = json_object_get_string(componentObject, "Type");

		if (type == "Transform") {

			component->ctype = ComponentType::TRANSFORM;

		}

		if (type == "Mesh") {

			component->ctype = ComponentType::MESH;

		}

		if (type == "Material") {

			component->ctype = ComponentType::MATERIAL;

		}

		if (type == "Camera") {

			component->ctype = ComponentType::CAMERA;

		}

		if (type == "Animation") {

			component->ctype = ComponentType::ANIMATION;

		}

		if (type == "ParticleSystem") {

			component->ctype = ComponentType::PARTICLE;

		}

		return component;
	}

	return nullptr;
}

void JsonFile::SetGameObject(const char* key, const GameObject& gameObject)
{
	JSON_Value* gameObjectValue = json_value_init_object();
	JSON_Object* gameObjectObject = json_value_get_object(gameObjectValue);

	// Set Name

	json_object_set_string(gameObjectObject, "Name", gameObject.name.c_str());


	json_object_set_boolean(gameObjectObject, "Static", gameObject.isStatic);

	// Set Position

	//JSON_Value* positionValue = json_value_init_array();
	//JSON_Array* positionArray = json_value_get_array(positionValue);
	//json_array_append_number(positionArray, gameObject.mTransform->translation.x);
	//json_array_append_number(positionArray, gameObject.mTransform->translation.y);
	//json_array_append_number(positionArray, gameObject.mTransform->translation.z);
	//json_object_set_value(gameObjectObject, "Position", positionValue);

	// Set Rotation

	//JSON_Value* rotationValue = json_value_init_array();
	//JSON_Array* rotationArray = json_value_get_array(rotationValue);
	//json_array_append_number(rotationArray, gameObject.mTransform->rotation.x);
	//json_array_append_number(rotationArray, gameObject.mTransform->rotation.y);
	//json_array_append_number(rotationArray, gameObject.mTransform->rotation.z);
	//json_object_set_value(gameObjectObject, "Rotation", rotationValue);

	// Set Scale

	/*JSON_Value* scaleValue = json_value_init_array();
	JSON_Array* scaleArray = json_value_get_array(scaleValue);
	json_array_append_number(scaleArray, gameObject.mTransform->scale.x);
	json_array_append_number(scaleArray, gameObject.mTransform->scale.y);
	json_array_append_number(scaleArray, gameObject.mTransform->scale.z);
	json_object_set_value(gameObjectObject, "Scale", scaleValue);*/

	// Get Tag
	json_object_set_string(gameObjectObject, "Tag", gameObject.tag.c_str());

	// Set UID
	json_object_set_number(gameObjectObject, "UID", gameObject.UID);

	// Set Parent UID

	if (gameObject.mParent != nullptr) {

		json_object_set_number(gameObjectObject, "Parent UID", gameObject.mParent->UID);

	}

	// Set Children UID

	std::vector<int> childrenUID;

	for (auto& child : gameObject.mChildren) {

		childrenUID.push_back(child->UID);

	}

	if (!childrenUID.empty()) {

		JSON_Value* childrenValue = json_value_init_array();
		JSON_Array* childrenArray = json_value_get_array(childrenValue);

		for (const auto& childUID : childrenUID) {

			json_array_append_number(childrenArray, childUID);

		}

		json_object_set_value(gameObjectObject, "Children UID", childrenValue);

	}

	// Save Components Info

	JSON_Value* componentsValue = json_value_init_array();
	JSON_Array* componentsArray = json_value_get_array(componentsValue);

	for (const auto& component : gameObject.mComponents) {

		JSON_Value* componentValue = json_value_init_object();
		JSON_Object* componentObject = json_value_get_object(componentValue);

		// Call the existing SetGameObject function to set individual GameObject properties
		SetComponent(componentObject, *component);

		// Add the GameObject to the hierarchy array
		json_array_append_value(componentsArray, componentValue);
	}

	// Add the hierarchy array to the main object
	json_object_set_value(gameObjectObject, "Components", componentsValue);

	// Add the GameObject to the main array
	json_object_set_value(rootObject, key, gameObjectValue);
}

GameObject* JsonFile::GetGameObject(const char* key) const
{
	JSON_Value* gameObjectValue = json_object_get_value(rootObject, key);

	if (gameObjectValue != nullptr && json_value_get_type(gameObjectValue) == JSONObject) {
		JSON_Object* gameObjectObject = json_value_get_object(gameObjectValue);

		// Create a new GameObject
		GameObject* gameObject = new GameObject();

		// Get Name
		const char* name = json_object_get_string(gameObjectObject, "Name");
		gameObject->name = (name != nullptr) ? name : "";

		//Get isStatic
		bool _isStatic;
		if (json_object_has_value(gameObjectObject, "Static") == 1) {
			_isStatic = json_object_get_boolean(gameObjectObject, "Static");
		}
		else {
			_isStatic = false;
		}
		gameObject->isStatic = _isStatic;

		// Get UID
		gameObject->UID = static_cast<int>(json_object_get_number(gameObjectObject, "UID"));

		// Get Parent UID
		if (json_object_has_value_of_type(gameObjectObject, "Parent UID", JSONNumber)) {
			gameObject->mParent->UID = static_cast<int>(json_object_get_number(gameObjectObject, "Parent UID"));
		}

		// Get Children UID
		if (json_object_has_value_of_type(gameObjectObject, "Children UID", JSONArray)) {
			JSON_Array* childrenArray = json_object_get_array(gameObjectObject, "Children UID");
			size_t numChildren = json_array_get_count(childrenArray);

			for (size_t i = 0; i < numChildren; ++i) {
				int childUID = static_cast<int>(json_array_get_number(childrenArray, i));
				// You need to find the corresponding child GameObject using the UID
				// and add it to gameObject->mChildren.
				// Assuming you have a function like FindGameObjectByUID, implement it accordingly.
				//gameObject->mChildren.push_back(FindGameObjectByUID(childUID));
			}

		}

		// Get Components
		if (json_object_has_value_of_type(gameObjectObject, "Components", JSONArray)) {
			JSON_Array* componentsArray = json_object_get_array(gameObjectObject, "Components");
			size_t numComponents = json_array_get_count(componentsArray);

			for (size_t i = 0; i < numComponents; ++i) {
				JSON_Value* componentValue = json_array_get_value(componentsArray, i);

				if (json_value_get_type(componentValue) == JSONObject) {
					JSON_Object* componentObject = json_value_get_object(componentValue);

					// Create a new Component
					Component* component = new Component();

					// Call the existing GetComponent function to retrieve individual Component properties
					//GetComponent(componentObject, *component);

					// Add the Component to the GameObject's components vector
					gameObject->mComponents.push_back(component);
				}
			}
		}

		return gameObject;
	}

	return nullptr;
}

// ---------- Save Scene 

void JsonFile::SetHierarchy(const char* key, const std::vector<GameObject*>& gameObjects)
{
	JSON_Value* hierarchyValue = json_value_init_array();
	JSON_Array* hierarchyArray = json_value_get_array(hierarchyValue);

	//for (const auto& gameObject : gameObjects) {

	//	JSON_Value* gameObjectValue = json_value_init_object();
	//	JSON_Object* gameObjectObject = json_value_get_object(gameObjectValue);

	//	// Call the existing SetGameObject function to set individual GameObject properties
	//	SetGameObject(gameObjectObject, *gameObject);

	//	// Add the GameObject to the hierarchy array
	//	json_array_append_value(hierarchyArray, gameObjectValue);
	//}

	SetGameObject(hierarchyArray, *External->scene->mRootNode);

	// Add the hierarchy array to the main object
	json_object_set_value(rootObject, key, hierarchyValue);
}

void JsonFile::SetGameObject(JSON_Array* hArray, const GameObject& gameObject)
{
	JSON_Value* gameObjectValue = json_value_init_object();
	JSON_Object* gameObjectObject = json_value_get_object(gameObjectValue);

	json_array_append_value(hArray, gameObjectValue);

	// Set Name
	json_object_set_string(gameObjectObject, "Name", gameObject.name.c_str());

	json_object_set_boolean(gameObjectObject, "Static", gameObject.isStatic);
	// Set Tag
	json_object_set_string(gameObjectObject, "Tag", gameObject.tag.c_str());

	// Set UID
	json_object_set_number(gameObjectObject, "UID", gameObject.UID);

	// Set Active
	json_object_set_boolean(gameObjectObject, "Active", gameObject.active);

	// Set Type 
	json_object_set_string(gameObjectObject, "Element_Type", gameObject.type.c_str());

	if (gameObject.type == "Mesh") {

		// Set Origin Path
		json_object_set_string(gameObjectObject, "Origin_Path", gameObject.originPath.c_str());

	}

	// Set Parent UID
	if (gameObject.mParent != nullptr) {
		json_object_set_number(gameObjectObject, "Parent UID", gameObject.mParent->UID);
	}

	JSON_Value* childrenValue = json_value_init_array();
	JSON_Array* childrenArray = json_value_get_array(childrenValue);

	// Set Children
	for (int i = 0; i < gameObject.mChildren.size(); i++)
	{
		SetGameObject(hArray, *gameObject.mChildren[i]);

		json_array_append_value(childrenArray, childrenValue);
		json_object_set_value(gameObjectObject, "Children UID", childrenValue);

		//counter = GameObjectJSON(gameObject.mChildren[i], subInfo + ".Child " + std::to_string(counter), counter, subInfo);
	}

	// Save Components Info

	JSON_Value* componentsValue = json_value_init_array();
	JSON_Array* componentsArray = json_value_get_array(componentsValue);

	for (const auto& component : gameObject.mComponents) {

		JSON_Value* componentValue = json_value_init_object();
		JSON_Object* componentObject = json_value_get_object(componentValue);

		// Call the existing SetGameObject function to set individual GameObject properties
		SetComponent(componentObject, *component);

		// Add the GameObject to the hierarchy array
		json_array_append_value(componentsArray, componentValue);
	}

	// Add the hierarchy array to the main object
	json_object_set_value(gameObjectObject, "Components", componentsValue);

	// Set Children UID

	std::vector<int> childrenUID;

	for (const auto& child : gameObject.mChildren) {

		childrenUID.push_back(child->UID);

	}

	if (!childrenUID.empty()) {

		JSON_Value* childrenValue = json_value_init_array();

		JSON_Array* childrenArray = json_value_get_array(childrenValue);

		for (const auto& childUID : childrenUID) {

			json_array_append_number(childrenArray, childUID);

		}

		json_object_set_value(gameObjectObject, "Children UID", childrenValue);
	}
}

void JsonFile::SetComponent(JSON_Object* componentObject, const Component& component)
{
	switch (component.ctype)
	{
	case NONE:
	{
		// Handle NONE case (if needed)
	}
	break;
	case TRANSFORM:
	{
		json_object_set_string(componentObject, "Type", "Transform");

		CTransform* ctransform = (CTransform*)&component;

		json_object_set_number(componentObject, "Active", ctransform->active);

		// Translation

		JSON_Value* translationArrayValue = json_value_init_array();
		JSON_Array* translationArray = json_value_get_array(translationArrayValue);

		json_array_append_number(translationArray, ctransform->translation.x);
		json_array_append_number(translationArray, ctransform->translation.y);
		json_array_append_number(translationArray, ctransform->translation.z);

		json_object_set_value(componentObject, "Translation", translationArrayValue);

		// Rotation

		JSON_Value* rotationArrayValue = json_value_init_array();
		JSON_Array* rotationArray = json_value_get_array(rotationArrayValue);

		json_array_append_number(rotationArray, ctransform->eulerRot.x);
		json_array_append_number(rotationArray, ctransform->eulerRot.y);
		json_array_append_number(rotationArray, ctransform->eulerRot.z);

		json_object_set_value(componentObject, "Rotation", rotationArrayValue);

		// Scale

		JSON_Value* scaleArrayValue = json_value_init_array();
		JSON_Array* scaleArray = json_value_get_array(scaleArrayValue);

		json_array_append_number(scaleArray, ctransform->scale.x);
		json_array_append_number(scaleArray, ctransform->scale.y);
		json_array_append_number(scaleArray, ctransform->scale.z);

		json_object_set_value(componentObject, "Scale", scaleArrayValue);
	}
	break;
	case MESH:
	{
		json_object_set_string(componentObject, "Type", "Mesh");

		CMesh* cmesh = (CMesh*)&component;

		json_object_set_number(componentObject, "Active", cmesh->active);

		json_object_set_number(componentObject, "UID", cmesh->rMeshReference->GetUID());

		json_object_set_number(componentObject, "Vertex Count", cmesh->nVertices);
		json_object_set_number(componentObject, "Index Count", cmesh->nIndices);
	}
	break;
	case MATERIAL:
	{
		json_object_set_string(componentObject, "Type", "Material");

		CMaterial* cmaterial = (CMaterial*)&component;

		json_object_set_number(componentObject, "Active", cmaterial->active);

		// Shader

		json_object_set_string(componentObject, "Shader", cmaterial->shaderPath.c_str());

		// Shader Uniforms Configuration

		JSON_Value* uniformArrayValue = json_value_init_array();
		JSON_Array* uniformArray = json_value_get_array(uniformArrayValue);

		JSON_Value* uniformObjectValue = json_value_init_object();
		JSON_Object* uniformObject = json_value_get_object(uniformObjectValue);

		for (auto& kt = cmaterial->shader.uniforms.begin(); kt != cmaterial->shader.uniforms.end(); ++kt) {

			switch (kt->type)
			{
			case UniformType::boolean:
			{
				json_object_set_boolean(uniformObject, kt->name.c_str(), *static_cast<bool*>(kt->value.get()));

				break;
			}
			case UniformType::i1:
			{
				json_object_set_number(uniformObject, kt->name.c_str(), *static_cast<int*>(kt->value.get()));

				break;
			}
			case UniformType::i2:
			case UniformType::i3:
			case UniformType::i4:
			{
				JSON_Value* vectorArrayValue = json_value_init_array();
				JSON_Array* vectorArray = json_value_get_array(vectorArrayValue);

				for (int i = 0; i < kt->nElements; ++i) {

					json_array_append_number(vectorArray, static_cast<int*>(kt->value.get())[i]);

				}

				json_object_set_value(uniformObject, kt->name.c_str(), vectorArrayValue);

				break;
			}
			case UniformType::f1:
			{
				json_object_set_number(uniformObject, kt->name.c_str(), *static_cast<float*>(kt->value.get()));

				break;
			}
			case UniformType::f2:
			case UniformType::f3:
			case UniformType::f4:
			{
				JSON_Value* vectorArrayValue = json_value_init_array();
				JSON_Array* vectorArray = json_value_get_array(vectorArrayValue);

				for (int i = 0; i < kt->nElements; ++i) {

					json_array_append_number(vectorArray, static_cast<float*>(kt->value.get())[i]);

				}

				json_object_set_value(uniformObject, kt->name.c_str(), vectorArrayValue);

				break;
			}
			}

		}

		json_array_append_value(uniformArray, uniformObjectValue);
		json_object_set_value(componentObject, "Uniforms", uniformArrayValue);

		// Texture maps

		if (!cmaterial->diffuse_path.empty()) {

			json_object_set_number(componentObject, "Diffuse_ID", cmaterial->diffuse_ID);
			json_object_set_string(componentObject, "Diffuse_Path", cmaterial->diffuse_path.c_str());
			json_object_set_number(componentObject, "Diffuse_UID", cmaterial->diffuse_UID);

		}

		if (!cmaterial->specular_path.empty()) {

			json_object_set_number(componentObject, "Specular_ID", cmaterial->specular_ID);
			json_object_set_string(componentObject, "Specular_Path", cmaterial->specular_path.c_str());
			json_object_set_number(componentObject, "Specular_UID", cmaterial->specular_UID);

		}

		if (!cmaterial->normal_path.empty()) {

			json_object_set_number(componentObject, "Normal_ID", cmaterial->normal_ID);
			json_object_set_string(componentObject, "Normal_Path", cmaterial->normal_path.c_str());
			json_object_set_number(componentObject, "Normal_UID", cmaterial->normal_UID);

		}

		if (!cmaterial->height_path.empty()) {

			json_object_set_number(componentObject, "Height_ID", cmaterial->height_ID);
			json_object_set_string(componentObject, "Height_Path", cmaterial->height_path.c_str());
			json_object_set_number(componentObject, "Height_UID", cmaterial->height_UID);

		}

		if (!cmaterial->ambient_path.empty()) {

			json_object_set_number(componentObject, "Ambient_ID", cmaterial->ambient_ID);
			json_object_set_string(componentObject, "Ambient_Path", cmaterial->ambient_path.c_str());
			json_object_set_number(componentObject, "Ambient_UID", cmaterial->ambient_UID);

		}

		if (!cmaterial->emissive_path.empty()) {

			json_object_set_number(componentObject, "Emissive_ID", cmaterial->emissive_ID);
			json_object_set_string(componentObject, "Emissive_Path", cmaterial->emissive_path.c_str());
			json_object_set_number(componentObject, "Emissive_UID", cmaterial->emissive_UID);

		}

	}
	break;
	case CAMERA:
	{
		json_object_set_string(componentObject, "Type", "Camera");

		CCamera* ccamera = (CCamera*)&component;

		json_object_set_number(componentObject, "Active", ccamera->active);

		// Right

		JSON_Value* rightArrayValue = json_value_init_array();
		JSON_Array* rightArray = json_value_get_array(rightArrayValue);

		json_array_append_number(rightArray, ccamera->GetRight().x);
		json_array_append_number(rightArray, ccamera->GetRight().y);
		json_array_append_number(rightArray, ccamera->GetRight().z);

		json_object_set_value(componentObject, "Right", rightArrayValue);

		// Up

		JSON_Value* upArrayValue = json_value_init_array();
		JSON_Array* upArray = json_value_get_array(upArrayValue);

		json_array_append_number(upArray, ccamera->GetUp().x);
		json_array_append_number(upArray, ccamera->GetUp().y);
		json_array_append_number(upArray, ccamera->GetUp().z);

		json_object_set_value(componentObject, "Up", upArrayValue);

		// Front

		JSON_Value* frontArrayValue = json_value_init_array();
		JSON_Array* frontArray = json_value_get_array(frontArrayValue);

		json_array_append_number(frontArray, ccamera->GetFront().x);
		json_array_append_number(frontArray, ccamera->GetFront().y);
		json_array_append_number(frontArray, ccamera->GetFront().z);

		json_object_set_value(componentObject, "Front", frontArrayValue);

		// FOV

		json_object_set_number(componentObject, "FOV", ccamera->GetVerticalFOV());

		// Near Plane

		json_object_set_number(componentObject, "Near Plane", ccamera->GetNearPlane());

		// Far Plane

		json_object_set_number(componentObject, "Far Plane", ccamera->GetFarPlane());

		// Enable/Disable Frustum Culling

		json_object_set_number(componentObject, "Frustum Culling", ccamera->enableFrustumCulling);

		// Enable/Disable Bounding Boxes

		json_object_set_number(componentObject, "Draw Bounding Boxes", ccamera->drawBoundingBoxes);

		// Is game camera

		json_object_set_boolean(componentObject, "Game Camera", ccamera->isGameCam);

		break;
	}
	case ANIMATION:
	{
		json_object_set_string(componentObject, "Type", "Animation");

		// Save component animation
		CAnimation* cAnimation = (CAnimation*)&component;

		json_object_set_number(componentObject, "Active", cAnimation->active);

		json_object_set_number(componentObject, "NumPaths", cAnimation->animator->animations.size());

		JSON_Value* sizeArrayValue = json_value_init_array();
		JSON_Array* sizeArray = json_value_get_array(sizeArrayValue);

		for (int i = 0; i < cAnimation->animator->animations.size(); i++) {
			json_array_append_string(sizeArray, cAnimation->animator->animations[i]->GetLibraryFilePath().c_str());
		}
		json_object_set_value(componentObject, "Paths", sizeArrayValue);

		JSON_Value* sizeArrayValueAssets = json_value_init_array();
		JSON_Array* sizeArrayAssets = json_value_get_array(sizeArrayValueAssets);

		for (int i = 0; i < cAnimation->animator->animations.size(); i++) {
			json_array_append_number(sizeArrayAssets, cAnimation->animator->animations[i]->GetUID());
		}
		json_object_set_value(componentObject, "UIDs", sizeArrayValueAssets);

		break;
	}
	case PHYSICS:
	{
		// World gravity

		JSON_Value* gravityArrayValue = json_value_init_array();
		JSON_Array* gravityArray = json_value_get_array(gravityArrayValue);

		json_array_append_number(gravityArray, External->physics->btVector3_to_float3(External->physics->GetWorldGravity()).x);
		json_array_append_number(gravityArray, External->physics->btVector3_to_float3(External->physics->GetWorldGravity()).y);
		json_array_append_number(gravityArray, External->physics->btVector3_to_float3(External->physics->GetWorldGravity()).z);

		json_object_set_value(componentObject, "World gravity", gravityArrayValue);

		// Component Collider

		json_object_set_string(componentObject, "Type", "Physics");

		CCollider* ccollider = (CCollider*)&component;

		json_object_set_boolean(componentObject, "Active", ccollider->isActive);

		// Collider Type

		json_object_set_number(componentObject, "Collider Type", (int)static_cast<const CCollider&>(component).collType);

		// Size

		JSON_Value* sizeArrayValue = json_value_init_array();
		JSON_Array* sizeArray = json_value_get_array(sizeArrayValue);

		json_array_append_number(sizeArray, ccollider->size.x);
		json_array_append_number(sizeArray, ccollider->size.y);
		json_array_append_number(sizeArray, ccollider->size.z);

		json_object_set_value(componentObject, "Size", sizeArrayValue);

		// Physics type

		json_object_set_number(componentObject, "Physics Type", (int)static_cast<const CCollider&>(component).physType);

		// Mass

		json_object_set_number(componentObject, "Mass", ccollider->mass);

		//Friction

		json_object_set_number(componentObject, "Friction", ccollider->friction);

		//Angular Friction

		json_object_set_number(componentObject, "Angular Friction", ccollider->angularFriction);

		// Gravity

		json_object_set_boolean(componentObject, "UseGravity", ccollider->useGravity);

		// IsSensor

		json_object_set_boolean(componentObject, "IsSensor", ccollider->isSensor);

		// Lock axis rotation

		json_object_set_boolean(componentObject, "LockX", ccollider->lockX);
		json_object_set_boolean(componentObject, "LockY", ccollider->lockY);
		json_object_set_boolean(componentObject, "LockZ", ccollider->lockZ);

		// Offset

		JSON_Value* offsetArrayValue = json_value_init_array();
		JSON_Array* offsetArray = json_value_get_array(offsetArrayValue);

		json_array_append_number(offsetArray, ccollider->offset.x);
		json_array_append_number(offsetArray, ccollider->offset.y);
		json_array_append_number(offsetArray, ccollider->offset.z);

		json_object_set_value(componentObject, "Offset", offsetArrayValue);

		// Offset Rotation
		
		JSON_Value* rotationOffsetArrayValue = json_value_init_array();
		JSON_Array* rotationOffsetArray = json_value_get_array(rotationOffsetArrayValue);

		json_array_append_number(rotationOffsetArray, ccollider->offsetRotation.x);
		json_array_append_number(rotationOffsetArray, ccollider->offsetRotation.y);
		json_array_append_number(rotationOffsetArray, ccollider->offsetRotation.z); 
		json_array_append_number(rotationOffsetArray, ccollider->offsetRotation.w);

		json_object_set_value(componentObject, "Offset Rotation", rotationOffsetArrayValue);

		break;
	}
	case SCRIPT:
	{
		CScript* cscript = (CScript*)&component;

		//TODO: A�adir la funci�n de SaveData() para que se guarde todo en el documento
		json_object_set_string(componentObject, "Type", "Script");

		json_object_set_string(componentObject, "ScriptName", cscript->name.c_str());

		json_object_set_number(componentObject, "Active", component.active);

		for (int i = 0; i < cscript->fields.size(); i++)
		{
			switch (cscript->fields[i].type)
			{
			case MonoTypeEnum::MONO_TYPE_BOOLEAN:
				mono_field_get_value(mono_gchandle_get_target(cscript->noGCobject), cscript->fields[i].field, &cscript->fields[i].fiValue.bValue);
				json_object_set_boolean(componentObject, mono_field_get_name(cscript->fields[i].field), cscript->fields[i].fiValue.bValue);
				break;

			case MonoTypeEnum::MONO_TYPE_I4:
				mono_field_get_value(mono_gchandle_get_target(cscript->noGCobject), cscript->fields[i].field, &cscript->fields[i].fiValue.iValue);
				json_object_set_number(componentObject, mono_field_get_name(cscript->fields[i].field), cscript->fields[i].fiValue.iValue);
				break;

			case MonoTypeEnum::MONO_TYPE_CLASS:
				if (cscript->fields[i].fiValue.goValue != nullptr)
					json_object_set_number(componentObject, mono_field_get_name(cscript->fields[i].field), cscript->fields[i].fiValue.goValue->UID);
				break;

			case MonoTypeEnum::MONO_TYPE_R4:
				mono_field_get_value(mono_gchandle_get_target(cscript->noGCobject), cscript->fields[i].field, &cscript->fields[i].fiValue.fValue);
				json_object_set_number(componentObject, mono_field_get_name(cscript->fields[i].field), cscript->fields[i].fiValue.fValue);
				break;

			case MonoTypeEnum::MONO_TYPE_STRING:
				json_object_set_string(componentObject, mono_field_get_name(cscript->fields[i].field), cscript->fields[i].fiValue.strValue);
				break;

			default:
				json_object_set_number(componentObject, mono_field_get_name(cscript->fields[i].field), cscript->fields[i].fiValue.iValue);
				break;
			}
		}

		break;
	}
	case UI:
	{
		json_object_set_string(componentObject, "Type", "UI");

		json_object_set_number(componentObject, "Active", component.active);
		json_object_set_boolean(componentObject, "Draggeable", static_cast<const C_UI&>(component).isDraggeable);

		json_object_set_number(componentObject, "Position X", (int)static_cast<const C_UI&>(component).posX);
		json_object_set_number(componentObject, "Position Y", (int)static_cast<const C_UI&>(component).posY);
		json_object_set_number(componentObject, "Width", (int)static_cast<const C_UI&>(component).width);
		json_object_set_number(componentObject, "Height", (int)static_cast<const C_UI&>(component).height);

		json_object_set_number(componentObject, "UI Type", (int)static_cast<const C_UI&>(component).UI_type);
		json_object_set_number(componentObject, "UI State", (int)static_cast<const C_UI&>(component).state);

		// Drag limits
		JSON_Value* dragLimitsArrayValue = json_value_init_array();
		JSON_Array* dragLimitsArray = json_value_get_array(dragLimitsArrayValue);

		json_array_append_number(dragLimitsArray, static_cast<const UI_Image&>(component).dragLimits.x);
		json_array_append_number(dragLimitsArray, static_cast<const UI_Image&>(component).dragLimits.y);
		json_array_append_number(dragLimitsArray, static_cast<const UI_Image&>(component).dragLimits.z);
		json_array_append_number(dragLimitsArray, static_cast<const UI_Image&>(component).dragLimits.w);

		json_object_set_value(componentObject, "Drag Limits", dragLimitsArrayValue);

		switch (static_cast<const C_UI&>(component).UI_type)
		{
		case UI_TYPE::CANVAS:
			break;
		case UI_TYPE::IMAGE:
		{
			json_object_set_number(componentObject, "Width", static_cast<const UI_Image&>(component).width);
			json_object_set_number(componentObject, "Height", static_cast<const UI_Image&>(component).height);

			json_object_set_string(componentObject, "Path", (static_cast<const UI_Image&>(component).mat->diffuse_path == "" ? "" : static_cast<const UI_Image&>(component).mat->diffuse_path).c_str());
			json_object_set_string(componentObject, "Shader Path", (static_cast<const UI_Image&>(component).mat->shaderPath == "" ? "" : static_cast<const UI_Image&>(component).mat->shaderPath).c_str());

			json_object_set_number(componentObject, "Current Frame X", static_cast<const UI_Image&>(component).ssCoordsY);
			json_object_set_number(componentObject, "Current Frame Y", static_cast<const UI_Image&>(component).ssCoordsX);

			json_object_set_number(componentObject, "Rows", static_cast<const UI_Image&>(component).ssRows);
			json_object_set_number(componentObject, "Columns", static_cast<const UI_Image&>(component).ssColumns);

			// Colors
			JSON_Value* colorArrayValue = json_value_init_array();
			JSON_Array* colorArray = json_value_get_array(colorArrayValue);

			json_array_append_number(colorArray, static_cast<const UI_Image&>(component).color.r);
			json_array_append_number(colorArray, static_cast<const UI_Image&>(component).color.g);
			json_array_append_number(colorArray, static_cast<const UI_Image&>(component).color.b);
			json_array_append_number(colorArray, static_cast<const UI_Image&>(component).color.a);

			json_object_set_value(componentObject, "Color", colorArrayValue);
		}

		break;
		case UI_TYPE::TEXT:
		{
			json_object_set_string(componentObject, "Text", static_cast<const UI_Text&>(component).text.c_str());
			json_object_set_string(componentObject, "Font name", static_cast<const UI_Text&>(component).font->name.c_str());
			json_object_set_string(componentObject, "Font path", static_cast<const UI_Text&>(component).font->path.c_str());
			json_object_set_number(componentObject, "Font size", static_cast<const UI_Text&>(component).fontSize);
			json_object_set_number(componentObject, "Line Spacing", static_cast<const UI_Text&>(component).lineSpacing);

			// Colors
			JSON_Value* colorArrayValue = json_value_init_array();
			JSON_Array* colorArray = json_value_get_array(colorArrayValue);

			json_array_append_number(colorArray, static_cast<const UI_Text&>(component).color.r);
			json_array_append_number(colorArray, static_cast<const UI_Text&>(component).color.g);
			json_array_append_number(colorArray, static_cast<const UI_Text&>(component).color.b);
			json_array_append_number(colorArray, static_cast<const UI_Text&>(component).color.a);

			json_object_set_value(componentObject, "Color", colorArrayValue);
		}

		break;
		case UI_TYPE::BUTTON:
		{
			json_object_set_boolean(componentObject, "Is interactable", static_cast<const UI_Button&>(component).isInteractable);

			SetReference(componentObject, *(GameObject*)(static_cast<const UI_Button&>(component).displayText), "Text");
			SetReference(componentObject, *(GameObject*)(static_cast<const UI_Button&>(component).image->mOwner), "Image");

			json_object_set_string(componentObject, "Normal", static_cast<UI_Image*>(static_cast<const UI_Button&>(component).
				image)->mapTextures[UI_STATE::NORMAL]->GetAssetsFilePath().c_str());

			json_object_set_string(componentObject, "Focused", static_cast<UI_Image*>(static_cast<const UI_Button&>(component).
				image)->mapTextures[UI_STATE::FOCUSED]->GetAssetsFilePath().c_str());

			json_object_set_string(componentObject, "Pressed", static_cast<UI_Image*>(static_cast<const UI_Button&>(component).
				image)->mapTextures[UI_STATE::PRESSED]->GetAssetsFilePath().c_str());

			json_object_set_string(componentObject, "Selected", static_cast<UI_Image*>(static_cast<const UI_Button&>(component).
				image)->mapTextures[UI_STATE::SELECTED]->GetAssetsFilePath().c_str());

			json_object_set_string(componentObject, "Release", static_cast<UI_Image*>(static_cast<const UI_Button&>(component).
				image)->mapTextures[UI_STATE::RELEASE]->GetAssetsFilePath().c_str());

			json_object_set_string(componentObject, "Disabled", static_cast<UI_Image*>(static_cast<const UI_Button&>(component).
				image)->mapTextures[UI_STATE::DISABLED]->GetAssetsFilePath().c_str());

			// Colors
			SetColor(componentObject, component);
		}

		break;
		case UI_TYPE::INPUTBOX:
		{
			json_object_set_number(componentObject, "Max characters", static_cast<const UI_InputBox&>(component).maxChars);
			json_object_set_string(componentObject, "Text", static_cast<const UI_InputBox&>(component).text.c_str());

			// Colors
			SetColor(componentObject, component);
		}

		break;
		case UI_TYPE::CHECKBOX:
		{
			json_object_set_boolean(componentObject, "Is interactable", static_cast<const UI_Checkbox&>(component).isInteractable);
			json_object_set_boolean(componentObject, "Is checked", static_cast<const UI_Checkbox&>(component).isChecked);

			// Colors
			SetColor(componentObject, component);
		}

		break;
		case UI_TYPE::SLIDER:
		{
			json_object_set_boolean(componentObject, "Is interactable", static_cast<const UI_Slider&>(component).isInteractable);

			SetReference(componentObject, *(GameObject*)(static_cast<const UI_Slider&>(component).fillImage), "Fill image");
			SetReference(componentObject, *(GameObject*)(static_cast<const UI_Slider&>(component).handleImage), "Handle image");

			/*json_object_set_number(componentObject, "Handle image", static_cast<const UI_Slider&>(component).handleImage == nullptr
				? -1 : static_cast<const UI_Slider&>(component).handleImage->UID);*/

				// Values
			json_object_set_boolean(componentObject, "Use Floats", static_cast<const UI_Slider&>(component).useFloat);

			if (static_cast<const UI_Slider&>(component).useFloat)
			{
				json_object_set_number(componentObject, "Value", static_cast<const UI_Slider&>(component).value.fValue);
				json_object_set_number(componentObject, "Value min", static_cast<const UI_Slider&>(component).minValue.fValue);
				json_object_set_number(componentObject, "Value max", static_cast<const UI_Slider&>(component).maxValue.fValue);
			}
			else
			{
				json_object_set_number(componentObject, "Value", static_cast<const UI_Slider&>(component).value.iValue);
				json_object_set_number(componentObject, "Value min", static_cast<const UI_Slider&>(component).minValue.iValue);
				json_object_set_number(componentObject, "Value max", static_cast<const UI_Slider&>(component).maxValue.iValue);
			}

			json_object_set_number(componentObject, "Direction", (int)static_cast<const UI_Slider&>(component).direction);

			// Colors
			SetColor(componentObject, component);
		}

		break;
		case UI_TYPE::NONE:
			break;
		default:
			break;
		}
	}
	break;

	case UI_TRAMSFORM:
	{
		json_object_set_string(componentObject, "Type", "UI Transform");

		json_object_set_number(componentObject, "Component reference", (int)static_cast<const UI_Transform&>(component).componentReference->UI_type);

	}
	break;

	case AUDIO_SOURCE:
	{
		json_object_set_string(componentObject, "Type", "Audio source");

		CAudioSource* caudiosource = (CAudioSource*)&component;

		json_object_set_number(componentObject, "Active", caudiosource->active);

		json_object_set_string(componentObject, "Bank Name", caudiosource->audBankName.c_str());
		json_object_set_string(componentObject, "Event Name", caudiosource->evName.c_str());
		json_object_set_number(componentObject, "Event ID", caudiosource->evID);
	}
	break;
	case AUDIO_LISTENER:
	{
		json_object_set_string(componentObject, "Type", "Audio listener");

		CAudioListener* caudiolistener = (CAudioListener*)&component;

		json_object_set_number(componentObject, "Active", caudiolistener->active);
		json_object_set_number(componentObject, "Default Listener", caudiolistener->isDefaultListener);
	}
	break;
	case LIGHT:
	{
		json_object_set_string(componentObject, "Type", "Light");

		CLight* clight = (CLight*)&component;

		json_object_set_number(componentObject, "Active", clight->active);

		json_object_set_number(componentObject, "Debug", clight->lightReference->debug);

		JSON_Value* colorArrayValue = json_value_init_array();
		JSON_Array* colorArray = json_value_get_array(colorArrayValue);

		json_array_append_number(colorArray, clight->lightReference->GetColor().x);
		json_array_append_number(colorArray, clight->lightReference->GetColor().y);
		json_array_append_number(colorArray, clight->lightReference->GetColor().z);

		json_object_set_value(componentObject, "Color", colorArrayValue);

		json_object_set_number(componentObject, "Intensity", clight->lightReference->GetIntensity());

		switch (clight->lightReference->GetType())
		{
		case LightType::POINT_LIGHT:
		{
			json_object_set_number(componentObject, "Light Type", (uint)LightType::POINT_LIGHT);

			PointLight* pointLight = static_cast<PointLight*>(clight->lightReference);

			json_object_set_number(componentObject, "Radius", pointLight->GetRadius());

			break;
		}
		case LightType::DIRECTIONAL_LIGHT:
		{
			json_object_set_number(componentObject, "Light Type", (uint)LightType::DIRECTIONAL_LIGHT);

			// DirectionalLight* directionalLight = static_cast<DirectionalLight*>(clight->lightReference);

			break;
		}
		case LightType::SPOT_LIGHT:
		{
			json_object_set_number(componentObject, "Light Type", (uint)LightType::SPOT_LIGHT);

			SpotLight* spotLight = static_cast<SpotLight*>(clight->lightReference);

			json_object_set_number(componentObject, "Range", spotLight->GetRange());
			json_object_set_number(componentObject, "Radius", spotLight->GetRadius());

			break;
		}
		case LightType::AREA_LIGHT:
		{
			json_object_set_number(componentObject, "Light Type", (uint)LightType::AREA_LIGHT);

			AreaLight* areaLight = static_cast<AreaLight*>(clight->lightReference);

			json_object_set_number(componentObject, "Width", areaLight->GetWidth());
			json_object_set_number(componentObject, "Height", areaLight->GetHeight());
			json_object_set_number(componentObject, "Range", areaLight->GetRange());

			break;
		}

		}

	}
	break;
	case PARTICLE:
	{
		json_object_set_string(componentObject, "Type", "ParticleSystem");

		CParticleSystem* cparticles = (CParticleSystem*)&component;

		json_object_set_number(componentObject, "Active", cparticles->active);
		json_object_set_boolean(componentObject, "ShowEmitters", cparticles->showEmitters);

		//List of UUID , as each emmiter is save 

		JSON_Value* emmitersArrayValue = json_value_init_array();
		JSON_Array* emmitersArray = json_value_get_array(emmitersArrayValue);

		json_object_set_number(componentObject, "NumEmitters", cparticles->allEmitters.size());

		for (int i = 0; i < cparticles->allEmitters.size(); i++)
		{
			//We create an array for each emmiter that will hold its settings
			json_array_append_value(emmitersArray, cparticles->SaveEmmiterJSON2(cparticles->allEmitters.at(i)));
			//json_array_append_number(emmitersArray, cparticles->SaveEmmiterJSON(cparticles->allEmitters.at(i))); //Guarda en su propia carpeta el emiiter con sus settings.
		}

		json_object_set_value(componentObject, "Emitters", emmitersArrayValue);
		break;
	}
	break;
	case NAVMESHAGENT:
	{
		json_object_set_string(componentObject, "Type", "NavMesh");

		CNavMeshAgent* cNavMesh = (CNavMeshAgent*)&component;

		json_object_set_number(componentObject, "Active", cNavMesh->active);
		json_object_set_number(componentObject, "Speed", cNavMesh->properties.speed);
		json_object_set_number(componentObject, "Angular Speed", cNavMesh->properties.angularSpeed);
		json_object_set_number(componentObject, "Stopping Distance", cNavMesh->properties.stoppingDistance);

		break;
	}

	break;
	default:
		break;
	}
}

void JsonFile::SetColor(JSON_Object* componentObject, const Component& component)
{
	// Colors
	JSON_Value* focusedColorArrayValue = json_value_init_array();
	JSON_Array* focusedColorArray = json_value_get_array(focusedColorArrayValue);

	json_array_append_number(focusedColorArray, static_cast<const UI_Button&>(component).focusedColor.r);
	json_array_append_number(focusedColorArray, static_cast<const UI_Button&>(component).focusedColor.g);
	json_array_append_number(focusedColorArray, static_cast<const UI_Button&>(component).focusedColor.b);
	json_array_append_number(focusedColorArray, static_cast<const UI_Button&>(component).focusedColor.a);

	json_object_set_value(componentObject, "Focused color", focusedColorArrayValue);

	//
	JSON_Value* pressedColorArrayValue = json_value_init_array();
	JSON_Array* pressedColorArray = json_value_get_array(pressedColorArrayValue);

	json_array_append_number(pressedColorArray, static_cast<const UI_Button&>(component).pressedColor.r);
	json_array_append_number(pressedColorArray, static_cast<const UI_Button&>(component).pressedColor.g);
	json_array_append_number(pressedColorArray, static_cast<const UI_Button&>(component).pressedColor.b);
	json_array_append_number(pressedColorArray, static_cast<const UI_Button&>(component).pressedColor.a);

	json_object_set_value(componentObject, "Pressed color", pressedColorArrayValue);

	//
	JSON_Value* selectedColorArrayValue = json_value_init_array();
	JSON_Array* selectedColorArray = json_value_get_array(selectedColorArrayValue);

	json_array_append_number(selectedColorArray, static_cast<const UI_Button&>(component).selectedColor.r);
	json_array_append_number(selectedColorArray, static_cast<const UI_Button&>(component).selectedColor.g);
	json_array_append_number(selectedColorArray, static_cast<const UI_Button&>(component).selectedColor.b);
	json_array_append_number(selectedColorArray, static_cast<const UI_Button&>(component).selectedColor.a);

	json_object_set_value(componentObject, "Selected color", selectedColorArrayValue);

	//
	JSON_Value* disabledColorArrayValue = json_value_init_array();
	JSON_Array* disabledColorArray = json_value_get_array(disabledColorArrayValue);

	json_array_append_number(disabledColorArray, static_cast<const UI_Button&>(component).disabledColor.r);
	json_array_append_number(disabledColorArray, static_cast<const UI_Button&>(component).disabledColor.g);
	json_array_append_number(disabledColorArray, static_cast<const UI_Button&>(component).disabledColor.b);
	json_array_append_number(disabledColorArray, static_cast<const UI_Button&>(component).disabledColor.a);

	json_object_set_value(componentObject, "Disabled color", disabledColorArrayValue);
}

void JsonFile::SetReference(JSON_Object* componentObject, GameObject& pointer, const char* name)
{
	json_object_set_number(componentObject, name, &pointer == nullptr
		? -1 : pointer.UID);
}

// ---------- Load Scene

void JsonFile::GetHierarchyNoMemoryLeaks(const char* key) const
{
	JSON_Value* hierarchyValue = json_object_get_value(rootObject, key);

	if (hierarchyValue != nullptr && json_value_get_type(hierarchyValue) == JSONArray) {

		JSON_Array* hierarchyArray = json_value_get_array(hierarchyValue);

		size_t numGameObjects = json_array_get_count(hierarchyArray);

		External->scene->gameObjects.reserve(numGameObjects);

		for (size_t i = 1; i < numGameObjects; ++i) {

			JSON_Value* gameObjectValue = json_array_get_value(hierarchyArray, i);

			if (json_value_get_type(gameObjectValue) == JSONObject) {

				JSON_Object* gameObjectObject = json_value_get_object(gameObjectValue);

				// Create a new GameObject
				// TODO: Preguntar monica
				//GameObject* gameObject = new GameObject();	// asi deberia estar
				G_UI* gameObject = new G_UI();

				// Call a function to extract individual GameObject properties
				GetGameObject(External->scene->gameObjects, gameObjectObject, *gameObject);

				// Add the GameObject to the vector
				External->scene->gameObjects.push_back(gameObject);

			}

		}

		External->scene->mRootNode->mParent = nullptr;

		for (auto it = External->scene->vTempComponents.begin(); it != External->scene->vTempComponents.end(); ++it)
		{
			(*it)->SetReference();
		}

	}
}

std::vector<GameObject*> JsonFile::GetHierarchy(const char* key) const
{
	std::vector<GameObject*> gameObjects;

	JSON_Value* hierarchyValue = json_object_get_value(rootObject, key);

	if (hierarchyValue != nullptr && json_value_get_type(hierarchyValue) == JSONArray) {

		JSON_Array* hierarchyArray = json_value_get_array(hierarchyValue);

		size_t numGameObjects = json_array_get_count(hierarchyArray);

		gameObjects.reserve(numGameObjects);

		for (size_t i = 0; i < numGameObjects; ++i) {

			JSON_Value* gameObjectValue = json_array_get_value(hierarchyArray, i);

			if (json_value_get_type(gameObjectValue) == JSONObject) {

				JSON_Object* gameObjectObject = json_value_get_object(gameObjectValue);

				// Create a new GameObject
				// TODO: Preguntar monica
				//GameObject* gameObject = new GameObject();	// asi deberia estar
				G_UI* gameObject = new G_UI();

				// Call a function to extract individual GameObject properties
				GetGameObject(gameObjects, gameObjectObject, *gameObject);

				// Add the GameObject to the vector
				gameObjects.push_back(gameObject);

			}

		}

		External->scene->mRootNode->mParent = nullptr;

		for (auto it = External->scene->vTempComponents.begin(); it != External->scene->vTempComponents.end(); ++it)
		{
			(*it)->SetReference();
		}

	}

	return gameObjects;
}

const char* JsonFile::GetNavMeshPath(const char* key) const
{
	const char* str = json_object_get_string(rootObject, key);
	if (str != nullptr) {
		return str;
	}
	return "";
}

void JsonFile::GetGameObject(const std::vector<GameObject*>& gameObjects, const JSON_Object* gameObjectObject, G_UI& gameObject) const
{
	// Get Name

	gameObject.name = json_object_get_string(gameObjectObject, "Name");

	// Get Position, Rotation, Scale
	// Get Name

	gameObject.type = json_object_get_string(gameObjectObject, "Element_Type");

	if (gameObject.type == "Mesh") {

		// Get Origin Path
		gameObject.originPath = json_object_get_string(gameObjectObject, "Origin_Path");

	}

	// Get Tag

	bool _isStatic;
	if (json_object_has_value(gameObjectObject, "Static") == 1) {
		_isStatic = json_object_get_boolean(gameObjectObject, "Static");
	}
	else {
		_isStatic = false;
	}
	gameObject.isStatic = _isStatic;

	if (json_object_has_value_of_type(gameObjectObject, "Tag", JSONString)) {

		gameObject.tag = json_object_get_string(gameObjectObject, "Tag");

		std::vector<std::string> tagsCopy = External->scene->tags;
		External->scene->tags.push_back(gameObject.tag);

	}

	// Get UID
	gameObject.UID = json_object_get_number(gameObjectObject, "UID");

	// Get UID

	gameObject.active = json_object_get_boolean(gameObjectObject, "Active");

	// Get Type

	gameObject.type = json_object_get_string(gameObjectObject, "Element_Type");

	// Get Components Info

	if (json_object_has_value_of_type(gameObjectObject, "Components", JSONArray)) {

		JSON_Array* componentsArray = json_object_get_array(gameObjectObject, "Components");
		size_t numComponents = json_array_get_count(componentsArray);

		for (size_t i = 0; i < numComponents; ++i) {

			JSON_Value* componentValue = json_array_get_value(componentsArray, i);

			if (json_value_get_type(componentValue) == JSONObject) {

				JSON_Object* componentObject = json_value_get_object(componentValue);

				// Call the existing GetComponent function to extract individual Component properties
				GetComponent(componentObject, &gameObject);

			}

		}

		// Get Parent UID

		if (json_object_has_value_of_type(gameObjectObject, "Parent UID", JSONNumber)) {

			int parentUID = static_cast<int>(json_object_get_number(gameObjectObject, "Parent UID"));

			// You need to find the corresponding parent GameObject using the UID
			// and set it to gameObject.mParent.

			GameObject* parent = GameObject::GetGameObjectFromUID(gameObjects, parentUID);

			if (parent) {

				gameObject.SetParent(parent);

			}
			else {

				gameObject.SetParent(External->scene->mRootNode);

			}

		}

	}

}

void JsonFile::GetComponent(const JSON_Object* componentObject, G_UI* gameObject) const {

	// Get common properties
	std::string type = json_object_get_string(componentObject, "Type");

	if (type == "Transform") {

		//gameObject->mTransform = new CTransform(gameObject);

		// Translation

		JSON_Value* jsonTranslationValue = json_object_get_value(componentObject, "Translation");

		if (jsonTranslationValue == nullptr || json_value_get_type(jsonTranslationValue) != JSONArray) {

			return;
		}


		JSON_Array* jsonTranslationArray = json_value_get_array(jsonTranslationValue);

		float3 translation;

		translation.x = static_cast<float>(json_array_get_number(jsonTranslationArray, 0));
		translation.y = static_cast<float>(json_array_get_number(jsonTranslationArray, 1));
		translation.z = static_cast<float>(json_array_get_number(jsonTranslationArray, 2));

		// Rotation

		JSON_Value* jsonRotationValue = json_object_get_value(componentObject, "Rotation");

		if (jsonRotationValue == nullptr || json_value_get_type(jsonRotationValue) != JSONArray) {

			return;
		}

		JSON_Array* jsonRotationArray = json_value_get_array(jsonRotationValue);

		float3 eulerRotation;

		eulerRotation.x = static_cast<float>(json_array_get_number(jsonRotationArray, 0));
		eulerRotation.y = static_cast<float>(json_array_get_number(jsonRotationArray, 1));
		eulerRotation.z = static_cast<float>(json_array_get_number(jsonRotationArray, 2));

		//eulerRotation *= RADTODEG;

		// Scale

		JSON_Value* jsonScaleValue = json_object_get_value(componentObject, "Scale");

		if (jsonScaleValue == nullptr || json_value_get_type(jsonScaleValue) != JSONArray) {

			return;
		}

		JSON_Array* jsonScaleArray = json_value_get_array(jsonScaleValue);

		float3 scale;

		scale.x = static_cast<float>(json_array_get_number(jsonScaleArray, 0));
		scale.y = static_cast<float>(json_array_get_number(jsonScaleArray, 1));
		scale.z = static_cast<float>(json_array_get_number(jsonScaleArray, 2));

		gameObject->mTransform->SetPosition(translation);
		gameObject->mTransform->SetRotation(eulerRotation);
		gameObject->mTransform->SetScale(scale);

		// Solves the problem where everything resets when loading
		gameObject->mTransform->eulerRot = gameObject->mTransform->rotation.ToEulerXYZ();
		//gameObject->mTransform->eulerRot *= RADTODEG;
		gameObject->mTransform->mGlobalMatrix = math::float4x4::FromTRS(gameObject->mTransform->translation, gameObject->mTransform->rotation, gameObject->mTransform->scale);
		gameObject->mTransform->mLocalMatrix = math::float4x4::identity;

		//gameObject->AddComponent(gameObject->mTransform);

		gameObject->mTransform->active = json_object_get_number(componentObject, "Active");

	}
	else if (type == "Mesh") {

		uint UID = json_object_get_number(componentObject, "UID");

		std::string libraryPath = "Library/Meshes/" + std::to_string(UID) + ".ymesh";

		if (!PhysfsEncapsule::FileExists(libraryPath)) {

			External->resourceManager->ReImportModel(gameObject->originPath, true);

		}

		auto itr = External->resourceManager->resources.find(UID);

		if (itr == External->resourceManager->resources.end()) {

			ResourceMesh* rMesh = (ResourceMesh*)External->resourceManager->CreateResourceFromLibrary(libraryPath, ResourceType::MESH, UID);

			CMesh* cmesh = new CMesh(gameObject);

			cmesh->active = json_object_get_number(componentObject, "Active");

			cmesh->nVertices = json_object_get_number(componentObject, "Vertex Count");
			cmesh->nIndices = json_object_get_number(componentObject, "Index Count");

			cmesh->rMeshReference = rMesh;

			cmesh->InitBoundingBoxes();

			gameObject->AddComponent(cmesh);

		}
		else {

			ResourceMesh* rMesh = static_cast<ResourceMesh*>(itr->second);

			itr->second->IncreaseReferenceCount();

			CMesh* cmesh = new CMesh(gameObject);

			cmesh->active = json_object_get_number(componentObject, "Active");

			cmesh->nVertices = json_object_get_number(componentObject, "Vertex Count");
			cmesh->nIndices = json_object_get_number(componentObject, "Index Count");

			cmesh->rMeshReference = rMesh;

			cmesh->InitBoundingBoxes();

			gameObject->AddComponent(cmesh);

		}

	}
	else if (type == "Material") {

		CMaterial* cmaterial = new CMaterial(gameObject);

		// Load shader from .yscene file
		std::string shaderPath = json_object_get_string(componentObject, "Shader");
		cmaterial->shaderPath = shaderPath;
		cmaterial->shader.LoadShader(shaderPath);

		// Load Shader Uniforms

		JSON_Value* jsonUniformValue = json_object_get_value(componentObject, "Uniforms");

		if (jsonUniformValue != nullptr && json_value_get_type(jsonUniformValue) == JSONArray) {

			JSON_Array* jsonUniformArray = json_value_get_array(jsonUniformValue);

			for (auto& kt = cmaterial->shader.uniforms.begin(); kt != cmaterial->shader.uniforms.end(); ++kt) {

				switch (kt->type)
				{
				case UniformType::boolean:
				{
					for (int i = 0; i < json_array_get_count(jsonUniformArray) && kt != cmaterial->shader.uniforms.end(); ++i) {

						JSON_Object* uniformObject = json_array_get_object(jsonUniformArray, i);

						if (json_object_has_value_of_type(uniformObject, kt->name.c_str(), JSONBoolean)) {

							bool value = json_object_get_boolean(uniformObject, kt->name.c_str());

							cmaterial->shader.SetUniformValue(kt->name, &value);

						}

					}

					break;
				}
				case UniformType::i1:
				{
					for (int i = 0; i < json_array_get_count(jsonUniformArray) && kt != cmaterial->shader.uniforms.end(); ++i) {

						JSON_Object* uniformObject = json_array_get_object(jsonUniformArray, i);

						if (json_object_has_value_of_type(uniformObject, kt->name.c_str(), JSONNumber)) {

							int value = json_object_get_number(uniformObject, kt->name.c_str());

							cmaterial->shader.SetUniformValue(kt->name, &value);

						}

					}

					break;
				}
				case UniformType::i2:
				case UniformType::i3:
				case UniformType::i4:
				{
					for (int i = 0; i < json_array_get_count(jsonUniformArray) && kt != cmaterial->shader.uniforms.end(); ++i) {

						JSON_Object* uniformObject = json_array_get_object(jsonUniformArray, i);

						if (json_object_has_value_of_type(uniformObject, kt->name.c_str(), JSONArray)) {

							JSON_Array* jsonArray = json_object_get_array(uniformObject, kt->name.c_str());

							int arraySize = json_array_get_count(jsonArray);

							// Allocate memory for the integer array
							int* value = new int[arraySize];

							// Copy values from the JSON array to the integer array
							for (int j = 0; j < arraySize; ++j) {

								value[j] = (int)json_array_get_number(jsonArray, j); // Assuming JSON array contains numbers

							}

							// Set uniform value
							cmaterial->shader.SetUniformValue(kt->name, value);

							// Delete memory when no longer needed
							delete[] value;

						}

					}

					break;
				}
				case UniformType::f1:
				{
					for (int i = 0; i < json_array_get_count(jsonUniformArray) && kt != cmaterial->shader.uniforms.end(); ++i) {

						JSON_Object* uniformObject = json_array_get_object(jsonUniformArray, i);

						if (json_object_has_value_of_type(uniformObject, kt->name.c_str(), JSONNumber)) {

							float value = json_object_get_number(uniformObject, kt->name.c_str());

							cmaterial->shader.SetUniformValue(kt->name, &value);

						}

					}

					break;
				}
				case UniformType::f2:
				case UniformType::f3:
				case UniformType::f4:
				{
					for (int i = 0; i < json_array_get_count(jsonUniformArray) && kt != cmaterial->shader.uniforms.end(); ++i) {

						JSON_Object* uniformObject = json_array_get_object(jsonUniformArray, i);

						if (json_object_has_value_of_type(uniformObject, kt->name.c_str(), JSONArray)) {

							JSON_Array* jsonArray = json_object_get_array(uniformObject, kt->name.c_str());

							int arraySize = json_array_get_count(jsonArray);

							// Allocate memory for the integer array
							float* value = new float[arraySize];

							// Copy values from the JSON array to the integer array
							for (int j = 0; j < arraySize; ++j) {

								value[j] = (float)json_array_get_number(jsonArray, j); // Assuming JSON array contains numbers

							}

							// Set uniform value
							cmaterial->shader.SetUniformValue(kt->name, value);

							// Delete memory when no longer needed
							delete[] value;

						}

					}

					break;
				}
				}

			}

		}

		if (json_object_has_value_of_type(componentObject, "Diffuse_Path", JSONString)) {

			uint ID = json_object_get_number(componentObject, "Diffuse_ID");
			cmaterial->diffuse_ID = ID;

			std::string diffusePath = json_object_get_string(componentObject, "Diffuse_Path");
			cmaterial->diffuse_path = diffusePath;

			uint UID = json_object_get_number(componentObject, "Diffuse_UID");
			cmaterial->diffuse_UID = UID;

			if (diffusePath == "Checker Image") {

				ResourceTexture* rTex = new ResourceTexture();

				rTex->type = TextureType::DIFFUSE;
				rTex->LoadCheckerImage();

				cmaterial->rTextures.push_back(rTex);

			}
			else {

				auto itr = External->resourceManager->resources.find(UID);

				if (itr == External->resourceManager->resources.end()) {

					ResourceTexture* rTex = (ResourceTexture*)External->resourceManager->CreateResourceFromLibrary(diffusePath, ResourceType::TEXTURE, UID, TextureType::DIFFUSE);

					cmaterial->rTextures.push_back(rTex);

				}
				else {

					ResourceTexture* rTex = static_cast<ResourceTexture*>(itr->second);

					rTex->type = TextureType::DIFFUSE;
					itr->second->IncreaseReferenceCount();

					cmaterial->rTextures.push_back(rTex);

				}

			}

		}

		if (json_object_has_value_of_type(componentObject, "Specular_Path", JSONString)) {

			uint ID = json_object_get_number(componentObject, "Specular_ID");
			cmaterial->specular_ID = ID;

			std::string specularPath = json_object_get_string(componentObject, "Specular_Path");
			cmaterial->specular_path = specularPath;

			uint UID = json_object_get_number(componentObject, "Specular_UID");
			cmaterial->specular_UID = UID;

			auto itr = External->resourceManager->resources.find(UID);

			if (itr == External->resourceManager->resources.end()) {

				ResourceTexture* rTex = (ResourceTexture*)External->resourceManager->CreateResourceFromLibrary(specularPath, ResourceType::TEXTURE, UID, TextureType::SPECULAR);
				cmaterial->rTextures.push_back(rTex);
			}
			else {

				ResourceTexture* rTex = static_cast<ResourceTexture*>(itr->second);
				rTex->type = TextureType::SPECULAR;
				itr->second->IncreaseReferenceCount();
				cmaterial->rTextures.push_back(rTex);
			}

		}

		if (json_object_has_value_of_type(componentObject, "Normal_Path", JSONString)) {

			uint ID = json_object_get_number(componentObject, "Normal_ID");
			cmaterial->normal_ID = ID;

			std::string normalPath = json_object_get_string(componentObject, "Normal_Path");
			cmaterial->normal_path = normalPath;

			uint UID = json_object_get_number(componentObject, "Normal_UID");
			cmaterial->normal_UID = UID;

			auto itr = External->resourceManager->resources.find(UID);

			if (itr == External->resourceManager->resources.end()) {

				ResourceTexture* rTex = (ResourceTexture*)External->resourceManager->CreateResourceFromLibrary(normalPath, ResourceType::TEXTURE, UID, TextureType::NORMAL);
				cmaterial->rTextures.push_back(rTex);
			}
			else {

				ResourceTexture* rTex = static_cast<ResourceTexture*>(itr->second);
				rTex->type = TextureType::NORMAL;
				itr->second->IncreaseReferenceCount();
				cmaterial->rTextures.push_back(rTex);
			}

		}

		if (json_object_has_value_of_type(componentObject, "Height_Path", JSONString)) {

			uint ID = json_object_get_number(componentObject, "Height_ID");
			cmaterial->height_ID = ID;

			std::string heightPath = json_object_get_string(componentObject, "Height_Path");
			cmaterial->height_path = heightPath;

			uint UID = json_object_get_number(componentObject, "Height_UID");
			cmaterial->height_UID = UID;

			auto itr = External->resourceManager->resources.find(UID);

			if (itr == External->resourceManager->resources.end()) {

				ResourceTexture* rTex = (ResourceTexture*)External->resourceManager->CreateResourceFromLibrary(heightPath, ResourceType::TEXTURE, UID, TextureType::HEIGHT);
				cmaterial->rTextures.push_back(rTex);
			}
			else {

				ResourceTexture* rTex = static_cast<ResourceTexture*>(itr->second);
				rTex->type = TextureType::HEIGHT;
				itr->second->IncreaseReferenceCount();
				cmaterial->rTextures.push_back(rTex);
			}

		}

		if (json_object_has_value_of_type(componentObject, "Ambient_Path", JSONString)) {

			uint ID = json_object_get_number(componentObject, "Ambient_ID");
			cmaterial->ambient_ID = ID;

			std::string ambientPath = json_object_get_string(componentObject, "Ambient_Path");
			cmaterial->ambient_path = ambientPath;

			uint UID = json_object_get_number(componentObject, "Ambient_UID");
			cmaterial->ambient_UID = UID;

			auto itr = External->resourceManager->resources.find(UID);

			if (itr == External->resourceManager->resources.end()) {

				ResourceTexture* rTex = (ResourceTexture*)External->resourceManager->CreateResourceFromLibrary(ambientPath, ResourceType::TEXTURE, UID, TextureType::AMBIENT);
				cmaterial->rTextures.push_back(rTex);
			}
			else {

				ResourceTexture* rTex = static_cast<ResourceTexture*>(itr->second);
				rTex->type = TextureType::AMBIENT;
				itr->second->IncreaseReferenceCount();
				cmaterial->rTextures.push_back(rTex);
			}

		}

		if (json_object_has_value_of_type(componentObject, "Emissive_Path", JSONString)) {

			uint ID = json_object_get_number(componentObject, "Emissive_ID");
			cmaterial->emissive_ID = ID;

			std::string emissivePath = json_object_get_string(componentObject, "Emissive_Path");
			cmaterial->emissive_path = emissivePath;

			uint UID = json_object_get_number(componentObject, "Emissive_UID");
			cmaterial->emissive_UID = UID;

			auto itr = External->resourceManager->resources.find(UID);

			if (itr == External->resourceManager->resources.end()) {

				ResourceTexture* rTex = (ResourceTexture*)External->resourceManager->CreateResourceFromLibrary(emissivePath, ResourceType::TEXTURE, UID, TextureType::EMISSIVE);

				cmaterial->rTextures.push_back(rTex);
			}
			else {

				ResourceTexture* rTex = static_cast<ResourceTexture*>(itr->second);
				rTex->type = TextureType::EMISSIVE;
				itr->second->IncreaseReferenceCount();

				cmaterial->rTextures.push_back(rTex);
			}

			

		}

		cmaterial->active = json_object_get_number(componentObject, "Active");
		gameObject->AddComponent(cmaterial);

	}
	else if (type == "Camera") {

		CCamera* ccamera = new CCamera(gameObject, json_object_get_boolean(componentObject, "Game Camera"));
		ccamera->framebuffer.Load();

		// FOV
		ccamera->SetVerticalFOV(json_object_get_number(componentObject, "FOV"));

		// Near Plane
		ccamera->SetNearPlane(json_object_get_number(componentObject, "Near Plane"));

		// Far Plane
		ccamera->SetFarPlane(json_object_get_number(componentObject, "Far Plane"));

		// Frustum Culling
		ccamera->enableFrustumCulling = json_object_get_number(componentObject, "Frustum Culling");

		// Draw Bounding Boxes
		ccamera->drawBoundingBoxes = json_object_get_number(componentObject, "Draw Bounding Boxes");

		// Game Camera
		ccamera->isGameCam = json_object_get_boolean(componentObject, "Game Camera");

		if (ccamera->isGameCam) {

			External->scene->gameCameraObject = gameObject;
			External->scene->gameCameraComponent = ccamera;
		}

		ccamera->active = json_object_get_number(componentObject, "Active");

		gameObject->AddComponent(ccamera);

	}
	else if (type == "Animation") {

		CAnimation* cAnim = new CAnimation(gameObject);

		JSON_Value* jsonSizeValue = json_object_get_value(componentObject, "Paths");
		JSON_Value* jsonSizeUIDValue = json_object_get_value(componentObject, "UIDs");

		if (jsonSizeValue == nullptr || json_value_get_type(jsonSizeValue) != JSONArray) {

			return;
		}

		if (jsonSizeUIDValue == nullptr || json_value_get_type(jsonSizeUIDValue) != JSONArray) {

			return;
		}

		JSON_Array* jsonSizeArray = json_value_get_array(jsonSizeValue);
		JSON_Array* jsonSizeUIDArray = json_value_get_array(jsonSizeUIDValue);


		for (int i = 0; i < json_object_get_number(componentObject, "NumPaths"); i++) {
			uint UID = json_array_get_number(jsonSizeUIDArray, i);
			auto itr = External->resourceManager->resources.find(UID);

			if (itr == External->resourceManager->resources.end()) {

				ResourceAnimation* rAnim = (ResourceAnimation*)External->resourceManager->CreateResourceFromLibrary(json_array_get_string(jsonSizeArray, i), ResourceType::ANIMATION, UID);

				cAnim->AddAnimation(rAnim);

				LOG("Loaded animation '%s' from Library", rAnim->name.c_str());
			}
			else {

				ResourceAnimation* rAnim = static_cast<ResourceAnimation*>(itr->second);
				itr->second->IncreaseReferenceCount();

				cAnim->AddAnimation(rAnim);
			}
		}

		cAnim->active = json_object_get_number(componentObject, "Active");

		gameObject->AddComponent(cAnim);
}
	else if (type == "Physics") {

		// World Gravity

		JSON_Value* jsonGravityValue = json_object_get_value(componentObject, "World gravity");

		if (jsonGravityValue == nullptr || json_value_get_type(jsonGravityValue) != JSONArray) {

			return;
		}

		JSON_Array* jsonGravityArray = json_value_get_array(jsonGravityValue);

		float3 gravity;

		gravity.x = static_cast<float>(json_array_get_number(jsonGravityArray, 0));
		gravity.y = static_cast<float>(json_array_get_number(jsonGravityArray, 1));
		gravity.z = static_cast<float>(json_array_get_number(jsonGravityArray, 2));

		External->physics->SetWorldGravity(External->physics->float3_to_btVector3(gravity));

		// Collider Type

		ColliderType collider = static_cast<ColliderType>(json_object_get_number(componentObject, "Collider Type"));

		// Physics Type

		PhysicsType physics = static_cast<PhysicsType>(json_object_get_number(componentObject, "Physics Type"));


		CCollider* ccollider = new CCollider(gameObject, collider, physics);

		ccollider->physType = physics;
		ccollider->collType = collider;

		// Size

		JSON_Value* jsonSizeValue = json_object_get_value(componentObject, "Size");

		if (jsonSizeValue == nullptr || json_value_get_type(jsonSizeValue) != JSONArray) {

			return;
		}

		JSON_Array* jsonSizeArray = json_value_get_array(jsonSizeValue);

		float3 size;

		size.x = static_cast<float>(json_array_get_number(jsonSizeArray, 0));
		size.y = static_cast<float>(json_array_get_number(jsonSizeArray, 1));
		size.z = static_cast<float>(json_array_get_number(jsonSizeArray, 2));

		ccollider->size = size;

		// Offset

		JSON_Value* jsonOffsetValue = json_object_get_value(componentObject, "Offset");

		if (jsonOffsetValue == nullptr || json_value_get_type(jsonOffsetValue) != JSONArray) {

			ccollider->offset = { 0, 0, 0 };
		}
		else
		{
			JSON_Array* jsonOffsetArray = json_value_get_array(jsonOffsetValue);

			float3 _offset;

			_offset.x = static_cast<float>(json_array_get_number(jsonOffsetArray, 0));
			_offset.y = static_cast<float>(json_array_get_number(jsonOffsetArray, 1));
			_offset.z = static_cast<float>(json_array_get_number(jsonOffsetArray, 2));

			ccollider->offset = _offset;
		}

		// Offset Rotation

		JSON_Value* jsonRotationOffsetValue = json_object_get_value(componentObject, "Offset Rotation"); 

		if (jsonRotationOffsetValue == nullptr || json_value_get_type(jsonRotationOffsetValue) != JSONArray) {

			ccollider->offsetRotation = Quat().identity;
		}
		else
		{
			JSON_Array* jsonRotationOffsetArray = json_value_get_array(jsonRotationOffsetValue);

			Quat _offsetRotation;

			_offsetRotation.x = static_cast<float>(json_array_get_number(jsonRotationOffsetArray, 0));
			_offsetRotation.y = static_cast<float>(json_array_get_number(jsonRotationOffsetArray, 1));
			_offsetRotation.z = static_cast<float>(json_array_get_number(jsonRotationOffsetArray, 2));
			_offsetRotation.w = static_cast<float>(json_array_get_number(jsonRotationOffsetArray, 3));

			ccollider->offsetRotation = _offsetRotation;
		}
		
		// Mass

		ccollider->mass = static_cast<float>(json_object_get_number(componentObject, "Mass"));

		// Friction

		ccollider->friction = static_cast<float>(json_object_get_number(componentObject, "Friction"));

		// Angular Friction

		ccollider->angularFriction = static_cast<float>(json_object_get_number(componentObject, "Angular Friction"));

		// Gravity

		ccollider->useGravity = json_object_get_boolean(componentObject, "UseGravity");

		// IsSensor

		ccollider->SetAsSensor(json_object_get_boolean(componentObject, "IsSensor"));

		// Lock axis rotation

		ccollider->lockX = json_object_get_boolean(componentObject, "LockX");
		ccollider->lockY = json_object_get_boolean(componentObject, "LockY");
		ccollider->lockZ = json_object_get_boolean(componentObject, "LockZ");

		ccollider->isActive = json_object_get_boolean(componentObject, "Active");

		gameObject->AddComponent(ccollider);

	}
	else if (type == "Script") {

		std::string scriptName = json_object_get_string(componentObject, "ScriptName");

		CScript* cscript = new CScript(gameObject, scriptName.c_str());

		SerializedField* _field = nullptr;
		for (int i = 0; i < cscript->fields.size(); i++) //TODO IMPORTANT ASK: There must be a better way to do this... too much use of switches with this stuff, look at MONOMANAGER
		{
			_field = &cscript->fields[i];

			switch (_field->type)
			{
			case MonoTypeEnum::MONO_TYPE_BOOLEAN:
				_field->fiValue.bValue = json_object_get_boolean(componentObject, mono_field_get_name(_field->field));
				mono_field_set_value(mono_gchandle_get_target(cscript->noGCobject), _field->field, &_field->fiValue.bValue);
				break;

			case MonoTypeEnum::MONO_TYPE_I4:
				_field->fiValue.iValue = json_object_get_number(componentObject, mono_field_get_name(_field->field));
				mono_field_set_value(mono_gchandle_get_target(cscript->noGCobject), _field->field, &_field->fiValue.iValue);
				break;

			case MonoTypeEnum::MONO_TYPE_CLASS:
			{
				if (strcmp(mono_type_get_name(mono_field_get_type(_field->field)), "YmirEngine.GameObject") == 0)
				{
					const char* name = mono_field_get_name(_field->field);
					int uid = json_object_get_number(componentObject, name);
					_field->goUID = uid;

					External->scene->AddToReferenceMap(uid, _field);
				}

				break;
			}
			case MonoTypeEnum::MONO_TYPE_R4:
				_field->fiValue.fValue = json_object_get_number(componentObject, mono_field_get_name(_field->field));
				mono_field_set_value(mono_gchandle_get_target(cscript->noGCobject), _field->field, &_field->fiValue.fValue);
				break;

			case MonoTypeEnum::MONO_TYPE_STRING:
			{
				const char* ret = json_object_get_string(componentObject, mono_field_get_name(_field->field));

				if (ret == NULL)
					ret = "\0";

				strcpy(&_field->fiValue.strValue[0], ret);

				MonoString* str = mono_string_new(External->moduleMono->domain, _field->fiValue.strValue);
				mono_field_set_value(mono_gchandle_get_target(cscript->noGCobject), _field->field, str);
				break;
			}

			default:
				_field->fiValue.iValue = json_object_get_number(componentObject, mono_field_get_name(_field->field));
				mono_field_set_value(mono_gchandle_get_target(cscript->noGCobject), _field->field, &_field->fiValue.iValue);
				break;
			}
		}

		cscript->active = json_object_get_number(componentObject, "Active");

		gameObject->AddComponent(cscript);

	}
	else if (type == "UI")
	{
		gameObject = static_cast<G_UI*>(gameObject);

		C_UI* comp = nullptr;

		switch ((UI_TYPE)json_object_get_number(componentObject, "UI Type"))
		{
		case UI_TYPE::CANVAS:
		{
			UI_Canvas* ui_comp = new UI_Canvas(gameObject);
			gameObject->AddComponent(ui_comp);
			comp = ui_comp;
			break;
		}
		case UI_TYPE::IMAGE:
		{
			UI_Image* ui_comp = new UI_Image(gameObject);
			ui_comp->width = json_object_get_number(componentObject, "Width");
			ui_comp->height = json_object_get_number(componentObject, "Height");

			ui_comp->mat->diffuse_path = json_object_get_string(componentObject, "Path");
			ui_comp->mat->shaderPath = json_object_get_string(componentObject, "Shader Path");

			if (External->scene->GetCanvas() == nullptr)
			{
				External->scene->CreateGUI(UI_TYPE::CANVAS);
			}
			if (gameObject->mParent == External->scene->mRootNode)
			{
				gameObject->ReParent(External->scene->GetCanvas());
			}

			gameObject->canvas = static_cast<G_UI*>(gameObject->mParent)->canvas;

			ui_comp->SetImg(ui_comp->mat->diffuse_path, UI_STATE::NORMAL);
			ui_comp->selectedTexture = ui_comp->mapTextures.find(ui_comp->state)->second;

			//
			ui_comp->ssCoordsY = json_object_get_number(componentObject, "Current Frame X");
			ui_comp->ssCoordsX = json_object_get_number(componentObject, "Current Frame Y");

			ui_comp->ssRows = json_object_get_number(componentObject, "Rows");
			ui_comp->ssColumns = json_object_get_number(componentObject, "Columns");

			ui_comp->SetSpriteSize();

			// Colors
			JSON_Value* jsonUIValue = json_object_get_value(componentObject, "Color");

			if (jsonUIValue == nullptr || json_value_get_type(jsonUIValue) != JSONArray) {

				return;
			}

			JSON_Array* jsonUIArray = json_value_get_array(jsonUIValue);

			ui_comp->color.r = static_cast<float>(json_array_get_number(jsonUIArray, 0));
			ui_comp->color.g = static_cast<float>(json_array_get_number(jsonUIArray, 1));
			ui_comp->color.b = static_cast<float>(json_array_get_number(jsonUIArray, 2));
			ui_comp->color.a = static_cast<float>(json_array_get_number(jsonUIArray, 3));

			float* aux = &ui_comp->color.r;
			ui_comp->mat->shader.SetUniformValue("color", aux);

			gameObject->AddComponent(ui_comp);
			comp = ui_comp;

			break;
		}
		case UI_TYPE::TEXT:
		{
			UI_Text* ui_comp = new UI_Text(gameObject, 0, 0, json_object_get_string(componentObject, "Text"), json_object_get_number(componentObject, "Font size"), json_object_get_number(componentObject, "Line Spacing"),
				json_object_get_string(componentObject, "Font name"), json_object_get_string(componentObject, "Font path"), 200, 50);

			if (External->scene->GetCanvas() == nullptr)
			{
				External->scene->CreateGUI(UI_TYPE::CANVAS);
			}
			if (gameObject->mParent == External->scene->mRootNode)
			{
				gameObject->ReParent(External->scene->GetCanvas());
			}

			gameObject->canvas = static_cast<G_UI*>(gameObject->mParent)->canvas;


			// Colors
			JSON_Value* jsonUIValue = json_object_get_value(componentObject, "Color");

			if (jsonUIValue == nullptr || json_value_get_type(jsonUIValue) != JSONArray) {

				return;
			}

			JSON_Array* jsonUIArray = json_value_get_array(jsonUIValue);

			ui_comp->color.r = static_cast<float>(json_array_get_number(jsonUIArray, 0));
			ui_comp->color.g = static_cast<float>(json_array_get_number(jsonUIArray, 1));
			ui_comp->color.b = static_cast<float>(json_array_get_number(jsonUIArray, 2));
			ui_comp->color.a = static_cast<float>(json_array_get_number(jsonUIArray, 3));

			float* aux = &ui_comp->color.r;
			ui_comp->mat->shader.SetUniformValue("color", aux);

			gameObject->AddComponent(ui_comp);
			comp = ui_comp;

			break;
		}
		case UI_TYPE::BUTTON:
		{
			UI_Button* ui_comp = new UI_Button(gameObject);
			ui_comp->isInteractable = json_object_get_boolean(componentObject, "Is interactable");

			int ret = GetReference(componentObject, *ui_comp, "Text", true);
			GetReference(componentObject, *ui_comp, "Image", (ret == -1 ? true : false));

			if (External->scene->GetCanvas() == nullptr)
			{
				External->scene->CreateGUI(UI_TYPE::CANVAS);
			}
			if (gameObject->mParent == External->scene->mRootNode)
			{
				gameObject->ReParent(External->scene->GetCanvas());
			}

			gameObject->canvas = static_cast<G_UI*>(gameObject->mParent)->canvas;

			ui_comp->mPaths.insert(std::pair<UI_STATE, std::string>(UI_STATE::NORMAL, json_object_get_string(componentObject, "Normal")));
			ui_comp->mPaths.insert(std::pair<UI_STATE, std::string>(UI_STATE::FOCUSED, json_object_get_string(componentObject, "Focused")));
			ui_comp->mPaths.insert(std::pair<UI_STATE, std::string>(UI_STATE::PRESSED, json_object_get_string(componentObject, "Pressed")));
			ui_comp->mPaths.insert(std::pair<UI_STATE, std::string>(UI_STATE::SELECTED, json_object_get_string(componentObject, "Selected")));
			ui_comp->mPaths.insert(std::pair<UI_STATE, std::string>(UI_STATE::RELEASE, json_object_get_string(componentObject, "Release")));
			ui_comp->mPaths.insert(std::pair<UI_STATE, std::string>(UI_STATE::DISABLED, json_object_get_string(componentObject, "Disabled")));

			// Colors
			JSON_Value* focusedColorArrayValue = json_object_get_value(componentObject, "Focused color");

			if (focusedColorArrayValue == nullptr || json_value_get_type(focusedColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* focusedColorArray = json_value_get_array(focusedColorArrayValue);

			ui_comp->focusedColor.r = static_cast<float>(json_array_get_number(focusedColorArray, 0));
			ui_comp->focusedColor.g = static_cast<float>(json_array_get_number(focusedColorArray, 1));
			ui_comp->focusedColor.b = static_cast<float>(json_array_get_number(focusedColorArray, 2));
			ui_comp->focusedColor.a = static_cast<float>(json_array_get_number(focusedColorArray, 3));

			//
			JSON_Value* pressedColorArrayValue = json_object_get_value(componentObject, "Pressed color");

			if (pressedColorArrayValue == nullptr || json_value_get_type(pressedColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* pressedColorArray = json_value_get_array(pressedColorArrayValue);

			ui_comp->pressedColor.r = static_cast<float>(json_array_get_number(pressedColorArray, 0));
			ui_comp->pressedColor.g = static_cast<float>(json_array_get_number(pressedColorArray, 1));
			ui_comp->pressedColor.b = static_cast<float>(json_array_get_number(pressedColorArray, 2));
			ui_comp->pressedColor.a = static_cast<float>(json_array_get_number(pressedColorArray, 3));

			//
			JSON_Value* selectedColorArrayValue = json_object_get_value(componentObject, "Selected color");

			if (selectedColorArrayValue == nullptr || json_value_get_type(selectedColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* selectedColorArray = json_value_get_array(selectedColorArrayValue);

			ui_comp->selectedColor.r = static_cast<float>(json_array_get_number(selectedColorArray, 0));
			ui_comp->selectedColor.g = static_cast<float>(json_array_get_number(selectedColorArray, 1));
			ui_comp->selectedColor.b = static_cast<float>(json_array_get_number(selectedColorArray, 2));
			ui_comp->selectedColor.a = static_cast<float>(json_array_get_number(selectedColorArray, 3));

			//
			JSON_Value* disabledColorArrayValue = json_object_get_value(componentObject, "Disabled color");

			if (disabledColorArrayValue == nullptr || json_value_get_type(disabledColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* disabledColorArray = json_value_get_array(disabledColorArrayValue);

			ui_comp->disabledColor.r = static_cast<float>(json_array_get_number(disabledColorArray, 0));
			ui_comp->disabledColor.g = static_cast<float>(json_array_get_number(disabledColorArray, 1));
			ui_comp->disabledColor.b = static_cast<float>(json_array_get_number(disabledColorArray, 2));
			ui_comp->disabledColor.a = static_cast<float>(json_array_get_number(disabledColorArray, 3));

			gameObject->AddComponent(ui_comp);
			comp = ui_comp;

			break;
		}
		case UI_TYPE::INPUTBOX:
		{
			UI_InputBox* ui_comp = new UI_InputBox(gameObject);
			ui_comp->text = json_object_get_string(componentObject, "Text");
			ui_comp->maxChars = json_object_get_number(componentObject, "Max characters");

			if (External->scene->GetCanvas() == nullptr)
			{
				External->scene->CreateGUI(UI_TYPE::CANVAS);
			}
			if (gameObject->mParent == External->scene->mRootNode)
			{
				gameObject->ReParent(External->scene->GetCanvas());
			}

			gameObject->canvas = static_cast<G_UI*>(gameObject->mParent)->canvas;


			// Colors
			JSON_Value* focusedColorArrayValue = json_object_get_value(componentObject, "Focused color");

			if (focusedColorArrayValue == nullptr || json_value_get_type(focusedColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* focusedColorArray = json_value_get_array(focusedColorArrayValue);

			ui_comp->focusedColor.r = static_cast<float>(json_array_get_number(focusedColorArray, 0));
			ui_comp->focusedColor.g = static_cast<float>(json_array_get_number(focusedColorArray, 1));
			ui_comp->focusedColor.b = static_cast<float>(json_array_get_number(focusedColorArray, 2));
			ui_comp->focusedColor.a = static_cast<float>(json_array_get_number(focusedColorArray, 3));

			//
			JSON_Value* pressedColorArrayValue = json_object_get_value(componentObject, "Pressed color");

			if (pressedColorArrayValue == nullptr || json_value_get_type(pressedColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* pressedColorArray = json_value_get_array(pressedColorArrayValue);

			ui_comp->pressedColor.r = static_cast<float>(json_array_get_number(pressedColorArray, 0));
			ui_comp->pressedColor.g = static_cast<float>(json_array_get_number(pressedColorArray, 1));
			ui_comp->pressedColor.b = static_cast<float>(json_array_get_number(pressedColorArray, 2));
			ui_comp->pressedColor.a = static_cast<float>(json_array_get_number(pressedColorArray, 3));

			//
			JSON_Value* selectedColorArrayValue = json_object_get_value(componentObject, "Selected color");

			if (selectedColorArrayValue == nullptr || json_value_get_type(selectedColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* selectedColorArray = json_value_get_array(selectedColorArrayValue);

			ui_comp->selectedColor.r = static_cast<float>(json_array_get_number(selectedColorArray, 0));
			ui_comp->selectedColor.g = static_cast<float>(json_array_get_number(selectedColorArray, 1));
			ui_comp->selectedColor.b = static_cast<float>(json_array_get_number(selectedColorArray, 2));
			ui_comp->selectedColor.a = static_cast<float>(json_array_get_number(selectedColorArray, 3));

			//
			JSON_Value* disabledColorArrayValue = json_object_get_value(componentObject, "Disabled color");

			if (disabledColorArrayValue == nullptr || json_value_get_type(disabledColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* disabledColorArray = json_value_get_array(disabledColorArrayValue);

			ui_comp->disabledColor.r = static_cast<float>(json_array_get_number(disabledColorArray, 0));
			ui_comp->disabledColor.g = static_cast<float>(json_array_get_number(disabledColorArray, 1));
			ui_comp->disabledColor.b = static_cast<float>(json_array_get_number(disabledColorArray, 2));
			ui_comp->disabledColor.a = static_cast<float>(json_array_get_number(disabledColorArray, 3));

			gameObject->AddComponent(ui_comp);
			comp = ui_comp;

			break;
		}
		case UI_TYPE::CHECKBOX:
		{
			UI_Checkbox* ui_comp = new UI_Checkbox(gameObject);
			ui_comp->isInteractable = json_object_get_boolean(componentObject, "Is interactable");
			ui_comp->isChecked = json_object_get_boolean(componentObject, "Is checked");

			if (External->scene->GetCanvas() == nullptr)
			{
				External->scene->CreateGUI(UI_TYPE::CANVAS);
			}
			if (gameObject->mParent == External->scene->mRootNode)
			{
				gameObject->ReParent(External->scene->GetCanvas());
			}

			gameObject->canvas = static_cast<G_UI*>(gameObject->mParent)->canvas;

			// Colors
			JSON_Value* focusedColorArrayValue = json_object_get_value(componentObject, "Focused color");

			if (focusedColorArrayValue == nullptr || json_value_get_type(focusedColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* focusedColorArray = json_value_get_array(focusedColorArrayValue);

			ui_comp->focusedColor.r = static_cast<float>(json_array_get_number(focusedColorArray, 0));
			ui_comp->focusedColor.g = static_cast<float>(json_array_get_number(focusedColorArray, 1));
			ui_comp->focusedColor.b = static_cast<float>(json_array_get_number(focusedColorArray, 2));
			ui_comp->focusedColor.a = static_cast<float>(json_array_get_number(focusedColorArray, 3));

			//
			JSON_Value* pressedColorArrayValue = json_object_get_value(componentObject, "Pressed color");

			if (pressedColorArrayValue == nullptr || json_value_get_type(pressedColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* pressedColorArray = json_value_get_array(pressedColorArrayValue);

			ui_comp->pressedColor.r = static_cast<float>(json_array_get_number(pressedColorArray, 0));
			ui_comp->pressedColor.g = static_cast<float>(json_array_get_number(pressedColorArray, 1));
			ui_comp->pressedColor.b = static_cast<float>(json_array_get_number(pressedColorArray, 2));
			ui_comp->pressedColor.a = static_cast<float>(json_array_get_number(pressedColorArray, 3));

			//
			JSON_Value* selectedColorArrayValue = json_object_get_value(componentObject, "Selected color");

			if (selectedColorArrayValue == nullptr || json_value_get_type(selectedColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* selectedColorArray = json_value_get_array(selectedColorArrayValue);

			ui_comp->selectedColor.r = static_cast<float>(json_array_get_number(selectedColorArray, 0));
			ui_comp->selectedColor.g = static_cast<float>(json_array_get_number(selectedColorArray, 1));
			ui_comp->selectedColor.b = static_cast<float>(json_array_get_number(selectedColorArray, 2));
			ui_comp->selectedColor.a = static_cast<float>(json_array_get_number(selectedColorArray, 3));

			//
			JSON_Value* disabledColorArrayValue = json_object_get_value(componentObject, "Disabled color");

			if (disabledColorArrayValue == nullptr || json_value_get_type(disabledColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* disabledColorArray = json_value_get_array(disabledColorArrayValue);

			ui_comp->disabledColor.r = static_cast<float>(json_array_get_number(disabledColorArray, 0));
			ui_comp->disabledColor.g = static_cast<float>(json_array_get_number(disabledColorArray, 1));
			ui_comp->disabledColor.b = static_cast<float>(json_array_get_number(disabledColorArray, 2));
			ui_comp->disabledColor.a = static_cast<float>(json_array_get_number(disabledColorArray, 3));

			gameObject->AddComponent(ui_comp);
			comp = ui_comp;

			break;
		}
		case UI_TYPE::SLIDER:
		{
			UI_Slider* ui_comp = new UI_Slider(gameObject);
			ui_comp->isInteractable = json_object_get_boolean(componentObject, "Is interactable");

			// Get references UIDs for later
			int ret = GetReference(componentObject, *ui_comp, "Fill image", true);
			GetReference(componentObject, *ui_comp, "Handle image", (ret == -1 ? true : false));

			if (External->scene->GetCanvas() == nullptr)
			{
				External->scene->CreateGUI(UI_TYPE::CANVAS);
			}
			if (gameObject->mParent == External->scene->mRootNode)
			{
				gameObject->ReParent(External->scene->GetCanvas());
			}

			gameObject->canvas = static_cast<G_UI*>(gameObject->mParent)->canvas;

			// Values
			ui_comp->useFloat = json_object_get_boolean(componentObject, "Use Floats");

			if (ui_comp->useFloat)
			{
				ui_comp->value.fValue = json_object_get_number(componentObject, "Value");
				ui_comp->minValue.fValue = json_object_get_number(componentObject, "Value min");
				ui_comp->maxValue.fValue = json_object_get_number(componentObject, "Value max");
			}
			else
			{
				ui_comp->value.iValue = json_object_get_number(componentObject, "Value");
				ui_comp->minValue.iValue = json_object_get_number(componentObject, "Value min");
				ui_comp->maxValue.iValue = json_object_get_number(componentObject, "Value max");
			}

			ui_comp->direction = (SLIDER_DIRECTION)json_object_get_number(componentObject, "Direction");

			// Colors
			JSON_Value* focusedColorArrayValue = json_object_get_value(componentObject, "Focused color");

			if (focusedColorArrayValue == nullptr || json_value_get_type(focusedColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* focusedColorArray = json_value_get_array(focusedColorArrayValue);

			ui_comp->focusedColor.r = static_cast<float>(json_array_get_number(focusedColorArray, 0));
			ui_comp->focusedColor.g = static_cast<float>(json_array_get_number(focusedColorArray, 1));
			ui_comp->focusedColor.b = static_cast<float>(json_array_get_number(focusedColorArray, 2));
			ui_comp->focusedColor.a = static_cast<float>(json_array_get_number(focusedColorArray, 3));

			//
			JSON_Value* pressedColorArrayValue = json_object_get_value(componentObject, "Pressed color");

			if (pressedColorArrayValue == nullptr || json_value_get_type(pressedColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* pressedColorArray = json_value_get_array(pressedColorArrayValue);

			ui_comp->pressedColor.r = static_cast<float>(json_array_get_number(pressedColorArray, 0));
			ui_comp->pressedColor.g = static_cast<float>(json_array_get_number(pressedColorArray, 1));
			ui_comp->pressedColor.b = static_cast<float>(json_array_get_number(pressedColorArray, 2));
			ui_comp->pressedColor.a = static_cast<float>(json_array_get_number(pressedColorArray, 3));

			//
			JSON_Value* selectedColorArrayValue = json_object_get_value(componentObject, "Selected color");

			if (selectedColorArrayValue == nullptr || json_value_get_type(selectedColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* selectedColorArray = json_value_get_array(selectedColorArrayValue);

			ui_comp->selectedColor.r = static_cast<float>(json_array_get_number(selectedColorArray, 0));
			ui_comp->selectedColor.g = static_cast<float>(json_array_get_number(selectedColorArray, 1));
			ui_comp->selectedColor.b = static_cast<float>(json_array_get_number(selectedColorArray, 2));
			ui_comp->selectedColor.a = static_cast<float>(json_array_get_number(selectedColorArray, 3));

			//
			JSON_Value* disabledColorArrayValue = json_object_get_value(componentObject, "Disabled color");

			if (disabledColorArrayValue == nullptr || json_value_get_type(disabledColorArrayValue) != JSONArray) {

				return;
			}

			JSON_Array* disabledColorArray = json_value_get_array(disabledColorArrayValue);

			ui_comp->disabledColor.r = static_cast<float>(json_array_get_number(disabledColorArray, 0));
			ui_comp->disabledColor.g = static_cast<float>(json_array_get_number(disabledColorArray, 1));
			ui_comp->disabledColor.b = static_cast<float>(json_array_get_number(disabledColorArray, 2));
			ui_comp->disabledColor.a = static_cast<float>(json_array_get_number(disabledColorArray, 3));

			gameObject->AddComponent(ui_comp);
			comp = ui_comp;

			break;
		}
		case UI_TYPE::NONE:
			break;
		default:
			break;
		}

		if (comp != nullptr)
		{
			comp->active = json_object_get_number(componentObject, "Active");
			comp->isDraggeable = json_object_get_boolean(componentObject, "Draggeable");

			comp->posX = json_object_get_number(componentObject, "Position X");
			comp->posY = json_object_get_number(componentObject, "Position Y");
			comp->width = json_object_get_number(componentObject, "Width");
			comp->height = json_object_get_number(componentObject, "Height");

			comp->UI_type = (UI_TYPE)json_object_get_number(componentObject, "UI Type");
			comp->state = (UI_STATE)json_object_get_number(componentObject, "UI State");

			// Drag limits
			JSON_Value* jsonUIValue = json_object_get_value(componentObject, "Drag Limits");

			if (jsonUIValue == nullptr || json_value_get_type(jsonUIValue) != JSONArray) {

				return;
			}

			JSON_Array* jsonUIArray = json_value_get_array(jsonUIValue);

			comp->dragLimits.x = static_cast<float>(json_array_get_number(jsonUIArray, 0));
			comp->dragLimits.y = static_cast<float>(json_array_get_number(jsonUIArray, 1));
			comp->dragLimits.z = static_cast<float>(json_array_get_number(jsonUIArray, 2));
			comp->dragLimits.w = static_cast<float>(json_array_get_number(jsonUIArray, 3));
		}

		comp = nullptr;

	}
	else if (type == "UI Transform")
	{
		C_UI* ui = static_cast<G_UI*>(gameObject)->GetComponentUI((UI_TYPE)json_object_get_number(componentObject, "Component reference"));

		if (ui != nullptr)
		{
			ui->transformUI = new UI_Transform(ui);

			gameObject->AddComponent(ui->transformUI);
		}

	}
	else if (type == "Audio listener")
	{
		CAudioListener* caudiolistener = new CAudioListener(gameObject);
		caudiolistener->active = json_object_get_number(componentObject, "Active");
		caudiolistener->isDefaultListener = json_object_get_number(componentObject, "Default Listener");

		gameObject->AddComponent(caudiolistener);
		CAudioListener* a = (CAudioListener*)gameObject->GetComponent(AUDIO_LISTENER);
		if (a->isDefaultListener) {
			a->SetAsDefaultListener(gameObject);
		}

	}
	else if (type == "Audio source")
	{
		CAudioSource* caudiosource = new CAudioSource(gameObject);
		caudiosource->active = json_object_get_number(componentObject, "Active");
		caudiosource->audBankName = json_object_get_string(componentObject, "Bank Name");
		caudiosource->evName = json_object_get_string(componentObject, "Event Name");
		caudiosource->evID = json_object_get_number(componentObject, "Event ID");

		for (std::vector<AudioBank*>::iterator& it = External->audio->banks.begin(); it != External->audio->banks.end(); ++it)
		{
			if ((*it)->bank_name == caudiosource->audBankName)
			{
				caudiosource->audBankReference = (*it);
		
				if (!(*it)->loaded_in_heap)
				{
					External->audio->LoadBank(caudiosource->audBankName);
					(*it)->loaded_in_heap = true;
				}
				
			}
			else 
			{
				caudiosource->audBankReference = nullptr;
			}
		}

		gameObject->AddComponent(caudiosource); 

	}
	else if (type == "Light")
	{
		switch ((LightType)json_object_get_number(componentObject, "Light Type"))
		{
		case LightType::POINT_LIGHT:
		{
			PointLight* pLight = new PointLight();

			pLight->lightGO = gameObject;
			pLight->debug = json_object_get_number(componentObject, "Debug");

			// Color

			JSON_Value* jsonColorValue = json_object_get_value(componentObject, "Color");

			if (jsonColorValue == nullptr || json_value_get_type(jsonColorValue) != JSONArray) {

				return;
			}

			JSON_Array* jsonColorArray = json_value_get_array(jsonColorValue);

			float3 color;

			color.x = static_cast<float>(json_array_get_number(jsonColorArray, 0));
			color.y = static_cast<float>(json_array_get_number(jsonColorArray, 1));
			color.z = static_cast<float>(json_array_get_number(jsonColorArray, 2));

			pLight->SetColor(color);

			float intensity = json_object_get_number(componentObject, "Intensity");
			pLight->SetIntensity(intensity);

			float radius = json_object_get_number(componentObject, "Radius");
			pLight->SetRadius(radius);

			CLight* componentLight = new CLight(gameObject, pLight);

			gameObject->AddComponent(componentLight);

			External->lightManager->lights.push_back(pLight);

			break;
		}
		case LightType::DIRECTIONAL_LIGHT:
		{
			DirectionalLight* dLight = new DirectionalLight();

			dLight->lightGO = gameObject;
			dLight->debug = json_object_get_number(componentObject, "Debug");

			// Color

			JSON_Value* jsonColorValue = json_object_get_value(componentObject, "Color");

			if (jsonColorValue == nullptr || json_value_get_type(jsonColorValue) != JSONArray) {

				return;
			}

			JSON_Array* jsonColorArray = json_value_get_array(jsonColorValue);

			float3 color;

			color.x = static_cast<float>(json_array_get_number(jsonColorArray, 0));
			color.y = static_cast<float>(json_array_get_number(jsonColorArray, 1));
			color.z = static_cast<float>(json_array_get_number(jsonColorArray, 2));

			dLight->SetColor(color);

			float intensity = json_object_get_number(componentObject, "Intensity");
			dLight->SetIntensity(intensity);

			CLight* componentLight = new CLight(gameObject, dLight);

			gameObject->AddComponent(componentLight);

			External->lightManager->lights.push_back(dLight);

			break;
		}
		case LightType::SPOT_LIGHT:
		{
			SpotLight* sLight = new SpotLight();

			sLight->lightGO = gameObject;
			sLight->debug = json_object_get_number(componentObject, "Debug");

			// Color

			JSON_Value* jsonColorValue = json_object_get_value(componentObject, "Color");

			if (jsonColorValue == nullptr || json_value_get_type(jsonColorValue) != JSONArray) {

				return;
			}

			JSON_Array* jsonColorArray = json_value_get_array(jsonColorValue);

			float3 color;

			color.x = static_cast<float>(json_array_get_number(jsonColorArray, 0));
			color.y = static_cast<float>(json_array_get_number(jsonColorArray, 1));
			color.z = static_cast<float>(json_array_get_number(jsonColorArray, 2));

			sLight->SetColor(color);

			float intensity = json_object_get_number(componentObject, "Intensity");
			sLight->SetIntensity(intensity);

			float range = json_object_get_number(componentObject, "Range");
			sLight->SetRange(range);

			float radius = json_object_get_number(componentObject, "Radius");
			sLight->SetRadius(radius);

			CLight* componentLight = new CLight(gameObject, sLight);

			gameObject->AddComponent(componentLight);

			External->lightManager->lights.push_back(sLight);

			break;
		}
		case LightType::AREA_LIGHT:
		{
			AreaLight* aLight = new AreaLight();

			aLight->lightGO = gameObject;
			aLight->debug = json_object_get_number(componentObject, "Debug");

			// Color

			JSON_Value* jsonColorValue = json_object_get_value(componentObject, "Color");

			if (jsonColorValue == nullptr || json_value_get_type(jsonColorValue) != JSONArray) {

				return;
			}

			JSON_Array* jsonColorArray = json_value_get_array(jsonColorValue);

			float3 color;

			color.x = static_cast<float>(json_array_get_number(jsonColorArray, 0));
			color.y = static_cast<float>(json_array_get_number(jsonColorArray, 1));
			color.z = static_cast<float>(json_array_get_number(jsonColorArray, 2));

			aLight->SetColor(color);

			float intensity = json_object_get_number(componentObject, "Intensity");
			aLight->SetIntensity(intensity);

			float range = json_object_get_number(componentObject, "Range");
			aLight->SetRange(range);

			float width = json_object_get_number(componentObject, "Width");
			aLight->SetWidth(width);

			float height = json_object_get_number(componentObject, "Height");
			aLight->SetHeight(height);

			CLight* componentLight = new CLight(gameObject, aLight);

			componentLight->active = json_object_get_number(componentObject, "Active");

			gameObject->AddComponent(componentLight);

			External->lightManager->lights.push_back(aLight);

			break;
		}

		}

	}
	else if (type == "ParticleSystem")
	{
		CParticleSystem* cparticleSystem = new CParticleSystem(gameObject);
		cparticleSystem->active = json_object_get_boolean(componentObject, "Active");
		cparticleSystem->showEmitters = json_object_get_boolean(componentObject, "ShowEmitters");

		int numEmitters = json_object_get_number(componentObject, "NumEmitters");
		
		for (int i = cparticleSystem->allEmitters.size(); i < numEmitters; i = cparticleSystem->allEmitters.size())
		{
			cparticleSystem->CreateEmitter();
		}

		JSON_Array* emittersArray = json_object_get_array(componentObject, "Emitters");
		for (int i = 0; i < numEmitters; i++)
		{
			ParticleEmitter* pEmmiter = cparticleSystem->allEmitters.at(i);

			//JSON_Array* settingsArray = json_array_get_array(emittersArray, i); 
			JSON_Array* settingsArray = json_object_get_array(json_array_get_object(emittersArray, i), "Settings");

			//Get cuantos settings tiene el emitter
			int numSettings = json_object_get_number(json_array_get_object(emittersArray, i), "ModulesSize");
			
			//Get cuantos settings tiene el emitter
			uint32_t UIDofEmitter = json_object_get_number(json_array_get_object(emittersArray, i), "EmitterUID");
			pEmmiter->UID = UIDofEmitter;
			const char* name = json_object_get_string(json_array_get_object(emittersArray, i), "Name");
			if(name == nullptr)
			{
				name = "";
			}
			pEmmiter->name.assign(name);
			for (int j = 0; j < numSettings; j++)
			{
				JSON_Object* modulo = json_array_get_object(settingsArray, j);

				EmitterType type = (EmitterType)json_object_get_number(modulo, "Type");

				//No se si es lo mas correcto pero es para evitar los problemas con los settings que se crean automaticos por defecto (base + spawner)
				EmitterSetting* instancia = pEmmiter->CreateEmitterSettingByType((uint)type);
				if (instancia == nullptr && pEmmiter->modules.size() > j)
				{
					instancia = pEmmiter->modules[j]; //En caso de que ya exista el modulo se assigna a dicho modulo.
				}

				switch (type)
				{
				case PAR_BASE:
				{
					EmitterBase* eBase = (EmitterBase*)instancia;
					eBase->particlesLifeTime1 = (float)json_object_get_number(modulo, "Lifetime");
					eBase->particlesLifeTime2 = (float)json_object_get_number(modulo, "Lifetime2");
					eBase->randomLT = json_object_get_boolean(modulo, "RandomLT");

					//Get position array
					JSON_Array* posArr = json_object_get_array(modulo, "Position");

					//Get elements of position
					float posX = json_array_get_number(posArr, 0);
					float posY = json_array_get_number(posArr, 1);
					float posZ = json_array_get_number(posArr, 2);
					eBase->emitterOrigin = { posX,posY,posZ };

					eBase->distanceLimit = (float)json_object_get_number(modulo, "DistanceLimit");
					eBase->hasDistanceLimit = json_object_get_boolean(modulo, "HasDistanceLimit");

					//Rotacion particle position (is not billboarding)
					eBase->rotacionBase = (RotationInheritanceParticles)json_object_get_number(modulo, "BaseRotation");

					//Shape
					eBase->currentShape = (SpawnAreaShape)json_object_get_number(modulo, "BaseShape");

					//Box specifics
					//Get box points array
					JSON_Array* positivesArr = json_object_get_array(modulo, "PositiveBoxPoints");

					//Get elements of position
					float bpposX = json_array_get_number(positivesArr, 0);
					float bpposY = json_array_get_number(positivesArr, 1);
					float bpposZ = json_array_get_number(positivesArr, 2);
					eBase->boxPointsPositives = { bpposX,bpposY,bpposZ };

					JSON_Array* negativesArr = json_object_get_array(modulo, "NegativeBoxPoints");

					//Get elements of position
					float bnposX = json_array_get_number(negativesArr, 0);
					float bnposY = json_array_get_number(negativesArr, 1);
					float bnposZ = json_array_get_number(negativesArr, 2);
					eBase->boxPointsNegatives = { bnposX,bnposY,bnposZ };

					//Cylinder and Sphere specifics
					eBase->radiusHollow = (float)json_object_get_number(modulo, "RadiusHollow");
					eBase->baseRadius = (float)json_object_get_number(modulo, "RadiusBase");
					eBase->topRadius = (float)json_object_get_number(modulo, "RadiusTop");
					eBase->heigth = (float)json_object_get_number(modulo, "Heigth");

					eBase = nullptr;
					break;
				}
				case PAR_SPAWN:
				{
					EmitterSpawner* eSpawn = (EmitterSpawner*)instancia;

					eSpawn->spawnMode = (ParticlesSpawnMode)json_object_get_number(modulo, "SpawnMode");
					eSpawn->startMode = (ParticlesSpawnEnabeling)json_object_get_number(modulo, "StartMode");

					eSpawn->numParticlesToSpawn = json_object_get_number(modulo, "NumParticles");
					eSpawn->spawnRatio = (float)json_object_get_number(modulo, "SpawnRatio");
					eSpawn->numParticlesForStop = json_object_get_number(modulo, "NumParticlesToStop");

					//Emitter things
					eSpawn->pointingEmitter = nullptr; //ParticleEmitter* pointingEmitter; //NO tengo ni idea de como guardar esto (ERIC)
					eSpawn->pointingUID = json_object_get_number(modulo, "PointingUID");
					eSpawn->conditionForSpawn = (SpawnConditionSubemitter)json_object_get_number(modulo, "ConditionForSpawn");
					eSpawn->subMaxLifetime = json_object_get_number(modulo, "SubemitterMaxLifetime");
					eSpawn->subMinLifetime = json_object_get_number(modulo, "SubemitterMinLifetime");

					eSpawn = nullptr;
					break;
				}
				case PAR_POSITION:
				{
					EmitterPosition* ePos = (EmitterPosition*)instancia;

					ePos->randomized = json_object_get_boolean(modulo, "Random");
					ePos->useBaseShape = json_object_get_boolean(modulo, "UseShapeBase");

					ePos->particleSpeed1 = json_object_get_number(modulo, "Speed1");
					ePos->particleSpeed2 = json_object_get_number(modulo, "Speed2");
					ePos->acceleration = json_object_get_boolean(modulo, "Accelerates");

					//Get position array
					JSON_Array* dirArr1 = json_object_get_array(modulo, "Direction1");

					//Get elements of position
					float posX1 = json_array_get_number(dirArr1, 0);
					float posY1 = json_array_get_number(dirArr1, 1);
					float posZ1 = json_array_get_number(dirArr1, 2);
					ePos->direction1 = { posX1,posY1,posZ1 };

					ePos->normalizedSpeed = json_object_get_boolean(modulo, "NormalizedBaseDirection");

					//Get position array
					JSON_Array* dirArr2 = json_object_get_array(modulo, "Direction2");

					//Get elements of position
					float posX2 = json_array_get_number(dirArr2, 0);
					float posY2 = json_array_get_number(dirArr2, 1);
					float posZ2 = json_array_get_number(dirArr2, 2);
					ePos->direction2 = { posX2,posY2,posZ2 };

					//Speed changes
					ePos->actualSpeedChange = (SpeedChangeMode)json_object_get_number(modulo, "ChangeSpeedMode");

					//Get newPosition array
					JSON_Array* newDirArr = json_object_get_array(modulo, "NewDirection");

					//Get elements of position
					float posXN = json_array_get_number(newDirArr, 0);
					float posYN = json_array_get_number(newDirArr, 1);
					float posZN = json_array_get_number(newDirArr, 2);
					ePos->newDirection = { posXN,posYN,posZN };

					ePos->changeSpeed1 = json_object_get_number(modulo, "ChangeSpeed1");
					ePos->changeSpeed2 = json_object_get_number(modulo, "ChangeSpeed2");

					ePos->normalizedChange = json_object_get_boolean(modulo, "NormalizedChange");

					ePos = nullptr;
					break;
				}
				case PAR_ROTATION:
				{
					EmitterRotation* eRot = (EmitterRotation*)instancia;

					//Enums of settings
					eRot->currentAlignmentMode = (BillboardType)json_object_get_number(modulo, "AligmentMode");;
					eRot->orientationFromWorld = (OrientationDirection)json_object_get_number(modulo, "WorldOrientation");
					eRot->orientationOfAxis = (OrientationDirection)json_object_get_number(modulo, "AxisOrientation");

					//Get newPosition array
					JSON_Array* arrFreeDir = json_object_get_array(modulo, "FreeRotation");

					//Get elements of position
					float posXN = json_array_get_number(arrFreeDir, 0);
					float posYN = json_array_get_number(arrFreeDir, 1);
					float posZN = json_array_get_number(arrFreeDir, 2);
					eRot->freeWorldRotation = { posXN,posYN,posZN };

					eRot = nullptr;
					break;
				}
				case PAR_SIZE:
				{
					EmitterSize* eSize = (EmitterSize*)instancia;

					eSize->progresive = json_object_get_boolean(modulo, "Progressive");
					eSize->loop = json_object_get_boolean(modulo, "Loop");
					eSize->sizeMultiplier1 = (float)json_object_get_number(modulo, "Size1");
					eSize->sizeMultiplier2 = (float)json_object_get_number(modulo, "Size2");
					eSize->startChange = (float)json_object_get_number(modulo, "TimeStart");
					eSize->stopChange = (float)json_object_get_number(modulo, "TimeStop");

					eSize = nullptr;
					break;
				}
				case PAR_COLOR:
				{
					EmitterColor* eColor = (EmitterColor*)instancia;

					eColor->progresive = json_object_get_boolean(modulo, "Progressive");
					eColor->loop = json_object_get_boolean(modulo, "Loop");

					eColor->startChange = (float)json_object_get_number(modulo, "TimeStart");
					eColor->stopChange = (float)json_object_get_number(modulo, "TimeStop");

					//Get COLOR array
					JSON_Array* colArr1 = json_object_get_array(modulo, "Color1");

					//Get elements of position
					eColor->color1.r = (float)json_array_get_number(colArr1, 0);
					eColor->color1.g = (float)json_array_get_number(colArr1, 1);
					eColor->color1.b = (float)json_array_get_number(colArr1, 2);
					eColor->color1.a = (float)json_array_get_number(colArr1, 3);

					//Get COLOR array
					JSON_Array* colArr2 = json_object_get_array(modulo, "Color2");

					//Get elements of position
					eColor->color2.r = (float)json_array_get_number(colArr2, 0);
					eColor->color2.g = (float)json_array_get_number(colArr2, 1);
					eColor->color2.b = (float)json_array_get_number(colArr2, 2);
					eColor->color2.a = (float)json_array_get_number(colArr2, 3);

					eColor = nullptr;
					break;
				}
				case PAR_IMAGE:
				{
					EmitterImage* eImage = (EmitterImage*)instancia;
					eImage->imgPath = json_object_get_string(modulo, "Path");
					eImage->SetImage(json_object_get_string(modulo, "Path"));
					eImage = nullptr;
					break;
				}
				case PARTICLES_MAX:
					break;
				default:
					break;
				}
			}
		}
		gameObject->AddComponent(cparticleSystem);
	}
	else if (type == "NavMesh") {

		CNavMeshAgent* comp = new CNavMeshAgent(gameObject);

		comp->active = json_object_get_boolean(componentObject, "Active");
		comp->properties.speed = json_object_get_number(componentObject, "Speed");
		comp->properties.angularSpeed = json_object_get_number(componentObject, "Angular Speed");
		comp->properties.stoppingDistance = json_object_get_number(componentObject, "Stopping Distance");

		gameObject->AddComponent(comp);

	}
}


void JsonFile::SetPrefab(const char* key, const GameObject& gameObject)
{
	JSON_Value* prefabValue = json_value_init_array();
	JSON_Array* prefabArray = json_value_get_array(prefabValue);

	SetGameObject(prefabArray, gameObject);

	// Add the hierarchy array to the main object
	json_object_set_value(rootObject, key, prefabValue);
}

int JsonFile::GetReference(const JSON_Object* componentObject, Component& comp, const char* name, bool push) const
{
	int id = json_object_get_number(componentObject, name);

	if (id != -1)
	{
		comp.vTempReferences.insert(std::pair<std::string, int>(name, id));

		if (push)
		{
			External->scene->vTempComponents.push_back(&comp);
		}
	}

	return id;
}
