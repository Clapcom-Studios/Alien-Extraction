#pragma once

class GameObject;

enum ComponentType {

	NONE = -1,

	TRANSFORM,
	MESH,
	MATERIAL,
	SCRIPT,
	AUDIO_SOURCE,
	AUDIO_LISTENER,
	CAMERA,

	ALL_TYPES

};

class Component {
public:

	Component();
	Component(GameObject* owner, ComponentType type);
	virtual ~Component();

	void Enable(); 
	void Disable();

	virtual void Update();
	virtual void OnInspector();

public:

	GameObject* mOwner;

	bool active;
	ComponentType ctype;

};