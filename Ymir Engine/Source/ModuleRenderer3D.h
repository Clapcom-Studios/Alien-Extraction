#pragma once

#include "External/Glew/include/glew.h"
#include "External/SDL/include/SDL_opengl.h"
#include <gl/GL.h>

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Source/External/Glew/libx86/glew32.lib")

#include "External/DevIL/include/il.h"	
#include "External/DevIL/include/ilu.h"	

#include "External/MathGeoLib/include/Math/float3x3.h"
#include "External/MathGeoLib/include/Math/float4x4.h"
#include "External/MathGeoLib/include/Geometry/Plane.h"

#include "External/MathGeoLib/include/Geometry/AABB.h"
#include "External/MathGeoLib/include/Geometry/OBB.h"

#include "External/FreeType/include/ft2build.h"
#include "External/FreeType/include/freetype/freetype.h"
#include "External/FreeType/include/freetype/ftglyph.h"
#pragma comment(lib, "Source/External/FreeType/libx86/freetype.lib")

#ifdef _DEBUG
#pragma comment (lib, "Source/External/MathGeoLib/libx86/lib_Debug/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#else
#pragma comment (lib, "Source/External/MathGeoLib/libx86/lib_Release/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#endif // _DEBUG

#include <fstream>
#include <vector>
#include "Module.h"
#include "Globals.h"
#include "Primitive.h"

#include "Model.h"
#include "Texture.h"
#include "Shader.h"
#include "UI_Text.h"
#include "CMesh.h"
#include "Log.h"

class GameObject;
class CCamera;
class CParticleSystem;
class ParticleEmitter;

struct LineRender
{
	LineRender(float3& _a, float3& _b, float3& _color, float _width = 10.f) : a(_a), b(_b), color(_color), width(_width) {}
	float3 a, b, color;
	float width;
};

struct DebugTriangle
{
	DebugTriangle(float3& _a, float3& _b, float3& _c, float3& _color) : a(_a), b(_b), c(_c), color(_color) {}
	float3 a, b, c, color;
};

struct DebugPoint
{
	DebugPoint(float3& _position, float3& _color) : position(_position), color(_color) {}
	float3 position, color;
};

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, const void* userParam);

class ModuleRenderer3D : public Module
{
public:

	ModuleRenderer3D(Application* app, bool start_enabled = true);
	virtual ~ModuleRenderer3D();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void OnResize(int width, int height);
	void SetGameCamera(CCamera* cam = nullptr);
	bool IsWalkable(float3 pointToCheck);
	void DrawGameObjects(bool isGame);
	void ClearModels();

	void EnableAssimpDebugger();
	void CleanUpAssimpDebugger();

	void DrawDebugLines();

	void AddDebugLines(float3& a, float3& b, float3& color);

	void AddDebugTriangles(float3& a, float3& b, float3& c, float3& color);

	void AddDebugPoints(float3& position, float3& color);

	void DebugLine(LineSegment& line);

	void AddRay(float3& a, float3& b, float3& color, float& rayWidth);

	void DrawRays();

	bool IsFileExtension(const char* directory, const char* extension);

	void ClearActualTexture();

	void ReloadTextures();

	void DrawBox(float3* vertices, float3 color);

	// Draw Meshes Bounding Boxes
	void DrawBoundingBoxes();

	// Handle Frustum Culling
	bool IsInsideFrustum(const CCamera* camera, const AABB& aabb);

	// Draw Physics Colliders
	void DrawPhysicsColliders();

	// Draw UI
	void GetUIGOs(GameObject* go, std::vector<C_UI*>& listgo);
	void DrawUIElements(bool isGame, bool isBuild);

	void DrawLightsDebug();

	void DrawParticles(ParticleEmitter* emitter);
	void DrawParticlesSystem(CParticleSystem* pSystem);
	bool DrawParticlesShapeDebug(CParticleSystem* obj);
	void DrawOutline(CMesh* cMeshReference, float4x4 transform);

public:

	SDL_GLContext context;

	CPlane Grid;
	bool showGrid = true;

	// 3D Models
	std::vector<Model> models;

	// UI Models
	std::vector<C_UI*> listUI; // clean up 

	bool texturingEnabled = true;

	//std::vector<ParticleEmitter*> particleEmitters;
	//bool initParticles = false;

private:

	std::vector<LineRender> lines;
	std::vector<DebugTriangle> triangles;
	std::vector<DebugPoint> points;
	std::vector<LineRender> rays;

	LineSegment pickingDebug;
	// Outline Shader
	Shader* outlineShader;

};