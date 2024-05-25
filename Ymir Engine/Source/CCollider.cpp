#include "CCollider.h"
#include "GameObject.h"
#include "ModulePhysics.h"

#include "External/Bullet/include/btBulletDynamicsCommon.h"
#include "External/ImGui/imgui.h"
#include "TimeManager.h"

#include <string>
#include <vector>
#include "External/ImGuizmo/include/ImGuizmo.h"
#include <MathGeoLib/include/Math/float4x4.h>

#include "External/mmgr/mmgr.h"

CCollider::CCollider(GameObject* owner, ColliderType collider, PhysicsType physics) : Component(owner, ComponentType::PHYSICS)
{
	this->mOwner = owner;

	collType = collider;
	physType = physics;

	isActive = true;

	isSensor = false;

	mass = 1;
	friction = 0;
	angularFriction = 0;
	useGravity = true;
	lockX = false;
	lockY = false;
	lockZ = false;
	
	offset = { 0, 0, 0 };
	offsetRotation = Quat().identity;
	size = float3(5, 5, 5); 

	transform = mOwner->mTransform;

	switch (collType)
	{
	case BOX:
		SetBoxCollider();
		break;
	case SPHERE:
		SetSphereCollider();
		break;
	case CAPSULE:
		SetCapsuleCollider();
		break;
	case CONE:
		SetConeCollider();
		break;
	case CYLINDER:
		SetCylinderCollider();
		break;
	case MESH_COLLIDER:
		SetMeshCollider();
		break;
	default:
		break;
	}

	// Get info at start (chuekada espectacular)

	if (physBody != nullptr) {

		External->physics->RecalculateInertia(physBody, mass, useGravity);

		//CMesh* componentMesh = (CMesh*)mOwner->GetComponent(ComponentType::MESH);

		//CTransform* componentTransform = (CTransform*)mOwner->GetComponent(ComponentType::TRANSFORM);

		//if (componentMesh != nullptr) 
		//{
		//	float3 pos; 

		//	if (collType != ColliderType::MESH_COLLIDER)
		//	{
		//		pos = componentMesh->obb.CenterPoint();
		//	}
		//	else 
		//	{
		//		pos = componentTransform->GetGlobalPosition();
		//	}

		//	physBody->SetPosition(pos);
		//	physBody->SetRotation(componentTransform->GetLocalRotation());
		//}
		//else {

		//	LOG("This: %s | GT: %f, %f, %f", mOwner->name.c_str(), componentTransform->GetGlobalTransform().TranslatePart().x,
		//		componentTransform->GetGlobalTransform().TranslatePart().y, componentTransform->GetGlobalTransform().TranslatePart().z);

		//	physBody->SetPosition(componentTransform->GetGlobalTransform().TranslatePart());
		//	physBody->SetRotation(componentTransform->GetGlobalRotation()); 
		//}

		if (size.x == 0) size.x = 0.1;
		if (size.y == 0) size.y = 0.1;
		if (size.z == 0) size.z = 0.1;

		btSize = float3_to_btVector3(size);
		//btSize.dot(btVector3(mOwner->mTransform->scale.x, mOwner->mTransform->scale.y, mOwner->mTransform->scale.z));
		shape->setLocalScaling(btSize);
	}
	
	isFirstTick = true;
}

CCollider::~CCollider()
{
	RemovePhysbody();

	delete physBody;
	delete shape;
}

void CCollider::Update()
{		
	if (isActive)
	{
		if (!isSensor)
			physBody->body->setCollisionFlags(physBody->body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
		physBody->body->setActivationState(ACTIVE_TAG);
	}
	else
	{
		physBody->body->setCollisionFlags(physBody->body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		physBody->body->setActivationState(ISLAND_SLEEPING);
	}

	if (physBody != nullptr)
	{
		External->physics->RecalculateInertia(physBody, mass, useGravity);

		//Esto NO deberia estar en update. Pero de otra forma no funcionaba
		if (friction != 0)
			physBody->body->setFriction(friction);

		if (angularFriction != 0)
			physBody->body->setRollingFriction(angularFriction);
	}

	if (size.x == 0) size.x = 0.1;
	if (size.y == 0) size.y = 0.1;
	if (size.z == 0) size.z = 0.1;

	// --------------------------- Physics Simulation Stopped ---------------------------

	if (isFirstTick || (TimeManager::gameTimer.GetState() == TimerState::STOPPED))
	{
		if (physBody != nullptr)
		{
			// Get ctransform global matrix
			transform = mOwner->mTransform;
			trans = transform->GetGlobalTransform();

			// Fix offset
			trans.SetTranslatePart(trans.TranslatePart() + offset);

			//CMesh* componentMesh = (CMesh*)mOwner->GetComponent(ComponentType::MESH);

			//if (componentMesh != nullptr) physBody->SetPosition(trans.TranslatePart() + componentMesh->rMeshReference->obb.CenterPoint());
			//else physBody->SetPosition(trans.TranslatePart());

			physBody->SetPosition(trans.TranslatePart());
			//physBody->SetRotation(Quat(trans.RotatePart()));
			physBody->SetRotation(Quat(trans.RotatePart()) * offsetRotation);

			if (collType != ColliderType::MESH_COLLIDER)
			{
				btSize = float3_to_btVector3(
					{
						size.x * trans.GetScale().x,
						size.y * trans.GetScale().y,
						size.z * trans.GetScale().z,
					});
			}
			else
			{
				btSize = float3_to_btVector3(trans.GetScale());
			}

			shape->setLocalScaling(btSize);
		}

		isFirstTick = false;
	}

	// --------------------------- Physics Simulation Started --------------------------- 
	
	if (TimeManager::gameTimer.GetState() == TimerState::RUNNING && physBody != nullptr)
	{
		if (isActive)
		{
			float4x4 mat;
			physBody->GetTransform(mat);

			CMesh* componentMesh = (CMesh*)mOwner->GetComponent(ComponentType::MESH);
			CTransform* componentTransform = (CTransform*)mOwner->GetComponent(ComponentType::TRANSFORM);

			CTransform* parentTransform = (CTransform*)mOwner->mParent->GetComponent(ComponentType::TRANSFORM);

			float meshOffsetX = 0;
			float meshOffsetY = 0;
			float meshOffsetZ = 0;

			if (componentMesh != nullptr)
			{
				meshOffsetX = componentMesh->obb.CenterPoint().x - componentTransform->GetGlobalPosition().x; 
				meshOffsetY = componentMesh->obb.CenterPoint().y - componentTransform->GetGlobalPosition().y; 
				meshOffsetZ = componentMesh->obb.CenterPoint().z - componentTransform->GetGlobalPosition().z; 
			}

			float4x4 newMat;
			float* matrix = mat.ptr();

			newMat.SetCol(0, float4(matrix[0], matrix[1], matrix[2], matrix[3]));
			newMat.SetCol(1, float4(matrix[4], matrix[5], matrix[6], matrix[7]));
			newMat.SetCol(2, float4(matrix[8], matrix[9], matrix[10], matrix[11]));
			newMat.SetCol(3, float4(matrix[12], matrix[13], matrix[14], matrix[15]));

			float3 pos = newMat.TranslatePart();
			//float3 pos = totalMatrix.TranslatePart();

			if (parentTransform)
			{
				pos.x -= parentTransform->translation.x;
				pos.y -= parentTransform->translation.y;
				pos.z -= parentTransform->translation.z;

				pos.x /= parentTransform->scale.x;
				pos.y /= parentTransform->scale.y;
				pos.z /= parentTransform->scale.z;
			}

			// Puede ser que a la matriz newMat le falte tener en cuenta la rotacion del parent (?)
			mOwner->mTransform->SetPosition(pos - offset);
			//mOwner->mTransform->SetOrientation(physBody->body->getOrientation());

			Quat rotation = Quat(physBody->body->getOrientation().x(), physBody->body->getOrientation().y(), 
								 physBody->body->getOrientation().z(), physBody->body->getOrientation().w());

			mOwner->mTransform->SetRotation(rotation / offsetRotation);

			if (lockX) {
				UpdateLockRotation();
			}

			if (lockY) {
				UpdateLockRotation();
			}

			if (lockZ) {
				UpdateLockRotation();
			}

		}
	}
}

void CCollider::OnInspector()
{
	char* titles[]	{ "Box", "Sphere", "Capsule", "Cone", "Cylinder", "Mesh" };

	std::string headerLabel = std::string(titles[*reinterpret_cast<int*>(&collType)]) + " " + "Collider"; // label = "Collider Type" + Collider
	
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

	bool exists = true;

	ImGui::Checkbox(("##" + std::to_string(UID)).c_str(), &isActive);

	ImGui::SameLine();

	if (ImGui::CollapsingHeader((headerLabel + "##" + std::to_string(UID)).c_str(), &exists, flags))
	{
		ImGui::Indent();

		ImGui::Spacing();

		if (!isActive) { ImGui::BeginDisabled(); }

		ImGui::SeparatorText("COLLIDER");
		ImGui::Spacing();

		bool auxIsSensor = isSensor;
		if (ImGui::Checkbox("Is Sensor", &auxIsSensor))
		{
			SetAsSensor(auxIsSensor);

			if (auxIsSensor)
			{
				physType = PhysicsType::STATIC;
				SetDefaultValues(physType);
			}
		}

		if (ImGui::Checkbox("Draw Shape", &physBody->drawShape))
		{
			!physBody->drawShape;
		}

		ImGui::Text("Shape: "); ImGui::SameLine();
		
		int hasNotMesh = 0;
		if (mOwner->GetComponent(ComponentType::MESH) == nullptr) hasNotMesh = 1; // if mesh = false -> value = 1

		// For operations related with previous collierType after changing it
		ColliderType currentType = collType;

		if (ImGui::Combo("##Collider Type", reinterpret_cast<int*>(&collType), titles, IM_ARRAYSIZE(titles) - hasNotMesh)) 
		{
			if (currentType != collType)
			{
				switch (collType)
				{
				case ColliderType::BOX:
					RemovePhysbody();
					AdaptSizeToShape(currentType);
					SetBoxCollider();
					break;
				case ColliderType::SPHERE:
					RemovePhysbody();
					AdaptSizeToShape(currentType);
					SetSphereCollider();
					break;
				case ColliderType::CAPSULE:
					RemovePhysbody();
					AdaptSizeToShape(currentType);
					SetCapsuleCollider();
					break;
				case ColliderType::CONE:
					RemovePhysbody();
					AdaptSizeToShape(currentType);
					SetConeCollider();
					break;
				case ColliderType::CYLINDER:
					RemovePhysbody();
					AdaptSizeToShape(currentType);
					SetCylinderCollider();
					break;
				case ColliderType::MESH_COLLIDER:
					RemovePhysbody();
					AdaptSizeToShape(currentType);
					SetMeshCollider();
					break;
				default:
					break;
				}
			}
		}

		float3 prevSize = size;

		if (shape != nullptr && collType != ColliderType::MESH_COLLIDER) 
		{
			switch (collType)
			{
			case ColliderType::BOX: 
				ImGui::Text("Size: "); ImGui::SameLine();
				ImGui::DragFloat3("##Size", size.ptr(), 0.1f, 0.1f);
				break;

			case ColliderType::SPHERE:
				ImGui::Text("Radius: "); ImGui::SameLine();
				ImGui::DragFloat("##Radius", &size.x, 0.1f, 0.1f);
				break;

			case ColliderType::CAPSULE:
				ImGui::Text("Radius: "); ImGui::SameLine();
				ImGui::DragFloat("##Radius", &size.x, 0.1f, 0.1f);
				ImGui::Text("Height: "); ImGui::SameLine();
				ImGui::DragFloat("##Height", &size.y, 0.1f, 0.1f);
				break;

			case ColliderType::CONE:
				ImGui::Text("Radius: "); ImGui::SameLine();
				ImGui::DragFloat("##Radius", &size.x, 0.1f, 0.1f);
				ImGui::Text("Height: "); ImGui::SameLine();
				ImGui::DragFloat("##Height", &size.y, 0.1f, 0.1f);
				break;

			case ColliderType::CYLINDER:
				ImGui::Text("Radius: "); ImGui::SameLine();
				ImGui::DragFloat("##Radius", &size.x, 0.1f, 0.1f);
				ImGui::Text("Height: "); ImGui::SameLine();
				ImGui::DragFloat("##Height", &size.y, 0.1f, 0.1f);
				break;
			}

			if ((prevSize.x != size.x) || (prevSize.y != size.y) || (prevSize.z != size.z))
			{
				btSize = float3_to_btVector3(size);
				shape->setLocalScaling(btSize);
			}

			ImGui::Text("Offset Position"); ImGui::SameLine();
			ImGui::DragFloat3("##OffsetPos", offset.ptr(), 0.1f, 0.1f);

			ImGui::Text("Offset Rotation"); ImGui::SameLine();
			ImGui::DragFloat3("##OffsetRot", offsetRotation.ptr(), 0.1f, 0.1f);

			if (ImGui::Button("Set size from OBB")) {

				SetOBBSize();
			}
		}

		// -----------------------------------------------------------------------------------------------------
		ImGui::Spacing();
		ImGui::SeparatorText("RIGIDBODY");
		ImGui::Spacing();

		ImGui::PushItemWidth(120.0f);

		const char* items[] = { "Dynamic", "Kinematic", "Static" };
		int currentItem = static_cast<int>(physType);

		ImGui::Text("Physics Type: "); ImGui::SameLine();
		if (ImGui::Combo("##Physics Type", &currentItem, items, IM_ARRAYSIZE(items)))
		{
			physType = static_cast<PhysicsType>(currentItem);
			SetDefaultValues(physType);
			External->physics->RecalculateInertia(physBody, mass, useGravity);
		}

		switch (physType) 
		{
		case PhysicsType::DYNAMIC:
			ImGui::Text("Mass:				  "); ImGui::SameLine();
			if (ImGui::DragFloat("##Mass", &mass, 1.0f, 0.0f, 1000.0f))
				External->physics->RecalculateInertia(physBody, mass, useGravity);

			ImGui::Text("Friction:			   "); ImGui::SameLine();
			if (ImGui::DragFloat("##Friction", &friction, 0.05f, 0.0f, 1.0f))
				ImGui::SetTooltip("Es posible que se bloquee la rotacion del cuerpo en anadirle friccion");

			ImGui::Text("Angular friction:	"); ImGui::SameLine();
			ImGui::DragFloat("##Angular friction", &angularFriction, 0.05f, 0.0f, 1.0f);

			if (ImGui::Checkbox("Use gravity\t", &useGravity))
				External->physics->RecalculateInertia(physBody, mass, useGravity);

			ImGui::Spacing();
			ImGui::Text("Lock rotation");

			if (ImGui::Checkbox("Lock x\t", &lockX))
				UpdateLockRotation();

			ImGui::SameLine();
			if (ImGui::Checkbox("Lock y\t", &lockY))
				UpdateLockRotation();
				
			ImGui::SameLine();
			if (ImGui::Checkbox("Lock z\t", &lockZ))
				UpdateLockRotation();

            break;

        case PhysicsType::KINEMATIC:
			ImGui::Text("Mass: "); ImGui::SameLine();
			if (ImGui::DragFloat("##Mass", &mass, 1.0f, 0.0f, 1000.0f))
				External->physics->RecalculateInertia(physBody, mass, false);
            break;

        case PhysicsType::STATIC:			

            break;

        default:
            break;
		}

		// -----------------------------------------------------------------------------------------------------

		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Remove Component"))
		{
			mOwner->RemoveComponent(this);
			return;
		}

		if (!isActive) { ImGui::EndDisabled(); }

		ImGui::Unindent();
	}
	if (!exists) { mOwner->RemoveComponent(this); }
}

btCollisionShape* CCollider::GetShape()
{
	return shape;
}

// Setters ----------------------------------------------------------------------
void CCollider::SetBoxCollider()
{
	LOG("Set Box Collider");

	collType = ColliderType::BOX;

	CCube cube;

	if (mOwner) 
	{
		transform = mOwner->mTransform;

		physBody = External->physics->AddBody(cube, PhysicsType::DYNAMIC, mass, true, shape);
		physBody->SetGameObject(mOwner);
	}

	shape->setMargin(0.0f);
}

void CCollider::SetSphereCollider()
{
	LOG("Set Sphere Collider");
	collType = ColliderType::SPHERE;

	CSphere sphere;

	physBody = External->physics->AddBody(sphere, PhysicsType::DYNAMIC, mass, true, shape);
	physBody->SetGameObject(mOwner);

	shape->setMargin(0.0f);
}

void CCollider::SetCapsuleCollider()
{
	LOG("Set Capsule Collider");
	collType = ColliderType::CAPSULE;
	
	CCapsule capsule;

	physBody = External->physics->AddBody(capsule, PhysicsType::DYNAMIC, mass, true, shape);
	physBody->SetGameObject(mOwner);

	shape->setMargin(0.0f);
}

void CCollider::SetConeCollider()
{
	LOG("Set Cone Collider");
	collType = ColliderType::CONE;

	CCone cone;

	physBody = External->physics->AddBody(cone, PhysicsType::DYNAMIC, mass, true, shape);
	physBody->SetGameObject(mOwner);

	shape->setMargin(0.0f);
}

void CCollider::SetCylinderCollider()
{
	LOG("Set Cylinder Collider");
	collType = ColliderType::CYLINDER;

	CCylinder cylinder;

	physBody = External->physics->AddBody(cylinder, PhysicsType::DYNAMIC, mass, true, shape);
	physBody->SetGameObject(mOwner);

	shape->setMargin(0.0f);
}

void CCollider::SetMeshCollider()
{

	LOG("Set Mesh Collider");
	collType = ColliderType::MESH_COLLIDER;
	
	CMesh* auxMesh = (CMesh*)mOwner->GetComponent(ComponentType::MESH);

	physBody = External->physics->AddBody(auxMesh, PhysicsType::DYNAMIC, mass, true, shape);
	physBody->SetGameObject(mOwner);

	//float3 size = auxMesh->rMeshReference->globalAABB.CenterPoint();
	////convexShape->setLocalScaling(btVector3(size.x, size.y, size.z));
	//convexShape->setImplicitShapeDimensions(btVector3(size.x, size.y, size.z));

	shape->setMargin(0.0f);
}

void CCollider::AdaptSizeToShape(ColliderType prevType)
{
	switch (collType) // Hardcode maestro
	{
	case ColliderType::BOX:
		if		(prevType == ColliderType::SPHERE) { size.x *= 2; size.y = size.x; size.z = size.x; }
		else if (prevType == ColliderType::CAPSULE) { size.x *= 2; size.y *= 2; size.z = size.x; }
		else if (prevType == ColliderType::CYLINDER) { size.x *= 4; size.y *= 2; size.z = size.x; }
		break;
	case ColliderType::SPHERE:
		if		(prevType == ColliderType::BOX) { size.x *= 0.5f; size.y = size.x; size.z = size.x; } 
		else if (prevType == ColliderType::CYLINDER) { size.x *= 2; size.z = size.x; } 
		break;
	case ColliderType::CAPSULE:
		if		(prevType == ColliderType::BOX) { size.x *= 0.5f; size.y *= 0.5f; size.z = size.x; }
		else if (prevType == ColliderType::CYLINDER) { size.x *= 2; size.z = size.x; } 
		break;
	case ColliderType::CONE:
		//if (prevType == ColliderType::BOX) { size.x *= 0.5f; size.z = size.x; } 
		//else if (prevType == ColliderType::CYLINDER) { size.x *= 2; size.z = size.x; } 
		break;
	case ColliderType::CYLINDER:
		if		(prevType == ColliderType::BOX) { size.x *= 0.25f; size.y *= 0.5f; size.z = size.x; }
		else if (prevType == ColliderType::CAPSULE || prevType == ColliderType::SPHERE) { size.x *= 0.5; size.z = size.x; }
		break;
	case ColliderType::MESH_COLLIDER:
		break;
	}
}

void CCollider::SetDefaultValues(PhysicsType type)
{
	switch (physType)
	{
	case PhysicsType::DYNAMIC:
		mass = 1;
		useGravity = true;
		break;
	case PhysicsType::KINEMATIC:
		mass = 1;
		useGravity = false;
		break;
	case PhysicsType::STATIC:
		mass = 0;
		useGravity = false;
		break;
	default:
		mass = 0;
		useGravity = false;
		break;
	}
}

void CCollider::UpdateLockRotation()
{
	btVector3 rot;

	if (lockX) rot.setX(0);
	else rot.setX(1);

	if (lockY) rot.setY(0);
	else rot.setY(1);

	if (lockZ) rot.setZ(0);
	else rot.setZ(1);

	physBody->body->setAngularFactor(rot);

}

void CCollider::SetOBBSize()
{
	CMesh* componentMesh = (CMesh*)mOwner->GetComponent(ComponentType::MESH);

	if (componentMesh != nullptr) {

		if (collType == ColliderType::MESH_COLLIDER) size = { mOwner->mTransform->scale.x, mOwner->mTransform->scale.y, mOwner->mTransform->scale.z };
		else size = float3(
			componentMesh->obb.Size().x / trans.GetScale().x,
			componentMesh->obb.Size().y / trans.GetScale().y,
			componentMesh->obb.Size().z / trans.GetScale().z
		);

		//trans.SetTranslatePart(componentMesh->obb.CenterPoint());
		//transform->SetPosition(componentMesh->obb.CenterPoint());
		//physBody->SetPosition(componentMesh->obb.CenterPoint());
		offset = componentMesh->obb.CenterPoint();

	}
	if (size.x == 0) size.x = 0.1;
	if (size.y == 0) size.y = 0.1;
	if (size.z == 0) size.z = 0.1;
}

void CCollider::RemovePhysbody()
{
	if (physBody != nullptr) {
		External->physics->RemoveBody(physBody);
		physBody = nullptr;
	}
}

void CCollider::SetAsSensor(bool is_sensor)
{
	isSensor = is_sensor;

	if (isSensor == true)
	{
		physBody->body->setCollisionFlags(physBody->body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		External->physics->RecalculateInertia(physBody, mass, useGravity);
	}
	else
		physBody->body->setCollisionFlags(physBody->body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
}