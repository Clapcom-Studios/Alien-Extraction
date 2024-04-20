#include "ParticleEmitter.h"
#include "EmitterSetting.h"
//#include "EmitterShape.h"


#include "Application.h"
#include "CTransform.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ImporterTexture.h"
#include "ModuleScene.h"

ParticleEmitter::ParticleEmitter(CParticleSystem* cParticleParent) //TODO: Solo se puede crear un emiiter funcional por game object, en el segundo no funciona el tree node de ImGui
{
	//Quiza haya que meterle alguna info? IDK
	name = "";
}

ParticleEmitter::~ParticleEmitter()
{
	KillAllParticles();
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
	case PAR_BASE:
	{
		nuevoEmitter = new EmitterBase;
		nuevoEmitter->type = EmitterType::PAR_BASE;
		nuevoEmitter->unique = true;

		break;
	}
	case PAR_SPAWN:
	{
		nuevoEmitter = new EmitterSpawner;
		nuevoEmitter->type = EmitterType::PAR_SPAWN;
		nuevoEmitter->unique = true;

		break;
	}
	case PAR_POSITION:
	{
		nuevoEmitter = new EmitterPosition;
		nuevoEmitter->type = EmitterType::PAR_POSITION;
		nuevoEmitter->unique = true;

		break;
	}
	case PAR_ROTATION:
	{
		nuevoEmitter = new EmitterRotation;
		nuevoEmitter->type = EmitterType::PAR_ROTATION;
		nuevoEmitter->unique = true;

		break;
	}
	case PAR_SIZE:
	{
		nuevoEmitter = new EmitterSize;
		nuevoEmitter->type = EmitterType::PAR_SIZE;
		nuevoEmitter->unique = true;

		break;
	}
	case PAR_COLOR:
	{
		nuevoEmitter = new EmitterColor;
		nuevoEmitter->type = EmitterType::PAR_COLOR;
		nuevoEmitter->unique = true;

		break;
	}
	case PAR_IMAGE:
	{
		nuevoEmitter = new EmitterImage(this);
		nuevoEmitter->type = EmitterType::PAR_IMAGE;
		nuevoEmitter->unique = true;

		break;
	}
	case PAR_SHAPE:
	{
		nuevoEmitter = new EmitterShape(this);
		nuevoEmitter->type = EmitterType::PAR_SHAPE;
		nuevoEmitter->unique = true;
		break;
	}
	case PARTICLES_MAX:
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

int ParticleEmitter::DestroyEmitter(uint pos)
{
	if (modules.size() >= pos)
	{
		modules.erase(modules.begin() + pos);
	}
	return modules.size(); //Para un check de seguridad porque sino el arbol peta
}

void ParticleEmitter::KillDeadParticles()
{
	//A�adimos en una lista todas las posiciones de particulas que queremos eliminar
	std::vector<int> particlesToDelete;

	//Buscamos en toda la lista que particulas estan muertas
	for (int i = 0; i < listParticles.size(); i++)
	{
		//Si la particula esta muerta eliminarla del vector
		if (listParticles.at(i)->lifetime >= 1.0f || (listParticles.at(i)->diesByDistance && (math::Distance3( float4(listParticles.at(i)->position,0.0f), float4(listParticles.at(i)->initialPosition,0.0f) ) ) > (listParticles.at(i)->distanceLimit) ) )
		{
			particlesToDelete.push_back(i);
		}
	}

	//Leemos de final a principio la lista de particulas para eliminarlas y que no haya problemas de cambio de tama�o
	for (int j = particlesToDelete.size() - 1; j >= 0; --j)
	{
		listParticles.erase(listParticles.begin() + particlesToDelete.at(j));
	}
}

void ParticleEmitter::KillAllParticles()
{
	listParticles.clear();
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

	//Llamamos a Draw particles para que printe todas las particulas con su info updateada
	DrawParticles();
	//External->renderer3D->DrawParticles();
}

void ParticleEmitter::DrawParticles()
{
	//ERIC: Si lo unico que se hace es poner esto en true jamas parara, lo cual no ha de ser el caso
	//External->renderer3D->initParticles = true;
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

void ParticleEmitter::SpawnParticle(uint particlesToAdd)
{
	if (listParticles.size() < MAXPARTICLES)
	{
		for (int i = 0; i < particlesToAdd; i++)
		{
			Particle* particula = new Particle();
			for (int m = 0; m < modules.size(); ++m)
			{
				modules.at(m)->Spawn(this, particula);
			}
			//TODO TONI: En principio creo que esto no aplica con la camara del juego, ya que es: camera->editorCamera
			float lineToZ = (External->camera->editorCamera->GetPos().z - (particula->position.z + owner->mOwner->mTransform->GetGlobalPosition().z + (particula->velocity.z * particula->velocity.w)));
			for (int j = 0; j < listParticles.size(); ++j)
			{
				float lineToZVec = (External->camera->editorCamera->GetPos().z - (listParticles.at(j)->position.z + owner->mOwner->mTransform->GetGlobalPosition().z + (listParticles.at(j)->velocity.z * listParticles.at(j)->velocity.w)));
				if (lineToZVec * lineToZVec < lineToZ * lineToZ) //Si la particula esta mas lejos se printa primero para las transparencias
				{
					listParticles.emplace(listParticles.begin() + j, particula);
					break;
				}
			}
			//Si no esta lo suficientemente lejos se coloca al final
			listParticles.push_back(particula);
		}
	}
}
