#include "EmitterSetting.h"
#include "CTransform.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ImporterTexture.h"
#include "ModuleScene.h"

#include "External/mmgr/mmgr.h"

EmitterSetting::EmitterSetting()
{
}

EmitterSetting::~EmitterSetting()
{
	//ERIC: Esto peta
	/*switch (type)
	{
	case PAR_BASE:
	{
		EmitterBase* eBase = (EmitterBase*)this;
		eBase->~EmitterBase();
		eBase = nullptr;
		break;
	}
	case PAR_SPAWN:
	{
		EmitterSpawner* eSpawner = (EmitterSpawner*)this;
		eSpawner->~EmitterSpawner();
		eSpawner = nullptr;
		break;
	}
	case PAR_POSITION:
		{
		EmitterSpawner* eSpawner = (EmitterSpawner*)this;
		eSpawner->~EmitterSpawner();
		eSpawner = nullptr;
		break;
		}
	case PAR_ROTATION:
		{
		EmitterSpawner* eSpawner = (EmitterSpawner*)this;
		eSpawner->~EmitterSpawner();
		eSpawner = nullptr;
		break;
		}
	case PAR_SIZE:
		{
		EmitterSpawner* eSpawner = (EmitterSpawner*)this;
		eSpawner->~EmitterSpawner();
		eSpawner = nullptr;
		break;
		}
	case PAR_COLOR:
		{
		EmitterSpawner* eSpawner = (EmitterSpawner*)this;
		eSpawner->~EmitterSpawner();
		eSpawner = nullptr;
		break;
		}
	case PAR_IMAGE:
		{
		EmitterSpawner* eSpawner = (EmitterSpawner*)this;
		eSpawner->~EmitterSpawner();
		eSpawner = nullptr;
		break;
		}
	case PARTICLES_MAX:
		{
		break;
		}
	default:
		break;
	}*/
}

void EmitterSetting::Update(float dt, ParticleEmitter* emitter)
{

}

void EmitterSetting::OnInspector()
{

}

EmitterBase::EmitterBase()
{
	currentShape = SpawnAreaShape::PAR_CONE;

	//Point
	emitterOrigin = float3::zero;

	//Cone && Sphere
	radiusHollow = 0.0f;
	baseRadius = 2.0f;
	topRadius = 4.0f;
	heigth = 3.0f;
	useAngle = true;
	angle = 45.0f;

	//Box
	boxPointsPositives = { 1,1,1 };
	boxPointsNegatives = { -1,-1,-1 };
	scaleAll = false;

	//Life time
	randomLT = false;
	particlesLifeTime1 = 1.0f;
	particlesLifeTime2 = 2.0f;
	inmortal = false;

	//Die by distance
	hasDistanceLimit = false;
	distanceLimit = 0.0f;

	//World rotation
	rotacionBase = RotationInheritanceParticles::PAR_WORLD_MATRIX;
}

EmitterBase::~EmitterBase()
{
}

void EmitterBase::Spawn(ParticleEmitter* emitter, Particle* particle)
{
	if (randomLT)
	{
		//Lifetime
		float maxLT;
		float minLT;
		if (particlesLifeTime1 > particlesLifeTime2)
		{
			maxLT = particlesLifeTime1;
			minLT = particlesLifeTime2;
		}
		else
		{
			maxLT = particlesLifeTime2;
			minLT = particlesLifeTime1;
		}

		float randomLT = ((float)rand()) / (float)RAND_MAX;
		float rangeLT = maxLT - minLT;
		float fLT = (randomLT * rangeLT) + minLT;

		particle->oneOverMaxLifetime = 1 / fLT;
	}
	else
	{
		particle->oneOverMaxLifetime = 1 / particlesLifeTime1;
	}

	particle->diesByDistance = hasDistanceLimit;
	particle->distanceLimit = distanceLimit;
	
	//World rotation (changes the origin 
	switch (rotacionBase)
	{
	case PAR_WORLD_MATRIX:
		particle->directionRotation = Quat::identity;
		break;
	case PAR_GAMEOBJECT_MATRIX:
		particle->directionRotation = emitter->owner->mOwner->mTransform->GetLocalRotation();
		break;
	case PAR_PARENT_MATRIX:
		//particle->directionRotation = emitter->owner->mOwner->mParent->mTransform->GetGlobalRotation();
		particle->directionRotation = Quat::identity;
		break;
	case PAR_INITIAL_ROTATION_END:
		break;
	default:
		break;
	}

	//Get rotated point from the world
	Quat nuwDirQuat = particle->directionRotation.Mul(Quat(emitterOrigin.x, emitterOrigin.y, emitterOrigin.z, 0));
	float3 originModified = float3(nuwDirQuat.x, nuwDirQuat.y, nuwDirQuat.z);

	switch (currentShape)
	{
	case PAR_POINT:
	{
		CTransform* cTra = (CTransform*)emitter->owner->mOwner->GetComponent(ComponentType::TRANSFORM);
		if (cTra != nullptr)
		{
			float4x4 matrix = cTra->GetGlobalTransform();
			float3 position;
			Quat rotation;
			float3 escale;
			matrix.Decompose(position, rotation, escale);
			particle->position += position + originModified; //Se inicializan desde 0,0,0 asi que no deberia haber problema en hacer += pero deberia ser lo mismo.
			particle->worldRotation = rotation;
			particle->size = escale;
		}

		particle->initialPosition = particle->position;
	}
		break;
	case PAR_CONE:
	{
		//Get random radius size
		float randomLength = Random::GenerateRandomFloat(radiusHollow, baseRadius);
		float randomAngle = Random::GenerateRandomFloat(0.0f, pi * 2);
		float randomHeigth = Random::GenerateRandomFloat(0, heigth);

		float baseTopDiference = topRadius - baseRadius;
		float normalizedRefPos = randomLength / baseRadius;
		float3 randPos = { cos(randomAngle )* ((1 - randomHeigth / heigth) * randomLength + (randomLength * (topRadius / baseRadius)) * randomHeigth / heigth), randomHeigth, -sin(randomAngle) * ((1 - randomHeigth/heigth) * randomLength + (randomLength * (topRadius/baseRadius)) * randomHeigth/heigth) };

		CTransform* cTra = (CTransform*)emitter->owner->mOwner->GetComponent(ComponentType::TRANSFORM);
		if (cTra != nullptr)
		{
			float4x4 matrix = cTra->GetGlobalTransform();
			float3 position;
			Quat rotation;
			float3 escale;
			matrix.Decompose(position, rotation, escale);
			particle->position += position + originModified + randPos; //Se inicializan desde 0,0,0 asi que no deberia haber problema en hacer += pero deberia ser lo mismo.
			particle->worldRotation = rotation;
			particle->size = escale;
		}

		particle->initialPosition = particle->position;
	}
		break;
	case PAR_BOX:
	{
		//Get rotated point from the world
		Quat nuwDirQuat = particle->directionRotation.Mul(Quat(emitterOrigin.x, emitterOrigin.y, emitterOrigin.z, 0));
		float3 originModified = float3(nuwDirQuat.x, nuwDirQuat.y, nuwDirQuat.z);
		
		//Get rotated positives point from the world
		Quat nuwDirPositives = particle->directionRotation.Mul(Quat(boxPointsPositives.x, boxPointsPositives.y, boxPointsPositives.z, 0));
		float3 positivesModified = float3(nuwDirPositives.x, nuwDirPositives.y, nuwDirPositives.z);

		//Get rotated negatives point from the world
		Quat nuwDirNegative = particle->directionRotation.Mul(Quat(boxPointsNegatives.x, boxPointsNegatives.y, boxPointsNegatives.z, 0));
		float3 negativesModified = float3(nuwDirNegative.x, nuwDirNegative.y, nuwDirNegative.z);

		//Random values
		float3 randPos;
		if(negativesModified.x < positivesModified.x){ randPos.x = Random::GenerateRandomFloat(negativesModified.x, positivesModified.x); }
		else{ randPos.x = Random::GenerateRandomFloat(positivesModified.x, negativesModified.x); }
		if (negativesModified.y < positivesModified.y) { randPos.y = Random::GenerateRandomFloat(negativesModified.y, positivesModified.y); }
		else { randPos.y = Random::GenerateRandomFloat(positivesModified.y, negativesModified.y); }
		if (negativesModified.z < positivesModified.z) { randPos.z = Random::GenerateRandomFloat(negativesModified.z, positivesModified.z); }
		else { randPos.z = Random::GenerateRandomFloat(positivesModified.z, negativesModified.z); }


		CTransform* cTra = (CTransform*)emitter->owner->mOwner->GetComponent(ComponentType::TRANSFORM);
		if (cTra != nullptr)
		{
			float4x4 matrix = cTra->GetGlobalTransform();
			float3 position;
			Quat rotation;
			float3 escale;
			matrix.Decompose(position, rotation, escale);
			particle->position += position + originModified + randPos; //Se inicializan desde 0,0,0 asi que no deberia haber problema en hacer += pero deberia ser lo mismo.
			particle->worldRotation = rotation;
			particle->size = escale;
		}

		particle->initialPosition = particle->position;
	}
		break;
	case PAR_SPHERE:
	{
		//Get random radius size
		float randomLength = Random::GenerateRandomFloat(radiusHollow, baseRadius);

		float3 randPos;
		math::LCG lgc;
		randPos = randPos.RandomSphere(lgc, originModified, randomLength);

		CTransform* cTra = (CTransform*)emitter->owner->mOwner->GetComponent(ComponentType::TRANSFORM);
		if (cTra != nullptr)
		{
			float4x4 matrix = cTra->GetGlobalTransform();
			float3 position;
			Quat rotation;
			float3 escale;
			matrix.Decompose(position, rotation, escale);
			particle->position += position + randPos; //Se inicializan desde 0,0,0 asi que no deberia haber problema en hacer += pero deberia ser lo mismo.
			particle->worldRotation = rotation;
			particle->size = escale;
		}

		particle->initialPosition = particle->position;
	}
		break;
	case PAR_SHAPE_ENUM_END:
	{
	
	}
		break;
	default:
		break;
	}
	
}

void EmitterBase::Update(float dt, ParticleEmitter* emitter)
{
	if(!inmortal)
	{
		for (int i = 0; i < emitter->listParticles.size(); i++)
		{
			//Actualizamos el tiempo de vida de cada particula
			emitter->listParticles.at(i)->lifetime += emitter->listParticles.at(i)->oneOverMaxLifetime * dt;
		}
	}
	
}

void EmitterBase::OnInspector()
{

	//Init types (when plays and when it stops)
	std::string modeShape;
	switch (currentShape)
	{
	case SpawnAreaShape::PAR_POINT: modeShape = "Point";break;
	case SpawnAreaShape::PAR_CONE: modeShape = "Cone";break;
	case SpawnAreaShape::PAR_BOX: modeShape = "Box";break;
	case SpawnAreaShape::PAR_SPHERE: modeShape = "Sphere";break;
	case SpawnAreaShape::PAR_SHAPE_ENUM_END:break;
	default:break;
	}

	if (ImGui::BeginCombo("##SpawnShape", modeShape.c_str()))
	{
		for (int i = 0; i < SpawnAreaShape::PAR_SHAPE_ENUM_END; i++)
		{
			switch (SpawnAreaShape(i))
			{
			case SpawnAreaShape::PAR_POINT: modeShape = "Point"; break;
			case SpawnAreaShape::PAR_CONE: modeShape = "Cone"; break;
			case SpawnAreaShape::PAR_BOX: modeShape = "Box"; break;
			case SpawnAreaShape::PAR_SPHERE: modeShape = "Sphere"; break;
			case SpawnAreaShape::PAR_SHAPE_ENUM_END:break;
			default: break;
			}
			if (ImGui::Selectable(modeShape.c_str()))
			{
				currentShape = (SpawnAreaShape)i;
			}
		}

		ImGui::EndCombo();
	}
	ImGui::SameLine;
	ImGui::Text("Shape Mode");

	switch (currentShape)
	{
		case SpawnAreaShape::PAR_POINT:
		{
			ImGui::DragFloat3("Initial Pos. ## BASE", &(this->emitterOrigin[0]), 0.1f);
		}
			break;
		case SpawnAreaShape::PAR_CONE:
		{
			ImGui::DragFloat3("Initial Pos. ## BASE", &(this->emitterOrigin[0]), 0.1f);
			ImGui::DragFloat("Heigth ## BASE", &(this->heigth), 0.2f, 0.01f, 200.f);
			ImGui::DragFloat("Base Radius ## BASE", &(this->baseRadius), 0.2f, radiusHollow+0.01f, 200.f);
			ImGui::Checkbox("Angle", &this->useAngle); ImGui::SameLine();
			if (useAngle) 
			{
				angle = RadToDeg(math::Atan((topRadius - baseRadius) / heigth));
				if(ImGui::DragFloat("Angle ## BASE", &(this->angle), 0.2f, -90, 90,"%.1f"))
				{
					float angleRad = math::Tan(DEGTORAD * angle);
					topRadius = angleRad* heigth + baseRadius;
				}
			}
			else 
			{
				if(ImGui::DragFloat("Radius F. ## BASE", &(this->topRadius), 0.2f, 0.01f, 200.f, "%.1f"))
				{
					angle = RadToDeg(math::Atan((topRadius - baseRadius) / heigth));
				}
			}
			
			ImGui::DragFloat("Empty Area ## BASE", &(this->radiusHollow), 0.1f, -0.0f, baseRadius-0.01f);
		};
			break;
		case SpawnAreaShape::PAR_BOX:
		{
			ImGui::DragFloat3("Initial Pos. ## BASE", &(this->emitterOrigin[0]), 0.1f);
			ImGui::Checkbox("Scale All", &scaleAll);
			if (scaleAll)
			{
				if (ImGui::DragFloat("Box Size ## BASE", &this->boxPointsPositives[0], 0.1F, 0.01F, 100.0F))
				{
					boxPointsPositives[1] = boxPointsPositives[0];
					boxPointsPositives[2] = boxPointsPositives[0];
					boxPointsNegatives[0] = -boxPointsPositives[0];
					boxPointsNegatives[1] = -boxPointsPositives[0];
					boxPointsNegatives[2] = -boxPointsPositives[0];
				}
			}
			else
			{
				ImGui::DragFloat3("Box Size 1 ## BASE", &(this->boxPointsPositives[0]), 0.1f, 0.001f, 200.0f);
				ImGui::DragFloat3("Box Size 2 ## BASE", &(this->boxPointsNegatives[0]), 0.1f, -200.0f, -0.0f);
			}
		}
			break;
		case SpawnAreaShape::PAR_SPHERE: 
		{
			ImGui::DragFloat3("Initial Pos. ## BASE", &(this->emitterOrigin[0]), 0.1f);
			ImGui::DragFloat("Base Radius ## BASE", &(this->baseRadius), 0.2f, radiusHollow + 0.01f, 200.f);
			//ImGui::DragFloat("Empty Area ## BASE", &(this->radiusHollow), 0.1f, -0.0f, baseRadius - 0.01f);
		} 
			break;
		case SpawnAreaShape::PAR_SHAPE_ENUM_END:
			break;
		default:
			break;
	}
	
	ImGui::Checkbox("Inmortal Particle ##BASE", &this->inmortal);
	if (!inmortal)
	{
		ImGui::Checkbox("Random Lifetime ##BASE", &this->randomLT);
		if (this->randomLT)
		{
			ImGui::DragFloat("Life Time1 ## BASE", &(this->particlesLifeTime1), 0.5F, 0.1F, 720.0F);
			ImGui::DragFloat("Life Time2 ## BASE", &(this->particlesLifeTime2), 0.5F, 0.1F, 720.0F);
		}
		else
		{
			ImGui::DragFloat("Life Time ## BASE", &(this->particlesLifeTime1), 0.5F, 0.5F, 720.0F);
		}
	}
	
	ImGui::Checkbox("Dies by Distance ##BASE", &this->hasDistanceLimit);
	if(this->hasDistanceLimit)
	{
		ImGui::SliderFloat("Max Distance ##BASE", &(this->distanceLimit), 0.1f, 100.0f);

	}

	//Init types (when plays and when it stops)
	std::string modeRotInheritance;
	switch (rotacionBase)
	{
	case RotationInheritanceParticles::PAR_WORLD_MATRIX: modeRotInheritance = "No Rotation"; break;
	case RotationInheritanceParticles::PAR_GAMEOBJECT_MATRIX: modeRotInheritance = "Own Rotation"; break;
	case RotationInheritanceParticles::PAR_PARENT_MATRIX: modeRotInheritance = "Parent Rotation"; break;
	case RotationInheritanceParticles::PAR_INITIAL_ROTATION_END:break;
	default: break;
	}

	if (ImGui::BeginCombo("##RotationMode", modeRotInheritance.c_str()))
	{
		for (int i = 0; i < RotationInheritanceParticles::PAR_INITIAL_ROTATION_END; i++)
		{
			switch (RotationInheritanceParticles(i))
			{
			case PAR_WORLD_MATRIX: modeRotInheritance = "No Rotation"; break;
			case PAR_GAMEOBJECT_MATRIX: modeRotInheritance = "Own Rotation"; break;
			case PAR_PARENT_MATRIX: modeRotInheritance = "Parent Rotation"; break;
			case PAR_INITIAL_ROTATION_END:break;
			default: break;
			}
			if (ImGui::Selectable(modeRotInheritance.c_str()))
			{
				rotacionBase = (RotationInheritanceParticles)i;
			}
		}

		ImGui::EndCombo();
	}

	//ImGui::Separator();
}

EmitterSpawner::EmitterSpawner()
{
	startMode = ParticlesSpawnEnabeling::PAR_START_NON_STOP;
	playTriggered = false;
	spawnMode = ParticlesSpawnMode::PAR_ONE_PARTICLE_OVER_DELAY;
	spawnRatio = 0.2f; //Dividir en current time por cuantas se spawnean 
	currentTimer = 0.0f;
	numParticlesToSpawn = 5;
	numParticlesForStop = 100;
	numParticlesSpawned = 0;

	pointingEmitter = nullptr;
	pointingUID = 0;
	conditionForSpawn = SpawnConditionSubemitter::PAR_DIE;
	subMaxLifetime = 0.0f;
	subMinLifetime = 1.0f;
	positionParticleForSub = { 0,0,0 };
}

EmitterSpawner::~EmitterSpawner()
{
	//pointingEmitter = nullptr;
}

void EmitterSpawner::Spawn(ParticleEmitter* emitter, Particle* particle)
{
	if(pointingEmitter != nullptr)
	{
		EmitterBase* pointingBase = (EmitterBase*)emitter->modules.at(0);
		

		switch (pointingBase->currentShape)
		{
		case PAR_POINT:
		{
			//Asignar valores
			particle->position = positionParticleForSub + pointingBase->emitterOrigin;

			particle->initialPosition = particle->position;
		}
		break;
		case PAR_CONE:
		{
			//Get random radius size
			float randomLength = Random::GenerateRandomFloat(pointingBase->radiusHollow, pointingBase->baseRadius);
			float randomAngle = Random::GenerateRandomFloat(0.0f, pi * 2);
			float randomHeigth = Random::GenerateRandomFloat(0, pointingBase->heigth);

			float baseTopDiference = pointingBase->topRadius - pointingBase->baseRadius;
			float normalizedRefPos = randomLength / pointingBase->baseRadius;
			float3 randPos = { cos(randomAngle) * ((1 - randomHeigth / pointingBase->heigth) * randomLength + (randomLength * (pointingBase->topRadius / pointingBase->baseRadius)) * randomHeigth / pointingBase->heigth), randomHeigth, -sin(randomAngle) * ((1 - randomHeigth / pointingBase->heigth) * randomLength + (randomLength * (pointingBase->topRadius / pointingBase->baseRadius)) * randomHeigth / pointingBase->heigth) };
			
			//Asignar valores
			particle->position = positionParticleForSub + randPos; //Se inicializan desde 0,0,0 asi que no deberia haber problema en hacer += pero deberia ser lo mismo

			particle->initialPosition = particle->position;
		}
		break;
		case PAR_BOX:
		{
			//Get rotated point from the world
			Quat nuwDirQuat = particle->directionRotation.Mul(Quat(pointingBase->emitterOrigin.x, pointingBase->emitterOrigin.y, pointingBase->emitterOrigin.z, 0));
			float3 originModified = float3(nuwDirQuat.x, nuwDirQuat.y, nuwDirQuat.z);

			//Get rotated positives point from the world
			Quat nuwDirPositives = particle->directionRotation.Mul(Quat(pointingBase->boxPointsPositives.x, pointingBase->boxPointsPositives.y, pointingBase->boxPointsPositives.z, 0));
			float3 positivesModified = float3(nuwDirPositives.x, nuwDirPositives.y, nuwDirPositives.z);

			//Get rotated negatives point from the world
			Quat nuwDirNegative = particle->directionRotation.Mul(Quat(pointingBase->boxPointsNegatives.x, pointingBase->boxPointsNegatives.y, pointingBase->boxPointsNegatives.z, 0));
			float3 negativesModified = float3(nuwDirNegative.x, nuwDirNegative.y, nuwDirNegative.z);

			//Random values
			float3 randPos;
			if (negativesModified.x < positivesModified.x) { randPos.x = Random::GenerateRandomFloat(negativesModified.x, positivesModified.x); }
			else { randPos.x = Random::GenerateRandomFloat(positivesModified.x, negativesModified.x); }
			if (negativesModified.y < positivesModified.y) { randPos.y = Random::GenerateRandomFloat(negativesModified.y, positivesModified.y); }
			else { randPos.y = Random::GenerateRandomFloat(positivesModified.y, negativesModified.y); }
			if (negativesModified.z < positivesModified.z) { randPos.z = Random::GenerateRandomFloat(negativesModified.z, positivesModified.z); }
			else { randPos.z = Random::GenerateRandomFloat(positivesModified.z, negativesModified.z); }

			//Asignar valores
			particle->position = positionParticleForSub + originModified + randPos;

			particle->initialPosition = particle->position;
		}
		break;
		case PAR_SPHERE:
		{
			//Get random radius size
			float randomLength = Random::GenerateRandomFloat(pointingBase->radiusHollow, pointingBase->baseRadius);

			float3 randPos;
			math::LCG lgc;
			randPos = randPos.RandomSphere(lgc, pointingBase->emitterOrigin, randomLength);

			//Asignar valores
			particle->position = positionParticleForSub + randPos;

			particle->initialPosition = particle->position;
		}
		break;
		case PAR_SHAPE_ENUM_END:
		{}
		break;
		default:
			break;
		}
	}
}

void EmitterSpawner::Update(float dt, ParticleEmitter* emitter)
{
	bool spawnFromStart = false;
	bool countParticles = false;
	switch (startMode)
	{

	case ParticlesSpawnEnabeling::PAR_WAIT_SUBEMITTER:
	{
		spawnFromStart = false;
		countParticles = false;

		if (pointingUID != 0)
		{
			for (int u = 0; u < emitter->owner->allEmitters.size(); u++) 
			{
				if(emitter->owner->allEmitters.at(u)->UID == pointingUID)
				{
					pointingEmitter = emitter->owner->allEmitters.at(u);
				}
			}
			pointingUID = 0;
		}
		if(pointingEmitter != nullptr)
		{
			for (int i = 0; pointingEmitter->listParticles.size() > i; i++)
			{
				switch (conditionForSpawn)
				{
				case SpawnConditionSubemitter::PAR_NEW_SPAWN:
					if (pointingEmitter->listParticles.at(i)->lifetime <= (0.0f + dt)) //Cumple condicion para spawn
					{
						positionParticleForSub = pointingEmitter->listParticles.at(i)->position;
						switch (spawnMode)
						{
						case ParticlesSpawnMode::PAR_NUM_PARTICLES_BURST:
						{
							int remainingParticlesToSpawn = numParticlesToSpawn - emitter->listParticles.size();
							if (remainingParticlesToSpawn > 0)
							{
								emitter->SpawnParticle(remainingParticlesToSpawn);
								if (countParticles)
								{
									numParticlesSpawned += remainingParticlesToSpawn;
								}
							}
						}

						break;
						case ParticlesSpawnMode::PAR_ONE_PARTICLE_OVER_DELAY:
						{
							currentTimer += dt;
							int numToSpawn = 0;
							if (currentTimer > spawnRatio)
							{
								numToSpawn = currentTimer / spawnRatio;
								emitter->SpawnParticle(numToSpawn);
								if (countParticles)
								{
									numParticlesSpawned++;
								}
							}
							currentTimer -= (spawnRatio * numToSpawn);
						}
						break;
						case ParticlesSpawnMode::PAR_NUM_PARTICLES_OVER_DELAY:
						{
							currentTimer += dt;
							int numToSpawn = numParticlesToSpawn;
							int substractTime = 0;
							if (currentTimer > spawnRatio)
							{
								substractTime = currentTimer / spawnRatio;
								emitter->SpawnParticle(numToSpawn);
								if (countParticles)
								{
									numParticlesSpawned += numParticlesToSpawn;
								}
							}
							currentTimer -= (spawnRatio * substractTime);
						}
						break;
						case PAR_SPAWN_MODE_END:
							break;
						default:
							break;
						}
					}
					break;
				case SpawnConditionSubemitter::PAR_DIE:
					if (pointingEmitter->listParticles.at(i)->lifetime >= (1.0f-dt) || (pointingEmitter->listParticles.at(i)->diesByDistance && (math::Distance3(float4(pointingEmitter->listParticles.at(i)->position, 0.0f), float4(pointingEmitter->listParticles.at(i)->initialPosition, 0.0f))) > (pointingEmitter->listParticles.at(i)->distanceLimit))) //Cumple condicion para spawn
					{
						positionParticleForSub = pointingEmitter->listParticles.at(i)->position;
						switch (spawnMode)
						{
						case ParticlesSpawnMode::PAR_NUM_PARTICLES_BURST:
						{
							int remainingParticlesToSpawn = numParticlesToSpawn - emitter->listParticles.size();
							if (remainingParticlesToSpawn > 0)
							{
								emitter->SpawnParticle(remainingParticlesToSpawn);
								if (countParticles)
								{
									numParticlesSpawned += remainingParticlesToSpawn;
								}
							}
						}

						break;
						case ParticlesSpawnMode::PAR_ONE_PARTICLE_OVER_DELAY:
						{
							currentTimer += dt;
							int numToSpawn = 0;
							if (currentTimer > spawnRatio)
							{
								numToSpawn = currentTimer / spawnRatio;
								emitter->SpawnParticle(numToSpawn);
								if (countParticles)
								{
									numParticlesSpawned++;
								}
							}
							currentTimer -= (spawnRatio * numToSpawn);
						}
						break;
						case ParticlesSpawnMode::PAR_NUM_PARTICLES_OVER_DELAY:
						{
							currentTimer += dt;
							int numToSpawn = numParticlesToSpawn;
							int substractTime = 0;
							if (currentTimer > spawnRatio)
							{
								substractTime = currentTimer / spawnRatio;
								emitter->SpawnParticle(numToSpawn);
								if (countParticles)
								{
									numParticlesSpawned += numParticlesToSpawn;
								}
							}
							currentTimer -= (spawnRatio * substractTime);
						}
						break;
						case PAR_SPAWN_MODE_END:
							break;
						default:
							break;
						}
					}
					break;
				case SpawnConditionSubemitter::PAR_INBETWEEN_OF:
					if (pointingEmitter->listParticles.at(i)->lifetime >= subMinLifetime && pointingEmitter->listParticles.at(i)->lifetime <= subMaxLifetime) //Cumple condicion para spawn
					{
						positionParticleForSub = pointingEmitter->listParticles.at(i)->position;
						switch (spawnMode)
						{
						case ParticlesSpawnMode::PAR_NUM_PARTICLES_BURST:
						{
							int remainingParticlesToSpawn = numParticlesToSpawn - emitter->listParticles.size();
							if (remainingParticlesToSpawn > 0)
							{
								emitter->SpawnParticle(remainingParticlesToSpawn);
								if (countParticles)
								{
									numParticlesSpawned += remainingParticlesToSpawn;
								}
							}
						}

						break;
						case ParticlesSpawnMode::PAR_ONE_PARTICLE_OVER_DELAY:
						{
							currentTimer += dt;
							int numToSpawn = 0;
							if (currentTimer > spawnRatio)
							{
								numToSpawn = currentTimer / spawnRatio;
								emitter->SpawnParticle(numToSpawn);
								if (countParticles)
								{
									numParticlesSpawned++;
								}
							}
							currentTimer -= (spawnRatio * numToSpawn);
						}
						break;
						case ParticlesSpawnMode::PAR_NUM_PARTICLES_OVER_DELAY:
						{
							currentTimer += dt;
							int numToSpawn = numParticlesToSpawn;
							int substractTime = 0;
							if (currentTimer > spawnRatio)
							{
								substractTime = currentTimer / spawnRatio;
								emitter->SpawnParticle(numToSpawn);
								if (countParticles)
								{
									numParticlesSpawned += numParticlesToSpawn;
								}
							}
							currentTimer -= (spawnRatio * substractTime);
						}
						break;
						case PAR_SPAWN_MODE_END:
							break;
						default:
							break;
						}
					}
					break;
				case SpawnConditionSubemitter::PAR_END_SPAWN_CONDITION:
					break;
				default:
					break;
				}
			}
		}
		
	}
	break;
	case ParticlesSpawnEnabeling::PAR_START_NON_STOP:
	{
		spawnFromStart = true;
		countParticles = false;
	}
		break;
	case ParticlesSpawnEnabeling::PAR_START_STOP:
	{
		spawnFromStart = true;
		countParticles = true;
	}
		break;
	case ParticlesSpawnEnabeling::PAR_WAIT_NON_STOP:
	{
		spawnFromStart = false;
		countParticles = false;
	}
		break;
	case ParticlesSpawnEnabeling::PAR_WAIT_STOP:
	{
		spawnFromStart = false;
		countParticles = true;
	}
		break;
	case ParticlesSpawnEnabeling::PAR_ENABLE_MODES_END:
		break;
	default:
		break;
	}

	if((spawnFromStart || playTriggered) && numParticlesSpawned < numParticlesForStop && startMode != ParticlesSpawnEnabeling::PAR_WAIT_SUBEMITTER)
	{
		switch (spawnMode)
		{
		case ParticlesSpawnMode::PAR_NUM_PARTICLES_BURST:
		{
			int remainingParticlesToSpawn = numParticlesToSpawn - emitter->listParticles.size();
			if (remainingParticlesToSpawn > 0)
			{
				emitter->SpawnParticle(remainingParticlesToSpawn);
				if (countParticles)
				{
					numParticlesSpawned += remainingParticlesToSpawn;
				}
			}
		}

		break;
		case ParticlesSpawnMode::PAR_ONE_PARTICLE_OVER_DELAY:
		{
			currentTimer += dt;
			int numToSpawn = 0;
			if (currentTimer > spawnRatio)
			{
				numToSpawn = currentTimer / spawnRatio;
				emitter->SpawnParticle(numToSpawn);
				if (countParticles)
				{
					numParticlesSpawned++;
				}
			}
			currentTimer -= (spawnRatio * numToSpawn);
		}
		break;
		case ParticlesSpawnMode::PAR_NUM_PARTICLES_OVER_DELAY:
		{
			currentTimer += dt;
			int numToSpawn = numParticlesToSpawn;
			int substractTime = 0;
			if (currentTimer > spawnRatio)
			{
				substractTime = currentTimer / spawnRatio;
				emitter->SpawnParticle(numToSpawn);
				if (countParticles)
				{
					numParticlesSpawned += numParticlesToSpawn;
				}
			}
			currentTimer -= (spawnRatio * substractTime);
		}
		break;
		case PAR_SPAWN_MODE_END:
			break;
		default:
			break;
		}
	}

	//Stop
	if (numParticlesSpawned >= numParticlesForStop)
	{
		playTriggered = false;
	}
}

bool EmitterSpawner::PlayTrigger(bool val) //Stats play or pause
{
	playTriggered = val;
	numParticlesSpawned = 0;
	return playTriggered;
}

void EmitterSpawner::OnInspector(ParticleEmitter* thisEmitter)
{
	int numParticles = this->numParticlesToSpawn;
	if(thisEmitter->owner->allEmitters.size()<=1 && startMode == ParticlesSpawnEnabeling::PAR_WAIT_SUBEMITTER)
	{
		startMode = ParticlesSpawnEnabeling::PAR_WAIT_NON_STOP;
	}
	if (pointingUID != 0)
	{
		for (int u = 0; u < thisEmitter->owner->allEmitters.size(); u++)
		{
			if (thisEmitter->owner->allEmitters.at(u)->UID == pointingUID)
			{
				pointingEmitter = thisEmitter->owner->allEmitters.at(u);
			}
		}
		pointingUID = 0;
	}

	//Spawn types
	std::string modeName;
	switch (spawnMode)
	{
	case ParticlesSpawnMode::PAR_NUM_PARTICLES_BURST:
		modeName = "Maximun Particles";
		break;
	case ParticlesSpawnMode::PAR_ONE_PARTICLE_OVER_DELAY:
		modeName = "Spawn after delay";
		break;
	case ParticlesSpawnMode::PAR_NUM_PARTICLES_OVER_DELAY:
		modeName = "Burst after delay";
		break;
	case ParticlesSpawnMode::PAR_SPAWN_MODE_END:
		modeName = "";
		break;
	default:
		break;
	}

	if (ImGui::BeginCombo("##SpawnMode", modeName.c_str()))
	{
		for (int i = 0; i < ParticlesSpawnMode::PAR_SPAWN_MODE_END; i++)
		{
			/*std::string modeName;*/

			switch (ParticlesSpawnMode(i))
			{
			case ParticlesSpawnMode::PAR_NUM_PARTICLES_BURST:
				modeName = "Maximun Particles";
				break;
			case ParticlesSpawnMode::PAR_ONE_PARTICLE_OVER_DELAY:
				modeName = "Spawn after delay";
				break;
			case ParticlesSpawnMode::PAR_NUM_PARTICLES_OVER_DELAY:
				modeName = "Burst after delay";
				break;
			case ParticlesSpawnMode::PAR_SPAWN_MODE_END:
				modeName = "";
				break;
			default:
				break;
			}
			if (ImGui::Selectable(modeName.c_str()))
			{
				spawnMode = (ParticlesSpawnMode)i;
			}
		}

		ImGui::EndCombo();
	}
	ImGui::SameLine();
	ImGui::Text("Spawn Mode");

	switch (spawnMode)
	{
	case ParticlesSpawnMode::PAR_NUM_PARTICLES_BURST:
		if (ImGui::SliderInt("Number Particles ## SPAWN", &numParticles, 0, MAXPARTICLES)) 
		{
			this->numParticlesToSpawn = numParticles; //I thing this was made because it exploded in the original engine if using nPRS directly (Eric)
		}
		break;
	case ParticlesSpawnMode::PAR_ONE_PARTICLE_OVER_DELAY:
		ImGui::SliderFloat("Delay ##SPAWN", &(this->spawnRatio), 0.02f, 1.0f);
		break;
	case ParticlesSpawnMode::PAR_NUM_PARTICLES_OVER_DELAY:
		if (ImGui::SliderInt("Number Particles ## SPAWN", &numParticles, 0, MAXPARTICLES))
		{
			this->numParticlesToSpawn = numParticles; //I thing this was made because it exploded in the original engine if using nPRS directly (Eric)
		}
		ImGui::SliderFloat("Delay ##SPAWN", &(this->spawnRatio), 0.02f, 1.0f);
		break;
	case ParticlesSpawnMode::PAR_SPAWN_MODE_END:
		modeName = "";
		break;
	default:
		break;
	}

	//Init types (when plays and when it stops)
	std::string modeName2;
	switch (startMode)
	{
	case ParticlesSpawnEnabeling::PAR_WAIT_SUBEMITTER:modeName2 = "Wait Subemitter";break;
	case ParticlesSpawnEnabeling::PAR_START_NON_STOP:modeName2 = "Start and NonStop";break;
	case ParticlesSpawnEnabeling::PAR_START_STOP:modeName2 = "Start with Stop";break;
	case ParticlesSpawnEnabeling::PAR_WAIT_NON_STOP:modeName2 = "Wait then NonStop";break;
	case ParticlesSpawnEnabeling::PAR_WAIT_STOP:modeName2 = "Wait with Stop";break;
	case ParticlesSpawnEnabeling::PAR_ENABLE_MODES_END:break;
	default:break;
	}

	if (ImGui::BeginCombo("##InitMode", modeName2.c_str()))
	{
		for (int i = (thisEmitter->owner->allEmitters.size()>1) ? -1 : 0; i < ParticlesSpawnEnabeling::PAR_ENABLE_MODES_END; i++)
		{

			switch (ParticlesSpawnEnabeling(i))
			{
			case ParticlesSpawnEnabeling::PAR_WAIT_SUBEMITTER:modeName2 = "Wait Subemitter";break;
			case ParticlesSpawnEnabeling::PAR_START_NON_STOP:modeName2 = "Start and NonStop";break;
			case ParticlesSpawnEnabeling::PAR_START_STOP:modeName2 = "Start with Stop";break;
			case ParticlesSpawnEnabeling::PAR_WAIT_NON_STOP:modeName2 = "Wait then NonStop";break;
			case ParticlesSpawnEnabeling::PAR_WAIT_STOP:modeName2 = "Wait with Stop";break;
			case ParticlesSpawnEnabeling::PAR_ENABLE_MODES_END:break;
			}
			if (ImGui::Selectable(modeName2.c_str()))
			{
				startMode = (ParticlesSpawnEnabeling)i;
			}
		}

		ImGui::EndCombo();
	}
	ImGui::SameLine();
	ImGui::Text("Start Mode");

	switch (startMode)
	{
	case ParticlesSpawnEnabeling::PAR_WAIT_SUBEMITTER:
	{
		std::string actualEmitterName;
		if (pointingEmitter == nullptr)
		{
			actualEmitterName = "None";
		}
		else
		{
			actualEmitterName = pointingEmitter->name;
		}
		if (ImGui::BeginCombo("##Change Emitter", actualEmitterName.c_str()))
		{
			for (auto it = thisEmitter->owner->allEmitters.begin(); it != thisEmitter->owner->allEmitters.end(); it++)
			{
				if ((*it)->UID != thisEmitter->UID)
				{
					actualEmitterName = (*it)->name;
				}
				else
				{
					actualEmitterName = "None";
				}

				if (actualEmitterName.c_str() == "None")
				{
					pointingEmitter = nullptr;
				}
				else if (ImGui::Selectable(actualEmitterName.c_str()))
				{
					pointingEmitter = (*it);
					if (pointingEmitter == thisEmitter)
					{
						pointingEmitter = nullptr;
					}
				}
			}

			ImGui::EndCombo();
		}
		ImGui::SameLine();
		ImGui::Text("Emitter Being Observed");

		if (pointingEmitter != nullptr) //If we have a pointing emitter spawn conditions
		{
			ImGui::Text("Subemitter Conditions");
			std::string spawnCon;

			switch (conditionForSpawn)
			{
			case SpawnConditionSubemitter::PAR_NEW_SPAWN: spawnCon = "Less Than";	break;
			case SpawnConditionSubemitter::PAR_DIE: spawnCon = "More Than";	break;
			case SpawnConditionSubemitter::PAR_INBETWEEN_OF: spawnCon = "In Between Of";	break;
			case SpawnConditionSubemitter::PAR_END_SPAWN_CONDITION: spawnCon = "";	break;
			default:
				break;
			}

			if (ImGui::BeginCombo("##SpawnCondition", spawnCon.c_str()))
			{
				for (int i = 0; i < SpawnConditionSubemitter::PAR_END_SPAWN_CONDITION; i++)
				{
					switch ((SpawnConditionSubemitter)i)
					{
					case SpawnConditionSubemitter::PAR_NEW_SPAWN: spawnCon = "Less Than";	break;
					case SpawnConditionSubemitter::PAR_DIE: spawnCon = "More Than";	break;
					case SpawnConditionSubemitter::PAR_INBETWEEN_OF: spawnCon = "In Between Of";	break;
					case SpawnConditionSubemitter::PAR_END_SPAWN_CONDITION: spawnCon = "";	break;
					}
					if (ImGui::Selectable(spawnCon.c_str()))
					{
						conditionForSpawn = (SpawnConditionSubemitter)i;
					}
				}

				ImGui::EndCombo();
			}

			switch (conditionForSpawn)
			{
			case SpawnConditionSubemitter::PAR_NEW_SPAWN:
			{
				//ImGui::DragFloat("Maximun Time ## SUBEMITTER", &subMaxLifetime, 0.05f, 0.0f, 1.0f);
			}
			break;
			case SpawnConditionSubemitter::PAR_DIE:
			{
				//ImGui::DragFloat("Minimun Time ## SUBEMITTER", &subMinLifetime, 0.05f, 0.0f, 1.0f);
			}
			break;
			case SpawnConditionSubemitter::PAR_INBETWEEN_OF:
			{
				if (subMinLifetime > subMaxLifetime) { subMinLifetime = subMaxLifetime; } //Protection against errors
				ImGui::DragFloat("Minimun Time ## SUBEMITTER", &subMinLifetime, 0.05f, 0.0f, Min(subMaxLifetime, 1.0f));
				ImGui::DragFloat("Maximun Time ## SUBEMITTER", &subMaxLifetime, 0.05f, Max(subMinLifetime, 0.0f), 1.0f);
			}
			break;
			case SpawnConditionSubemitter::PAR_END_SPAWN_CONDITION:
			{
				spawnCon = "";
			}
			break;
			default:
				break;
			}
		}
	}
		break;
	case ParticlesSpawnEnabeling::PAR_START_NON_STOP:
		//modeName2 = "Start and NonStop";
		break;
	case ParticlesSpawnEnabeling::PAR_START_STOP:
		ImGui::SliderInt("Max Particles until stop ## SPAWN", &numParticlesForStop, 1, MAXPARTICLES);
		ImGui::Text(("Remaining Particles:" + std::to_string(numParticlesForStop - numParticlesSpawned)).c_str());
		break;
	case ParticlesSpawnEnabeling::PAR_WAIT_NON_STOP:
		//modeName2 = "Wait then NonStop";
		if (ImGui::Button("Play Trigger"))
		{
			PlayTrigger();
		}
		break;
	case ParticlesSpawnEnabeling::PAR_WAIT_STOP:
		ImGui::SliderInt("Max Particles until stop ## SPAWN", &numParticlesForStop, 1, MAXPARTICLES);
		ImGui::Text(("Remaining Particles:" + std::to_string(numParticlesForStop - numParticlesSpawned)).c_str());
		if (ImGui::Button("Play Trigger"))
		{
			PlayTrigger();
		}
		break;
	case ParticlesSpawnEnabeling::PAR_ENABLE_MODES_END:
		break;
	default:
		break;
	}
	
	ImGui::Separator();
}

EmitterPosition::EmitterPosition()
{
	//Normal direction
	randomized = false; //Si la direccion es solo la uno o un numero random entre la 1 y la 2
	direction1 = { 0,0,0 };
	direction2 = { 0,0,0 };
	normalizedSpeed = true;

	//Use shape for movement
	useBaseShape = false;

	//Normal acceleration
	acceleration = false;
	particleSpeed1 = 1.0f;
	particleSpeed2 = 0.0f;

	//Direction change mid execution
	actualSpeedChange = SpeedChangeMode::PAR_NO_SPEED_CHANGE;
	newDirection = { 0,0,0 };
	changeSpeed1 = 0.0f;
	changeSpeed2 = 1.0f;
	normalizedChange = true;

}

void EmitterPosition::Spawn(ParticleEmitter* emitter, Particle* particle)
{
	if (!useBaseShape)
	{
		if (randomized)
		{
			float3 dir1, dir2;

			dir1 = { direction1.x ,direction1.y ,direction1.z };
			dir2 = { direction2.x ,direction2.y ,direction2.z };

			float maxX, minX;
			if (dir1.x > dir2.x)
			{
				maxX = dir1.x;
				minX = dir2.x;
			}
			else
			{
				maxX = dir2.x;
				minX = dir1.x;
			}

			float maxY, minY;
			if (dir1.y > dir2.y)
			{
				maxY = dir1.y;
				minY = dir2.y;
			}
			else
			{
				maxY = dir2.y;
				minY = dir1.y;
			}

			float maxZ, minZ;
			if (dir1.z > dir2.z)
			{
				maxZ = dir1.z;
				minZ = dir2.z;
			}
			else
			{
				maxZ = dir2.z;
				minZ = dir1.z;
			}

			float randomX = ((float)rand()) / (float)RAND_MAX;
			float rangeX = maxX - minX;
			float fX = (randomX * rangeX) + minX;

			float randomY = ((float)rand()) / (float)RAND_MAX;
			float rangeY = maxY - minY;
			float fY = (randomY * rangeY) + minY;

			float randomZ = ((float)rand()) / (float)RAND_MAX;
			float rangeZ = maxZ - minZ;
			float fZ = (randomZ * rangeZ) + minZ;
			if (normalizedSpeed)
			{
				float newModul = GetModuleVec({ fX,fY,fZ });
				particle->velocity.x += fX / newModul;
				particle->velocity.y += fY / newModul;
				particle->velocity.z += fZ / newModul;
			}
			else
			{
				particle->velocity.x += fX;
				particle->velocity.y += fY;
				particle->velocity.z += fZ;
			}

		}
		else
		{
			if (normalizedSpeed)
			{
				float modul1 = GetModuleVec(direction1);
				if (modul1 > 0)
				{
					particle->velocity.x += direction1.x / modul1;
					particle->velocity.y += direction1.y / modul1;
					particle->velocity.z += direction1.z / modul1;
				}
			}
			else
			{
				particle->velocity.x += direction1.x;
				particle->velocity.y += direction1.y;
				particle->velocity.z += direction1.z;
			}
		}
	}
	else
	{
		if (emitter->modules.at(0)->type == EmitterType::PAR_BASE) //Check of security
		{
			EmitterBase* eBase = (EmitterBase*)emitter->modules.at(0);

			switch (eBase->currentShape)
			{
			case SpawnAreaShape::PAR_POINT:
			{
				if (normalizedSpeed)
				{
					float modul1 = GetModuleVec(eBase->emitterOrigin);
					if (modul1 > 0)
					{
						particle->velocity.x = eBase->emitterOrigin.x / modul1;
						particle->velocity.y = eBase->emitterOrigin.y / modul1;
						particle->velocity.z = eBase->emitterOrigin.z / modul1;
					}
				}
				else
				{
					particle->velocity.x = eBase->emitterOrigin.x;
					particle->velocity.y = eBase->emitterOrigin.y;
					particle->velocity.z = eBase->emitterOrigin.z;
				}
			}
				break;
			case SpawnAreaShape::PAR_CONE:
			{
				float3 vecDire = (particle->initialPosition - emitter->owner->mOwner->mTransform->GetGlobalPosition()) - eBase->emitterOrigin; //Here we can extract 
				float progressOfHeigth = vecDire.y / eBase->heigth; //Value in reference of heigth to base between 0-1
				float3 finalDir = { (eBase->topRadius - eBase->baseRadius) * (vecDire.x) / (eBase->baseRadius * (1 - progressOfHeigth) + (eBase->topRadius * progressOfHeigth)) , eBase->heigth, (eBase->topRadius - eBase->baseRadius) * (vecDire.z) / (eBase->baseRadius * (1 - progressOfHeigth) + (eBase->topRadius * progressOfHeigth)) };
				if (normalizedSpeed)
				{
					float modul1 = GetModuleVec(finalDir);
					if (modul1 > 0)
					{
						particle->velocity.x = finalDir.x / modul1;
						particle->velocity.y = finalDir.y / modul1;
						particle->velocity.z = finalDir.z / modul1;
					}
				}
				else
				{
					particle->velocity.x = finalDir.x;
					particle->velocity.y = finalDir.y;
					particle->velocity.z = finalDir.z;
				}
			}
			break;
			case SpawnAreaShape::PAR_BOX:
			{
				float3 vecDire = (particle->initialPosition - emitter->owner->mOwner->mTransform->GetGlobalPosition()) - eBase->emitterOrigin;
				float3 finalDire = {0,0,0};

				if (Abs(vecDire.x) > Abs(vecDire.y) && Abs(vecDire.x) > Abs(vecDire.z)) //If its X is away enough from the center make it count for movement 
				{
					finalDire.x = vecDire.x/Abs(vecDire.x);
				}
				else if (Abs(vecDire.y) > Abs(vecDire.z)) //If its Y is away enough from the center make it count for movement 
				{
					finalDire.y = vecDire.y / Abs(vecDire.y);
				}
				else //If its Z is away enough from the center make it count for movement 
				{
					finalDire.z = vecDire.z / Abs(vecDire.z);
				}
				if (normalizedSpeed)
				{
					particle->velocity.x = finalDire.x;
					particle->velocity.y = finalDire.y;
					particle->velocity.z = finalDire.z;
				}
				else
				{
					particle->velocity.x = finalDire.x * ((finalDire.x > 0.0f) ? eBase->boxPointsPositives.x : eBase->boxPointsNegatives.x);
					particle->velocity.y = finalDire.y * ((finalDire.y > 0.0f) ? eBase->boxPointsPositives.y : eBase->boxPointsNegatives.y);
					particle->velocity.z = finalDire.z * ((finalDire.z > 0.0f) ? eBase->boxPointsPositives.z : eBase->boxPointsNegatives.z);
				}
			}
			break;
			case SpawnAreaShape::PAR_SPHERE:
			{
				float3 vecDire = (particle->initialPosition - emitter->owner->mOwner->mTransform->GetGlobalPosition()) - eBase->emitterOrigin;
				if (normalizedSpeed)
				{

					float modul1 = GetModuleVec(vecDire);
					if (modul1 > 0)
					{
						particle->velocity.x = vecDire.x / modul1;
						particle->velocity.y = vecDire.y / modul1;
						particle->velocity.z = vecDire.z / modul1;
					}
				}
				else
				{
					particle->velocity.x = vecDire.x;
					particle->velocity.y = vecDire.y;
					particle->velocity.z = vecDire.z;
				}
			}
			break;
			case SpawnAreaShape::PAR_SHAPE_ENUM_END:
			{
				//Nothing
			}
				break;
			default:
				break;
			}

			eBase = nullptr;
		}

	}
	
	particle->velocity.w += particleSpeed1;
	
}

void EmitterPosition::Update(float dt, ParticleEmitter* emitter)
{

	for (int i = 0; i < emitter->listParticles.size(); i++)
	{
		//Acceleration
		float actualLT = emitter->listParticles.at(i)->lifetime;
		if (acceleration)
		{
			emitter->listParticles.at(i)->velocity.w = particleSpeed1 + ((particleSpeed2 - particleSpeed1) * (actualLT / 1.0f));
		}
		else 
		{
			emitter->listParticles.at(i)->velocity.w = particleSpeed1;
		}

		Quat nuwDirQuat = emitter->listParticles.at(i)->directionRotation.Mul(Quat(emitter->listParticles.at(i)->velocity.x, emitter->listParticles.at(i)->velocity.y, emitter->listParticles.at(i)->velocity.z, 0));
		float3 directionParticle = float3(nuwDirQuat.x, nuwDirQuat.y, nuwDirQuat.z);

		float3 directionChange; // Se declara aqui para poder decidir si se normaliza o no;
		if (normalizedChange)
		{
			float newModule = GetModuleVec(newDirection);
			if (newModule == 0) { newModule = 1; } //Para evitar el NaN infinito
			directionChange = { newDirection.x / newModule, newDirection.y / newModule , newDirection.z / newModule };
		}
		else 
		{
			directionChange = newDirection;
		}

		//Maybe aqui podrian haber casos en los que no se quiera usar esto. De momento lo dejo
		Quat nuwDirQuat2 = emitter->listParticles.at(i)->directionRotation.Mul(Quat(directionChange.x, directionChange.y, directionChange.z, 0));
		directionChange = float3(nuwDirQuat2.x, nuwDirQuat2.y, nuwDirQuat2.z);
		 

		switch (actualSpeedChange)
		{
		case SpeedChangeMode::PAR_NO_SPEED_CHANGE:
		{
			emitter->listParticles.at(i)->position.x += directionParticle.x * emitter->listParticles.at(i)->velocity.w * dt;
			emitter->listParticles.at(i)->position.y += directionParticle.y * emitter->listParticles.at(i)->velocity.w * dt;
			emitter->listParticles.at(i)->position.z += directionParticle.z * emitter->listParticles.at(i)->velocity.w * dt; 
		}
			break;
		case SpeedChangeMode::PAR_IF_TIME_ADD:
		{
			if (changeSpeed1<=actualLT && changeSpeed2 >= actualLT)
			{
				emitter->listParticles.at(i)->position.x += (directionParticle.x + directionChange.x) * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.y += (directionParticle.y + directionChange.y) * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.z += (directionParticle.z + directionChange.z) * emitter->listParticles.at(i)->velocity.w * dt;
			}
			else
			{
				emitter->listParticles.at(i)->position.x += directionParticle.x * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.y += directionParticle.y * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.z += directionParticle.z * emitter->listParticles.at(i)->velocity.w * dt;
			}
		}
		break;
		case SpeedChangeMode::PAR_ADD_OVER_TIME:
		{

			if (changeSpeed1 <= actualLT && actualLT <= changeSpeed2)
			{
				emitter->listParticles.at(i)->position.x += (directionParticle.x + directionChange.x * ((actualLT - changeSpeed1) / (changeSpeed2 - changeSpeed1))) * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.y += (directionParticle.y + directionChange.y * ((actualLT - changeSpeed1) / (changeSpeed2 - changeSpeed1))) * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.z += (directionParticle.z + directionChange.z * ((actualLT - changeSpeed1) / (changeSpeed2 - changeSpeed1))) * emitter->listParticles.at(i)->velocity.w * dt;
			}
			else if (actualLT >= changeSpeed2)
			{
				emitter->listParticles.at(i)->position.x += (directionParticle.x + directionChange.x) * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.y += (directionParticle.y + directionChange.y) * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.z += (directionParticle.z + directionChange.z) * emitter->listParticles.at(i)->velocity.w * dt;
			}
			else
			{
				emitter->listParticles.at(i)->position.x += directionParticle.x * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.y += directionParticle.y * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.z += directionParticle.z * emitter->listParticles.at(i)->velocity.w * dt;
			}
		}
		break;
		case SpeedChangeMode::PAR_IF_TIME_SUBSTITUTE:
		{
			if (changeSpeed1 <= actualLT && changeSpeed2 >= actualLT)
			{
				emitter->listParticles.at(i)->position.x += directionChange.x * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.y += directionChange.y * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.z += directionChange.z * emitter->listParticles.at(i)->velocity.w * dt;
			}
			else
			{
				emitter->listParticles.at(i)->position.x += directionParticle.x * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.y += directionParticle.y * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.z += directionParticle.z * emitter->listParticles.at(i)->velocity.w * dt;
			}
		}
		break;
		case SpeedChangeMode::PAR_SUBSTITUTE_OVER_TIME:
		{
			if (changeSpeed1 <= actualLT && actualLT <= changeSpeed2)
			{
				emitter->listParticles.at(i)->position.x += (directionParticle.x * (1 - ((actualLT - changeSpeed1) / (changeSpeed2 - changeSpeed1))) + directionChange.x * ((actualLT - changeSpeed1) / (changeSpeed2 - changeSpeed1))) * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.y += (directionParticle.y * (1 - ((actualLT - changeSpeed1) / (changeSpeed2 - changeSpeed1))) + directionChange.y * ((actualLT - changeSpeed1) / (changeSpeed2 - changeSpeed1))) * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.z += (directionParticle.z * (1 - ((actualLT - changeSpeed1) / (changeSpeed2 - changeSpeed1))) + directionChange.z * ((actualLT - changeSpeed1) / (changeSpeed2 - changeSpeed1))) * emitter->listParticles.at(i)->velocity.w * dt;
			}
			else if (actualLT >= changeSpeed2)
			{
				emitter->listParticles.at(i)->position.x += directionChange.x * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.y += directionChange.y * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.z += directionChange.z * emitter->listParticles.at(i)->velocity.w * dt;
			}
			else
			{
				emitter->listParticles.at(i)->position.x += directionParticle.x * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.y += directionParticle.y * emitter->listParticles.at(i)->velocity.w * dt;
				emitter->listParticles.at(i)->position.z += directionParticle.z * emitter->listParticles.at(i)->velocity.w * dt;
			}
		}
		break;
		default:
			break;
		}
	}
}

void EmitterPosition::OnInspector()
{
	ImGui::Checkbox("Use Shape ##POSITION", &this->useBaseShape);
	if(useBaseShape)
	{
		ImGui::Text("Using the Base Shape");
	}
	else
	{
		ImGui::Checkbox("Random Movement ##POSITION", &this->randomized);
		if (this->randomized)
		{
			ImGui::DragFloat3("Range 1 ##POSITION", &(this->direction1[0]), 0.1f);
			if (normalizedSpeed)
			{
				float newModul = GetModuleVec({ direction1[0],direction1[1],direction1[2] });
				if (newModul == 0.0f) { newModul = 1.0f; } //Evitar en nan defined de dividir entre 0
				ImGui::Text(("( " + std::to_string(direction1[0] / newModul) + ", " + std::to_string(direction1[1] / newModul) + ", " + std::to_string(direction1[2] / newModul) + " )").c_str());
			}
			ImGui::DragFloat3("Range 2 ##POSITION", &(this->direction2[0]), 0.1f);
			if (normalizedSpeed)
			{
				float newModul = GetModuleVec({ direction2[0],direction2[1],direction2[2] });
				if (newModul == 0.0f) { newModul = 1.0f; } //Evitar en nan defined de dividir entre 0
				ImGui::Text(("( " + std::to_string(direction2[0] / newModul) + ", " + std::to_string(direction2[1] / newModul) + ", " + std::to_string(direction2[2] / newModul) + " )").c_str());
			}
		}
		else
		{
			ImGui::DragFloat3("Position", &(this->direction1[0]), 0.1f);
			if (normalizedSpeed)
			{
				float newModul = GetModuleVec({ direction1[0],direction1[1],direction1[2] });
				if (newModul == 0.0f) { newModul = 1.0f; } //Evitar en nan defined de dividir entre 0
				ImGui::Text(("( " + std::to_string(direction1[0] / newModul) + ", " + std::to_string(direction1[1] / newModul) + ", " + std::to_string(direction1[2] / newModul) + " )").c_str());
			}
		}
	}
	
	ImGui::Checkbox("Normalize Dir. ##POSITION", &this->normalizedSpeed);
	ImGui::Checkbox("Acceleration ##POSITION", &this->acceleration);
	if (this->acceleration)
	{
		ImGui::DragFloat("SpeedInit ##POSITION", &(this->particleSpeed1), 0.2F);
		ImGui::DragFloat("SpeedFinal ##POSITION", &(this->particleSpeed2), 0.2F);
	}
	else
	{
		ImGui::DragFloat("Speed ##POSITION", &(this->particleSpeed1), 0.2F);
	}

	std::string modeName;
	switch (actualSpeedChange)
	{
	case SpeedChangeMode::PAR_NO_SPEED_CHANGE:modeName = "None"; break;
	case SpeedChangeMode::PAR_IF_TIME_ADD:modeName = "Add during time"; break;
	case SpeedChangeMode::PAR_ADD_OVER_TIME:modeName = "Add over time"; break;
	case SpeedChangeMode::PAR_IF_TIME_SUBSTITUTE:modeName = "Change during time"; break;
	case SpeedChangeMode::PAR_SUBSTITUTE_OVER_TIME:modeName = "Change over time"; break;
	default:break;
	}

	if (ImGui::BeginCombo("##ChangeSpeed", modeName.c_str()))
	{
		for (int i = 0; i < SpeedChangeMode::PAR_SPEED_CHANGE_MODE_END; i++)
		{
			/*std::string modeName;*/

			switch ((SpeedChangeMode)i)
			{
			case SpeedChangeMode::PAR_NO_SPEED_CHANGE:modeName = "None"; break;
			case SpeedChangeMode::PAR_IF_TIME_ADD:modeName = "Add during time"; break;
			case SpeedChangeMode::PAR_ADD_OVER_TIME:modeName = "Add over time"; break;
			case SpeedChangeMode::PAR_IF_TIME_SUBSTITUTE:modeName = "Change during time"; break;
			case SpeedChangeMode::PAR_SUBSTITUTE_OVER_TIME:modeName = "Change over time"; break;
			default:break;
			}
			if (ImGui::Selectable(modeName.c_str()))
			{
				actualSpeedChange = (SpeedChangeMode)i;
			}
		}

		ImGui::EndCombo();
	}

	ImGui::Checkbox("Normalize Change Dir. ##POSITION", &this->normalizedChange);

	switch (actualSpeedChange)
	{
	case SpeedChangeMode::PAR_NO_SPEED_CHANGE:
		//Nothing
		break;
	case SpeedChangeMode::PAR_IF_TIME_ADD:
		ImGui::DragFloat3("New Direction", &(this->newDirection[0]), 0.1f);
		if (normalizedChange)
		{
			float newModul = GetModuleVec({ newDirection[0],newDirection[1],newDirection[2] });
			if (newModul == 0.0f) { newModul = 1.0f; } //Evitar en nan defined de dividir entre 0
			ImGui::Text(("( " + std::to_string(newDirection[0] / newModul) + ", " + std::to_string(newDirection[1] / newModul) + ", " + std::to_string(newDirection[2] / newModul) + " )").c_str());
		}
		ImGui::SliderFloat("Start Adding ##PositionsChange", &(this->changeSpeed1), 0.0f, (this->changeSpeed2 - 0.05f));
		ImGui::SliderFloat("Stop Adding ##PositionsChange", &(this->changeSpeed2), this->changeSpeed1 + 0.05f, 1.0f);
		break;
	case SpeedChangeMode::PAR_ADD_OVER_TIME:
		ImGui::DragFloat3("New Direction", &(this->newDirection[0]), 0.1f);
		if (normalizedChange)
		{
			float newModul = GetModuleVec({ newDirection[0],newDirection[1],newDirection[2] });
			if (newModul == 0.0f) { newModul = 1.0f; } //Evitar en nan defined de dividir entre 0
			ImGui::Text(("( " + std::to_string(newDirection[0] / newModul) + ", " + std::to_string(newDirection[1] / newModul) + ", " + std::to_string(newDirection[2] / newModul) + " )").c_str());
		}
		ImGui::SliderFloat("Start Change ##PositionsChange", &(this->changeSpeed1), 0.0f, (this->changeSpeed2 - 0.05f));
		ImGui::SliderFloat("Stop Change ##PositionsChange", &(this->changeSpeed2), this->changeSpeed1 + 0.05f, 1.0f);
		break;
	case SpeedChangeMode::PAR_IF_TIME_SUBSTITUTE:
		ImGui::DragFloat3("New Direction", &(this->newDirection[0]), 0.1f);
		if (normalizedChange)
		{
			float newModul = GetModuleVec({ newDirection[0],newDirection[1],newDirection[2] });
			if (newModul == 0.0f) { newModul = 1.0f; } //Evitar en nan defined de dividir entre 0
			ImGui::Text(("( " + std::to_string(newDirection[0] / newModul) + ", " + std::to_string(newDirection[1] / newModul) + ", " + std::to_string(newDirection[2] / newModul) + " )").c_str());
		}
		ImGui::SliderFloat("Start Adding ##PositionsChange", &(this->changeSpeed1), 0.0f, (this->changeSpeed2 - 0.05f));
		ImGui::SliderFloat("Stop Adding ##PositionsChange", &(this->changeSpeed2), this->changeSpeed1 + 0.05f, 1.0f);
		break;
	case SpeedChangeMode::PAR_SUBSTITUTE_OVER_TIME:
		ImGui::DragFloat3("New Direction", &(this->newDirection[0]), 0.1f);
		if (normalizedChange)
		{
			float newModul = GetModuleVec({ newDirection[0],newDirection[1],newDirection[2] });
			if (newModul == 0.0f) { newModul = 1.0f; } //Evitar en nan defined de dividir entre 0
			ImGui::Text(("( " + std::to_string(newDirection[0] / newModul) + ", " + std::to_string(newDirection[1] / newModul) + ", " + std::to_string(newDirection[2] / newModul) + " )").c_str());
		}
		ImGui::SliderFloat("Start Change ##PositionsChange", &(this->changeSpeed1), 0.0f, (this->changeSpeed2 - 0.05f));
		ImGui::SliderFloat("Stop Change ##PositionsChange", &(this->changeSpeed2), this->changeSpeed1 + 0.05f, 1.0f);
		break;
	case SpeedChangeMode::PAR_SPEED_CHANGE_MODE_END:
		break;
	default:
		break;
	}

	ImGui::Separator();
}

float EmitterPosition::GetModuleVec(float3 vec)
{
	return sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
}

EmitterRotation::EmitterRotation()
{
	updateRotation = true;
	currentAlignmentMode = BillboardType::PAR_LOOK_GAME_CAMERA;
	orientationFromWorld = OrientationDirection::PAR_Y_AXIS;
	orientationOfAxis = OrientationDirection::PAR_Y_AXIS;
	freeWorldRotation = { 0,0,0 };
}

void EmitterRotation::Spawn(ParticleEmitter* emitter, Particle* particle)
{
	switch (currentAlignmentMode)
	{
	case BillboardType::PAR_LOOK_EDITOR_CAMERA:
		EditorCameraAlign();
		break;
	case BillboardType::PAR_LOOK_GAME_CAMERA:
		GameCameraAlign();
		break;
	case BillboardType::PAR_WORLD_ALIGNED:
		WorldAlign();
		break;
	case BillboardType::PAR_AXIS_ALIGNED:
		AxisAlign(emitter); //Este calculo requiere de tantas cosas que necesita propio acceso a emitter y recorrer cada particula
		break;
	case BillboardType::PAR_BILLBOARDING_MODE_END:
		break;
	default:
		break;
	}

	particle->worldRotation = rotation;
}

void EmitterRotation::Update(float dt, ParticleEmitter* emitter)
{
	if(updateRotation)
	{
		switch (currentAlignmentMode)
		{
		case BillboardType::PAR_LOOK_EDITOR_CAMERA:
			EditorCameraAlign();
			break;
		case BillboardType::PAR_LOOK_GAME_CAMERA:
			GameCameraAlign();
			break;
		case BillboardType::PAR_WORLD_ALIGNED:
			WorldAlign();
			break;
		case BillboardType::PAR_AXIS_ALIGNED:
			AxisAlign(emitter); //Este calculo requiere de tantas cosas que necesita propio acceso a emitter y recorrer cada particula
			break;
		case BillboardType::PAR_BILLBOARDING_MODE_END:
			break;
		default:
			break;
		}

		if(currentAlignmentMode!= BillboardType::PAR_AXIS_ALIGNED)
		{
			for (int i = 0; i < emitter->listParticles.size(); i++)
			{
				//emitter->listParticles.at(i)->worldRotation = tempRot;
				emitter->listParticles.at(i)->worldRotation = rotation;
			}
		}
	}
	
	
}

void EmitterRotation::OnInspector()
{
	std::string tempAlignment;

	ImGui::Checkbox("Update Rotation ## ROT", &updateRotation);

	switch (currentAlignmentMode) 
	{
	case BillboardType::PAR_AXIS_ALIGNED: tempAlignment = "Axis Aligned"; break;
		case BillboardType::PAR_LOOK_EDITOR_CAMERA: tempAlignment = "Screen Aligned"; break;
		case BillboardType::PAR_LOOK_GAME_CAMERA: tempAlignment = "Camera Aligned"; break;
		case BillboardType::PAR_WORLD_ALIGNED: tempAlignment = "World Aligned"; break;
		case BillboardType::PAR_BILLBOARDING_MODE_END: tempAlignment = "Not Aligned Yet"; break;
	}
	
	ImGui::Text("Current Billboard: %s", tempAlignment.c_str());

	if (ImGui::BeginCombo("##ChangeBillboard", tempAlignment.c_str()))
	{
		for (int i = 0; i < BillboardType::PAR_BILLBOARDING_MODE_END; i++)
		{
			switch ((BillboardType)i)
			{
				case BillboardType::PAR_AXIS_ALIGNED: tempAlignment = "Axis Aligned"; break;
				case BillboardType::PAR_LOOK_EDITOR_CAMERA: tempAlignment = "Screen Aligned"; break;
				case BillboardType::PAR_LOOK_GAME_CAMERA: tempAlignment = "Camera Aligned"; break;
				case BillboardType::PAR_WORLD_ALIGNED: tempAlignment = "World Aligned"; break;
				case BillboardType::PAR_BILLBOARDING_MODE_END: tempAlignment = ""; break;
			}
			if (ImGui::Selectable(tempAlignment.c_str()))
			{
				currentAlignmentMode = (BillboardType)i;
			}
		}

		ImGui::EndCombo();
	}

	switch (currentAlignmentMode)
	{
	case BillboardType::PAR_AXIS_ALIGNED: 
	{
		//Yo aqui pondria un float 3 o algo, idk, revisare como lo hace unity
		//Unity hace algo raro, vertical es que si rota pero solo en el eje Y, horizontal no rota solo mira arriba. La idea seria mas despues que una opcion sea mirar solo dirrecion y otra con el vertical rotar pero en un solo eje
		//Lo unico que no es lo mas util del mundo, se puede dejar para lo ultimo o quiza ni hacerlo, porque con vertical axis fijo tenemos de sobra

		ImGui::Text("Actual Axis Orientation");
		std::string actualAxisOrient;
		switch (orientationOfAxis)
		{
		case OrientationDirection::PAR_X_AXIS:actualAxisOrient = "Fixed X Axis"; break;
		case OrientationDirection::PAR_Y_AXIS:actualAxisOrient = "Fixed Y Axis"; break;
		case OrientationDirection::PAR_Z_AXIS:actualAxisOrient = "Fixed Z Axis"; break;
		case OrientationDirection::PAR_ORIENTATION_DIRECTION_END:actualAxisOrient = ""; break;
		default:actualAxisOrient = "None";
			break;
		}

		if (ImGui::BeginCombo("##Change In Fixed Axis Direction", actualAxisOrient.c_str()))
		{
			for (int i = OrientationDirection::PAR_X_AXIS; i < OrientationDirection::PAR_ORIENTATION_DIRECTION_END; i+=2)
			{
				switch ((OrientationDirection)i)
				{
				case OrientationDirection::PAR_X_AXIS:actualAxisOrient = "Fixed X Axis"; break;
				case OrientationDirection::PAR_Y_AXIS:actualAxisOrient = "Fixed Y Axis"; break;
				case OrientationDirection::PAR_Z_AXIS:actualAxisOrient = "Fixed Z Axis"; break;
				case OrientationDirection::PAR_ORIENTATION_DIRECTION_END:actualAxisOrient = ""; break;
				}
				if (ImGui::Selectable(actualAxisOrient.c_str()))
				{
					orientationOfAxis = (OrientationDirection)i;
				}
			}

			ImGui::EndCombo();
		}

		switch (orientationOfAxis)
		{
		case OrientationDirection::PAR_X_AXIS:ImGui::Text("Billboarding Fixed on X Axis"); break;
		case OrientationDirection::PAR_Y_AXIS:ImGui::Text("Billboarding Fixed on Y Axis"); break;
		case OrientationDirection::PAR_Z_AXIS:ImGui::Text("Billboarding Fixed on Z Axis"); break;
		case OrientationDirection::PAR_ORIENTATION_DIRECTION_END:actualAxisOrient = ""; break;
		default:actualAxisOrient = "None";
			break;
		}
	}
	break;
	case BillboardType::PAR_LOOK_EDITOR_CAMERA: 
	{
		//Solo texto
	}
	break;
	case BillboardType::PAR_LOOK_GAME_CAMERA: 
	{
		//Solo texto
	}  
	break;
	case BillboardType::PAR_WORLD_ALIGNED: 
	{
		ImGui::Text("Actual World Orientation");
		std::string actualWorldOrient;
		switch (orientationFromWorld)
		{		
		case OrientationDirection::PAR_FREE_ORIENT: actualWorldOrient = "Free Orientation";break;
		case OrientationDirection::PAR_X_AXIS:actualWorldOrient = "X Axis"; break;
		case OrientationDirection::PAR_X_AXIS_NEGATIVE:actualWorldOrient = "Negative X Axis"; break;
		case OrientationDirection::PAR_Y_AXIS:actualWorldOrient = "Y Axis"; break;
		case OrientationDirection::PAR_Y_AXIS_NEGATIVE:actualWorldOrient = "Negative Y Axis"; break;
		case OrientationDirection::PAR_Z_AXIS:actualWorldOrient = "Z Axis"; break;
		case OrientationDirection::PAR_Z_AXIS_NEGATIVE:actualWorldOrient = "Negative Z Axia"; break;
		case OrientationDirection::PAR_ORIENTATION_DIRECTION_END:actualWorldOrient = ""; break;
		default:
			break;
		}

		if (ImGui::BeginCombo("##Change In World Direction", actualWorldOrient.c_str()))
		{
			for (int i = 0; i < OrientationDirection::PAR_ORIENTATION_DIRECTION_END; i++)
			{
				switch ((OrientationDirection)i)
				{
				case OrientationDirection::PAR_FREE_ORIENT: actualWorldOrient = "Free Rotation"; break;
				case OrientationDirection::PAR_X_AXIS:actualWorldOrient = "X Axis"; break;
				case OrientationDirection::PAR_X_AXIS_NEGATIVE:actualWorldOrient = "Negative X Axis"; break;
				case OrientationDirection::PAR_Y_AXIS:actualWorldOrient = "Y Axis"; break;
				case OrientationDirection::PAR_Y_AXIS_NEGATIVE:actualWorldOrient = "Negative Y Axis"; break;
				case OrientationDirection::PAR_Z_AXIS:actualWorldOrient = "Z Axis"; break;
				case OrientationDirection::PAR_Z_AXIS_NEGATIVE:actualWorldOrient = "Negative Z Axia"; break;
				case OrientationDirection::PAR_ORIENTATION_DIRECTION_END:actualWorldOrient = ""; break;
				}
				if (ImGui::Selectable(actualWorldOrient.c_str()))
				{
					orientationFromWorld = (OrientationDirection)i;
				}
			}

			ImGui::EndCombo();
		}

		switch (orientationFromWorld)
		{
		case OrientationDirection::PAR_FREE_ORIENT: ImGui::DragFloat3("Free Rotation# ROTATION World Orient",&freeWorldRotation[0],1.0f,-180.0f,180.0f); break;
		case OrientationDirection::PAR_X_AXIS: ImGui::Text("Looking to Positive X"); break;
		case OrientationDirection::PAR_X_AXIS_NEGATIVE:ImGui::Text("Looking to Negative X"); break;
		case OrientationDirection::PAR_Y_AXIS:ImGui::Text("Looking to Positive Y"); break;
		case OrientationDirection::PAR_Y_AXIS_NEGATIVE:ImGui::Text("Looking to Negative Y"); break;
		case OrientationDirection::PAR_Z_AXIS: ImGui::Text("Looking to Positive Z"); break;
		case OrientationDirection::PAR_Z_AXIS_NEGATIVE:ImGui::Text("Looking to Negative Z"); break;
		case OrientationDirection::PAR_ORIENTATION_DIRECTION_END:actualWorldOrient = ""; break;
		default:
			break;
		}

	}
	break;
	case BillboardType::PAR_BILLBOARDING_MODE_END: break;
	}

	ImGui::Separator();
}

float4x4 EmitterRotation::LookAt(float3& Spot, float3& position) // https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/lookat-function/framing-lookat-function.html
{
	//Z
	float3 forward = (Spot - position).Normalized();

	//X
	float3 tmp(0, 1, 0);
	float3 right = tmp.Cross(forward);//crossProduct(tmp, forward); //Order is important!!

	//Y
	float3 up = forward.Cross(right);//crossProduct(forward, right);
	
	float4x4 m = float4x4::identity;
	m[0][0] = right.x, m[0][1] = right.y, m[0][2] = right.z;
	m[1][0] = up.x, m[1][1] = up.y, m[1][2] = up.z;
	m[2][0] = forward.x, m[2][1] = forward.y, m[2][2] = forward.z;
	m[3][0] = position.x, m[3][1] = position.y, m[3][2] = position.z;
	return m;
}

void EmitterRotation::SetRotation(Quat rot)
{
	rotation = rot;
}

void EmitterRotation::EditorCameraAlign()
{ 
	float4x4* camaraMatrix = (float4x4*)External->camera->editorCamera->GetViewMatrix().ptr();
	float3 tempPos;
	Quat tempRot;
	float3 tempSca;
	camaraMatrix->Decompose(tempPos, tempRot, tempSca);
	SetRotation(tempRot);
}

void EmitterRotation::GameCameraAlign()
{
	float4x4* camaraMatrix = (float4x4*)External->scene->gameCameraComponent->GetViewMatrix().ptr();
	float3 tempPos;
	Quat tempRot;
	float3 tempSca;
	camaraMatrix->Decompose(tempPos, tempRot, tempSca);
	SetRotation(tempRot);
}

void EmitterRotation::WorldAlign()
{
	SetRotation(Quat::identity);
	switch (orientationFromWorld)
	{
	case PAR_FREE_ORIENT:
	{
		SetRotation(Quat::FromEulerXYZ(DegToRad(freeWorldRotation.x), DegToRad(freeWorldRotation.y), DegToRad(freeWorldRotation.z)));
	}
		break;
	case PAR_X_AXIS:
	{
		SetRotation(Quat::FromEulerXYZ(0.0f, DegToRad(90.0f), 0.0f));
	}
		break;
	case PAR_X_AXIS_NEGATIVE:
	{
		SetRotation(Quat::FromEulerXYZ(0.0f, DegToRad(-90.0f), 0.0f));
	}
		break;
	case PAR_Y_AXIS:
	{
		SetRotation(Quat::FromEulerXYZ(DegToRad(-90.0f) , 0.0f, 0.0f));
	}
	break;
	case PAR_Y_AXIS_NEGATIVE:
	{
		SetRotation(Quat::FromEulerXYZ(DegToRad(90.0f), 0.0f, 0.0f));
	}
		break;
	case PAR_Z_AXIS: 
	{
		SetRotation(Quat::FromEulerXYZ(0.0f, 0.0f, 0.0f));
	}
		break;
	case PAR_Z_AXIS_NEGATIVE:
	{
		SetRotation(Quat::FromEulerXYZ(0.0f, DegToRad(180.0f), 0.0f));
	}
		break;
	case PAR_ORIENTATION_DIRECTION_END:
		break;
	default:
		break;
	}
}

void EmitterRotation::AxisAlign(ParticleEmitter* emitter)
{
	float4x4* camaraMatrix;
	#ifdef _STANDALONE
		 camaraMatrix = (float4x4*)External->scene->gameCameraComponent->GetViewMatrix().ptr();
	#else
		camaraMatrix = (float4x4*)External->camera->editorCamera->GetViewMatrix().ptr();
	#endif // _STANDALONE
	
	float3 tempPos;
	Quat tempRot;
	float3 tempSca;
	camaraMatrix->Decompose(tempPos, tempRot, tempSca);
	
	//We obtain the look at vector to create a matrix
	float4x4 newMatrix = LookAt(tempPos, emitter->owner->mOwner->mTransform->GetGlobalPosition());
	newMatrix.Decompose(tempPos, tempRot, tempSca);

	float3 newRot = tempRot.ToEulerXYZ();
	switch (orientationOfAxis)
	{
	case OrientationDirection::PAR_X_AXIS:
	{
		newRot.y = 0;
		newRot.z = 0;
		//tempRot = tempRot.FromEulerXYZ(newRot.x,newRot.y,newRot.z);

		//tempRot = Quat(math::Cross(tempRot.ToEulerXYZ(), {0,1,0}));
	}
	break;
	case OrientationDirection::PAR_Y_AXIS:
	{
		newRot.x = 0;
		newRot.z = 0;
		//tempRot = tempRot.FromEulerXYZ(newRot.x, newRot.y, newRot.z);
	}
	break;
	case OrientationDirection::PAR_Z_AXIS:
	{
		newRot.x = 0;
		newRot.y = 0;
		//tempRot = tempRot.FromEulerXYZ(newRot.x, newRot.y, newRot.z);
	}
	break;
	case OrientationDirection::PAR_ORIENTATION_DIRECTION_END: break;
	default:
		break;
		
	}
	tempRot = tempRot.FromEulerXYZ(newRot.x, newRot.y, newRot.z);
	SetRotation(tempRot);
}

EmitterSize::EmitterSize()
{
	originalSize = { 1,1,1 };
	progresive = false;
	loop = false;
	startChange = 0.0f; //Range from 0 to 1 as lifetime
	stopChange = 1.0f; //Range from 0 to 1 as lifetime
	sizeMultiplier1 = 1.0f;
	sizeMultiplier2 = 1.0f;
}

void EmitterSize::Spawn(ParticleEmitter* emitter, Particle* particle)
{
	originalSize = particle->size;
}

void EmitterSize::Update(float dt, ParticleEmitter* emitter)
{
	float3 base = originalSize;
	if (progresive)
	{
		if(loop)
		{
			float timeForLerp = stopChange - startChange;
			for (int i = 0; i < emitter->listParticles.size(); i++)
			{
				float actualLT = emitter->listParticles.at(i)->lifetime;

				if (actualLT <= stopChange)
				{
					emitter->listParticles.at(i)->size = base * (sizeMultiplier1 + ((sizeMultiplier2 - sizeMultiplier1) * ((actualLT - startChange) / timeForLerp))); //Lerp size multiplication
				}
				else
				{
					timeForLerp = 1.0f - stopChange;
					emitter->listParticles.at(i)->size = base * (sizeMultiplier2 + ((sizeMultiplier1 - sizeMultiplier2) * ((actualLT - stopChange) / timeForLerp))); //Lerp size multiplication
				}
			}
		}
		else
		{
			float timeForLerp = stopChange - startChange;
			for (int i = 0; i < emitter->listParticles.size(); i++)
			{
				float actualLT = emitter->listParticles.at(i)->lifetime;

				if (startChange <= actualLT && actualLT <= stopChange)
				{
					emitter->listParticles.at(i)->size = base * (sizeMultiplier1 + ((sizeMultiplier2 - sizeMultiplier1) * ((actualLT - startChange) / timeForLerp))); //Lerp size multiplication
				}
			}
		}
		
	}
	else
	{
		for (int i = 0; i < emitter->listParticles.size(); i++)
		{
			emitter->listParticles.at(i)->size = base * sizeMultiplier1;
		}
	}
}

void EmitterSize::OnInspector()
{
	ImGui::Checkbox("Progresive Scaling ## SCALE", &(this->progresive));
	ImGui::DragFloat("First Scale ## SCALE", &(this->sizeMultiplier1), 0.5f, 0.0f, 720.0f);
	if (this->progresive)
	{
		ImGui::Checkbox("Loop ## SCALE", &this->loop); ImGui::SameLine();
		ImGui::DragFloat("End Scale ## SCALE", &(this->sizeMultiplier2), 0.5f, 0.0f, 720.0f);
		ImGui::SliderFloat("Start Change ##SCALE", &(this->startChange), 0.0f, (this->stopChange - 0.05f));
		ImGui::SliderFloat("Stop Change ##SCALE", &(this->stopChange), this->startChange + 0.05f, 1.0f);
	}	
	
	ImGui::Separator();
}

EmitterColor::EmitterColor()
{
	progresive = false;
	loop = false;
	startChange = 0.0f; //Range from 0 to 1 as lifetime
	stopChange = 1.0f; //Range from 0 to 1 as lifetime
	color1 = { 1,1,1,1 };
	color2 = { 0,0,0,0 };
}

void EmitterColor::Spawn(ParticleEmitter* emitter, Particle* particle)
{
	particle->color = color1;
}

void EmitterColor::Update(float dt, ParticleEmitter* emitter)
{
	if (progresive)
	{
		if(loop) 
		{
			float timeForLerp = stopChange - startChange;
			for (int i = 0; i < emitter->listParticles.size(); i++)
			{
				float actualLT = emitter->listParticles.at(i)->lifetime;

				if (actualLT <= stopChange)
				{
					emitter->listParticles.at(i)->color.r = color1.r + ((color2.r - color1.r) * ((actualLT - startChange) / timeForLerp)); //Lerp red
					emitter->listParticles.at(i)->color.g = color1.g + ((color2.g - color1.g) * ((actualLT - startChange) / timeForLerp)); //Lerp green
					emitter->listParticles.at(i)->color.b = color1.b + ((color2.b - color1.b) * ((actualLT - startChange) / timeForLerp)); //Lerp blue
					emitter->listParticles.at(i)->color.a = color1.a + ((color2.a - color1.a) * ((actualLT - startChange) / timeForLerp)); //Lerp alpha

				}
				else
				{
					timeForLerp = 1.0f - stopChange;
					emitter->listParticles.at(i)->color.r = color2.r + ((color1.r - color2.r) * ((actualLT - stopChange) / timeForLerp)); //Lerp red
					emitter->listParticles.at(i)->color.g = color2.g + ((color1.g - color2.g) * ((actualLT - stopChange) / timeForLerp)); //Lerp green
					emitter->listParticles.at(i)->color.b = color2.b + ((color1.b - color2.b) * ((actualLT - stopChange) / timeForLerp)); //Lerp blue
					emitter->listParticles.at(i)->color.a = color2.a + ((color1.a - color2.a) * ((actualLT - stopChange) / timeForLerp)); //Lerp alpha
				}
			}
		}
		else
		{
			float timeForLerp = stopChange - startChange;
			for (int i = 0; i < emitter->listParticles.size(); i++)
			{
				float actualLT = emitter->listParticles.at(i)->lifetime;

				if (startChange <= actualLT && actualLT <= stopChange)
				{
					emitter->listParticles.at(i)->color.r = color1.r + ((color2.r - color1.r) * ((actualLT - startChange) / timeForLerp)); //Lerp red
					emitter->listParticles.at(i)->color.g = color1.g + ((color2.g - color1.g) * ((actualLT - startChange) / timeForLerp)); //Lerp green
					emitter->listParticles.at(i)->color.b = color1.b + ((color2.b - color1.b) * ((actualLT - startChange) / timeForLerp)); //Lerp blue
					emitter->listParticles.at(i)->color.a = color1.a + ((color2.a - color1.a) * ((actualLT - startChange) / timeForLerp)); //Lerp alpha

				}
			}
		}
		
	}
}

void EmitterColor::OnInspector()
{
	ImGui::Checkbox("Progresive Color  ##COLOR", &(this->progresive));
	ImGui::ColorEdit4("First Color  ##COLOR", &(this->color1));
	if (this->progresive)
	{
		if (ImGui::ColorEdit4("End Color  ##COLOR", &(this->color2)));
		{
			this->color2 = this->color2;
		}
		ImGui::Checkbox("Loop ##COLOR", &this->loop);
		ImGui::SliderFloat("Start Change ##COLOR", &(this->startChange), 0.0f, (this->stopChange - 0.05f));
		ImGui::SliderFloat("End Change ##COLOR", &(this->stopChange), this->startChange + 0.05f, 1.0f);
	}
	
	ImGui::Separator();
}

EmitterImage::EmitterImage()
{
	rTexTemp = new ResourceTexture();
	imgPath = "Assets/Particles/IMAGES/particleExample.png";
	firstInit = true;
	//SetImage(imgPath);
}

EmitterImage::~EmitterImage()
{
	if (rTexTemp != nullptr) {
		delete rTexTemp;
		rTexTemp = nullptr;
	}
}

void EmitterImage::SetImage(std::string imagePath)
{
	firstInit = false;
	ImporterTexture::Import(imagePath, rTexTemp);
	rTexTemp->type = TextureType::DIFFUSE;
	rTexTemp->UID = Random::Generate();

	this->imgPath = imagePath;
}

void EmitterImage::Spawn(ParticleEmitter* emitter, Particle* particle)
{
	particle->pTexture = rTexTemp;
}

void EmitterImage::Update(float dt, ParticleEmitter* emitter)
{
	if(firstInit)
	{
		SetImage(imgPath);
	}
}

std::vector<std::string> ListFilesInParticlesFolder() {
	std::vector<std::string> files;
	const std::string particlesFolderPath = "Assets/Particles/IMAGES";
	for (const auto& entry : std::filesystem::directory_iterator(particlesFolderPath)) {
		if (entry.is_regular_file() && entry.path().extension() == ".png") {
			files.push_back(entry.path().filename().string());
		}
	}
	return files;
}

void EmitterImage::OnInspector()
{
	ImGui::Spacing();

	if (firstInit)
	{
		SetImage(imgPath);
	}

	std::vector<std::string> particleFiles = ListFilesInParticlesFolder();
	if (ImGui::BeginCombo("##Texture", rTexTemp->GetAssetsFilePath().c_str()))
	{
		if (!particleFiles.empty()) 
		{
			for (const auto& particleFile : particleFiles) 
			{
				if (ImGui::Selectable(particleFile.c_str())) 
				{
					std::string particlePath = "Assets/Particles/IMAGES/" + particleFile;
					SetImage(particlePath);
					break;
				}
			}
		}
		else 
		{ 
			ImGui::Text("There is no assets.");
		}

		ImGui::EndCombo();
	}

	ImGui::Image((ImTextureID*)rTexTemp->ID, ImVec2(128, 128));

	ImGui::Separator();
}


