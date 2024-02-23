#pragma once

class GameObject;

enum ComponentType {

	NONE = -1,
	TRANSFORM,
	MESH,
	MATERIAL,
	CAMERA,
	SCRIPT

};

class Component {
public:

	Component();
	Component(GameObject* owner, ComponentType type);
	virtual ~Component();

	virtual void Enable(); 
	virtual void Disable();

	virtual void Update();
	virtual void OnInspector();

public:

	GameObject* mOwner;

	bool active;
	ComponentType ctype;

};