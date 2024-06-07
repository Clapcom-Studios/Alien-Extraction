#pragma once
#ifndef __EMITTER_INSTANCE_H__
#define __EMITTER_INSTANCE_H__ 

#include "CParticleSystem.h"
#include "Particle.h"

#include <vector>

enum EmitterType
{
	PAR_BASE,
	PAR_SPAWN,
	PAR_POSITION,
	PAR_ROTATION,
	PAR_SIZE,
	PAR_COLOR,
	PAR_IMAGE,
	PARTICLES_MAX,
};

class ParticleEmitter;

//Plantillas para los Emitters, que seran los settings de las particulas
struct EmitterSetting //En Thor Engine esto seria el particle Module
{
public:
	EmitterSetting();
	~EmitterSetting();

	EmitterType type;

	virtual void Spawn(ParticleEmitter* emitter, Particle* particle) = 0;
	virtual void Update(float dt, ParticleEmitter* emitter) /*= 0*/;
	virtual void OnInspector();

	bool unique = true; //Si es unique solo puede haber uno de ese tipo
private:

};

//Enum of the modes of effect the orentation has over the game object.
enum RotationInheritanceParticles
{
	PAR_WORLD_MATRIX, //None, 1,0,0 is the X of the world,etc
	PAR_GAMEOBJECT_MATRIX, //The particle system matrix affects the position (if is rotated 90 degrees in Y 1,0,0 would be +Z dir)
	PAR_PARENT_MATRIX, //Similar to GAMEOBJECT_MATRIX but is the parent of the Particle System)
	PAR_INITIAL_ROTATION_END,
};

enum SpawnAreaShape 
{
	PAR_POINT,
	PAR_CONE, //Two circunferences
	PAR_BOX,
	PAR_SPHERE,
	PAR_SHAPE_ENUM_END,
};

struct EmitterBase : EmitterSetting
{
	EmitterBase();
	~EmitterBase();
	void Spawn(ParticleEmitter* emitter, Particle* particle);
	void Update(float dt, ParticleEmitter* emitter);
	void OnInspector();

	//Lifetime
	float particlesLifeTime1;
	bool randomLT;
	float particlesLifeTime2;
	bool inmortal;

	bool hasDistanceLimit;
	float distanceLimit;

	//Orientacion
	RotationInheritanceParticles rotacionBase;

	//Area donde spawnean las particulas
	SpawnAreaShape currentShape;

	//Variable unica, posicion donde spawnean (centro de las figuras)
	float3 emitterOrigin;

	//Cylinder Parameters
	float radiusHollow; //Espacio vacio del cono por si se quiere hacer un donut o algo asi
	float baseRadius; //Radius/face positioned on 0,0,0
	float topRadius; // Radius/face projected after the length
	float heigth; //Heigth of the cone.
	bool useAngle;
	float angle;

	//Box Parameters
	float3 boxPointsPositives;
	float3 boxPointsNegatives;
	bool scaleAll;
};

//EnumS of types of spawn of the spawn setting
enum ParticlesSpawnMode
{
	PAR_NUM_PARTICLES_BURST, //Spawn X particles at a time and doesn't spawn again until that number decreases due to the particles dying
	PAR_ONE_PARTICLE_OVER_DELAY, //Spawn one particle every X seconds (works using delta time).
	PAR_NUM_PARTICLES_OVER_DELAY, // Spawn X particles every Y seconds (also works using delta time)
	PAR_SPAWN_MODE_END
};

enum ParticlesSpawnEnabeling
{
	PAR_WAIT_SUBEMITTER = -	1,
	PAR_START_NON_STOP, //Starts emittng and won´t stop
	PAR_START_STOP, //Starts emitting but stops after X particles spawned
	PAR_WAIT_NON_STOP, //Starts waiting for a Play then won't stop
	PAR_WAIT_STOP, //Starts waiting for a Play and stops after X particles, waiting again
	PAR_ENABLE_MODES_END
};

enum SpawnConditionSubemitter
{
	PAR_NEW_SPAWN,
	PAR_DIE,
	PAR_INBETWEEN_OF,
	PAR_END_SPAWN_CONDITION,
};

struct EmitterSpawner : EmitterSetting
{
	EmitterSpawner();
	~EmitterSpawner();
	void Spawn(ParticleEmitter* emitter, Particle* particle);
	void Update(float dt, ParticleEmitter* emitter);
	bool PlayTrigger(bool val = true);
	void OnInspector(ParticleEmitter* thisEmitter);

	//Variable unica, ritmo de spawn
	ParticlesSpawnMode spawnMode;
	bool playTriggered;
	
	ParticlesSpawnEnabeling startMode;
	float spawnRatio; //Dividir en current time por cuantas se spawnean 
	float currentTimer;
	int numParticlesToSpawn;
	int numParticlesForStop; //When played, if enabeling mode is stop once it spawn X particles it stops playing
	int numParticlesSpawned;

	//Cosas de subemitter
	ParticleEmitter* pointingEmitter;
	uint32_t pointingUID; //Es una variable temporal solo usada para el save y load.
	SpawnConditionSubemitter conditionForSpawn;
	float subMaxLifetime;
	float subMinLifetime;
	float3 positionParticleForSub;
};

//Enum of the modes as positions change after spawn
enum SpeedChangeMode
{
	PAR_NO_SPEED_CHANGE,
	PAR_IF_TIME_ADD, //Si en el time añadir el vector a la direccion
	PAR_ADD_OVER_TIME, //Añadir paulatinamente el nuevo vector
	PAR_IF_TIME_SUBSTITUTE, //Si el time cuadra position substituir velocidad
	PAR_SUBSTITUTE_OVER_TIME, //Sobre el tiempo cambia la direccion
	PAR_SPEED_CHANGE_MODE_END,
};

struct EmitterPosition : EmitterSetting
{
	EmitterPosition();
	void Spawn(ParticleEmitter* emitter, Particle* particle);
	void Update(float dt, ParticleEmitter* emitter);
	void OnInspector();

	float GetModuleVec(float3 vec);

	bool randomized; //Si la direccion es solo la uno o un numero random entre la 1 y la 2
	float3 direction1;
	float3 direction2;
	bool normalizedSpeed;

	bool useBaseShape;

	bool acceleration;
	float particleSpeed1;
	float particleSpeed2;

	float3 newDirection;
	float changeSpeed1;
	float changeSpeed2;
	bool normalizedChange;
	
	SpeedChangeMode actualSpeedChange;

};

//Enum particles
enum BillboardType
{
	PAR_LOOK_EDITOR_CAMERA,
	PAR_LOOK_GAME_CAMERA,
	PAR_WORLD_ALIGNED,
	PAR_AXIS_ALIGNED,
	PAR_BILLBOARDING_MODE_END
};

enum OrientationDirection //Usado para world y axis orientation, aunque uno de ellos se salta de dos en dos para solo hacer los 3 axis
{
	PAR_FREE_ORIENT,
	PAR_X_AXIS,
	PAR_X_AXIS_NEGATIVE,
	PAR_Y_AXIS,
	PAR_Y_AXIS_NEGATIVE,
	PAR_Z_AXIS,
	PAR_Z_AXIS_NEGATIVE,
	PAR_ORIENTATION_DIRECTION_END
};

struct EmitterRotation : EmitterSetting
{
	EmitterRotation();
	void Spawn(ParticleEmitter* emitter, Particle* particle);
	void Update(float dt, ParticleEmitter* emitter);
	void OnInspector();

	float4x4 LookAt(float3& Spot, float3& position);
	void SetRotation(Quat rot);

	bool updateRotation;
	bool verAlign;

	Quat rotation;
	BillboardType currentAlignmentMode;
	OrientationDirection orientationOfAxis;
	OrientationDirection orientationFromWorld;
	float3 freeWorldRotation;
	void WorldAlign();
	
private:
	void EditorCameraAlign();
	void GameCameraAlign();
	
	void AxisAlign(ParticleEmitter* emitter); //Necesitamos una fucking referencia porque esto reqquiere de hacer vectores y mierdas
};

struct EmitterSize : EmitterSetting
{
	EmitterSize();
	void Spawn(ParticleEmitter* emitter, Particle* particle);
	void Update(float dt, ParticleEmitter* emitter);
	void OnInspector();

	float3 originalSize; //Para gestionar si se escala el GameObject
	bool progresive;
	bool loop;
	float startChange; //Range from 0 to 1 as lifetime
	float stopChange; //Range from 0 to 1 as lifetime
	float sizeMultiplier1;
	float sizeMultiplier2;


};

struct EmitterColor : EmitterSetting
{
	EmitterColor();
	void Spawn(ParticleEmitter* emitter, Particle* particle);
	void Update(float dt, ParticleEmitter* emitter);
	void OnInspector();

	bool progresive;
	bool loop;
	float startChange; //Range from 0 to 1 as lifetime
	float stopChange; //Range from 0 to 1 as lifetime
	Color color1;
	Color color2;
};

struct EmitterImage : EmitterSetting
{
	EmitterImage();
	void Spawn(ParticleEmitter* emitter, Particle* particle);
	void Update(float dt, ParticleEmitter* emitter);
	void OnInspector();
	void SetImage(std::string imgPath);

	ResourceTexture* rTexTemp;
	std::string imgPath;
	bool firstInit;
};

#endif //__EMITTER_INSTANCE_H__