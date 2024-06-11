#include "ModuleRenderer3D.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleResourceManager.h"
#include "ModulePathfinding.h"

#include "Globals.h"
#include "GameObject.h"
#include "G_UI.h"

#include "DefaultShader.h"

#include "External/Optick/include/optick.h"

#include "Tracy.h"
#include "External/mmgr/mmgr.h"

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	// Ignoring the following WARNING: Buffer detailed info: Buffer objects will use VIDEO memory as the source for buffer object operations.
	if (id == 131185) return;
	
	std::string logMessage = "[ERROR] OPENGL CALLBACK: ";

	switch (type) {

		case GL_DEBUG_TYPE_ERROR:
			logMessage += "Type: ERROR, ";
			break;

		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			logMessage += "Type: DEPRECATED_BEHAVIOR, ";
			break;

		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			logMessage += "Type: UNDEFINED_BEHAVIOR, ";
			break;

		case GL_DEBUG_TYPE_PORTABILITY:
			logMessage += "Type: PORTABILITY, ";
			break;

		case GL_DEBUG_TYPE_PERFORMANCE:
			logMessage += "Type: PERFORMANCE, ";
			break;

		case GL_DEBUG_TYPE_OTHER:
			logMessage += "Type: OTHER, ";
			break;

	}

	switch (severity) {

		case GL_DEBUG_SEVERITY_LOW:
			logMessage += "Severity: LOW, ";
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
			logMessage += "Severity: MEDIUM, ";
			break;

		case GL_DEBUG_SEVERITY_HIGH:
			logMessage += "Severity: HIGH, ";
			break;

	}

	logMessage += "ID: " + std::to_string(id) + ", ";

	logMessage += "MESSAGE: " + std::string(("%s", message));

	logMessage += "\n";

	LOG(logMessage.c_str());
}

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	context = nullptr;
	outlineShader = nullptr;

	LOG("Creating ModuleRenderer3D");

}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{

}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	OPTICK_EVENT();

	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	// Stream Assimp Log messages to Debug window
	EnableAssimpDebugger();

	// OpenGL initial attributes
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // Using OpenGL core profile
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	// Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG("[ERROR] OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		LOG("OpenGL context created sucessfully!");
	}

	// Initializing Glew
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		LOG("[ERROR] Could not load Glew: %s\n", glewGetErrorString(err));
	}
	else {
		LOG("Successfully using Glew %s", glewGetString(GLEW_VERSION));
	}

	// OpenGL Debug Callback
#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
#endif // _DEBUG

#ifdef TRACY_ENABLE
	TracyGpuContext;
#endif // TRACY_ENABLE

	// Initializing DevIL
	ilInit();
	ILenum ILerror = ilGetError();
	if (ILerror != IL_NO_ERROR) {
		LOG("[ERROR] Could not load DevIL: %s\n", iluErrorString(ILerror));
	}
	else {
		LOG("Successfully using DevIL %d", iluGetInteger(IL_VERSION_NUM));
	}

	if (ret == true)
	{
		// Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
		{
			LOG("[WARNING] Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Enable OpenGL initial configurations

		// Stencil Buffer (outline)
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
		
		// Additional OpenGL configurations (starting disabled)

		glDisable(GL_TEXTURE_3D);
		glDisable(GL_MULTISAMPLE);
		glDisable(GL_SCISSOR_TEST);
		glDisable(GL_POINT_SPRITE);
		glDisable(GL_FOG);
		glDisable(GL_POINT_SMOOTH);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_NORMALIZE);
		glDisable(GL_POLYGON_OFFSET_FILL);

		// Wireframe-Only Mode starting disabled

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}
	
	SDL_MaximizeWindow(App->window->window);

	LOG("OpenGL initialized successfully.");

	Grid.axis = true;

	// Load Street Environment from the start

	//models.push_back(Model("Assets/BakerHouse.fbx"));
	//models.push_back(Model("Assets/Street_Environment/StreetEnvironment.fbx"));

	// Skybox

	//models.push_back(Model("Assets/Skybox/Skybox.fbx"));

	// Water Shader

	//models.push_back(Model("Assets/Water.fbx", WATER_SHADER));

	// Shaders already loaded from the start

	Shader* defaultShader = new Shader;
	defaultShader->LoadShader("Assets/Shaders/DefaultShader.glsl");
	delete defaultShader;

	Shader* rainbowShader = new Shader;
	rainbowShader->LoadShader("Assets/Shaders/RainbowShader.glsl");
	delete rainbowShader;

	Shader* lavaShader = new Shader;
	lavaShader->LoadShader("Assets/Shaders/LavaShader.glsl");
	delete lavaShader;

	Shader* colorShader = new Shader;
	colorShader->LoadShader("Assets/Shaders/ColorShader.glsl");
	delete colorShader;

	Shader* waterShader = new Shader;
	waterShader->LoadShader("Assets/Shaders/WaterShader.glsl");
	delete waterShader;

	Shader* animationShader = new Shader;
	animationShader->LoadShader("Assets/Shaders/AnimationShader.glsl");
	delete animationShader;

	Shader* lightingShader = new Shader;
	lightingShader->LoadShader("Assets/Shaders/Lighting Shader.glsl");
	delete lightingShader;

	Shader* specularShader = new Shader;
	specularShader->LoadShader("Assets/Shaders/SpecularLight.glsl");
	delete specularShader;

	outlineShader = new Shader;
	outlineShader->LoadShader("Assets/Shaders/OutlineShader.glsl");

	// Load Editor and Game FrameBuffers

	App->camera->editorCamera->framebuffer.Load();

	App->scene->gameCameraComponent = new CCamera(App->scene->gameCameraObject);

	// TODO: remove and do with proper constructor
	App->scene->gameCameraObject->mTransform->SetPosition(float3(-40.0f, 29.0f, 54.0f));
	App->scene->gameCameraObject->mTransform->SetRotation(float3(180.0f, 40.0f, 180.0f));

	//App->scene->gameCameraComponent->SetPos(-40.0f, 29.0f, 54.0f);
	//App->scene->gameCameraComponent->LookAt(float3(0.f, 0.f, 0.f));

	//Hardcodeado para la VS1
	App->scene->gameCameraComponent->SetAspectRatio(SCREEN_WIDTH / SCREEN_HEIGHT);
	App->scene->gameCameraObject->AddComponent(App->scene->gameCameraComponent);
	
	//App->scene->gameCameraComponent->framebuffer.Load();

	uint UID = 1728623793; // UID of Cube.fbx mesh in meta (lo siento)

	std::string libraryPath = External->fileSystem->libraryMeshesPath + std::to_string(UID) + ".ymesh";

	if (!PhysfsEncapsule::FileExists(libraryPath)) {

		External->resourceManager->ImportFile("Assets/Primitives/Cube.fbx", true);

	}

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	OPTICK_EVENT();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->editorCamera->GetViewMatrix().ptr());

	App->editor->AddFPS(App->GetFPS());
	App->editor->AddDT(App->GetDT());
	App->editor->AddMS(App->GetMS());

	return UPDATE_CONTINUE;
}
static bool started = false; // Sorry

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
#ifdef TRACY_ENABLE
	ZoneScoped;
	TracyGpuZone("OpenGL Render");
#endif // TRACY_ENABLE

	OPTICK_EVENT();

#ifdef _STANDALONE // Sorry for doing this, it was necessary (Francesc) :(

	if (!started) {

		TimeManager::gameTimer.Start();

		started = true;

	}

#endif // _STANDALONE

	// Clear color buffer and depth buffer before each PostUpdate call
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Your rendering code here

	// --------------------------- Editor Camera FrameBuffer -----------------------------------

#ifndef _STANDALONE

	App->camera->editorCamera->framebuffer.Render(true);

	// Render Grid

	App->camera->editorCamera->Update();

	if (showGrid) {
		
		Grid.Render();

	}

	if (App->scene->gameCameraComponent != nullptr)
	{
		// Render Frustum Box

		if (App->scene->gameCameraObject->active) {

			App->scene->gameCameraComponent->DrawFrustumBox();

		}

		// Render Bounding Boxes

		if (External->scene->gameCameraComponent->drawBoundingBoxes)
		{
			DrawBoundingBoxes();
		}

		// Render Physics Colliders

		if (App->physics->debugScene)
		{
			DrawPhysicsColliders();
		}

		DrawGameObjects(false);

		DrawLightsDebug();

		DebugLine(pickingDebug);
		DrawDebugLines();

		DrawUIElements(false,false);

	}

	App->camera->editorCamera->framebuffer.Render(false);

	// --------------------------- Game Camera FrameBuffer -----------------------------------

	if (App->scene->gameCameraComponent != nullptr)
	{
		App->scene->gameCameraComponent->framebuffer.Render(true);

		App->scene->gameCameraComponent->Update();

		if (App->scene->gameCameraObject->active) {

			if (App->scene->godMode || App->physics->debugGame)
			{
				DrawPhysicsColliders();
			}

			DrawGameObjects(true);

			DrawUIElements(true,false);

		}

		App->scene->gameCameraComponent->framebuffer.Render(false);

	}

	// --------------------------- Drawing editor and Swaping Window -------------------------

	App->editor->DrawEditor();

#else

	if (App->scene->gameCameraComponent != nullptr)
	{
		App->scene->gameCameraComponent->framebuffer.Render(true);

		App->scene->gameCameraComponent->Update();

		if (App->scene->gameCameraObject->active) {

			if (App->scene->godMode || App->physics->debugGame)
			{
				DrawPhysicsColliders();
			}

			DrawGameObjects(true);

			DrawUIElements(true, true);

		}

		App->scene->gameCameraComponent->framebuffer.Render(false);

		// Adjust FrameBuffer for Standalone

		App->scene->gameCameraComponent->framebuffer.RenderToScreen();

	}

#endif // !_STANDALONE

	SDL_GL_SwapWindow(App->window->window);

#ifdef TRACY_ENABLE
	TracyGpuCollect;
#endif // TRACY_ENABLE

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	OPTICK_EVENT();

	LOG("Destroying 3D Renderer");

	// Delete things from Init

	delete outlineShader;

	Shader::loadedShaders.clear();

	// Clean Framebuffers
	App->camera->editorCamera->framebuffer.Delete();

	// Detach Assimp Log Stream
	CleanUpAssimpDebugger();

	// Shutdown DevIL
	ilShutDown();

	// Delete OpenGL context
	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	//for (auto& it = App->scene->cameras.begin(); it != App->scene->cameras.end(); ++it) {

	//	(*it)->SetAspectRatio((float)width / (float)height);

	//}

	App->camera->editorCamera->SetAspectRatio((float)width / (float)height);
	App->scene->gameCameraComponent->SetAspectRatio((float)width / (float)height);
}

void ModuleRenderer3D::SetGameCamera(CCamera* cam)
{
	if (cam != nullptr)
	{
		App->scene->gameCameraComponent = cam;
		OnResize(SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
	}
}

bool ModuleRenderer3D::IsFileExtension(const char* directory, const char* extension)
{
	size_t strLen = strlen(directory);
	size_t suffixLen = strlen(extension);

	if (strLen < suffixLen) {

		return false;
	}

	return strncmp(directory + strLen - suffixLen, extension, suffixLen) == 0;
}

void ModuleRenderer3D::ClearActualTexture()
{
	for (auto it = models.begin(); it != models.end(); ++it) {

		for (auto jt = (*it).meshes.begin(); jt != (*it).meshes.end(); ++jt) {

			if ((*jt).meshGO->selected || (*it).modelGO->selected) {

				(*jt).textures.clear();

			}

		}

	}

}

void ModuleRenderer3D::ReloadTextures()
{
	for (auto it = models.begin(); it != models.end(); ++it) {

		for (auto jt = (*it).meshes.begin(); jt != (*it).meshes.end(); ++jt) {

			(*jt).loadedTextures = false;
			(*jt).loadedShader = false;
		}
	}
}

void ModuleRenderer3D::DrawBox(float3* vertices, float3 color)
{
	uint indices[24] = {

		0,2,2,
		6,6,4,
		4,0,0,
		1,1,3,
		3,2,4,
		5,6,7,
		5,7,3,
		7,1,5

	};

	glLineWidth(2.f);
	glBegin(GL_LINES);
	glColor3fv(color.ptr());

	for (size_t i = 0; i < (sizeof(indices) / sizeof(indices[0])); i++) {

		glVertex3fv(vertices[indices[i]].ptr());

	}

	glColor3f(255.f, 255.f, 255.f);
	glEnd();
	glLineWidth(1.f);
}

void ModuleRenderer3D::DrawBoundingBoxes()
{
	/*for (auto it = models.begin(); it != models.end(); ++it) {

		for (auto jt = (*it).meshes.begin(); jt != (*it).meshes.end(); ++jt) {

			if (External->renderer3D->IsInsideFrustum(External->scene->gameCameraComponent, (*jt).globalAABB)) {

				(*jt).UpdateBoundingBoxes();
				(*jt).RenderBoundingBoxes();

			}

		}

	}*/

	for (auto it = App->scene->gameObjects.begin(); it != App->scene->gameObjects.end(); ++it)
	{
		CMesh* meshComponent = (CMesh*)(*it)->GetComponent(ComponentType::MESH);

		if (meshComponent != nullptr) {

			if (IsInsideFrustum(External->scene->gameCameraComponent, meshComponent->globalAABB))
			{
				meshComponent->RenderBoundingBoxes();
			}
		}

	}
}

void ModuleRenderer3D::DrawPhysicsColliders()
{
	if (App->editor->gl_Lighting) glDisable(GL_LIGHTING); // Los colliders y los sensores se ver�n siempre sin importar la iluminacion

	for (auto it = App->scene->gameObjects.begin(); it != App->scene->gameObjects.end(); ++it)
	{
		CCollider* colliderComponent = (CCollider*)(*it)->GetComponent(ComponentType::PHYSICS);

		if (colliderComponent != nullptr && colliderComponent->physBody->drawShape) 
		{
			Color color;
			if (colliderComponent->isSensor) color = App->physics->sensorColor;
			else color = App->physics->colliderColor;

			App->physics->world->debugDrawWorld();

			btCollisionShape* shape = colliderComponent->physBody->body->getCollisionShape();

			glColor3f(color.r, color.g, color.b); // Cambio aqu� el color para tenerlo m�s controlado
			glLineWidth(App->physics->shapeLineWidth); // Lo mismo con la lineWidth

			switch (shape->getShapeType())
			{
			case BroadphaseNativeTypes::BOX_SHAPE_PROXYTYPE: // RENDER BOX ==========================
				App->physics->RenderBoxCollider(colliderComponent->physBody);
				break;
			case BroadphaseNativeTypes::SPHERE_SHAPE_PROXYTYPE: // RENDER SPHERE ====================
				App->physics->RenderSphereCollider(colliderComponent->physBody);
				break;
			case BroadphaseNativeTypes::CAPSULE_SHAPE_PROXYTYPE: // RENDER CAPSULE ==================
				App->physics->RenderCapsuleCollider(colliderComponent->physBody);
				break;			
			case BroadphaseNativeTypes::CONE_SHAPE_PROXYTYPE: // RENDER CONE ========================
				App->physics->RenderConeCollider(colliderComponent->physBody);
				break;
			case BroadphaseNativeTypes::CYLINDER_SHAPE_PROXYTYPE: // RENDER CYLINDER ==================
				App->physics->RenderCylinderCollider(colliderComponent->physBody);
				break;
			case BroadphaseNativeTypes::CONVEX_TRIANGLEMESH_SHAPE_PROXYTYPE: // RENDER MESH =========
				App->physics->RenderMeshCollider(colliderComponent->physBody);
				break;
			} 

			glColor3f(255.0f, 255.0f, 255.0f);
			glLineWidth(1.f);
		}
	}

	if (App->editor->gl_Lighting) glEnable(GL_LIGHTING);
}

bool ModuleRenderer3D::IsInsideFrustum(const CCamera* camera, const AABB& aabb)
{
	// Check if frustum culling is enabled for the camera.
	if (camera->enableFrustumCulling) {

		// Get the planes of the camera frustum.
		Plane frustumPlanes[6];
		camera->frustum.GetPlanes(frustumPlanes);

		// Get the corner points of the AABB.
		float3 cornerPoints[8];
		aabb.GetCornerPoints(cornerPoints);

		// Loop through each plane of the frustum.
		for (int i = 0; i < 6; ++i) {

			// Counter for points inside the frustum plane.
			uint pointsInside = 8;

			// Loop through each corner point of the AABB.
			for (int j = 0; j < 8; ++j)
			{
				// Check if the corner point is on the positive side of the frustum plane.
				if (frustumPlanes[i].IsOnPositiveSide(cornerPoints[j]))
				{
					// Decrement the counter if the point is on the positive side.
					--pointsInside;
				}
			}

			// If all corner points are outside the frustum plane, the AABB is outside the frustum.
			if (pointsInside == 0)
			{
				return false;
			}

		}

	}

	// If frustum culling is not enabled or the AABB is inside the frustum, return true.
	return true;
}

void ModuleRenderer3D::GetUIGOs(GameObject* go, std::vector<C_UI*>& listgo)
{
	if (go->GetComponent(ComponentType::UI) != nullptr)
	{
		C_UI* ui = static_cast<C_UI*>(go->GetComponent(ComponentType::UI));
		listgo.push_back(ui);
	}

	if (!go->mChildren.empty())
	{
		for (auto i = 0; i < go->mChildren.size(); i++)
		{
			GetUIGOs(go->mChildren[i], listgo);
		}
	}
}

void ModuleRenderer3D::DrawUIElements(bool isGame, bool isBuild)
{
	if (isGame)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		isBuild ? glOrtho(0.0, External->window->width, External->window->height, 0.0, 1.0, -1.0) : glOrtho(0.0, App->editor->gameViewSize.x, App->editor->gameViewSize.y, 0.0, 1.0, -1.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// El resto bien menos semitransparencia
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5f);

		// alpha semitransparente bien
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//glEnable(GL_DEPTH_TEST); // Enable depth testing
		//glDepthFunc(GL_LEQUAL); // Set the depth test function

		//glDepthMask(GL_FALSE);
		// glAlphaFunc(GL_GREATER, 0.5f); // Commented out alpha testing
	}

	//Get UI elements to draw
	std::vector<C_UI*> listUI;

	for (auto it = App->scene->vCanvas.begin(); it != App->scene->vCanvas.end(); ++it)
	{
		GetUIGOs(*it, listUI);

	}

	if (!listUI.empty())
	{
		for (auto i = listUI.size() - 1; i >= 0; --i)
		{
			if (listUI[i]->mOwner->active && listUI[i]->active)
			{
				listUI[i]->Draw(isGame);
				//listUI[i]->DebugDraw();

			}
			
			if (i == 0) { break; }
		}
	}

	//glDepthMask(GL_TRUE);
	glAlphaFunc(GL_GREATER, 0.0f);
}

void ModuleRenderer3D::DrawParticlesSystem(CParticleSystem* pSystem)
{
	//This list will contain ALL the particles of a Particle System, so all the emitters
	std::vector<Particle*> listParticlesToPrint;

	//Order all the list of emitters between them.
	for (int i = 0; i < pSystem->allEmitters.size(); i++)
	{
		if(!pSystem->allEmitters.at(i)->listParticles.empty())
		{
			if(listParticlesToPrint.empty())
			{
				//We add the already ordered from furthest to closet particle to the list
				for (uint j = 0; j < pSystem->allEmitters.at(i)->listParticles.size(); ++j)
				{
					listParticlesToPrint.push_back(pSystem->allEmitters.at(i)->listParticles.at(j));
				}
			}
			else
			{
				int posToStartEmplacement = listParticlesToPrint.size();

				//Distancia particula mas lejana (la primera) del emitter
				float emitterToZ;
#ifdef _STANDALONE
					emitterToZ = (External->scene->gameCameraComponent->GetPos().z - pSystem->allEmitters.at(i)->listParticles.at(0)->position.z);
#else
					emitterToZ = (External->camera->editorCamera->GetPos().z - pSystem->allEmitters.at(i)->listParticles.at(0)->position.z);
#endif // _STANDALONE

				//Buscar la particula mas lejana en comparacion para empezar en ese punto
				for (int k = 0; k < listParticlesToPrint.size(); k++) 
				{
					float printToZ; //Distancia desde la particula de la print list hasta la camara
#ifdef _STANDALONE
					printToZ = (External->scene->gameCameraComponent->GetPos().z - (listParticlesToPrint.at(k)->position.z));
#else
					printToZ = (External->camera->editorCamera->GetPos().z -(listParticlesToPrint.at(k)->position.z));
#endif // _STANDALONE
					if(emitterToZ* emitterToZ < printToZ * printToZ)
					{
						//Guardarse la posicion para a empezar a colocar desde ahi de manera mas optima.
						posToStartEmplacement = k;
						break;
					}
				}

				//Add to the list starting to search from the pos to Start
				for(int j = 0; j < pSystem->allEmitters.at(i)->listParticles.size();j++)
				{
#ifdef _STANDALONE
					emitterToZ = (External->scene->gameCameraComponent->GetPos().z - pSystem->allEmitters.at(i)->listParticles.at(j)->position.z);
#else
					emitterToZ = (External->camera->editorCamera->GetPos().z - pSystem->allEmitters.at(i)->listParticles.at(j)->position.z);
#endif // _STANDALONE

					bool emplaced = false;
					for (int k = posToStartEmplacement; k < listParticlesToPrint.size(); k++)
					{
						float printToZ; //Distancia desde la particula de la print list hasta la camara
#ifdef _STANDALONE
						printToZ = (External->scene->gameCameraComponent->GetPos().z - (listParticlesToPrint.at(k)->position.z));
#else
						printToZ = (External->camera->editorCamera->GetPos().z - (listParticlesToPrint.at(k)->position.z));
#endif // _STANDALONE
						if (emitterToZ * emitterToZ < printToZ * printToZ)
						{
							//Guardarse la posicion para a empezar a colocar desde ahi de manera mas optima.
							listParticlesToPrint.emplace(listParticlesToPrint.begin() + k, pSystem->allEmitters.at(i)->listParticles.at(j));
							emplaced = true;
							posToStartEmplacement = k-1; //Como ya estan ordenados es asegurado que se ordenara despues de la particula que ya hay puesta
							break;
						}
					}

					//Si no ha sido colocado en ningun momento colocarlo al final de todo, como la particula mas cercana
					if(!emplaced)
					{
						listParticlesToPrint.push_back(pSystem->allEmitters.at(i)->listParticles.at(j));
					}
				}

			}
		}
	}

	//Print en si de las particulas
	glCullFace(GL_FALSE);


	for (int i = 0; i < listParticlesToPrint.size(); i++)
	{
		Particle* par = listParticlesToPrint.at(i);

		//Matrix transform de la particula
		float4x4 m = float4x4::FromTRS(par->position, par->worldRotation, par->size).Transposed();

		glPushMatrix();
		glMultMatrixf(m.ptr());

		//TODO TONI: ... this works, im sorry
		glColor4f(par->color.r * 4, par->color.g * 4, par->color.b * 4, par->color.a * 4);

		if (par->pTexture != nullptr)
		{
			par->pTexture->BindTexture(true, 0 /*par->pTexture->ID*/);
		}

		//Drawing to tris in direct mode
		glBegin(GL_TRIANGLES);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(.5f, -.5f, .0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-.5f, .5f, .0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-.5f, -.5f, .0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(.5f, -.5f, .0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(.5f, .5f, .0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-.5f, .5f, .0f);

		if (par->pTexture != nullptr)
		{
			par->pTexture->BindTexture(false, 0/*par->pTexture->ID*/);
		}

		glEnd();
		glPopMatrix();
		//glBindTexture(GL_TEXTURE_2D, 0);

		par = nullptr;
	}

	//Mas try fix de Xavi
	//glDepthMask(GL_TRUE);
	glCullFace(GL_TRUE); //Funciona mejor que el depthMask

	//Borrar todo de la lista
	if (!listParticlesToPrint.empty())
	{
		/*for (auto it = listParticlesToPrint.rbegin(); it != listParticlesToPrint.rend(); ++it)
		{
			delete (*it);
			(*it) = nullptr;
		}*/

		listParticlesToPrint.clear();
	}
}

void ModuleRenderer3D::DrawParticles(ParticleEmitter* emitter) //TODO ERIC: Posiblemente haya que reworkear un poco el sistema de particula y que sea una lista enorme de todas las particulas en el momento. Ya de paso aprovecho y que sea una pool.
{
	//Try fix Xavi transparencias particulas
	//glDepthMask(GL_FALSE);

	//Fix recomendado por BCN
	glCullFace(GL_FALSE);
	

	for (int i = 0; i < emitter->listParticles.size(); i++)
	{
		Particle* par = emitter->listParticles.at(i);

		//Matrix transform de la particula
		float4x4 m = float4x4::FromTRS(par->position, par->worldRotation, par->size).Transposed();

		glPushMatrix();
		glMultMatrixf(m.ptr());

		//TODO TONI: ... this works, im sorry
		glColor4f(par->color.r * 4, par->color.g * 4, par->color.b * 4, par->color.a * 4);

		if (par->pTexture != nullptr)
		{
			par->pTexture->BindTexture(true, 0 /*par->pTexture->ID*/);
		}
		
		//Drawing to tris in direct mode
		glBegin(GL_TRIANGLES);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(.5f, -.5f, .0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-.5f, .5f, .0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-.5f, -.5f, .0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(.5f, -.5f, .0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(.5f, .5f, .0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-.5f, .5f, .0f);

		if (par->pTexture != nullptr)
		{
			par->pTexture->BindTexture(false, 0/*par->pTexture->ID*/);
		}

		glEnd();
		glPopMatrix();
		//glBindTexture(GL_TEXTURE_2D, 0);

		par = nullptr;
	}

	//Mas try fix de Xavi
	//glDepthMask(GL_TRUE);
	glCullFace(GL_TRUE); //Funciona mejor que el depthMask
	
}

bool ModuleRenderer3D::DrawParticlesShapeDebug(CParticleSystem* pSystem)
{
	bool ret = true;
	if (pSystem->allEmitters.empty() || !pSystem->showEmitters)
	{
		return false;;
	}
	for (uint i = 0; i < pSystem->allEmitters.size(); i++)
	{
		EmitterBase* eBase = (EmitterBase*)pSystem->allEmitters.at(i)->modules.at(0); //We create the referemce to accces more easily

		Quat rotacion;
		switch (eBase->rotacionBase)
		{
		case PAR_WORLD_MATRIX:
			rotacion = Quat::identity;
			break;
		case PAR_GAMEOBJECT_MATRIX:
			rotacion = pSystem->mOwner->mTransform->GetLocalRotation();
			break;
		case PAR_PARENT_MATRIX:
			rotacion = pSystem->mOwner->mParent->mTransform->GetGlobalRotation();
			break;
		case PAR_INITIAL_ROTATION_END:
			break;
		default:
			break;
		}

		//POINT ROTATION
		Quat nuwDirQuat = rotacion.Mul(Quat(eBase->emitterOrigin.x, eBase->emitterOrigin.y, eBase->emitterOrigin.z, 0));
		float3 originModified = float3(nuwDirQuat.x, nuwDirQuat.y, nuwDirQuat.z);

		////BOX ROTATIONS ERIC:TODO Fixear esto, rota mal
		Quat nuwDirPositives = rotacion.Mul(Quat(eBase->boxPointsPositives.x, eBase->boxPointsPositives.y, eBase->boxPointsPositives.z, 0));
		float3 positivesModified = eBase->boxPointsPositives + originModified;

		//Get rotated negatives point from the world
		float3 negativesModified = eBase->boxPointsNegatives + originModified;

		float3 color = { 0.8f,0,0.8f };
		switch (eBase->currentShape)
		{
		case SpawnAreaShape::PAR_POINT:

			glLineWidth(4.f);
			glBegin(GL_LINES);
			glColor3fv(color.ptr());


			//Position 0,0,0 of game object and current origin emmiter
			glVertex3fv(pSystem->mOwner->mTransform->GetGlobalPosition().ptr());
			glVertex3fv((pSystem->mOwner->mTransform->GetGlobalPosition() + originModified).ptr());



			glColor3f(255.f, 255.f, 255.f);
			glEnd();
			glLineWidth(1.f);

			break;
		case SpawnAreaShape::PAR_CONE:
		{
			glLineWidth(2.f);
			glBegin(GL_LINES);
			glColor3fv(color.ptr());


			float3 temp;
			float3 temp2;
			int numDivision = 1;
			int numPuntos = 12;
			for (int j = 0; j <= numDivision + 1; j++) //Hara siempre 0 que es la base y 1 que es arriba
			{
				float subDiv = (j * eBase->heigth) / (eBase->heigth * (numDivision + 1));

				for (int i = 0; i < numPuntos; i++) //En vez de hacer ciruclos perfectos que requeririan demasiados puntos hacemos solo 12 que queda suficientemente redondeado
				{
					//Draw external circle
					temp = { cos((pi * 2 / numPuntos) * i) * ((1 - subDiv) * eBase->baseRadius + eBase->topRadius * subDiv),eBase->heigth * subDiv,-sin((pi * 2 / numPuntos) * i) * ((1 - subDiv) * eBase->baseRadius + eBase->topRadius * subDiv) }; //Lerp entre base y top usando subdiv
					temp2 = { cos((pi * 2 / numPuntos) * (i + 1)) * ((1 - subDiv) * eBase->baseRadius + eBase->topRadius * subDiv),eBase->heigth * subDiv,-sin((pi * 2 / numPuntos) * (i + 1)) * ((1 - subDiv) * eBase->baseRadius + eBase->topRadius * subDiv) }; //Lerp entre base y top usando subdiv
					glVertex3fv((originModified + temp + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());
					glVertex3fv((originModified + temp2 + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());

					if (eBase->radiusHollow > 0.0f)
					{
						//Draw inner circle
						temp = { cos((pi * 2 / numPuntos) * i) * ((1 - subDiv) * eBase->radiusHollow + eBase->radiusHollow * (eBase->topRadius / eBase->baseRadius) * subDiv),eBase->heigth * subDiv,-sin((pi * 2 / numPuntos) * i) * ((1 - subDiv) * eBase->radiusHollow + eBase->radiusHollow * (eBase->topRadius / eBase->baseRadius) * subDiv) }; //Lerp entre base y top usando subdiv
						temp2 = { cos((pi * 2 / numPuntos) * (i + 1)) * ((1 - subDiv) * eBase->radiusHollow + eBase->radiusHollow * (eBase->topRadius / eBase->baseRadius) * subDiv),eBase->heigth * subDiv,-sin((pi * 2 / numPuntos) * (i + 1)) * ((1 - subDiv) * eBase->radiusHollow + eBase->radiusHollow * (eBase->topRadius / eBase->baseRadius) * subDiv) }; //Lerp entre base y top usando subdiv
						glVertex3fv((originModified + temp + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());
						glVertex3fv((originModified + temp2 + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());
					}
				}
			}

			for (int i = 0; i < numPuntos; i++)
			{
				if (i == 0 || i % (numPuntos / 4) == 0)
				{
					//Draw external circle
					temp = { cos((pi * 2 / numPuntos) * i) * eBase->baseRadius ,0.0f ,-sin((pi * 2 / numPuntos) * i) * eBase->baseRadius }; //Punto Inferior
					temp2 = { cos((pi * 2 / numPuntos) * i) * eBase->topRadius ,eBase->heigth ,-sin((pi * 2 / numPuntos) * i) * eBase->topRadius }; //Punto Superior
					glVertex3fv((originModified + temp + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());
					glVertex3fv((originModified + temp2 + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());

					if (eBase->radiusHollow > 0.0f)
					{
						//Draw inner circle
						temp = { cos((pi * 2 / numPuntos) * i) * eBase->radiusHollow ,0.0f ,-sin((pi * 2 / numPuntos) * i) * eBase->radiusHollow }; //Punto Inferior
						temp2 = { cos((pi * 2 / numPuntos) * i) * eBase->radiusHollow * (eBase->topRadius / eBase->baseRadius) ,eBase->heigth ,-sin((pi * 2 / numPuntos) * i) * eBase->radiusHollow * (eBase->topRadius / eBase->baseRadius) }; //Punto Superior
						glVertex3fv((originModified + temp + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());
						glVertex3fv((originModified + temp2 + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());
					}
				}
			}



			glColor3f(255.f, 255.f, 255.f);
			glEnd();
			glLineWidth(1.f);


		}
		break;
		case SpawnAreaShape::PAR_BOX:
		{
			float3 temp;
			float3 vertices[8];
			temp = { negativesModified.x,negativesModified.y,positivesModified.z };
			vertices[0] = temp + pSystem->mOwner->mTransform->GetGlobalPosition();
			temp = { negativesModified.x,negativesModified.y,negativesModified.z };
			vertices[1] = temp + pSystem->mOwner->mTransform->GetGlobalPosition();
			temp = { negativesModified.x,positivesModified.y,positivesModified.z };
			vertices[2] = temp + pSystem->mOwner->mTransform->GetGlobalPosition();
			temp = { negativesModified.x,positivesModified.y,negativesModified.z };
			vertices[3] = temp + pSystem->mOwner->mTransform->GetGlobalPosition();
			temp = { positivesModified.x,negativesModified.y,positivesModified.z };
			vertices[4] = temp + pSystem->mOwner->mTransform->GetGlobalPosition();
			temp = { positivesModified.x,negativesModified.y,negativesModified.z };
			vertices[5] = temp + pSystem->mOwner->mTransform->GetGlobalPosition();
			temp = { positivesModified.x,positivesModified.y,positivesModified.z };
			vertices[6] = temp + pSystem->mOwner->mTransform->GetGlobalPosition();
			temp = { positivesModified.x,positivesModified.y,negativesModified.z };
			vertices[7] = temp + pSystem->mOwner->mTransform->GetGlobalPosition();

			DrawBox(vertices, color);

			if (External->scene->godMode)
			{
				glLineWidth(4.f);
				glBegin(GL_LINES);
				glColor3fv(color.ptr());

				glVertex3fv((pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());
				glVertex3fv((positivesModified + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());

				glVertex3fv((pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());
				glVertex3fv((negativesModified + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());

				glColor3f(255.f, 255.f, 255.f);
				glEnd();
				glLineWidth(1.f);
			}
		}
		break;
		case SpawnAreaShape::PAR_SPHERE:
		{
			glLineWidth(2.f);
			glBegin(GL_LINES);
			glColor3fv(color.ptr());

			float3 temp;
			float3 temp2;

			int numPuntos = 16;

			//Eje X
			for (int i = 0; i < numPuntos; i++) //En vez de hacer ciruclos perfectos que requeririan demasiados puntos hacemos solo 12 que queda suficientemente redondeado
			{
				//Draw external circle
				temp = { 0,sin((pi * 2 / numPuntos) * i) * eBase->baseRadius ,-cos((pi * 2 / numPuntos) * i) * eBase->baseRadius };
				temp2 = { 0 , sin((pi * 2 / numPuntos) * (i + 1)) * eBase->baseRadius ,-cos((pi * 2 / numPuntos) * (i + 1)) * eBase->baseRadius };
				glVertex3fv((originModified + temp + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());
				glVertex3fv((originModified + temp2 + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());
			}

			//Eje Y
			for (int i = 0; i < numPuntos; i++) //En vez de hacer ciruclos perfectos que requeririan demasiados puntos hacemos solo 12 que queda suficientemente redondeado
			{
				//Draw external circle
				temp = { cos((pi * 2 / numPuntos) * i) * eBase->baseRadius ,0,-sin((pi * 2 / numPuntos) * i) * eBase->baseRadius };
				temp2 = { cos((pi * 2 / numPuntos) * (i + 1)) * eBase->baseRadius ,0,-sin((pi * 2 / numPuntos) * (i + 1)) * eBase->baseRadius };
				glVertex3fv((originModified + temp + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());
				glVertex3fv((originModified + temp2 + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());
			}

			//Eje Z
			for (int i = 0; i < numPuntos; i++) //En vez de hacer ciruclos perfectos que requeririan demasiados puntos hacemos solo 12 que queda suficientemente redondeado
			{
				//Draw external circle
				temp = { cos((pi * 2 / numPuntos) * i) * eBase->baseRadius ,sin((pi * 2 / numPuntos) * i) * eBase->baseRadius, 0 };
				temp2 = { cos((pi * 2 / numPuntos) * (i + 1)) * eBase->baseRadius ,sin((pi * 2 / numPuntos) * (i + 1)) * eBase->baseRadius ,0 };
				glVertex3fv((originModified + temp + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());
				glVertex3fv((originModified + temp2 + pSystem->mOwner->mTransform->GetGlobalPosition()).ptr());
			}

			glColor3f(255.f, 255.f, 255.f);
			glEnd();
			glLineWidth(1.f);
		}
		break;
		case SpawnAreaShape::PAR_SHAPE_ENUM_END:
			break;
		default:
			break;
		}

		eBase = nullptr;
	}
	return ret;
}

void ModuleRenderer3D::DrawLightsDebug()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (App->lightManager->IsLightDebugEnabled()) {

		for (auto& it = App->lightManager->lights.begin(); it != App->lightManager->lights.end(); ++it) {

			if ((*it)->debug && (*it)->lightGO->active && (*it)->lightGO->GetComponent(ComponentType::LIGHT)->active) {

				(*it)->Render();

			}	

		}

	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ModuleRenderer3D::DrawOutline(CMesh* cmesh, float4x4 transform)
{
	glEnable(GL_DEPTH_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilMask(0x00);

	// Stencil Testing (outline)
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	cmesh->rMeshReference->Render();

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	App->editor->gl_DepthTesting = false;

	outlineShader->UseShader(true);

	// Scale the transformation matrix slightly (this should be done scaling smoothed normals)

	float scaleFactor = 1.05f;
	float3 scaleVector(scaleFactor, scaleFactor, scaleFactor);
	float4x4 scaledMatrix = transform * float4x4::Scale(scaleVector, cmesh->aabb.CenterPoint());

	outlineShader->SetShaderUniforms(&scaledMatrix, false);

	cmesh->rMeshReference->Render();

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glEnable(GL_DEPTH_TEST);
	App->editor->gl_DepthTesting = true;

	outlineShader->UseShader(false);
}

void ModuleRenderer3D::DrawGameObjects(bool isGame)
{
	for (auto it = App->scene->gameObjects.begin(); it != App->scene->gameObjects.end(); ++it)
	{
		CTransform* transformComponent = (CTransform*)(*it)->GetComponent(ComponentType::TRANSFORM);
		CMesh* meshComponent = (CMesh*)(*it)->GetComponent(ComponentType::MESH);
		CMaterial* materialComponent = (CMaterial*)(*it)->GetComponent(ComponentType::MATERIAL);
		CAnimation* animationComponent = (CAnimation*)(*it)->GetComponent(ComponentType::ANIMATION);
		CParticleSystem* particleComponent = (CParticleSystem*)(*it)->GetComponent(ComponentType::PARTICLE);

		if (animationComponent != nullptr && animationComponent->active) {
			for (int i = 0; i < (*it)->mChildren.size(); i++) {
				if ((*it)->mChildren[i]->GetComponent(MATERIAL)) {
					CMaterial* cmat = (CMaterial*)(*it)->mChildren[i]->GetComponent(MATERIAL);
					CTransform* ctrans = (CTransform*)(*it)->mChildren[i]->GetComponent(TRANSFORM);
					cmat->shader.UseShader(true);
					std::vector<float4x4> transforms = animationComponent->animator->GetFinalBoneMatrices();
					for (int i = 0; i < transforms.size(); i++) {
						cmat->shader.SetMatrix4x4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
						cmat->shader.SetShaderUniforms(&ctrans->mGlobalMatrix, (*it)->selected);
					}
				}
			}
		}

		if ((*it)->active && meshComponent != nullptr && meshComponent->active)
		{
			if (IsInsideFrustum(External->scene->gameCameraComponent, meshComponent->globalAABB))
			{
				if (materialComponent != nullptr && materialComponent->active) {

					materialComponent->shader.UseShader(true);

					for (size_t i = 0; i < materialComponent->rTextures.size(); ++i) {

						if (materialComponent->rTextures[i]) {

							std::string samplerName = materialComponent->rTextures[i]->GetSamplerName();

							materialComponent->shader.SetSampler2D(samplerName, i);
							materialComponent->rTextures[i]->BindTexture(true, i);

						}
						
					}

					if (animationComponent != nullptr && animationComponent->active) {
						std::vector<float4x4> transforms = animationComponent->animator->GetFinalBoneMatrices();
						for (int i = 0; i < transforms.size(); i++) {
							materialComponent->shader.SetMatrix4x4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
						}
					}
					materialComponent->shader.SetShaderUniforms(&transformComponent->mGlobalMatrix, (*it)->selected);
				}

				//if ((*it)->selected && !isGame) {

				//	DrawOutline(meshComponent, transformComponent->mGlobalMatrix);

				//}
				//else {

				meshComponent->rMeshReference->Render();

				//}

				if (materialComponent != nullptr && materialComponent->active) {

					for (size_t i = 0; i < materialComponent->rTextures.size(); ++i) {

						if (materialComponent->rTextures[i]) {

							materialComponent->rTextures[i]->BindTexture(false, i);

						}

					}

					materialComponent->shader.UseShader(false);

				}

			}

		}

		if(particleComponent != nullptr && particleComponent->active)
		{
			if (External->scene->godMode || particleComponent->mOwner == External->scene->selectedGO) 
			{
				DrawParticlesShapeDebug(particleComponent);
			}
			if (particleComponent->localPlay || TimeManager::gameTimer.GetState() != TimerState::STOPPED) { DrawParticlesSystem(particleComponent); } 
		}
	}

}

bool ModuleRenderer3D::IsWalkable(float3 pointToCheck)
{
	//LineSegment walkablePoint = LineSegment(float3(pointToCheck.x, -20.0, pointToCheck.z), float3(pointToCheck.x, 20.0, pointToCheck.z));

	//float nHit = 0;
	//float fHit = 0;

	//for (std::vector<CMesh*>::iterator i = renderQueue.begin(); i != renderQueue.end(); ++i)
	//{
	//	if (walkablePoint.Intersects((*i)->globalAABB, nHit, fHit))
	//	{
	//		//walkablePoints.push_back(walkablePoint);
	//		return true;
	//	}
	//}

	//for (std::vector<CMesh*>::iterator i = renderQueuePostStencil.begin(); i != renderQueuePostStencil.end(); ++i)
	//{
	//	if (walkablePoint.Intersects((*i)->globalAABB, nHit, fHit))
	//	{
	//		//walkablePoints.push_back(walkablePoint);
	//		return true;
	//	}
	//}

	///*if (walkable)
	//{
	//	glColor3f(0.f, 1.f, 0.f);
	//	glLineWidth(2.f);
	//	glBegin(GL_LINES);
	//	glVertex3fv(&walkablePoint.a.x);
	//	glVertex3fv(&walkablePoint.b.x);
	//	glEnd();
	//	glLineWidth(1.f);
	//	glColor3f(1.f, 1.f, 1.f);
	//}
	//else
	//{
	//	glColor3f(1.f, 0.f, 0.f);
	//	glLineWidth(2.f);
	//	glBegin(GL_LINES);
	//	glVertex3fv(&walkablePoint.a.x);
	//	glVertex3fv(&walkablePoint.b.x);
	//	glEnd();
	//	glLineWidth(1.f);
	//	glColor3f(1.f, 1.f, 1.f);
	//}*/

	return false;
}

void ModuleRenderer3D::ClearModels()
{
	models.clear();
}

void ModuleRenderer3D::EnableAssimpDebugger()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
}

void ModuleRenderer3D::CleanUpAssimpDebugger()
{
	aiDetachAllLogStreams();
}

void ModuleRenderer3D::DrawDebugLines()
{
	glBegin(GL_LINES);
	for (size_t i = 0; i < lines.size(); i++)
	{
		glColor3fv(lines[i].color.ptr());
		glVertex3fv(lines[i].a.ptr());
		glVertex3fv(lines[i].b.ptr());

		glColor3f(255.f, 255.f, 255.f);
	}
	glEnd();

	lines.clear();

	glBegin(GL_TRIANGLES);
	for (size_t i = 0; i < triangles.size(); i++)
	{
		glColor3fv(triangles[i].color.ptr());

		glVertex3fv(triangles[i].a.ptr());
		glVertex3fv(triangles[i].b.ptr());
		glVertex3fv(triangles[i].c.ptr());
	}

	glColor3f(255.f, 255.f, 255.f);
	glEnd();

	triangles.clear();

	glPointSize(20.0f);
	glBegin(GL_POINTS);
	for (size_t i = 0; i < points.size(); i++)
	{
		glColor3fv(points[i].color.ptr());
		glVertex3fv(points[i].position.ptr());
		glColor3f(255.f, 255.f, 255.f);
	}
	glEnd();
	glPointSize(1.0f);

	points.clear();
#ifndef _STANDALONE
	External->pathFinding->DebugDraw();
#endif
}
void ModuleRenderer3D::AddDebugLines(float3& a, float3& b, float3& color)
{
	lines.push_back(LineRender(a, b, color));
}

void ModuleRenderer3D::AddDebugTriangles(float3& a, float3& b, float3& c, float3& color)
{
	triangles.push_back(DebugTriangle(a, b, c, color));
}
void ModuleRenderer3D::AddDebugPoints(float3& position, float3& color)
{
	points.push_back(DebugPoint(position, color));
}

void ModuleRenderer3D::DebugLine(LineSegment& line)
{
	glLineWidth(2.f);
	this->AddDebugLines(pickingDebug.a, pickingDebug.b, float3(1.f, 0.f, 0.f));
	glLineWidth(1.f);
}


void ModuleRenderer3D::AddRay(float3& a, float3& b, float3& color, float& rayWidth)
{
	rays.push_back(LineRender(a, b, color, rayWidth));
}

void ModuleRenderer3D::DrawRays()
{
	for (size_t i = 0; i < rays.size(); i++)
	{
		glLineWidth(rays[i].width);
		glBegin(GL_LINES);
		glColor3fv(rays[i].color.ptr());
		glVertex3fv(rays[i].a.ptr());
		glVertex3fv(rays[i].b.ptr());

		glColor3f(255.f, 255.f, 255.f);
		glEnd();
	}
	//rays.clear();
	glLineWidth(1.0f);
}