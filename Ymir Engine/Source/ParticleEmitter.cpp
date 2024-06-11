#include "ParticleEmitter.h"
#include "EmitterSetting.h"
//#include "EmitterShape.h"


#include "Application.h"
#include "CTransform.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ImporterTexture.h"
#include "ModuleScene.h"

#include "External/mmgr/mmgr.h"

ParticleEmitter::ParticleEmitter(CParticleSystem* cParticleParent) //TODO: Solo se puede crear un emiiter funcional por game object, en el segundo no funciona el tree node de ImGui
{
	//Quiza haya que meterle alguna info? IDK
	name = "";
}

ParticleEmitter::~ParticleEmitter()
{
	KillAllParticles();
	if(!modules.empty())
	{
		for (auto it = modules.rbegin(); it != modules.rend(); ++it)
		{
			delete (*it);
			(*it) = nullptr;
		}
		modules.clear();
	}
}

EmitterSetting* ParticleEmitter::CreateEmitterSettingByType(uint type)
{
	//MOST emittter instances cannot be made twice so have an unique value //ERIC TODO: TENGO QUE REVISAR SI PUEDE EXISTIR ALGUN SETTING QUE NO SEA UNIQUE

	if (EmitterSettingExist((EmitterType)type))
	{
		return nullptr;
	}

	EmitterSetting* nuevoEmitter = nullptr;
	switch ((EmitterType)type)
	{
	case EmitterType::PAR_BASE:
	{
		nuevoEmitter = new EmitterBase;
		nuevoEmitter->type = EmitterType::PAR_BASE;
		nuevoEmitter->unique = true;

		break;
	}
	case EmitterType::PAR_SPAWN:
	{
		nuevoEmitter = new EmitterSpawner;
		nuevoEmitter->type = EmitterType::PAR_SPAWN;
		nuevoEmitter->unique = true;

		break;
	}
	case EmitterType::PAR_POSITION:
	{
		nuevoEmitter = new EmitterPosition;
		nuevoEmitter->type = EmitterType::PAR_POSITION;
		nuevoEmitter->unique = true;

		break;
	}
	case EmitterType::PAR_ROTATION:
	{
		nuevoEmitter = new EmitterRotation;
		nuevoEmitter->type = EmitterType::PAR_ROTATION;
		nuevoEmitter->unique = true;

		break;
	}
	case EmitterType::PAR_SIZE:
	{
		nuevoEmitter = new EmitterSize;
		nuevoEmitter->type = EmitterType::PAR_SIZE;
		nuevoEmitter->unique = true;

		break;
	}
	case EmitterType::PAR_COLOR:
	{
		nuevoEmitter = new EmitterColor;
		nuevoEmitter->type = EmitterType::PAR_COLOR;
		nuevoEmitter->unique = true;

		break;
	}
	case EmitterType::PAR_IMAGE:
	{
		nuevoEmitter = new EmitterImage();
		nuevoEmitter->type = EmitterType::PAR_IMAGE;
		nuevoEmitter->unique = true;

		break;
	}
	case EmitterType::PARTICLES_MAX:
		break;
	default:
		break;
	}

	modules.push_back(nuevoEmitter);

	return nuevoEmitter;
}

bool ParticleEmitter::EmitterSettingExist(uint typeS, bool excludeNonUniques)
{
	bool ret = false;

	for (int i = 0; i < modules.size(); i++)
	{
		//If exclude uniques is true the function will return false even if alreadyExists on the list
		if(modules.at(i)->type == (EmitterType)typeS && !(excludeNonUniques && !modules.at(i)->unique))
		{
			ret = true;
		}
	}

	return ret;
}

int ParticleEmitter::DestroySetting(uint pos)
{
	if (modules.size() >= pos)
	{
		delete modules.at(pos);
		modules.at(pos) = nullptr;
		modules.erase(modules.begin() + pos);
	}
	return modules.size(); //Para un check de seguridad porque sino el arbol peta
}

void ParticleEmitter::KillDeadParticles()
{
	for (int i = listParticles.size()-1; i >= 0; i--)
	{
		//Si la particula esta muerta eliminarla del vector
		if (listParticles.at(i)->lifetime >= 1.0f || (listParticles.at(i)->diesByDistance && (math::Distance3(float4(listParticles.at(i)->position, 0.0f), float4(listParticles.at(i)->initialPosition, 0.0f))) > (listParticles.at(i)->distanceLimit)))
		{
			delete listParticles.at(i);
			listParticles.at(i) = nullptr;
			listParticles.erase(listParticles.begin() + i);
		}
	}
}

void ParticleEmitter::KillAllParticles()
{
	if (!listParticles.empty())
	{
		for (auto it = listParticles.rbegin(); it != listParticles.rend(); ++it)
		{
			delete (*it);
			(*it) = nullptr;
		}

		listParticles.clear();
	}
}

void ParticleEmitter::UpdateModules(float dt)
{
	for (int i = 0; i < modules.size(); ++i)
	{
		modules[i]->Update(dt, this);
	}
}

void ParticleEmitter::Init(CParticleSystem* component)
{
	//Si el numero de particles es fijo y lo que hay pues
	this->owner = component;
	//listParticles.resize(MAXPARTICLES);
	//array de indices de particulas (int*)
	//particleIndices = new unsigned int[MAXPARTICLES]
	// 
	//Todo particle system necesita estos elementos de antes y no tenerlos puede tener problems asi que se les crea uno de base
	CreateEmitterSettingByType(EmitterType::PAR_BASE);
	CreateEmitterSettingByType(EmitterType::PAR_SPAWN);
}

void ParticleEmitter::Update(float dt)
{
	if (TimeManager::gameTimer.GetState() != TimerState::PAUSED) //ERIC: Asi esta bien, no se ha de tocar nada
	{	emitterTime += dt;

		//Eliminar las particulas de la lista que ya acabaron su tiempo de vida
		KillDeadParticles();

		//Actualizamos modulos que puedan cambiar con el tiempo (cambiar las particulas, moverlas o lo que haga falta)
		UpdateModules(dt);
	}
}

void ParticleEmitter::Reset()
{
	emitterTime = 0;

	for (int i = 0; i < modules.size(); i++)
	{
		if(modules.at(i)->type == EmitterType::PAR_SPAWN)
		{
			EmitterSpawner* temp = (EmitterSpawner*)modules.at(i);
			temp->PlayTrigger(false);
			temp = nullptr;
		}
	}

	KillAllParticles();
}

void ParticleEmitter::SpawnParticle(uint particlesToAdd) //This code only adds particles to the list, the actual spawning/drawing is made in renderer
{
	if (listParticles.size() < MAXPARTICLES)
	{
		for (int i = 0; i < particlesToAdd && i < MAXPARTICLES; i++)
		{
			//std::unique_ptr<Particle*> particula(new Particle*);
			Particle* particula = new Particle();
			for (int m = 0; m < modules.size(); ++m)
			{
				modules.at(m)->Spawn(this, particula);
			}

			if (listParticles.empty()) { listParticles.push_back(particula); } //Evitar petada acceso a la nada
			else
			{
				bool lastParticle = true;
				float lineToZ;

				#ifdef _STANDALONE
					lineToZ = (External->scene->gameCameraComponent->GetPos().z - (particula->position.z + owner->mOwner->mTransform->GetGlobalPosition().z + (particula->velocity.z * particula->velocity.w)));
				#else
					lineToZ = (External->camera->editorCamera->GetPos().z - (particula->position.z + owner->mOwner->mTransform->GetGlobalPosition().z + (particula->velocity.z * particula->velocity.w)));
				#endif // _STANDALONE

				for (int j = 0; j < listParticles.size(); ++j)
				{
					float lineToZVec;

					#ifdef _STANDALONE
						lineToZVec = (External->scene->gameCameraComponent->GetPos().z - (listParticles.at(j)->position.z + owner->mOwner->mTransform->GetGlobalPosition().z + (listParticles.at(j)->velocity.z * listParticles.at(j)->velocity.w)));
					#else
						lineToZVec = (External->camera->editorCamera->GetPos().z - (listParticles.at(j)->position.z + owner->mOwner->mTransform->GetGlobalPosition().z + (listParticles.at(j)->velocity.z * listParticles.at(j)->velocity.w)));
					#endif // _STANDALONE

					if (lineToZVec * lineToZVec < lineToZ * lineToZ) //Si la particula esta mas lejos se printa primero para las transparencias
					{
						listParticles.emplace(listParticles.begin() + j, particula);
						lastParticle = false;
						break;
					}
				}
				//Si no esta lo suficientemente lejos se coloca al final
				if (lastParticle) { listParticles.push_back(particula); }
			}
		}
			
			
	}
}
