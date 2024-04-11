#pragma once
#ifndef __PAR_EMITTER_H__
#define __PAR_EMITTER_H__ 

#include "CParticleSystem.h"
#include "EmitterSetting.h"
#include "Particle.h"
#include <vector>

#define MAXPARTICLES 500

class CParticleSystem;
class EmitterSetting;

//Ellos generan y controlan las particulas
class ParticleEmitter
{
public:
	ParticleEmitter(CParticleSystem* cParticleParent);
	~ParticleEmitter();

	std::vector<Particle*> listParticles;
	std::vector<EmitterSetting*> modules; //Son las Particle module del power, son las settings (posicion, tipo de billboarding, color)
	CParticleSystem* owner;

	std::string name;
	uint32_t UID;

	float emitterTime = 0; //Tiempo de las particulas

	void Init(CParticleSystem* component);

	void Update(float dt);
	void DrawParticles();
	void Reset();

	void SpawnParticle(uint particlesToAdd = 1);

	EmitterSetting* CreateEmitterSettingByType(uint type);
	bool EmitterSettingExist(uint typeS, bool excludeNonUniques = true);
	int DestroyEmitter(uint pos);

private:
	void KillDeadParticles();
	void KillAllParticles();
	void UpdateModules(float dt);

};
#endif //__PAR_EMITTER_H__