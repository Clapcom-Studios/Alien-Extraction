#include "Component.h"
#include "CParticleSystem.h"
#include "CMesh.h" //Necesary for AABBs
#include "GameObject.h"
#include "ModuleResourceManager.h"

#include "MathGeoLib/include/Math/Quat.h"

#include "imGui/imgui.h"
#include "imGui/backends/imgui_impl_opengl3.h"
#include "imGui/backends/imgui_impl_SDL2.h"

#include "EmitterSetting.h"
#include "ParticleEmitter.h"

#include "External/mmgr/mmgr.h"

//#include "parson-master/parson.h" //For JSONs

CParticleSystem::CParticleSystem()
{
	active = true;
	Enable();
	ctype = ComponentType::PARTICLE;
}

CParticleSystem::CParticleSystem(GameObject* own)
{
	mOwner = own;
	UID = External->resourceManager->GenerateNewUID();
	active = true;
	Enable();
	ctype = ComponentType::PARTICLE;

	CreateEmitter();
}


CParticleSystem::~CParticleSystem()
{
	for (auto it = allEmitters.rbegin(); it != allEmitters.rend(); ++it)
	{
		delete (*it);
		(*it) = nullptr;
	}
	allEmitters.clear();
}

void CParticleSystem::Update()
{
	//Si la primera vez que se ejecuta
	Update(TimeManager::DeltaTime);
}

bool CParticleSystem::Update(float dt)
{
	bool ret = true;


	if (dt > 0.00f && (localPlay || TimeManager::gameTimer.GetState() != TimerState::STOPPED)) //Si no esta parado sino que esta paused tambien ha de hacer cosas
	{
		for (uint i = 0; i < allEmitters.size(); ++i)
		{
			allEmitters.at(i)->Update(dt);
		}
	}
	else
	{
		for (uint i = 0; i < allEmitters.size(); ++i)
		{
			allEmitters.at(i)->Reset();
		}
	}

	return ret;
}

bool CParticleSystem::GetActive()
{
	return active;
}

ParticleEmitter* CParticleSystem::CreateEmitter()
{
	//Creamos un nuevo emisor
	ParticleEmitter* emisor = new ParticleEmitter(this);
	emisor->UID = External->resourceManager->GenerateNewUID();
	allEmitters.push_back(emisor);
	//External->renderer3D->particleEmitters = allEmitters;
	emisor->Init(this);

	return emisor;
}

void CParticleSystem::OnInspector()
{
	std::string butonChar;

	//Give an ID to each colapsing header to be able to have more than one of the same time
	//This must be done due to ImGui using the names as the ids of all menus and things
	//int myPosInComponents = mOwner->GetComponentPosition(this); //TODO TONI: Probably it has another way to do this //ERIC: Nada de esto hacia falta, basta con el UID, si que habra que hacer algo para si hay mas de un particle emmiter

	butonChar.append(butonChar.append(std::to_string(UID)).c_str());

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

	bool exists = true;
	int deleteEmitter = -1; //This is outside everything to delete after the ImGui to don't violate the acess.

	ImGui::Checkbox(("##" + std::to_string(UID)).c_str(), &active);
	ImGui::SameLine();
	if (ImGui::CollapsingHeader(("ParticleSystem##" + std::to_string(UID)).c_str(), &exists, flags))
	{
		int treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
		int leafFlags = treeFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		//Activate particles if and only if timer is stopped
		if (TimeManager::gameTimer.GetState() == TimerState::STOPPED)
		{
			std::string playButtonName = localPlay ? "Pause: " : "Play: ";
			if (ImGui::Button(playButtonName.c_str())) {
				if (localPlay)
				{
					Stop();
				}
				else
				{
					Play();
				}
			}
			ImGui::SameLine();
		}

		ImGui::Text("Playback time: %.2f", timer.ReadSec());

		ImGui::Checkbox("ShowOnGodMode", &showEmitters);

		if (ImGui::Button("Play ALL Triggers")) {
			for (int i = 0; i < allEmitters.size(); i++)
			{
				EmitterSpawner* spawner = (EmitterSpawner*)allEmitters.at(i)->modules.at(1);
				spawner->PlayTrigger();
			}
		}

		ImGui::Separator();

		//Crear emitter
		if (ImGui::TreeNodeEx((void*)(intptr_t)-1, treeFlags, "Emitters"))
		{
			for (int i = 0; i < allEmitters.size(); i++)
			{
				std::string nameEmitter;
				if (std::strcmp(allEmitters.at(i)->name.c_str(), "") == 0)
				{
					//ImGui::Text("ParticleEmmiter %i", i);
					nameEmitter.append("Particle Emitter ");
					nameEmitter.append(std::to_string(i + 1));
					allEmitters.at(i)->name = nameEmitter;
				}
				else
				{
					nameEmitter += allEmitters.at(i)->name;
					/*nameEmitter.append(" ## ");
					nameEmitter.append(std::to_string(i + 1));*/
				}

				ImGui::InputText(("Name ##" + std::to_string(i)).c_str(), &allEmitters.at(i)->name);
				//Delete emmiter
				
				if (ImGui::Button(("Delete ##" + std::to_string(i)).c_str()))
				{
					deleteEmitter = i;
				}
				ImGui::SameLine();

				if (ImGui::TreeNodeEx((void*)(intptr_t)i, (!allEmitters.at(i)->modules.empty()) ? treeFlags : leafFlags, nameEmitter.c_str())) //nameEmitter.c_str()
				{
					std::vector<EmitterSetting*> listModule = allEmitters.at(i)->modules;
					int securityCheckTree = 999;
					for (int j= 0; j < listModule.size(); j++)
					{
						std::string particleModule; //Les opciones
						std::string deleteButton; //Lo creamos aqui fuera para evitar petadas, pero como la ID va por nombre ha de ser un string para diferenciarlos

						switch (listModule.at(j)->type)
						{
						case EmitterType::PAR_BASE:
						{
							ImGui::SeparatorText(("BASE ##" + std::to_string(i)).c_str());

							EmitterBase* eBase = (EmitterBase*)listModule.at(j);
							eBase->OnInspector();
							eBase = nullptr;
							break;
						}
						case EmitterType::PAR_SPAWN:
						{
							ImGui::SeparatorText(("SPAWN ##" + std::to_string(i)).c_str());

							EmitterSpawner* eSpawner = (EmitterSpawner*)listModule.at(j);
							eSpawner->OnInspector(allEmitters.at(i));
							eSpawner = nullptr;
							break;
						}
						case EmitterType::PAR_POSITION:
						{
							ImGui::Text(particleModule.append(("POSITION ##" + std::to_string(i))).c_str());
							ImGui::SameLine();
							deleteButton.append("Delete ##").append(std::to_string(j));
							bool toDelete = false;
							if (ImGui::SmallButton(deleteButton.c_str()))
							{
								toDelete = true;
							}
							deleteButton.clear();

							EmitterPosition* ePosition = (EmitterPosition*)listModule.at(j);
							ePosition->OnInspector();
							ePosition = nullptr;

							if(toDelete) { securityCheckTree = allEmitters.at(i)->DestroySetting(j); }
							
							break;
						}
						case EmitterType::PAR_ROTATION:
						{
							ImGui::Text(particleModule.append(("ROTATION ##" + std::to_string(i))).c_str());
							ImGui::SameLine();
							deleteButton.append("Delete ##").append(std::to_string(j));
							bool toDelete = false;
							if (ImGui::SmallButton(deleteButton.c_str()))
							{
								toDelete = true;
							}
							deleteButton.clear();

							EmitterRotation* eRotation = (EmitterRotation*)listModule.at(j);
							eRotation->OnInspector();
							eRotation = nullptr;

							if (toDelete) { securityCheckTree = allEmitters.at(i)->DestroySetting(j); }

							break;
						}
						case EmitterType::PAR_SIZE:
						{
							ImGui::Text(particleModule.append(("SIZE ##" + std::to_string(i))).c_str());
							ImGui::SameLine();
							deleteButton.append("Delete ##").append(std::to_string(j));
							bool toDelete = false;
							if (ImGui::SmallButton(deleteButton.c_str()))
							{
								toDelete = true;
							}
							deleteButton.clear();

							EmitterSize* eSize = (EmitterSize*)listModule.at(j);
							eSize->OnInspector();
							eSize = nullptr;
							
							if (toDelete) { securityCheckTree = allEmitters.at(i)->DestroySetting(j); }

							break;
						}
						case EmitterType::PAR_COLOR:
						{
							ImGui::Text(particleModule.append(("COLOR ##" + std::to_string(i))).c_str());
							ImGui::SameLine();
							deleteButton.append("Delete ##").append(std::to_string(j));
							bool toDelete = false;
							if (ImGui::SmallButton(deleteButton.c_str()))
							{
								toDelete = true;
							}
							deleteButton.clear();

							EmitterColor* eColor = (EmitterColor*)listModule.at(j);
							eColor->OnInspector();
							eColor = nullptr;
							
							if (toDelete) { securityCheckTree = allEmitters.at(i)->DestroySetting(j); }

							break;
						}
						case EmitterType::PAR_IMAGE:
						{
							ImGui::Text(particleModule.append(("IMAGE ##" + std::to_string(i))).c_str());
							ImGui::SameLine();
							deleteButton.append("Delete ##").append(std::to_string(j));
							bool toDelete = false;
							if (ImGui::SmallButton(deleteButton.c_str()))
							{
								toDelete = true;
							}
							deleteButton.clear();

							EmitterImage* eImage = (EmitterImage*)listModule.at(j);
							eImage->OnInspector();
							eImage = nullptr;
							
							if (toDelete) { securityCheckTree = allEmitters.at(i)->DestroySetting(j); }

							break;
						}
						case EmitterType::PARTICLES_MAX:
							//Esto existe para que sea generico el recorrer el switch de emitters
							break;
						default:
							break;
						}
					}
					if (!allEmitters.at(i)->modules.empty() || securityCheckTree == 0) //Necesario para que no pete el tree pop
					{
						ImGui::TreePop();
					}
				}

				std::string CEid;
				if (ImGui::CollapsingHeader(CEid.append("Emitter Options ##").append(std::to_string(i)).c_str()))
				{
					for (int k = 0; k < EmitterType::PARTICLES_MAX; k++)
					{
						std::string emitterType;

						switch (k)
						{
						case EmitterType::PAR_BASE:
							emitterType.assign("Base Emitter ##");
							break;
						case EmitterType::PAR_SPAWN:
							emitterType.assign("Spawn Emitter ##");
							break;
						case EmitterType::PAR_POSITION:
							emitterType.assign("Position Emitter ##");
							break;
						case EmitterType::PAR_ROTATION:
							emitterType.assign("Rotation Emitter ##");
							break;
						case EmitterType::PAR_SIZE:
							emitterType.assign("Scale Emitter ##");
							break;
						case EmitterType::PAR_COLOR:
							emitterType.assign("Color Emitter ##");
							break;
						case EmitterType::PAR_IMAGE:
							emitterType.assign("Image Emitter ##");
							break;
						case EmitterType::PARTICLES_MAX:
							break;
						default:
							break;
						}
						if (!allEmitters.at(i)->EmitterSettingExist((EmitterType)k)) //If the setting already exist on the 
						{
							if (ImGui::Button((emitterType + std::to_string(i)).c_str()))
							{
								allEmitters.at(i)->CreateEmitterSettingByType((EmitterType)k);
							}
						}

					}
					//ImGui::End();
					//ImGui::TreePop();
				}
				std::string inputNameID;
				/*char* textInfo = "";
				ImGui::InputText( inputNameID.append("Emitter Name ##").append(std::to_string(i)).c_str(),textInfo, IM_ARRAYSIZE(textInfo));
				{
					allEmitters.at(i)->name.assign(textInfo);
				}*/
			}

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7, 0.3, 0, 1));
			if (ImGui::Button("Create Particle Emitter"))
			{
				CreateEmitter();
			}
			ImGui::PopStyleColor();

			ImGui::TreePop();
		}

	}

	if (deleteEmitter > -1) 
	{ 
		delete allEmitters.at(deleteEmitter);
		allEmitters.at(deleteEmitter) = nullptr;
		allEmitters.erase(allEmitters.begin() + deleteEmitter); 
	}

	if (!exists) { mOwner->RemoveComponent(this); }
}

//Save emmiter on JSON scene
JSON_Value* CParticleSystem::SaveEmmiterJSON2(ParticleEmitter* emitter)
{
	JSON_Value* root_value = json_value_init_object();
	JSON_Object* root_object = json_value_get_object(root_value);

	//If there is no failure loading
	if (root_value != nullptr && root_object != nullptr)
	{

		//Crear path
		char* serialized_string = NULL;

		json_object_set_number(root_object, "ModulesSize", emitter->modules.size());
		//json_object_set_number(root_object, "OwnerID", UID);
		json_object_set_number(root_object, "EmitterUID", emitter->UID);
		json_object_set_string(root_object, "Name", emitter->name.c_str());

		//Create array of all modules info
		JSON_Array* arr;
		JSON_Value* new_val = json_value_init_array();

		arr = json_value_get_array(new_val);
		json_object_dotset_value(root_object, "Settings", new_val);

		//We call save Emitter in Json for each mesh and we save their UUID
		for (int i = 0; i < emitter->modules.size(); i++)
		{
			//Components (son object)
			JSON_Value* modulo;
			modulo = json_value_init_object();
			JSON_Object* child_object = json_value_get_object(modulo);

			json_object_set_number(child_object, "Type", (int)emitter->modules.at(i)->type);

			//Y aqui un switch para crear en funcion del tipo
			switch (emitter->modules.at(i)->type)
			{
			case EmitterType::PAR_BASE:
			{
				EmitterBase* eBase = (EmitterBase*)emitter->modules.at(i);

				//Lifetime
				json_object_set_number(child_object, "Lifetime", eBase->particlesLifeTime1);
				json_object_set_number(child_object, "Lifetime2", eBase->particlesLifeTime2);
				json_object_set_boolean(child_object, "RandomLT", eBase->randomLT);
				//InitialPosition

				//Position
				JSON_Value* jValuePos = json_value_init_array();
				JSON_Array* arrInitialPos = json_value_get_array(jValuePos);

				json_object_dotset_value(child_object, "Position", jValuePos);
				json_array_append_number(arrInitialPos, eBase->emitterOrigin.x);
				json_array_append_number(arrInitialPos, eBase->emitterOrigin.y);
				json_array_append_number(arrInitialPos, eBase->emitterOrigin.z);

				json_object_set_number(child_object, "DistanceLimit", eBase->distanceLimit);
				json_object_set_boolean(child_object, "HasDistanceLimit", eBase->hasDistanceLimit);

				//Rotacion particle position (is not billboarding)
				json_object_set_number(child_object, "BaseRotation", eBase->rotacionBase);

				//Shape
				json_object_set_number(child_object, "BaseShape", eBase->currentShape);

				//Box Parameters
				JSON_Value* jValuePositiveCube = json_value_init_array();
				JSON_Array* arrInitialPositiveCube = json_value_get_array(jValuePositiveCube);

				json_object_dotset_value(child_object, "PositiveBoxPoints", jValuePositiveCube);
				json_array_append_number(arrInitialPositiveCube, eBase->boxPointsPositives.x);
				json_array_append_number(arrInitialPositiveCube, eBase->boxPointsPositives.y);
				json_array_append_number(arrInitialPositiveCube, eBase->boxPointsPositives.z);

				JSON_Value* jValueNegativeCube = json_value_init_array();
				JSON_Array* arrInitialNegativeCube = json_value_get_array(jValueNegativeCube);

				json_object_dotset_value(child_object, "NegativeBoxPoints", jValueNegativeCube);
				json_array_append_number(arrInitialNegativeCube, eBase->boxPointsNegatives.x);
				json_array_append_number(arrInitialNegativeCube, eBase->boxPointsNegatives.y);
				json_array_append_number(arrInitialNegativeCube, eBase->boxPointsNegatives.z);

				//Cone & Sphere parameters
				json_object_set_number(child_object, "RadiusHollow", eBase->radiusHollow);
				json_object_set_number(child_object, "RadiusBase", eBase->baseRadius);
				json_object_set_number(child_object, "RadiusTop", eBase->topRadius);
				json_object_set_number(child_object, "Heigth", eBase->heigth);

				eBase = nullptr;
				break;
			}
			case EmitterType::PAR_SPAWN:
			{
				EmitterSpawner* eSpawn = (EmitterSpawner*)emitter->modules.at(i);

				//Settings y enums
				json_object_set_number(child_object, "SpawnMode", eSpawn->spawnMode);
				json_object_set_number(child_object, "StartMode", eSpawn->startMode);

				//ParticlesToSpawn
				json_object_set_number(child_object, "NumParticles", eSpawn->numParticlesToSpawn);
				//SpawnSpeed
				json_object_set_number(child_object, "SpawnRatio", eSpawn->spawnRatio);
				//Particles until stop (depends of Start Mode)
				json_object_set_number(child_object, "NumParticlesToStop", eSpawn->numParticlesForStop);

				//Emitter things
				//eSpawn->pointingEmitter; //ParticleEmitter* pointingEmitter; //NO tengo ni idea de como guardar esto (ERIC)
				json_object_set_number(child_object, "PointingUID", (eSpawn->pointingEmitter != nullptr) ? eSpawn->pointingEmitter->UID : 0);
				json_object_set_number(child_object, "ConditionForSpawn", eSpawn->conditionForSpawn);
				json_object_set_number(child_object, "SubemitterMaxLifetime", eSpawn->subMaxLifetime);
				json_object_set_number(child_object, "SubemitterMinLifetime", eSpawn->subMinLifetime);

				eSpawn = nullptr;
				break;
			}
			case EmitterType::PAR_POSITION:
			{
				EmitterPosition* ePosition = (EmitterPosition*)emitter->modules.at(i);

				//Random
				json_object_set_boolean(child_object, "Random", ePosition->randomized);

				//Use shape
				json_object_set_boolean(child_object, "UseShapeBase", ePosition->useBaseShape);

				//Directions
				//InitialDirection
				JSON_Array* arrInitialDir1;

				//Position
				JSON_Value* jValueDir1 = json_value_init_array();
				arrInitialDir1 = json_value_get_array(jValueDir1);

				json_object_dotset_value(child_object, "Direction1", jValueDir1);
				json_array_append_number(arrInitialDir1, ePosition->direction1.x);
				json_array_append_number(arrInitialDir1, ePosition->direction1.y);
				json_array_append_number(arrInitialDir1, ePosition->direction1.z);

				ePosition->direction1;
				//SecondDirection

				//Position
				JSON_Value* jValueDir2 = json_value_init_array();
				JSON_Array* arrInitialDir2 = json_value_get_array(jValueDir2);

				json_object_dotset_value(child_object, "Direction2", jValueDir2);
				json_array_append_number(arrInitialDir2, ePosition->direction2.x);
				json_array_append_number(arrInitialDir2, ePosition->direction2.y);
				json_array_append_number(arrInitialDir2, ePosition->direction2.z);

				json_object_set_boolean(child_object, "NormalizedBaseDirection", ePosition->normalizedSpeed);

				//Accelerate
				json_object_set_boolean(child_object, "Accelerates", ePosition->acceleration);

				//Speed
				json_object_set_number(child_object, "Speed1", ePosition->particleSpeed1);
				json_object_set_number(child_object, "Speed2", ePosition->particleSpeed2);

				//Speed changes
				json_object_set_number(child_object, "ChangeSpeedMode", (int)ePosition->actualSpeedChange);

				JSON_Value* jValueNewDir = json_value_init_array();
				JSON_Array* arrInitialNewDir = json_value_get_array(jValueNewDir);

				json_object_dotset_value(child_object, "NewDirection", jValueNewDir);
				json_array_append_number(arrInitialNewDir, ePosition->newDirection.x);
				json_array_append_number(arrInitialNewDir, ePosition->newDirection.y);
				json_array_append_number(arrInitialNewDir, ePosition->newDirection.z);

				json_object_set_number(child_object, "ChangeSpeed1", ePosition->changeSpeed1);
				json_object_set_number(child_object, "ChangeSpeed2", ePosition->changeSpeed2);

				json_object_set_boolean(child_object, "NormalizedChange", ePosition->normalizedChange);

				ePosition = nullptr;
				break;
			}
			case EmitterType::PAR_ROTATION:
			{
				EmitterRotation* eRotation = (EmitterRotation*)emitter->modules.at(i);

				//Enums de options
				json_object_set_boolean(child_object, "UpdateRotation", eRotation->updateRotation);
				json_object_set_number(child_object, "AligmentMode", eRotation->currentAlignmentMode);
				json_object_set_number(child_object, "WorldOrientation", eRotation->orientationFromWorld);
				json_object_set_number(child_object, "AxisOrientation", eRotation->orientationOfAxis);

				JSON_Value* jValueFreeOrient = json_value_init_array();
				JSON_Array* arrFreeWorldRotation = json_value_get_array(jValueFreeOrient);

				json_object_dotset_value(child_object, "FreeRotation", jValueFreeOrient);
				json_array_append_number(arrFreeWorldRotation, eRotation->freeWorldRotation.x);
				json_array_append_number(arrFreeWorldRotation, eRotation->freeWorldRotation.y);
				json_array_append_number(arrFreeWorldRotation, eRotation->freeWorldRotation.z);

				eRotation = nullptr;
				break;
			}
			case EmitterType::PAR_SIZE:
			{
				EmitterSize* eSize = (EmitterSize*)emitter->modules.at(i);

				//Bool size increases progresivly
				json_object_set_boolean(child_object, "Progressive", eSize->progresive);
				json_object_set_boolean(child_object, "Loop", eSize->loop);

				//Scaling Factor
				json_object_set_number(child_object, "Size1", eSize->sizeMultiplier1);
				json_object_set_number(child_object, "Size2", eSize->sizeMultiplier2);

				//Time
				json_object_set_number(child_object, "TimeStart", eSize->startChange);
				json_object_set_number(child_object, "TimeStop", eSize->stopChange);

				eSize = nullptr;
				break;
			}
			case EmitterType::PAR_COLOR:
			{
				EmitterColor* eColor = (EmitterColor*)emitter->modules.at(i);

				//Bool size increases progresivly
				json_object_set_boolean(child_object, "Progressive", eColor->progresive);
				json_object_set_boolean(child_object, "Loop", eColor->loop);

				//Colors
				//InitialColor
				JSON_Array* arrInitialColor;

				//Position
				JSON_Value* jValueColor1 = json_value_init_array();
				arrInitialColor = json_value_get_array(jValueColor1);

				json_object_dotset_value(child_object, "Color1", jValueColor1);
				json_array_append_number(arrInitialColor, eColor->color1.r);
				json_array_append_number(arrInitialColor, eColor->color1.g);
				json_array_append_number(arrInitialColor, eColor->color1.b);
				json_array_append_number(arrInitialColor, eColor->color1.a);

				//InitialColor
				JSON_Array* arrFinalColor;

				//Position
				JSON_Value* jValueColor2 = json_value_init_array();
				arrFinalColor = json_value_get_array(jValueColor2);

				json_object_dotset_value(child_object, "Color2", jValueColor2);
				json_array_append_number(arrFinalColor, eColor->color2.r);
				json_array_append_number(arrFinalColor, eColor->color2.g);
				json_array_append_number(arrFinalColor, eColor->color2.b);
				json_array_append_number(arrFinalColor, eColor->color2.a);

				//Time
				json_object_set_number(child_object, "TimeStart", eColor->startChange);
				json_object_set_number(child_object, "TimeStop", eColor->stopChange);

				eColor = nullptr;
				break;
			}
			case EmitterType::PAR_IMAGE:
			{
				EmitterImage* eImage = (EmitterImage*)emitter->modules.at(i);
				json_object_set_string(child_object, "Path", eImage->imgPath.c_str());
				eImage = nullptr;
				break;
			}
			case EmitterType::PARTICLES_MAX:
				break;
			default:
				break;
			}

			json_array_append_value(arr, modulo);
		}
	}
	return root_value;
}

void CParticleSystem::Play()
{
	timer.Start();
	localPlay = true;

	int emittersCount = allEmitters.size();

	for (int i = 0; i < emittersCount; ++i)
	{
		allEmitters[i]->Reset();

	}
}

void CParticleSystem::Stop()
{
	timer.Stop();
	localPlay = false;
}


