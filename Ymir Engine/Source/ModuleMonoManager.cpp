#include "Globals.h"
#include "Application.h"
#include "ModuleMonoManager.h"
#include "ModuleRenderer3D.h"

#include "External/mono/jit/jit.h"
#include "External/mono/metadata/assembly.h"
#include "External/mono/metadata/environment.h"
#include "External/mono/metadata/mono-config.h"
#include "External/mono/metadata/object.h"
#include "External/mono/metadata/debug-helpers.h"
#include "External/mono/metadata/class.h"
#include "External/mono/metadata/threads.h"

#include "GameObject.h"
#include "CScript.h"
#include "CS_Bindings.h"
#include "CS_Input_Bindings.h"
#include "CS_Audio_Bindings.h"
#include "CS_UI_Bindings.h"
#include "CS_Animation_Bindings.h"
#include "CS_Particle_Bindings.h"
#include "CS_NavMesh_Bindings.h"
#include "CS_SaveLoad_Bindings.h"

#include "PhysfsEncapsule.h"
#include "ModuleEditor.h"

#include <iostream>
#include <fstream>  
#include <filesystem>

#include "External/PugiXML/pugixml.hpp"

//#include "IM_FileSystem.h"
//#include "ImGui/imgui.h"
//#include "WI_TextEditor.h"
//#include "MO_Editor.h"

#include "Log.h"
#include <ImGui/imgui.h>

#pragma comment( lib, "Source/External/mono/libx86/mono-2.0-boehm.lib" )
#pragma comment( lib, "Source/External/mono/libx86/mono-2.0-sgen.lib" )

#include "External/Optick/include/optick.h"

#include "External/mmgr/mmgr.h"

ModuleMonoManager::ModuleMonoManager(Application* app, bool start_enabled) : Module(app, start_enabled), domain(nullptr), domainThread(nullptr), assembly(nullptr), image(nullptr), jitDomain(nullptr)
{
	CMDCompileCS();

	//mono_jit_set_aot_mode(MonoAotMode::MONO_AOT_MODE_HYBRID);
	mono_set_dirs("mono-runtime/lib", "mono-runtime/etc");
	mono_config_parse(NULL);
	jitDomain = mono_jit_init("myapp");

	mono_add_internal_call("YmirEngine.Debug::Log", CSLog);
	mono_add_internal_call("YmirEngine.YmirComponent::get_gameObject", CS_Component_Get_GO);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateGameObject", CSCreateGameObject);
	mono_add_internal_call("YmirEngine.InternalCalls::GetGameObjectByName", FindObjectWithName);
	mono_add_internal_call("YmirEngine.InternalCalls::GetChildrenByName", FindChildrenWithName);
	mono_add_internal_call("YmirEngine.InternalCalls::GetGameObjectByUID", FindObjectWithUID);	
	mono_add_internal_call("YmirEngine.InternalCalls::GetEnemyGameObject", FindEnemyGameObject);
	mono_add_internal_call("YmirEngine.InternalCalls::GetHealtBarObject", FindHealthBarObject);
	mono_add_internal_call("YmirEngine.GameObject::get_parent", CS_GetParent);
	mono_add_internal_call("YmirEngine.InternalCalls::CS_GetChild", CS_GetChild);
	mono_add_internal_call("YmirEngine.InternalCalls::CS_GetChildrenSize", CS_GetChildrenSize);
	mono_add_internal_call("YmirEngine.InternalCalls::CompareGameObjectsByUID", CompareGameObjectsByUID);
	mono_add_internal_call("YmirEngine.GameObject::TryGetComponent", CS_GetComponent);
	mono_add_internal_call("YmirEngine.GameObject::get_Name", Get_GO_Name);


	mono_add_internal_call("YmirEngine.InternalCalls::ExitGame", ExitGame);
	mono_add_internal_call("YmirEngine.InternalCalls::LoadScene", LoadSceneCS);
	mono_add_internal_call("YmirEngine.InternalCalls::Destroy", Destroy);

	mono_add_internal_call("YmirEngine.InternalCalls::CSVToString", CSVToString);
	mono_add_internal_call("YmirEngine.InternalCalls::CSVToStringKeys", CSVToStringKeys);

	mono_add_internal_call("YmirEngine.InternalCalls::CreateGOFromPrefab", CreateGOFromPrefabCS);
	
	
	mono_add_internal_call("YmirEngine.InternalCalls::DisableComponent", DisableComponentCS);

	mono_add_internal_call("YmirEngine.InternalCalls::SpawnItem", SpawnItemCS);
	mono_add_internal_call("YmirEngine.InternalCalls::GetCurrentMap", GetCurrentMapCS);
	mono_add_internal_call("YmirEngine.InternalCalls::SetColor", SetColorMaterial);
	mono_add_internal_call("YmirEngine.InternalCalls::SetPlayerHitBoolean", SetPlayerHitBoolean);

	mono_add_internal_call("YmirEngine.InternalCalls::CS_SetBothFOV", CS_SetBothFOV);
	mono_add_internal_call("YmirEngine.InternalCalls::CS_SetHorizontalFOV", CS_SetHorizontalFOV);
	mono_add_internal_call("YmirEngine.InternalCalls::CS_SetVerticalFOV", CS_SetVerticalFOV);

#pragma region GameObject

	mono_add_internal_call("YmirEngine.GameObject::SetActive", SetActive);
	mono_add_internal_call("YmirEngine.GameObject::SetAsBillboard", SetGameObjectAsBillboardCS);
	mono_add_internal_call("YmirEngine.GameObject::IsActive", IsActiveCS);

	mono_add_internal_call("YmirEngine.GameObject::SetColliderSize", SetColliderSizeCS);
	mono_add_internal_call("YmirEngine.GameObject::SetScale", SetGameObjectScaleCS);

#pragma endregion

	mono_add_internal_call("YmirEngine.InternalCalls::CreateBullet", CreateBullet);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateShotgunSensor", CreateShotgunSensor);
	mono_add_internal_call("YmirEngine.InternalCalls::CreatePrefab", CreatePrefab);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateTailSensor", CreateTailSensor);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateAcidicSpit", CreateAcidicSpit);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateAcidPuddle", CreateAcidPuddle);

#pragma region Transform

	mono_add_internal_call("YmirEngine.Transform::GetForward", GetForward);
	mono_add_internal_call("YmirEngine.Transform::GetRight", GetRight);
	mono_add_internal_call("YmirEngine.Transform::GetUp", GetUp);

	mono_add_internal_call("YmirEngine.Transform::GetLocalForward", GetLocalForward);
	mono_add_internal_call("YmirEngine.Transform::GetLocalRight", GetLocalRight);
	mono_add_internal_call("YmirEngine.Transform::GetLocalUp", GetLocalUp);

	mono_add_internal_call("YmirEngine.Transform::get_localPosition", SendPosition);
	mono_add_internal_call("YmirEngine.Transform::get_globalPosition", SendGlobalPosition);
	mono_add_internal_call("YmirEngine.Transform::set_localPosition", RecievePosition);
	mono_add_internal_call("YmirEngine.Transform::get_localRotation", SendRotation);
	mono_add_internal_call("YmirEngine.Transform::get_globalRotation", SendGlobalRotation);
	mono_add_internal_call("YmirEngine.Transform::set_localRotation", RecieveRotation);

	mono_add_internal_call("YmirEngine.Transform::get_localScale", SendScale);
	mono_add_internal_call("YmirEngine.Transform::get_globalScale", SendGlobalScale);
	mono_add_internal_call("YmirEngine.Transform::set_localScale", RecieveScale);

#pragma endregion

#pragma region Physics

	mono_add_internal_call("YmirEngine.GameObject::SetVelocity", SetVelocity);
	mono_add_internal_call("YmirEngine.GameObject::SetImpulse", SetImpulse);
	mono_add_internal_call("YmirEngine.GameObject::SetRotation", SetRotation);
	mono_add_internal_call("YmirEngine.GameObject::SetPosition", SetPosition);
	mono_add_internal_call("YmirEngine.GameObject::SetColliderSize", SetColliderSize);
	mono_add_internal_call("YmirEngine.GameObject::GetColliderSize", GetColliderSize);
	mono_add_internal_call("YmirEngine.GameObject::ClearForces", ClearForces);
	mono_add_internal_call("YmirEngine.GameObject::RaycastHit", RaycastHit);
	mono_add_internal_call("YmirEngine.GameObject::RaycastTest", RaycastTest);
	mono_add_internal_call("YmirEngine.GameObject::RaycastLenght", RaycastLenght);
	mono_add_internal_call("YmirEngine.GameObject::SetColliderActive", SetColliderActive);

#pragma endregion

#pragma region Tag

	mono_add_internal_call("YmirEngine.GameObject::set_Tag", SetTag);
	mono_add_internal_call("YmirEngine.GameObject::get_Tag", GetTag);
	mono_add_internal_call("YmirEngine.GameObject::GetChildrenByTag", GetChildrenByTag);

#pragma endregion

#pragma region UI

	//
	mono_add_internal_call("YmirEngine.UI::GetCanNav", GetCanNav);
	mono_add_internal_call("YmirEngine.UI::SetCanNav", SetCanNav);

	//
	mono_add_internal_call("YmirEngine.UI::GetUIState", GetUIState);
	mono_add_internal_call("YmirEngine.UI::SetUIState", SetUIState);

	// Image
	mono_add_internal_call("YmirEngine.UI::CreateImageUI", CreateImageUI);
	mono_add_internal_call("YmirEngine.UI::ChangeImageUI", ChangeImageUI);
	//// Animations
	mono_add_internal_call("YmirEngine.UI::GetImageRows", GetImageRows);
	mono_add_internal_call("YmirEngine.UI::GetImageColumns", GetImageColumns);
	mono_add_internal_call("YmirEngine.UI::GetImageCurrentFrameX", GetImageCurrentFrameX);
	mono_add_internal_call("YmirEngine.UI::GetImageCurrentFrameY", GetImageCurrentFrameY);
	mono_add_internal_call("YmirEngine.UI::SetImageCurrentFrame", SetImageCurrentFrame);

	// Button

	// Text
	mono_add_internal_call("YmirEngine.UI::TextEdit", TextEdit);
	mono_add_internal_call("YmirEngine.UI::GetUIText", GetUIText);

	// Slider
	mono_add_internal_call("YmirEngine.UI::SliderEdit", SliderEdit);
	mono_add_internal_call("YmirEngine.UI::SliderSetRange", SliderSetRange);
	mono_add_internal_call("YmirEngine.UI::SliderSetMin", SliderSetMin);
	mono_add_internal_call("YmirEngine.UI::SliderSetMax", SliderSetMax);

	// Inventory
	mono_add_internal_call("YmirEngine.UI::GetSelected", GetSelected);
	mono_add_internal_call("YmirEngine.UI::GetFocused", GetFocused);
	mono_add_internal_call("YmirEngine.UI::SwitchPosition", SwitchPosition);
	mono_add_internal_call("YmirEngine.UI::NavigateGridHorizontal", NavigateGridHorizontal);
	mono_add_internal_call("YmirEngine.UI::NavigateGridVertical", NavigateGridVertical);
	mono_add_internal_call("YmirEngine.UI::NavigateCraftingVertical", NavigateCraftingVertical);
	mono_add_internal_call("YmirEngine.UI::NavigateCraftingHorizontal", NavigateCraftingHorizontal);
	mono_add_internal_call("YmirEngine.UI::CompareStringToName", CompareStringToName);
	mono_add_internal_call("YmirEngine.UI::SetActiveAllUI", SetActiveAllUI);
	mono_add_internal_call("YmirEngine.UI::SetFirstFocused", SetFirstFocused);
	mono_add_internal_call("YmirEngine.UI::SetUIPosWithOther", SetUIPosWithOther);
	mono_add_internal_call("YmirEngine.UI::CheckUI", CheckUI);


#pragma endregion

#pragma region Audio

	mono_add_internal_call("YmirEngine.Audio::PlayAudio", PlayAudio);
	mono_add_internal_call("YmirEngine.Audio::PlayEmbedAudio", PlayEmbedAudio);
	mono_add_internal_call("YmirEngine.Audio::StopAudio", StopAudio);
	mono_add_internal_call("YmirEngine.Audio::StopOneAudio", StopOneAudio);
	mono_add_internal_call("YmirEngine.Audio::ResumeAudio", ResumeAudio);
	mono_add_internal_call("YmirEngine.Audio::PauseAudio", PauseAudio);
	mono_add_internal_call("YmirEngine.Audio::GetVolume", GetVolume);
	mono_add_internal_call("YmirEngine.Audio::SetVolume", SetVolume);
	mono_add_internal_call("YmirEngine.Audio::GetPitch", GetPitch);
	mono_add_internal_call("YmirEngine.Audio::SetPitch", SetPitch);
	mono_add_internal_call("YmirEngine.Audio::GetMuted", GetMuted);
	mono_add_internal_call("YmirEngine.Audio::SetMuted", SetMuted);
	mono_add_internal_call("YmirEngine.Audio::SetState", SetState);
	mono_add_internal_call("YmirEngine.Audio::SetSwitch", SetSwitch);
	mono_add_internal_call("YmirEngine.Audio::PauseAllAudios", PauseAllAudios);
	mono_add_internal_call("YmirEngine.Audio::ResumeAllAudios", ResumeAllAudios);
	mono_add_internal_call("YmirEngine.Audio::StopAllAudios", StopAllAudios);

#pragma endregion

#pragma region GamePad

	mono_add_internal_call("YmirEngine.Input::GetGamepadButton", GetGamepadButton);
	mono_add_internal_call("YmirEngine.Input::GetLeftAxisY", GetLeftAxisY);
	mono_add_internal_call("YmirEngine.Input::GetLeftAxisX", GetLeftAxisX);
	mono_add_internal_call("YmirEngine.Input::GetRightAxisY", GetRightAxisY);
	mono_add_internal_call("YmirEngine.Input::GetRightAxisX", GetRightAxisX);
	mono_add_internal_call("YmirEngine.Input::GetGamepadLeftTrigger", GetGamepadLeftTrigger);
	mono_add_internal_call("YmirEngine.Input::GetGamepadRightTrigger", GetGamepadRightTrigger);
	mono_add_internal_call("YmirEngine.Input::GetKey", GetKey);
	mono_add_internal_call("YmirEngine.Input::IsGamepadButtonAPressedCS", IsGamepadButtonAPressedCS);
	mono_add_internal_call("YmirEngine.Input::IsGamepadButtonBPressedCS", IsGamepadButtonBPressedCS);
	mono_add_internal_call("YmirEngine.Input::GetMouseClick", GetMouseClick);
	mono_add_internal_call("YmirEngine.Input::GetMouseX", MouseX);
	mono_add_internal_call("YmirEngine.Input::GetMouseY", MouseY);
	mono_add_internal_call("YmirEngine.Input::Rumble_Controller", Rumble_Controller);

#pragma endregion

#pragma region Animation

	mono_add_internal_call("YmirEngine.Animation::PlayAnimation", PlayAnimation);
	mono_add_internal_call("YmirEngine.Animation::PauseAnimation", PauseAnimation);
	mono_add_internal_call("YmirEngine.Animation::ResumeAnimation", ResumeAnimation);
	mono_add_internal_call("YmirEngine.Animation::StopAnimation", StopAnimation);
	mono_add_internal_call("YmirEngine.Animation::SetLoop", SetLoop);
	mono_add_internal_call("YmirEngine.Animation::SetBackward", SetBackward);
	mono_add_internal_call("YmirEngine.Animation::SetSpeed", SetSpeed);
	mono_add_internal_call("YmirEngine.Animation::SetPingPong", SetPingPong);
	mono_add_internal_call("YmirEngine.Animation::AddBlendOption", AddBlendOption);
	mono_add_internal_call("YmirEngine.Animation::SetResetToZero", SetResetToZero);
	mono_add_internal_call("YmirEngine.Animation::AnimationHasFinished", HasFinished);
#pragma endregion

#pragma region Particles
	mono_add_internal_call("YmirEngine.Particles::PlayParticlesTrigger", PlayParticlesTrigger);
	mono_add_internal_call("YmirEngine.Particles::ParticleShoot", ParticleShoot);
	mono_add_internal_call("YmirEngine.Particles::PlayParticles", PlayParticles);
	mono_add_internal_call("YmirEngine.Particles::StopParticles", StopParticles);
	mono_add_internal_call("YmirEngine.Particles::RestartParticles", RestartParticles);
	mono_add_internal_call("YmirEngine.Particles::ParticlesForward", ParticlesForward);
	mono_add_internal_call("YmirEngine.Particles::ParticlesSetDirection", ParticlesSetDirection);
	mono_add_internal_call("YmirEngine.Particles::SetMaxDistance", SetMaxDistance);

#pragma endregion

#pragma region Pathfinder
	mono_add_internal_call("YmirEngine.PathFinding::CalculateRandomPath", CS_CalculateRandomPath);
	mono_add_internal_call("YmirEngine.PathFinding::CalculatePath", CS_CalculatePath);
	mono_add_internal_call("YmirEngine.PathFinding::GetDestination", CS_GetDestination);
	mono_add_internal_call("YmirEngine.PathFinding::GetPointAt", CS_GetPointAt);
	mono_add_internal_call("YmirEngine.PathFinding::GetPathSize", CS_GetPathSize);
	mono_add_internal_call("YmirEngine.PathFinding::ClearPath", CS_ClearPath);
	mono_add_internal_call("YmirEngine.PathFinding::GetLastVector", CS_GetLastVector);
	mono_add_internal_call("YmirEngine.PathFinding::get_speed", CS_GetSpeed);
	mono_add_internal_call("YmirEngine.PathFinding::set_speed", CS_SetSpeed);
	mono_add_internal_call("YmirEngine.PathFinding::get_angularSpeed", CS_GetAngularSpeed);
	mono_add_internal_call("YmirEngine.PathFinding::set_angularSpeed", CS_SetAngularSpeed);
	mono_add_internal_call("YmirEngine.PathFinding::get_stoppingDistance", CS_GetStoppingDistance);
	mono_add_internal_call("YmirEngine.PathFinding::set_stoppingDistance", CS_SetStoppingDistance);
	mono_add_internal_call("YmirEngine.PathFinding::IsPathPossible", CS_PathIsPossible);
	mono_add_internal_call("YmirEngine.PathFinding::GetPathIndex", CS_GetPathIndex);


	mono_add_internal_call("YmirEngine.InternalCalls::GetWalkablePointAround", GetWalkablePointAround);
#pragma endregion

#pragma region Enemies

	mono_add_internal_call("YmirEngine.InternalCalls::CreateSpitterAcidSpit", CreateSpitterAcidSpit);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateSpitterAcidExplosive", CreateSpitterAcidExplosive);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateSpitterAcidShrapnel", CreateSpitterAcidShrapnel);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateFaceHuggerTailAttack", CreateFaceHuggerTailAttack);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateDroneClawAttack", CreateDroneClawAttack);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateDroneTailAttack", CreateDroneTailAttack);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateQueenClawAttack", CreateQueenClawAttack);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateQueenTailAttack", CreateQueenTailAttack);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateQueenSpitAttack", CreateQueenSpitAttack);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateQueenPuddle", CreateQueenPuddle);
	mono_add_internal_call("YmirEngine.InternalCalls::CreateQueenShrapnel", CreateQueenShrapnel);

#pragma endregion

#pragma region SaveLoad

	mono_add_internal_call("YmirEngine.SaveLoad::GameFileExists", GameFileExists);
	mono_add_internal_call("YmirEngine.SaveLoad::CreateSaveGameFile", CreateSaveGameFile);
	mono_add_internal_call("YmirEngine.SaveLoad::SaveInt", SaveGameInt);
	mono_add_internal_call("YmirEngine.SaveLoad::SaveFloat", SaveGameFloat);
	mono_add_internal_call("YmirEngine.SaveLoad::SaveBool", SaveGameBool);
	mono_add_internal_call("YmirEngine.SaveLoad::SaveString", SaveGameString);
	mono_add_internal_call("YmirEngine.SaveLoad::SaveIntArray", SaveGameIntArray);
	mono_add_internal_call("YmirEngine.SaveLoad::SaveFloatArray", SaveGameFloatArray);

	mono_add_internal_call("YmirEngine.SaveLoad::LoadInt", LoadGameInt);
	mono_add_internal_call("YmirEngine.SaveLoad::LoadFloat", LoadGameFloat);
	mono_add_internal_call("YmirEngine.SaveLoad::LoadBool", LoadGameBool);
	mono_add_internal_call("YmirEngine.SaveLoad::LoadString", LoadGameString);
	//mono_add_internal_call("YmirEngine.SaveLoad::LoadIntArray", LoadGameIntArray);
	//mono_add_internal_call("YmirEngine.SaveLoad::LoadFloatArray", LoadGameFloatArray);

#pragma endregion

	mono_add_internal_call("YmirEngine.Time::get_deltaTime", GetDT);
	mono_add_internal_call("YmirEngine.Time::get_time", GetTimeCS);

	InitMono();
}

ModuleMonoManager::~ModuleMonoManager()
{}

// -----------------------------------------------------------------
bool ModuleMonoManager::Init()
{
	OPTICK_EVENT();

	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleMonoManager::CleanUp()
{
	OPTICK_EVENT();

	LOG("Cleaning mono domain");

	// Release Mono Classes ( peta :( )
	//ClearVecPtr(userScripts);

	// Release Mono Image and Assembly
	if (image != nullptr) {
		mono_image_close(image);
	}

	// Release Mono Assembly ( peta :( )
	//if (assembly != nullptr) {
	//	mono_assembly_close(assembly);
	//}

	// Release Mono Thread  ( peta :( )
	if (domainThread != nullptr) {
		mono_thread_detach(domainThread);
	}

	// Unload Mono Domains  ( peta :( )
	
	//if (domain != nullptr) {
	//	mono_domain_unload(domain);
	//}
	
	//if (jitDomain != nullptr) { 
	//	mono_domain_unload(jitDomain);
	//}

	mono_jit_cleanup(jitDomain); //Mono cleanup
	system("taskkill /F /IM VBCSCompiler.exe"); // Kills VBCSCompiler via CMD

	return true;
}

void ModuleMonoManager::OnGUI()
{
	if (ImGui::CollapsingHeader("Mono Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Compile tool: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "MSBuild portable version");

	}
}

void ModuleMonoManager::ReCompileCS()
{
	if (TimeManager::gameTimer.GetState() == TimerState::RUNNING)
		return;

	//App->scene->SaveScene("Library/Scenes/tmp.des");	//El Miquel lo tiene q marca la ruta de salida

	//App->scene->CleanScene();		//TODO: No tenemos estas funciones
	//App->renderer3D->ClearAllRenderData();

	mono_domain_unload(domain);
	mono_thread_cleanup();

	while (mono_domain_is_unloading(domain) == true)
	{

	}

	CMDCompileCS();
	InitMono();

	//TODO: No hay nada de esto creado en Ymir
	//App->scene->LoadScene("Library/Scenes/tmp.des");	//El Miquel lo tiene q marca la ruta de salida

	//App->scene->LoadScene();
	//App->fileSystem->DeleteAssetFile("Library/Scenes/tmp.des"); //TODO: Esta funci�n no existe



	External->editor->scriptEditor->LoadScriptTXT(("Assets/Scripts/" + External->editor->scriptEditor->txtName + ".cs").c_str());

	for (auto it = App->scene->gameObjects.begin(); it != App->scene->gameObjects.end(); ++it) {

		if ((*it) != nullptr) {

			CScript* script = (CScript*)(*it)->GetComponent(ComponentType::SCRIPT);
			if (script != nullptr) script->ReloadComponent();
		}
	}
}

//ASK: Is this the worst idea ever? TOO SLOW
float3 ModuleMonoManager::UnboxVector(MonoObject* _obj)
{
	float3 ret;

	MonoClass* klass = mono_object_get_class(_obj);
	mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "x"), &ret.x);
	mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "y"), &ret.y);
	mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "z"), &ret.z);
	return static_cast<float3>(ret);
}

std::vector<GameObject> ModuleMonoManager::UnboxList(MonoObject* _obj)
{
	std::vector<GameObject> ret;

	//MonoClass* klass = mono_object_get_class(_obj);
	//mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "x"), &ret.);
	//mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "y"), &ret.y);
	//mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "z"), &ret.z);
	//mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "w"), &ret.w);
	return ret;
}

//ASK: Is this the worst idea ever? TOO SLOW
Quat ModuleMonoManager::UnboxQuat(MonoObject* _obj)
{
	Quat ret;

	MonoClass* klass = mono_object_get_class(_obj);
	mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "x"), &ret.x);
	mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "y"), &ret.y);
	mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "z"), &ret.z);
	mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "w"), &ret.w);
	return ret;
}

void ModuleMonoManager::DebugAllFields(const char* className, std::vector<SerializedField>& _data, MonoObject* obj, CScript* script, const char* namespace_name)
{
	void* iter = NULL;
	MonoClassField* field;
	MonoClass* klass = mono_class_from_name(mono_assembly_get_image(External->moduleMono->assembly), namespace_name, className);
	while (field = mono_class_get_fields(klass, &iter)) {

		if (mono_field_get_flags(field) != 1) // Private = 1, public = 6, static = 22
		{
			SerializedField pushField = SerializedField(field, obj, script);

			if (pushField.displayName != "##pointer" && pushField.displayName != "##type" && pushField.displayName != "##componentTable")
				_data.push_back(pushField);
			//LOG(LogType::L_NORMAL, mono_field_full_name(method2));
		}
	}
}

void ModuleMonoManager::DebugAllMethods(const char* nsName, const char* className, std::vector<std::string>& _data)
{
	void* iter = NULL;
	MonoMethod* method2;
	MonoClass* klass = mono_class_from_name(mono_assembly_get_image(External->moduleMono->assembly), nsName, className);
	while (method2 = mono_class_get_methods(klass, &iter))
	{
		_data.push_back(mono_method_full_name(method2, 1));
		LOG("Debuging method: %s", mono_method_full_name(method2, 1));
	}
}

MonoObject* ModuleMonoManager::GoToCSGO(GameObject* inGo) const
{
	if (inGo == nullptr) {
		//LOG("[WARNING] GoTOCSGO inGo doesn't exist");
		return nullptr;
	}

	MonoClass* goClass = mono_class_from_name(image, YMIR_SCRIPTS_NAMESPACE, "GameObject");
	uintptr_t goPtr = reinterpret_cast<uintptr_t>(inGo);

	void* args[3];
	args[0] = &inGo->name;
	args[1] = &goPtr;

	uintptr_t transPTR = reinterpret_cast<uintptr_t>(inGo->mTransform);
	args[2] = &transPTR;

	MonoMethodDesc* constructorDesc = mono_method_desc_new("YmirEngine.GameObject:.ctor(string,uintptr,uintptr)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, goClass);
	MonoObject* goObj = mono_object_new(domain, goClass);
	mono_runtime_invoke(method, goObj, args, NULL);

	mono_method_desc_free(constructorDesc);

	return goObj;
}

MonoObject* ModuleMonoManager::Float3ToCS(float3& inVec) const
{

	MonoClass* vecClass = mono_class_from_name(image, YMIR_SCRIPTS_NAMESPACE, "Vector3");

	MonoObject* vecObject = mono_object_new(domain, vecClass);
	const char* name = mono_class_get_name(mono_object_get_class(vecObject));

	void* args[3];
	args[0] = &inVec.x;
	args[1] = &inVec.y;
	args[2] = &inVec.z;

	MonoMethodDesc* constDesc = mono_method_desc_new("YmirEngine.Vector3:.ctor(single,single,single)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constDesc, vecClass);

	mono_runtime_invoke(method, vecObject, args, NULL);

	mono_method_desc_free(constDesc);
	return vecObject;
}

void ModuleMonoManager::LoadFieldData(SerializedField& _field, MonoObject* _object)
{
	switch (_field.type)
	{
	case MonoTypeEnum::MONO_TYPE_BOOLEAN:
		mono_field_get_value(_object, _field.field, &_field.fiValue.bValue);
		break;

	case MonoTypeEnum::MONO_TYPE_I4:
		mono_field_get_value(_object, _field.field, &_field.fiValue.iValue);
		break;

	case MonoTypeEnum::MONO_TYPE_CLASS:
		_field.fiValue.goValue = nullptr;
		break;

	case MonoTypeEnum::MONO_TYPE_R4:
		mono_field_get_value(_object, _field.field, &_field.fiValue.fValue);
		break;

	case MonoTypeEnum::MONO_TYPE_STRING:
		mono_field_get_value(_object, _field.field, &_field.fiValue.strValue);
		break;

	default:
		mono_field_get_value(_object, _field.field, &_field.fiValue.iValue);
		break;
	}
}

MonoObject* ModuleMonoManager::QuatToCS(Quat& inVec) const
{

	MonoClass* quadClass = mono_class_from_name(image, YMIR_SCRIPTS_NAMESPACE, "Quaternion");
	MonoObject* quatObject = mono_object_new(domain, quadClass);

	void* args[4];
	args[0] = &inVec.x;
	args[1] = &inVec.y;
	args[2] = &inVec.z;
	args[3] = &inVec.w;

	MonoMethodDesc* constDesc = mono_method_desc_new("YmirEngine.Quaternion:.ctor(single,single,single,single)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constDesc, quadClass);

	mono_runtime_invoke(method, quatObject, args, NULL);

	mono_method_desc_free(constDesc);
	return quatObject;
}

GameObject* ModuleMonoManager::GameObject_From_CSGO(MonoObject* goObj)
{
	if (goObj == nullptr)
		return nullptr;
	uintptr_t ptr = 0;
	MonoClass* goClass = mono_class_from_name(image, YMIR_SCRIPTS_NAMESPACE, "GameObject");

	mono_field_get_value(goObj, mono_class_get_field_from_name(goClass, "pointer"), &ptr);

	return reinterpret_cast<GameObject*>(ptr);
}

SerializedField::SerializedField() : field(nullptr), parentSC(nullptr)
{
	fiValue.iValue = 0;
}

SerializedField::SerializedField(MonoClassField* _field, MonoObject* _object, CScript* parent) : field(_field)
{
	type = static_cast<MonoTypeEnum>(mono_type_get_type(mono_field_get_type(field)));
	fiValue.iValue = 0;
	parentSC = parent;

	displayName += "##";
	displayName += mono_field_get_name(_field);

	ModuleMonoManager::LoadFieldData(*this, _object);
}

void ModuleMonoManager::CreateAssetsScript(const char* localPath)
{
	std::string unnormalizedPath = localPath;
	unnormalizedPath = PhysfsEncapsule::UnNormalizePath(unnormalizedPath.c_str());

	std::ofstream outfile(unnormalizedPath.c_str());

	std::string className("Assets/");
	std::string startScript = "HelloWorld";
	className += localPath;
	className = className.substr(className.find_last_of("/") + 1);
	className = className.substr(0, className.find_last_of("."));

	className = PhysfsEncapsule::ConvertFileName(className);

	//Default Script Text
	outfile << "using System;"
		<< std::endl << "using System.Collections.Generic;"
		<< std::endl << "using System.Reflection;"
		<< std::endl << "using System.Runtime.CompilerServices;"
		<< std::endl << "using System.Runtime.InteropServices;"
		<< std::endl << ""
		<< std::endl << "using YmirEngine;"
		<< std::endl << ""
		<< std::endl << "public class " << className.c_str() << " : YmirComponent"
		<< std::endl << "{"
		<< std::endl << ""
		<< std::endl << "	public void Start()"
		<< std::endl << "	{"
		<< std::endl << "		Debug.Log(\"" + startScript + "\"); "
		<< std::endl << "	}"
		<< std::endl << ""
		<< std::endl << "	public void Update()"
		<< std::endl << "	{"
		<< std::endl << "		return;"
		<< std::endl << "	}"
		<< std::endl << "}";

	outfile.close();

	AddScriptToSLN(unnormalizedPath.c_str());
	ReCompileCS();
}

void ModuleMonoManager::AddScriptToSLN(const char* scriptLocalPath)
{
	//TODO: El Miquel usa XML, no entiendo como
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("Assembly-CSharp.csproj");

	if (result.status == pugi::xml_parse_status::status_file_not_found)
		assert(false, "XML File not loaded");

	std::string path; // Should be like ../Assets/Scripts/Hola.cs
	path += scriptLocalPath;
	std::string name = path.substr(path.find_last_of("/\\") + 1);
	std::string unnormalizedPath = PhysfsEncapsule::UnNormalizePath(path.c_str());

	pugi::xml_node whereToAdd = doc.child("Project");
	for (pugi::xml_node panel = whereToAdd.first_child(); panel != nullptr; panel = panel.next_sibling())
	{
		if (strcmp(panel.name(), "ItemGroup") == 0 && strcmp(panel.first_child().name(), "Compile") == 0)
		{
			panel = panel.append_child();
			panel.set_name("Compile");
			pugi::xml_attribute att = panel.append_attribute("Include");
			att.set_value(unnormalizedPath.c_str());

			break;
		}
	}

	doc.save_file("Assembly-CSharp.csproj");
}

void ModuleMonoManager::RemoveScriptFromSLN(const char* scriptLocalPath)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("Assembly-CSharp.csproj");

	if (result.status == pugi::xml_parse_status::status_file_not_found)
		assert(false, "XML File not loaded");

	std::string path; // Should be like ../Assets/Scripts/Hola.cs

	pugi::xml_node whereToRemove = doc.child("Project");
	for (pugi::xml_node panel = whereToRemove.first_child(); panel != nullptr; panel = panel.next_sibling())
	{
		if (strcmp(panel.name(), "ItemGroup") == 0 && strcmp(panel.first_child().name(), "Compile") == 0)
		{
			for (pugi::xml_node toRemove = panel.first_child(); toRemove != nullptr; toRemove = toRemove.next_sibling())
			{
				path = PhysfsEncapsule::NormalizePath(toRemove.attribute("Include").as_string());

				if (strcmp(path.c_str(), scriptLocalPath) == 0)
				{
					panel.remove_child(toRemove);
					break;
				}
			}
		}
	}

	doc.save_file("Assembly-CSharp.csproj");
}


void ModuleMonoManager::InitMono()
{
	OPTICK_EVENT();

	//mono_set_dirs("mono-runtime/lib", "mono-runtime/etc");
	//mono_config_parse(NULL);

	domain = mono_domain_create_appdomain("D1", NULL);
	mono_domain_set(domain, 0);
	domainThread = mono_thread_attach(domain);

	//mono_thread_attach(domain);

	MonoImageOpenStatus sts;
	assembly = mono_assembly_open("ScriptsAssembly-Output/Assembly-CSharp.dll", &sts);
	//assembly = mono_domain_assembly_open(domain, "CSSolution/Assembly-CSharp/Build/Assembly-CSharp.dll");
	if (!assembly)
		LOG("ERROR");

	image = mono_assembly_get_image(assembly);



	const MonoTableInfo* table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
	int rows = mono_table_info_get_rows(table_info);

	MonoClass* _class = nullptr;

	userScripts.clear();
	for (int i = 1; i < rows; i++)
	{
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
		const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
		if (name[0] != '<')
		{
			const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			_class = mono_class_from_name(image, name_space, name);

			if (_class != nullptr && strcmp(mono_class_get_namespace(_class), YMIR_SCRIPTS_NAMESPACE) != 0 && !mono_class_is_enum(_class))
			{
				userScripts.push_back(_class);
				LOG("%s", mono_class_get_name(_class));
			}
		}
	}
}