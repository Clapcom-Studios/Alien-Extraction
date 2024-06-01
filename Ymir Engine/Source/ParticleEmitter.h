#pragma once
#ifndef __PAR_EMITTER_H__
#define __PAR_EMITTER_H__ 

#include "CParticleSystem.h"
#include "EmitterSetting.h"
#include "Particle.h"
#include <vector>

#define MAXPARTICLES 300
#define MAXGLOBALPARTICLES 5000

class CParticleSystem;
class EmitterSetting;

//Ellos generan y controlan las particulas
class ParticleEmitter
{
public:
	ParticleEmitter(CParticleSystem* cParticleParent);
	~ParticleEmitter();

	std::vector<Particle*> listParticles;
	std::vector<EmitterSetting*> modules; //Lista de settings (posicion, tipo de billboarding, color)
	CParticleSystem* owner;

	std::string name;
	uint32_t UID;

	float emitterTime = 0; //Tiempo de las particulas

	void Init(CParticleSystem* component);

	void Update(float dt);
	void Reset();

	void SpawnParticle(uint particlesToAdd = 1);

	EmitterSetting* CreateEmitterSettingByType(uint type);
	bool EmitterSettingExist(uint typeS, bool excludeNonUniques = true);
	int DestroySetting(uint pos);

private:
	void KillDeadParticles();
	void KillAllParticles();
	void UpdateModules(float dt);

};
#endif //__PAR_EMITTER_H__