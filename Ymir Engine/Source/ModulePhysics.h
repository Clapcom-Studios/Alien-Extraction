 #pragma once
#include "Module.h"
#include "Globals.h"
#include "Primitive.h"
#include "Color.h"
#include "CMesh.h"

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

	void ResetGravity(); // Sets the world gravity to GRAVITY from globals.h

	// PhysBody related
	PhysBody* AddBody(CCube cube, PhysicsType physType, float mass, bool gravity, btCollisionShape*& shape);		 // Box
	PhysBody* AddBody(CSphere sphere, PhysicsType physType, float mass, bool gravity, btCollisionShape*& shape);	 // Sphere
	PhysBody* AddBody(CCapsule capsule, PhysicsType physType, float mass, bool gravity, btCollisionShape*& shape);	 // Capsule
	PhysBody* AddBody(CMesh* mesh, PhysicsType, float mass, bool gravity, btCollisionShape*& shape);				 // Mesh
	void RemoveBody(PhysBody* b);

	void RecalculateInertia(PhysBody* pbody, float mass, bool gravity);

	// Collision Shape
	btCollisionShape* CreateCollisionShape(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);

	// RayCast functions
	bool RayCast(const btVector3& from, const btVector3& to, btVector3& hitPoint);
	bool VolumetricRayCast(const btVector3& origin, const btVector3& direction, int numRays, float rayLength, std::vector<btVector3>& hitPoints);
	bool DirectionalRayCast(const btVector3& origin, const btVector3& direction, float rayLength, btVector3& hitPoint);

	btScalar* getOpenGLMatrix(float4x4 matrix);

	// Render functions
	void RenderBoxCollider(PhysBody* pbody);
	void RenderSphereCollider(PhysBody* pbody);
	void RenderCapsuleCollider(PhysBody* pbody);
	void RenderMeshCollider(PhysBody* pbody);

public:
	// TODO: create 2 lists, one for rigidbodies and one for colliders	
	std::vector<PhysBody*> bodiesList;
	std::vector <btDefaultMotionState*> motions;

	btDiscreteDynamicsWorld* world; // World se puede dejar en public si se requiere

	btDispatcher* dispatcher;
	btCollisionConfiguration* collisionConfig;
	btBroadphaseInterface* broadphase;
	btConstraintSolver* constraintSolver;

	bool beginPlay;

	bool debugScene = true; // If true, draws physics stuff in SCENE
	bool debugGame = false; // If true, draws physics stuff in GAME

	// Shapes customization
	Color colliderColor;
	Color sensorColor;
	int shapeLineWidth = 2;

private:

};