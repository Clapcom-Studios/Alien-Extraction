 #pragma once
#include "Module.h"
#include "Globals.h"
#include "Primitive.h"
#include "Color.h"
#include "CMesh.h"
#include "CScript.h"

#include "External/Bullet/include/btBulletDynamicsCommon.h"
#include "External/Bullet/include/BulletCollision/CollisionShapes/btTriangleMesh.h"
//#include "External/glmath.h"

#include <vector>

class DebugDrawer;
struct PhysBody;
enum PhysicsType;
class ModulePhysics : public Module 
{
public:

	ModulePhysics(Application* app, bool start_enabled = true); 
	~ModulePhysics();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void CreateWorld();
	void DeleteWorld();

	//Getters
	btVector3 GetWorldGravity();
	Color GetColliderColor();

	//Setters
	void SetWorldGravity(btVector3 g);
	void SetDrawScene(bool d);
	void SetDrawGame(bool d);
	void SetColliderColor(Color col);
	void SetSensorColor(Color col);
	void SetLineWidth(float w);

	bool GetDebugDraw();

	void ResetGravity(); // Sets the world gravity to GRAVITY from globals.h

	// PhysBody related
	PhysBody* AddBody(CCube cube, PhysicsType physType, float mass, bool useGravity, btCollisionShape*& shape);		 // Box
	PhysBody* AddBody(CSphere sphere, PhysicsType physType, float mass, bool useGravity, btCollisionShape*& shape);	 // Sphere
	PhysBody* AddBody(CCapsule capsule, PhysicsType physType, float mass, bool useGravity, btCollisionShape*& shape);// Capsule
	PhysBody* AddBody(CCone cone, PhysicsType physType, float mass, bool useGravity, btCollisionShape*& shape);		 // Cone
	PhysBody* AddBody(CCylinder cylinder, PhysicsType physType, float mass, bool useGravity, btCollisionShape*& shape);// Cylinder
	PhysBody* AddBody(CMesh* mesh, PhysicsType, float mass, bool useGravity, btCollisionShape*& shape);				 // Mesh
	void RemoveBody(PhysBody* b);

	void ClearBodiesList();
	void ClearMotions();

	void RecalculateInertia(PhysBody* pbody, float mass, bool useGravity);

	// Collision Shape
	btCollisionShape* CreateCollisionShape(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);

	// RayCast functions
	//bool RayCast(const btVector3& from, const btVector3& to, btVector3& hitPoint);
	bool VolumetricRayCast(const btVector3& origin, const btVector3& direction, int numRays, float rayLength, std::vector<btVector3>& hitPoints);
	bool DirectionalRayCast(const btVector3& origin, const btVector3& direction, float rayLength, btVector3& hitPoint);

	// Script RayCast
	bool Raycast(btVector3 origin, btVector3 direction, float rayLength, std::vector<btVector3>& hitPoints, std::vector<GameObject*>& hits);
	GameObject* RaycastHit(btVector3 origin, btVector3 direction, float rayLength);
	bool RaycastTest(btVector3 origin, btVector3 direction, float rayLength);
	float RaycastLenght(btVector3 origin, btVector3 direction, float rayLength);

	btScalar* getOpenGLMatrix(float4x4 matrix);

	// Render functions
	void RenderBoxCollider(PhysBody* pbody);
	void RenderSphereCollider(PhysBody* pbody);
	void RenderCapsuleCollider(PhysBody* pbody);
	void RenderConeCollider(PhysBody* pbody);
	void RenderCylinderCollider(PhysBody* pbody);
	void RenderMeshCollider(PhysBody* pbody);

	// Conversion function from btVector3 to float3
	float3 btVector3_to_float3(const btVector3& v) {
		return { v.x(), v.y(), v.z() };
	}

	// Conversion function from float3 to btVector3
	btVector3 float3_to_btVector3(const float3& v) {
		return btVector3(v.x, v.y, v.z);
	}

public:
	// TODO: create 2 lists, one for rigidbodies and one for colliders	
	std::vector<std::pair<PhysBody*, PhysBody*>> currentCollisions;
	std::vector<std::pair<PhysBody*, PhysBody*>> previousCollisions;

	std::vector<PhysBody*> bodiesList;
	std::vector<btDefaultMotionState*> motions;

	btDiscreteDynamicsWorld* world; // World se puede dejar en public si se requiere

	btDispatcher* dispatcher;
	btCollisionConfiguration* collisionConfig;
	btBroadphaseInterface* broadphase;
	btConstraintSolver* constraintSolver;

	bool debugScene = true; // If true, draws physics stuff in SCENE
	bool debugGame = false; // If true, draws physics stuff in GAME

	// Shapes customization
	Color colliderColor;
	Color sensorColor;
	int shapeLineWidth = 2;
	
	bool collisionEnded = true;
	bool inCollision = false;
	bool onExitCollision = false;
	bool stayCollision = true;

	bool isWorldFirstFrame = false;

private:

};