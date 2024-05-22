#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#include <fstream>

#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleResourceManager.h"
#include "ModulePhysics.h"
#include "ModuleMonoManager.h"
#include "ModuleLightManager.h"
#include "ModuleAudio.h"

#include "GameObject.h"
#include "G_UI.h"
#include "PhysfsEncapsule.h"
#include "External/RecastNavigation/NavMeshBuilder.h"

#include "External/SDL/include/SDL_opengl.h"

#include "External/Assimp/include/version.h"

#include "External/Optick/include/optick.h"

#include "Texture.h"
#include "ScriptEditor.h"
#include "ImGuiCustom.h"

#include "External/mmgr/mmgr.h"

// Constructor
ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	// Reserve memory for Framerate Histograms vectors
	FPSvec.reserve(30);
	DTvec.reserve(30);
	MSvec.reserve(30);

	licenseFileContents = ReadFile("../../LICENSE");
	memleaksFileContents = ReadFile("memleaks.log");
	AssimpLogFileContents = ReadFile("AssimpLog.txt");

	g = nullptr;

	LOG("Creating ModuleEditor");

}

// Destructor
ModuleEditor::~ModuleEditor()
{

}

bool ModuleEditor::Init()
{
	OPTICK_EVENT();

	LOG("Initializing editor...");

	bool ret = true;
	exit = update_status::UPDATE_CONTINUE;

	// Retrieving data from window initial status

	windowWidth = App->window->width;
	windowHeight = App->window->height;
	opacity = 1.0f;

	// Setup Dear ImGui context

	IMGUI_CHECKVERSION();

	g = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {

		UpdateAndRenderAdditionalPlatformWindows = true;

	}

	ImGuiCustom::Theme_OrangeFont();

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 15.0f);
	io.Fonts->AddFontDefault();

	// Setup Dear ImGui style

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.

	ImGuiStyle& style = ImGui::GetStyle();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	// Gizmo Handlers

	gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	gizmoMode = ImGuizmo::MODE::WORLD;
	modelMatrix = float4x4::identity;

	// Assets folder
	currentDir = "Assets";

	// Node Editor

	nodeEditor.Create();

	// Shader Editor

	shaderEditor.Init();

	// Editor Icons Textures

	folderIcon.LoadEngineIconTexture("Assets/Editor/folder.dds");
	fileIcon.LoadEngineIconTexture("Assets/Editor/files.dds");
	imageIcon.LoadEngineIconTexture("Assets/Editor/image.dds");
	modelIcon.LoadEngineIconTexture("Assets/Editor/model.dds");
	shaderIcon.LoadEngineIconTexture("Assets/Editor/shader.dds");
	sceneIcon.LoadEngineIconTexture("Assets/Editor/scene2.dds");
	prefabIcon.LoadEngineIconTexture("Assets/Editor/prefab.dds");
	animIcon.LoadEngineIconTexture("Assets/Editor/animation.dds");
	fontIcon.LoadEngineIconTexture("Assets/Editor/font.dds");

	scriptEditor = new ScriptEditor();
	scriptEditor->LoadScriptTXT("../Game/Assets/Scripts/Core.cs");

	//#ifdef _STANDALONE
	//
	//	TimeManager::gameTimer.Start();
	//
	//#endif // _STANDALONE

	return ret;
}

update_status ModuleEditor::Update(float dt)
{
	OPTICK_EVENT();

	return exit;
}

void ModuleEditor::DrawEditor()
{
	OPTICK_EVENT();

	// Start the Dear ImGui frame

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// --------------------------------- Here starts the code for the editor ----------------------------------------

	// Manages the docking functionality with the main window
	WindowDockSpaceManagement();

	// MAIN MENU BAR START

	if (ImGui::BeginMainMenuBar()) {

		if (ImGui::BeginMenu("File")) {

			ImGui::SeparatorText("Scene");

			if (ImGui::MenuItem("New Scene")) {



			}

			if (ImGui::MenuItem("Open Scene")) {



			}

			ImGui::SeparatorText("Save");


			if (ImGui::MenuItem("Save", "Ctrl+S"))
			{
				App->scene->SaveScene(App->scene->currentSceneDir, App->scene->currentSceneFile);
			}

			if (ImGui::MenuItem("Save As..."))
			{
				showSaveAs = true;
			}

			ImGui::SeparatorText("Project");

			if (ImGui::MenuItem("New Project")) {



			}

			if (ImGui::MenuItem("Open Project")) {



			}

			ImGui::Separator();
			if (ImGui::BeginMenu("Theme"))
			{
				if (ImGui::MenuItem("Classic"))
				{
					ImGui::StyleColorsClassic();
				}
				if (ImGui::MenuItem("Light (please don't)"))
				{
					ImGui::StyleColorsLight();
				}
				if (ImGui::MenuItem("Dark"))
				{
					ImGui::StyleColorsDark();
				}

				if (ImGui::MenuItem("Orange Font"))
				{
					ImGuiCustom::Theme_OrangeFont();
				}
				ImGui::EndMenu();
			}

			ImGui::SeparatorText("Exit");

			if (ImGui::MenuItem("Exit")) {

				// TODO: Sara --> make a popup to ask to save
				exit = update_status::UPDATE_STOP;

			}

			ImGui::EndMenu();

		}

		if (ImGui::BeginMenu("Edit")) {

			ImGui::SeparatorText("Editor");

			if (ImGui::MenuItem("Save editor configuration")) {



			}

			if (ImGui::MenuItem("Load editor configuration")) {



			}

			ImGui::SeparatorText("Other");

			if (ImGui::MenuItem("Preferences")) {



			}

			ImGui::EndMenu();

		}

		if (ImGui::BeginMenu("View")) {



			ImGui::EndMenu();

		}

		if (ImGui::BeginMenu("GameObject")) {

			if (ImGui::MenuItem("Empty")) {

				GameObject* empty = App->scene->CreateGameObject("Empty", App->scene->mRootNode);

			}

			PrimitivesMenu();

			CreateCameraMenu();

			UIMenu();

			LightsMenu();

			CreateParticleSystemMenu();

			ImGui::Separator();

			if (ImGui::MenuItem("Clear Scene")) {

				uint deletedSceneUID = App->scene->mRootNode->UID;

				App->scene->ClearScene();

				App->scene->mRootNode = App->scene->CreateGameObject("Scene", nullptr); // Recreate scene
				App->scene->mRootNode->UID = deletedSceneUID;

				LOG("Scene cleared successfully");

			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows")) {

			if (ImGui::MenuItem("Application")) {

				showApplication = true;

			}

			if (ImGui::MenuItem("Console")) {

				showConsole = true;

			}

			if (ImGui::MenuItem("Memory Leaks")) {

				showMemoryLeaks = true;



			}

			if (ImGui::MenuItem("Assimp Log")) {

				showAssimpLog = true;

			}

			if (ImGui::MenuItem("Hierarchy")) {

				showHierarchy = true;

			}

			if (ImGui::MenuItem("Inspector")) {

				showInspector = true;

			}

			if (ImGui::MenuItem("Navigation")) {

				showNavMesh = true;

			}

			if (ImGui::MenuItem("Scene")) {

				showScene = true;

			}

			if (ImGui::MenuItem("Game")) {

				showGame = true;

			}

			if (ImGui::MenuItem("Resources")) {

				showResources = true;

			}

			if (ImGui::MenuItem("File Explorer")) {

				showFileExplorer = true;

			}

			if (ImGui::MenuItem("Assets")) {

				showAssets = true;

			}

			if (ImGui::MenuItem("Library")) {

				showLibrary = true;

			}

			if (ImGui::MenuItem("Node Editor")) {

				showNodeEditor = true;

			}

			if (ImGui::MenuItem("Shader Editor")) {

				showShaderEditor = true;

			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help")) {

			if (ImGui::MenuItem("About")) {

				showAboutPopUp = true;

			}

			if (ImGui::MenuItem("Repository")) {

				RequestBrowser("https://github.com/francesctr4/Ymir-Engine");

			}

			if (ImGui::MenuItem("Documentation")) {



			}

			if (ImGui::MenuItem("Releases")) {



			}

			if (ImGui::MenuItem("Bug report")) {



			}

			ImGui::EndMenu();

		}

		ImGui::EndMainMenuBar();

	}

	SaveAs();
	RenderSaveAsPrefabPopUp();

	if (showAboutPopUp) {

		ImGui::OpenPopup("About");

		if (ImGui::BeginPopupModal("About")) {

			AboutModalWindowContent();

			ImGui::EndPopup();

		}

	}

	if (showNewScriptPopUp) {

		ImGui::OpenPopup("New Script");

		ImGui::SetNextWindowSize(ImVec2(300.0f, 100.0f), ImGuiCond_Appearing);
		if (ImGui::BeginPopupModal("New Script")) {

			scriptEditor->ShowNewScriptDialogue();

			ImGui::EndPopup();

		}

	}

	// END OF MAIN MENU BAR

	// APPLICATION MENU START

	if (showApplication) {

		if (ImGui::Begin("Application", &showApplication), true) {

			if (ImGui::CollapsingHeader("OpenGL")) {

				// Functions to enable/disable opengl config with checkboxes

				ImGui::Indent(); // Indent to make the checkbox visually nested under the header

				if (ImGui::BeginTable("split", 2))
				{
					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Depth Testing", &gl_DepthTesting)) {

						Toggle_GL_DepthTesting(gl_DepthTesting);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Face Culling", &gl_FaceCulling)) {

						Toggle_GL_FaceCulling(gl_FaceCulling);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Lighting", &gl_Lighting)) {

						Toggle_GL_Lighting(gl_Lighting);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Color Material", &gl_ColorMaterial)) {

						Toggle_GL_ColorMaterial(gl_ColorMaterial);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Texture Mapping 2D", &gl_TextureMapping2D)) {

						Toggle_GL_TextureMapping2D(gl_TextureMapping2D);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Texture Mapping 3D", &gl_TextureMapping3D)) {

						Toggle_GL_TextureMapping3D(gl_TextureMapping3D);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Blending", &gl_Blending)) {

						Toggle_GL_Blending(gl_Blending);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("MSAA", &gl_MSAA)) {

						Toggle_GL_MSAA(gl_MSAA);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Stencil Testing", &gl_StencilTesting)) {

						Toggle_GL_StencilTesting(gl_StencilTesting);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Scissor Testing", &gl_ScissorTesting)) {

						Toggle_GL_ScissorTesting(gl_ScissorTesting);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Alpha Testing", &gl_AlphaTesting)) {

						Toggle_GL_AlphaTesting(gl_AlphaTesting);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Point Sprites", &gl_PointSprites)) {

						Toggle_GL_PointSprites(gl_PointSprites);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Fog", &gl_Fog)) {

						Toggle_GL_Fog(gl_Fog);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Point Smooth", &gl_PointSmooth)) {

						Toggle_GL_PointSmooth(gl_PointSmooth);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Line Smooth", &gl_LineSmooth)) {

						Toggle_GL_LineSmooth(gl_LineSmooth);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Auto Normalization", &gl_Normalization)) {

						Toggle_GL_Normalization(gl_Normalization);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Polygon Offset", &gl_PolygonOffset)) {

						Toggle_GL_PolygonOffset(gl_PolygonOffset);

					}

					ImGui::TableNextColumn();

					if (ImGui::Checkbox("Wireframe Mode", &gl_WireframeMode)) {

						Toggle_GL_WireframeMode(gl_WireframeMode);

					}

					ImGui::EndTable();

				}

				ImGui::Unindent(); // Unindent to return to the previous level of indentation

			}

			if (ImGui::CollapsingHeader("Window")) {

				// Window Options

				ImGui::Indent(); // Indent to make the checkbox visually nested under the header

				// Width and Height Sliders
				ImGui::SliderInt("Width", &windowWidth, 0, 1280);
				ImGui::SliderInt("Height", &windowHeight, 0, 1024);
				SDL_SetWindowSize(App->window->window, windowWidth, windowHeight);

				// Opacity Slider
				ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f);
				SDL_SetWindowOpacity(App->window->window, opacity);

				// Window Options Checkbox
				if (ImGui::Checkbox("Fullscreen", &fullscreen)) {

					ToggleFullscreen(fullscreen);

				}
				ImGui::SameLine();
				if (ImGui::Checkbox("Resizable", &resizable)) {

					ToggleResizable(resizable);

				}

				if (ImGui::Checkbox("Borderless", &borderless)) {

					ToggleBorderless(borderless);

				}
				ImGui::SameLine();
				if (ImGui::Checkbox("Fullscreen Desktop", &fullscreenDesktop)) {

					ToggleFullscreenDesktop(fullscreenDesktop);

				}

				ImGui::Unindent(); // Unindent to return to the previous level of indentation

			}

			if (ImGui::CollapsingHeader("Renderer3D")) {

				ImGui::Indent(); // Indent to make the checkbox visually nested under the header

				if (ImGui::Checkbox("VSync", &vsync)) {

					ToggleVSync(vsync);

				}

				ImGui::Unindent(); // Unindent to return to the previous level of indentation

			}

			if (ImGui::CollapsingHeader("Camera3D")) {

				// Camera Options

				ImGui::SeparatorText("POSITION");

				ImGui::BulletText("Camera Position: (%.2f, %.2f, %.2f)", App->camera->editorCamera->GetPos().x, App->camera->editorCamera->GetPos().y, App->camera->editorCamera->GetPos().z);

				ImGui::SeparatorText("ORIENTATION");

				ImGui::BulletText("Camera X: (%.2f, %.2f, %.2f)", App->camera->editorCamera->GetRight().x, App->camera->editorCamera->GetRight().y, App->camera->editorCamera->GetRight().z);
				ImGui::BulletText("Camera Y: (%.2f, %.2f, %.2f)", App->camera->editorCamera->GetUp().x, App->camera->editorCamera->GetUp().y, App->camera->editorCamera->GetUp().z);
				ImGui::BulletText("Camera Z: (%.2f, %.2f, %.2f)", App->camera->editorCamera->GetFront().x, App->camera->editorCamera->GetFront().y, App->camera->editorCamera->GetFront().z);

			}

			if (ImGui::CollapsingHeader("Input")) {

				// Input Options

				ImGuiIO& io = ImGui::GetIO();

				// Mouse Info

				ImGui::SeparatorText("MOUSE");

				if (ImGui::IsMousePosValid()) {

					ImGui::BulletText("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);

				}
				else {

					ImGui::BulletText("Mouse pos: <INVALID>");

				}

				ImGui::BulletText("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
				ImGui::BulletText("Mouse down:");

				for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDown(i)) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
				ImGui::BulletText("Mouse wheel: %.1f", io.MouseWheel);

				// Keys info

				ImGui::SeparatorText("KEYS");

				struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; // Hide Native<>ImGuiKey duplicates when both exists in the array
				ImGuiKey start_key = (ImGuiKey)0;

				ImGui::BulletText("Keys down:");         for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) { if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue; ImGui::SameLine(); ImGui::Text((key < ImGuiKey_NamedKey_BEGIN) ? "\"%s\"" : "\"%s\" %d", ImGui::GetKeyName(key), key); }
				ImGui::BulletText("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
				ImGui::BulletText("Chars queue:");       for (int i = 0; i < io.InputQueueCharacters.Size; i++) { ImWchar c = io.InputQueueCharacters[i]; ImGui::SameLine();  ImGui::Text("\'%c\' (0x%04X)", (c > ' ' && c <= 255) ? (char)c : '?', c); } // FIXME: We should convert 'c' to UTF-8 here but the functions are not public.

			}

			if (ImGui::CollapsingHeader("Editor")) {

				ImGui::Indent(); // Indent to make the checkbox visually nested under the header

				// Light/Dark Mode Checkbox
				if (ImGui::Checkbox("Toggle light mode", &lightMode)) {

					ToggleLightMode(lightMode);

				}

				// ImGui Demo Window Checkbox
				if (ImGui::Checkbox("Show ImGui demo window", &showImGuiDemo));

				// World Grid Checkbox
				if (ImGui::Checkbox("Show Grid", &App->renderer3D->showGrid));

				// ImGui checkbox to manage whether to ignore .meta files or not
				if (ImGui::Checkbox("Ignore .meta files", &shouldIgnoreMeta));

				ImGui::Unindent(); // Unindent to return to the previous level of indentation

			}

			if (ImGui::CollapsingHeader("Framerate")) {

				// FPS Graph

				char title[50];

				sprintf_s(title, 50, "Framerate (FPS): %.3f", FPSvec[FPSvec.size() - 1]);
				ImGui::PlotHistogram("## Framerate", &FPSvec[0], FPSvec.size(), 0, title, 0.0f, 250.0f, ImVec2(300, 100));

				sprintf_s(title, 50, "DeltaTime (DT): %.3f", DTvec[DTvec.size() - 1]);
				ImGui::PlotHistogram("## DeltaTime", &DTvec[0], DTvec.size(), 0, title, 0.0f, 0.032f, ImVec2(300, 100));

				sprintf_s(title, 50, "Milliseconds (MS): %.3f", MSvec[MSvec.size() - 1]);
				ImGui::PlotHistogram("## Milliseconds", &MSvec[0], MSvec.size(), 0, title, 0.0f, 32.0f, ImVec2(300, 100));

			}

			if (ImGui::CollapsingHeader("Hardware")) {

				// Hardware Detection

				ShowPlatformInfo();

				ImGui::Separator();

				ShowCPUInfo();

				ImGui::Separator();

				ShowRAMInfo();

				ImGui::Separator();

				ShowGPUInfo();

				ImGui::Separator();

				ShowDiskInfo();

			}

			if (ImGui::CollapsingHeader("Meshes")) {

				// 3D Meshes Configuration

				ImGui::Indent(); // Indent to make the checkbox visually nested under the header

				if (ImGui::Checkbox("Show Vertex Normals", &showVertexNormals)) {

					ToggleMeshesVertexNormals(showVertexNormals);

				}

				if (ImGui::Checkbox("Show Face Normals", &showFaceNormals)) {

					ToggleMeshesFaceNormals(showFaceNormals);

				}

				//if (ImGui::Button("Apply Checker Texture")) {

				//	App->renderer3D->ApplyCheckerTexture();

				//}

				if (ImGui::Button("Clear Actual Texture")) {

					App->renderer3D->ClearActualTexture();

				}

				ImGui::Unindent(); // Unindent to return to the previous level of indentation

			}

			if (ImGui::CollapsingHeader("Physics"))
			{
				btVector3 auxGravity = App->physics->GetWorldGravity();
				bool auxDebugDrawScene = App->physics->debugScene;
				bool auxDebugDrawGame = App->physics->debugGame;
				int auxLineWidth = App->physics->shapeLineWidth;

				ImVec4 auxColliderColor = ImVec4(
					App->physics->colliderColor.r,
					App->physics->colliderColor.g,
					App->physics->colliderColor.b,
					App->physics->colliderColor.a
				);

				ImVec4 auxSensorColor = ImVec4(
					App->physics->sensorColor.r,
					App->physics->sensorColor.g,
					App->physics->sensorColor.b,
					App->physics->sensorColor.a
				);

				ImGui::SeparatorText("Gravity");

				ImGui::Indent();

				if (ImGui::DragFloat3("##Gravity", auxGravity))
				{
					App->physics->SetWorldGravity(auxGravity);
				}
				if (ImGui::Button("Reset Gravity"))
				{
					App->physics->ResetGravity();
				}

				ImGui::Unindent();
				ImGui::SeparatorText("Drawer");
				ImGui::Indent();


				ImGui::Text("Draw Scene"); ImGui::SameLine();
				if (ImGui::Checkbox("##DrawScene", &auxDebugDrawScene))
				{
					App->physics->debugScene = auxDebugDrawScene;
				}

				ImGui::Text("Draw Game"); ImGui::SameLine();
				if (ImGui::Checkbox("##DrawGame", &auxDebugDrawGame))
				{
					App->physics->debugGame = auxDebugDrawGame;
				}

				ImGui::Unindent();
				ImGui::SeparatorText("Customization");
				ImGui::Indent();

				ImGui::Text("Collider Color"); ImGui::SameLine();

				// Mostrar el boton de color personalizado
				if (ImGui::ColorButton("##ColliderColorButton", auxColliderColor))
				{
					ImGui::OpenPopup("ColliderColorPicker");
				}

				if (ImGui::BeginPopup("ColliderColorPicker"))
				{
					ImGui::ColorPicker4("Color", (float*)&auxColliderColor);
					App->physics->SetColliderColor(Color(auxColliderColor.x, auxColliderColor.y, auxColliderColor.z, auxColliderColor.w));
					ImGui::EndPopup();
				}

				ImGui::Text("Sensor Color"); ImGui::SameLine();

				// Mostrar el boton de color personalizado
				if (ImGui::ColorButton("##SensorColorButton", auxSensorColor))
				{
					ImGui::OpenPopup("SensorColorPicker");
				}

				if (ImGui::BeginPopup("SensorColorPicker"))
				{
					ImGui::ColorPicker4("Color", (float*)&auxSensorColor);
					App->physics->SetSensorColor(Color(auxSensorColor.x, auxSensorColor.y, auxSensorColor.z, auxSensorColor.w));
					ImGui::EndPopup();
				}

				ImGui::Text("Shape line width: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f);
				if (ImGui::SliderInt("##Shape line width", &auxLineWidth, 0, 10))
				{
					App->physics->SetLineWidth(auxLineWidth);
				}

				ImGui::Unindent();
			}
			ImGui::End();
		}
	}

	if (showConsole) {

		RedirectLogOutput();
	}

	if (showMemoryLeaks) {

		if (ImGui::Begin("Memory Leaks", &showMemoryLeaks), true) {

			// Show Memory Leaks File

			MemoryLeaksOutput();

			ImGui::End();
		}
	}

	if (showAssimpLog) {

		if (ImGui::Begin("Assimp Log", &showAssimpLog), true) {

			// Show Assimp Log File

			AssimpLogOutput();

			ImGui::End();
		}
	}

	if (showHierarchy) {

		if (ImGui::Begin("Hierarchy", &showHierarchy), true) {

			// Show scene hierarchy

			DrawHierarchy();

			ImGui::End();
		}
	}

	if (showInspector) {

		if (ImGui::Begin("Inspector", &showInspector, ImGuiWindowFlags_MenuBar), true) {

			if (ImGui::BeginMenuBar())
			{
				ImGui::Checkbox("Lock", &App->scene->isLocked);
			}
			ImGui::EndMenuBar();

			// Show GameObject Inspector

			DrawInspector();

			ImGui::End();

		}

	}

	if (showNavMesh) {


		if (ImGui::Begin("Navigation", &showNavMesh), true)
		{

			DrawBakingTab();;
		}
		ImGui::End();


	}

	// END OF APPLICATION MENU

	if (showImGuiDemo) {

		ImGui::ShowDemoWindow();

	}


	// END OF APPLICATION MENU

	// Time Management

	if (ImGui::Begin(" ", NULL, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {

		float windowWidth = ImGui::GetWindowWidth();
		float windowHeight = ImGui::GetWindowHeight();

		float buttonWidth = 50.0f;
		float posX = (windowWidth - (3 * buttonWidth + 2 * ImGui::GetStyle().ItemSpacing.x)) * 0.5f;

		float posY = 5.0f;

		ImGui::SameLine(20.0f);
		ImGui::SetCursorPosY(posY);

		ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "Graphics Time: %.3f", TimeManager::graphicsTimer.ReadSec());

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);
		ImGui::SetCursorPosY(posY);

		ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "Frame Count: %d", TimeManager::FrameCount);

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);
		ImGui::SetCursorPosY(posY);

		ImGui::PushItemWidth(100.0f);

		static float targetFPS = App->GetTargetFPS();

		if (ImGui::SliderFloat("Target FPS", &targetFPS, 30.0f, 144.0f, "%1.0f"))
		{
			App->SetTargetFPS(targetFPS);
		}

		ImGui::SameLine();
		ImGui::SetCursorPosY(posY);

		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Reset##1"))
		{
			targetFPS = 60.0f;
			App->SetTargetFPS(targetFPS);
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);
		ImGui::SetCursorPosY(posY);

		ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "FPS: %.2f", 1 / TimeManager::DeltaTime);

		ImGui::SetCursorPosX(posX);
		ImGui::SetCursorPosY(posY);

#ifdef _STANDALONE
		static bool isPlaying = true;
#else
		static bool isPlaying = false;
#endif // _STANDALONE

		static bool isPaused = false;

		if (isPlaying) {

			if (ImGui::Button("Stop")) {

				App->audio->StopAllSounds();
				TimeManager::gameTimer.Stop();

				isPlaying = false;
				isPaused = false;

				App->scene->LoadScene();
			}

		}
		else {

			if (ImGui::Button("Play")) {

				TimeManager::gameTimer.Start();

				isPlaying = true;

				App->scene->SaveScene();
			}

		}

		ImGui::SameLine();

		if (isPaused) {

			if (ImGui::Button("Resume")) {

				if (isPlaying) {

					TimeManager::gameTimer.Resume();

					isPaused = false;

				}

			}

		}
		else {

			if (ImGui::Button("Pause")) {

				if (isPlaying) {

					TimeManager::gameTimer.Pause();

					isPaused = true;

				}

			}

		}

		ImGui::SameLine();

		if (ImGui::Button("Step")) {

			TimeManager::gameTimer.StepFrame(TimeManager::DeltaTime);

		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);

		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Game Time: %.3f", TimeManager::gameTimer.ReadSec());

		ImGui::SameLine(ImGui::GetWindowWidth() - 250.0f);

		ImGui::PushItemWidth(100.0f);

		static float scale = 1.0f; // Default Scale
		if (ImGui::SliderFloat("Time Scale", &scale, 0.1f, 5.0f, "%.2f"))
		{
			TimeManager::gameTimer.SetTimeScale(scale);
		}

		ImGui::PushItemWidth(-1);

		ImGui::SameLine();

		if (ImGui::Button("Reset##2"))
		{
			scale = 1.0f;
			TimeManager::gameTimer.SetTimeScale(scale);
		}

		ImGui::End();
	}

	if (showAssets) {

		if (ImGui::Begin("Assets", &showAssets, ImGuiWindowFlags_MenuBar), true) {

			DrawAssetsWindow((currentDir + "/").c_str());

			// Display the modal when showModal is true
			if (showModal) {

				ImGui::OpenPopup(selectedFilePath.c_str());

				showModal = false;  // Reset the flag

			}

			// Modal window for displaying file contents
			if (ImGui::BeginPopupModal(selectedFilePath.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

				// Read and display the contents of the selected file

				std::ifstream file(selectedFilePath);

				if (file.is_open()) {

					std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

					ImGui::Text("%s", fileContents.c_str());

					file.close();

				}

				// Close the modal window

				if (ImGui::Button("Close")) {

					ImGui::CloseCurrentPopup();

				}

				ImGui::EndPopup();
			}

			ImGui::End();
		}

	}

	if (showLibrary) {

		if (ImGui::Begin("Library", &showLibrary), true) {

			DrawLibraryWindow("Library/");

			std::string completeFilePath = selectedFilePath.c_str();
			std::string shortenedFilePath;

			// Find the position of the first "/" in the string
			size_t found = completeFilePath.find("/");

			if (found != std::string::npos) {

				shortenedFilePath = selectedFilePath.substr(found + 1);

			}
			else {

				shortenedFilePath = completeFilePath;

			}

			// Display the modal when showModal is true
			if (showModal) {

				ImGui::OpenPopup(shortenedFilePath.c_str());
				showModal = false;  // Reset the flag

			}

			// Modal window for displaying file contents

			if (ImGui::BeginPopupModal(shortenedFilePath.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

				// Read and display the contents of the selected file

				std::ifstream file(selectedFilePath);

				if (file.is_open()) {

					std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

					ImGui::Text("%s", fileContents.c_str());

					file.close();
				}

				// Close the modal window
				if (ImGui::Button("Close")) {

					ImGui::CloseCurrentPopup();

				}

				ImGui::EndPopup();
			}

			ImGui::End();
		}

	}

	if (showFileExplorer) {

		if (ImGui::Begin("File Explorer", &showFileExplorer), true) {

			DrawFileExplorer(".");

			ImGui::End();
		}

	}

	if (showResources) {

		if (ImGui::Begin("Resources", &showResources), true) {

			ImGui::TextColored(ImVec4(1.f, 0.5f, 0.5f, 1.f), "Total Resources Loaded: %d", App->resourceManager->GetResourcesMap().size());

			for (const auto& [UID, Resource] : App->resourceManager->GetResourcesMap())
			{
				ImGui::Text("Type: %s | UID: %d | References: %d",
					App->resourceManager->GetStringFromType(Resource->GetType()).c_str(),
					Resource->GetUID(),
					Resource->GetReferenceCount());
			}

			ImGui::End();
		}

	}

	if (showNodeEditor) {

		if (ImGui::Begin("Node Editor", &showNodeEditor), true) {

			nodeEditor.Update();

			ImVec2 gameViewPos = ImGui::GetWindowPos();

			mouse.x = (ImGui::GetMousePos().x - gameViewPos.x) / gameViewSize.x;
			mouse.y = (ImGui::GetMousePos().y - gameViewPos.y) / gameViewSize.y;

			if (App->scene->gameCameraComponent != nullptr)
			{
				// Display the contents of the framebuffer texture
				gameViewSize = ImGui::GetContentRegionAvail();
				App->scene->gameCameraComponent->SetAspectRatio(gameViewSize.x / gameViewSize.y);
				ImGui::Image((ImTextureID)App->scene->gameCameraComponent->framebuffer.TCB, gameViewSize, ImVec2(0, 1), ImVec2(1, 0));
			}
			ImGui::End();
		}
	}

	if (showShaderEditor) {

		if (ImGui::Begin("Shader Editor", &showShaderEditor), true) {

			shaderEditor.Update();

			ImGui::End();
		}
	}
	if (showScriptingEditor) {

		if (ImGui::Begin("Script Editor", &showScriptingEditor), true) {

			scriptEditor->Draw();

			ImGui::End();

		}

	}

	if (showScene) {

		if (ImGui::Begin("Scene", &showScene), true) {

			// Display the contents of the framebuffer texture
			ImVec2 size = ImGui::GetContentRegionAvail();
			App->camera->editorCamera->SetAspectRatio(size.x / size.y);
			ImGui::Image((ImTextureID)App->camera->editorCamera->framebuffer.TCB, size, ImVec2(0, 1), ImVec2(1, 0));

			(ImGui::IsWindowHovered()) ? App->camera->hoveringEditor = true : App->camera->hoveringEditor = false;

			if (App->camera->hoveringEditor)
			{
				App->camera->CameraInput();
			}
			// Retrieve Info from ImGui Scene Window

			// Get the Mouse Position using ImGui.
			ImVec2 mousePosition = ImGui::GetMousePos();

			// Get the position of the ImGui window.
			ImVec2 sceneWindowPos = ImGui::GetWindowPos();

			// Get the size of the ImGui window.
			ImVec2 sceneWindowSize = ImGui::GetWindowSize();

			// Get the maximum content region size of the ImGui window.
			ImVec2 sceneContentRegionMax = ImGui::GetContentRegionMax();

			// Calculate the vertical offset for the gizmo to be centered in the ImGui window frame.
			float sceneFrameHeightOffset = ImGui::GetFrameHeight() / 2.0f;

			// Gizmo Management
			DrawGizmo(sceneWindowPos, sceneContentRegionMax, sceneFrameHeightOffset);

			// Mouse Picking Management

#ifndef _STANDALONE
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT && !ImGuizmo::IsUsing())
			{
				if (ImGui::IsWindowHovered())
				{
					if (!ImGuizmo::IsUsing() && !App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN)
					{


						App->camera->mousePickingRay.Transform(App->pathFinding->matrizglobal.Inverted());

						if (External->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
							External->pathFinding->CheckNavMeshIntersection(App->camera->mousePickingRay, SDL_BUTTON_LEFT);

					}
				}
			}

			if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT && !ImGuizmo::IsUsing())
			{
				if (ImGui::IsWindowHovered())
				{
					if (!ImGuizmo::IsUsing() && !App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN)
					{

						App->camera->mousePickingRay.Transform(App->pathFinding->matrizglobal.Inverted());


						if (External->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
							External->pathFinding->CheckNavMeshIntersection(App->camera->mousePickingRay, SDL_BUTTON_RIGHT);

					}
				}
			}

#endif // !STANDALONE



			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT && !ImGuizmo::IsUsing())
			{
				MousePickingManagement(mousePosition, sceneWindowPos, sceneWindowSize, sceneFrameHeightOffset);
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("tex"))
				{
					std::string assetsFilePathDrop = (const char*)payload->Data;
					if (assetsFilePathDrop.find(".png") != std::string::npos) {

						assetsFilePathDrop.erase(assetsFilePathDrop.find(".png") + 4);

						LOG("File path: %s", assetsFilePathDrop.c_str());

						App->resourceManager->ImportFile(assetsFilePathDrop);
					}
				}

				else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("asset"))
				{
					std::string assetsFilePathDrop = (const char*)payload->Data;

					if (assetsFilePathDrop.find(".fbx") != std::string::npos) {

						assetsFilePathDrop.erase(assetsFilePathDrop.find(".fbx") + 4);

						LOG("File path: %s", assetsFilePathDrop.c_str());

						App->resourceManager->ImportFile(assetsFilePathDrop);

					}

					else if (assetsFilePathDrop.find(".yscene") != std::string::npos) {

						assetsFilePathDrop.erase(assetsFilePathDrop.find(".yscene") + 7);

						LOG("File path: %s", assetsFilePathDrop.c_str());

						std::string path, name;
						PhysfsEncapsule::SplitFilePath(assetsFilePathDrop.c_str(), &path, &name);

						App->scene->LoadScene(path, name);

					}

					else if (assetsFilePathDrop.find(".yfab") != std::string::npos) {

						assetsFilePathDrop.erase(assetsFilePathDrop.find(".yfab") + 5);

						LOG("File path: %s", assetsFilePathDrop.c_str());

						std::string path, name;
						PhysfsEncapsule::SplitFilePath(assetsFilePathDrop.c_str(), &path, &name);

						App->scene->LoadPrefab(path, name);

					}

				}

				ImGui::EndDragDropTarget();

			}

			ImGui::End();
		}

	}

	if (showGame) {

		if (ImGui::Begin("Game", &showGame), true) {

			gameViewPos = ImGui::GetWindowPos();

			mouse.x = (ImGui::GetMousePos().x - gameViewPos.x) / gameViewSize.x;
			mouse.y = (ImGui::GetMousePos().y - gameViewPos.y) / gameViewSize.y;

			if (App->scene->gameCameraComponent != nullptr)
			{
				// Display the contents of the framebuffer texture
				gameViewPos = ImGui::GetWindowPos();
				gameViewSize = ImGui::GetContentRegionAvail();
				App->scene->gameCameraComponent->SetAspectRatio(gameViewSize.x / gameViewSize.y);
				ImGui::Image((ImTextureID)App->scene->gameCameraComponent->framebuffer.TCB, gameViewSize, ImVec2(0, 1), ImVec2(1, 0));
			}

			ImGui::End();
		}

	}

	RenderDeleteAssetConfirmationPopup();

	// --------------------------------- Here finishes the code for the editor ----------------------------------------

	// Rendering

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	   // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	   //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)

	if (UpdateAndRenderAdditionalPlatformWindows) {

		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);

	}

}

ImVec2 ModuleEditor::NormalizeOnSceneWindow(float x, float y, float z, float h, float w, ImVec2 point) {

	ImVec2 normalizePoint;

	normalizePoint.x = (point.x - x) / ((x + w) - x);
	normalizePoint.y = (point.y - y) / ((y + h) - y);

	return normalizePoint;

}

void ModuleEditor::PrimitivesMenu()
{
	if (ImGui::BeginMenu("3D Primitives")) {

		if (ImGui::MenuItem("Plane")) {

			App->resourceManager->ImportFile("Assets/Primitives/Plane.fbx");

			LOG("Plane created successfully");

		}

		if (ImGui::MenuItem("Cube")) {

			App->resourceManager->ImportFile("Assets/Primitives/Cube.fbx");

			LOG("Cube created successfully");

		}

		if (ImGui::MenuItem("Pyramid")) {

			App->resourceManager->ImportFile("Assets/Primitives/Pyramid.fbx");

			LOG("Pyramid created successfully");

		}

		if (ImGui::MenuItem("Cylinder")) {

			App->resourceManager->ImportFile("Assets/Primitives/Cylinder.fbx");

			LOG("Cylinder created successfully");

		}

		if (ImGui::MenuItem("Cone")) {

			App->resourceManager->ImportFile("Assets/Primitives/Cone.fbx");

			LOG("Cone created successfully");

		}

		if (ImGui::MenuItem("Sphere")) {

			App->resourceManager->ImportFile("Assets/Primitives/Sphere.fbx");

			LOG("Sphere created successfully");

		}

		if (ImGui::MenuItem("Torus")) {

			App->resourceManager->ImportFile("Assets/Primitives/Torus.fbx");

			LOG("Torus created successfully");

		}

		if (ImGui::MenuItem("Capsule")) {

			App->resourceManager->ImportFile("Assets/Primitives/Capsule.fbx");

			LOG("Capsule created successfully");

		}

		if (ImGui::MenuItem("Disc")) {

			App->resourceManager->ImportFile("Assets/Primitives/Disc.fbx");

			LOG("Disc created successfully");

		}

		if (ImGui::MenuItem("Platonic Solid")) {

			App->resourceManager->ImportFile("Assets/Primitives/PlatonicSolid.fbx");

			LOG("Platonic Solid created successfully");

		}

		if (ImGui::MenuItem("Prism")) {

			App->resourceManager->ImportFile("Assets/Primitives/Prism.fbx");

			LOG("Prism created successfully");

		}

		if (ImGui::MenuItem("Pipe")) {

			App->resourceManager->ImportFile("Assets/Primitives/Pipe.fbx");

			LOG("Pipe created successfully");

		}

		if (ImGui::MenuItem("Helix")) {

			App->resourceManager->ImportFile("Assets/Primitives/Helix.fbx");

			LOG("Helix created successfully");

		}

		ImGui::EndMenu();

	}

}

void ModuleEditor::CreateCameraMenu()
{
	if (ImGui::MenuItem("Camera")) {

		GameObject* empty = App->scene->CreateGameObject("Camera", App->scene->mRootNode);
		empty->UID = Random::Generate();

		empty->AddComponent(ComponentType::CAMERA);
	}
}

void ModuleEditor::LightsMenu()
{
	if (ImGui::BeginMenu("Light"))
	{

		if (ImGui::MenuItem("Point Light"))
		{
			App->lightManager->CreateLight(LightType::POINT_LIGHT);
		}
		if (ImGui::MenuItem("Directional Light"))
		{
			App->lightManager->CreateLight(LightType::DIRECTIONAL_LIGHT);
		}
		if (ImGui::MenuItem("Spot Light"))
		{
			App->lightManager->CreateLight(LightType::SPOT_LIGHT);
		}
		if (ImGui::MenuItem("Area Light"))
		{
			App->lightManager->CreateLight(LightType::AREA_LIGHT);
		}

		ImGui::EndMenu();

	}

}

void ModuleEditor::UIMenu()
{
	if (ImGui::BeginMenu("UI"))
	{
		std::array<std::string, 7> ui = { "Canvas", "Image", "Text", "Button", "Input Box", "Checkbox", "Slider" };

		for (int i = 0; i < ui.size(); i++)
		{
			if (ImGui::MenuItem(ui[i].c_str()))
			{
				App->scene->CreateGUI((UI_TYPE)i);
				break;
			}
		}
		ImGui::EndMenu();
	}
}

void ModuleEditor::CreateParticleSystemMenu()
{
	if (ImGui::MenuItem("Particle System")) {

		GameObject* empty = App->scene->CreateGameObject("Particle System", App->scene->selectedGO != nullptr ? App->scene->selectedGO : App->scene->mRootNode);
		empty->UID = Random::Generate();

		//TONI: We dont need material in Particle System
		//empty->AddComponent(ComponentType::MATERIAL);
		empty->AddComponent(ComponentType::PARTICLE);
	}
}

void ModuleEditor::SaveAs()
{
	if (showSaveAs)
	{
		ImGui::OpenPopup("Save As");
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Save As", &showSaveAs, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static std::string sceneName_saveAS = "New Scene";
		ImGui::Text("Scene will be saved in %s as '%s'", currentDir.c_str(), sceneName_saveAS.c_str());
		ImGui::Separator();

		ImGui::InputText("File Name", &sceneName_saveAS);

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			App->scene->SaveScene(currentDir, sceneName_saveAS);
			App->scene->currentSceneDir = currentDir;
			App->scene->currentSceneFile = sceneName_saveAS;

			showSaveAs = false;
			sceneName_saveAS = "New Scene";
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			showSaveAs = false;
			sceneName_saveAS = "New Scene";
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

static bool showSaveAsPrefabPopup = false;
static GameObject* prefabToSave;

void ModuleEditor::SaveAsPrefabPopUp(GameObject* prefab) {

	showSaveAsPrefabPopup = true;
	prefabToSave = prefab;

}

void ModuleEditor::RenderSaveAsPrefabPopUp()
{
	if (showSaveAsPrefabPopup) {

		ImGui::OpenPopup("Save As Prefab");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Save As Prefab", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static std::string prefabName_saveAS = "New Prefab";

			ImGui::Text("Prefab will be saved in %s as '%s'", currentDir.c_str(), prefabName_saveAS.c_str());
			ImGui::Separator();

			ImGui::InputText("File Name", &prefabName_saveAS);

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				App->scene->SavePrefab(prefabToSave, currentDir, prefabName_saveAS);

				showSaveAsPrefabPopup = false;

				prefabName_saveAS = "New Prefab";

				ImGui::CloseCurrentPopup();
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				showSaveAsPrefabPopup = false;

				prefabName_saveAS = "New Prefab";
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

	}

}

void ModuleEditor::WindowDockSpaceManagement()
{
	// Set DockSpace Invisible Window Flags
	ImGuiWindowFlags window = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	// Get Window Viewport
	ImGuiViewport* viewport = ImGui::GetWindowViewport();

	// Set Window Parameters
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.0f);

	// Set Window Style Parameters
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	// Begin DockSpace Invisible Window with the flags
	ImGui::Begin("Dockspace", (bool)0, window);

	// Apply Window Style Parameters
	ImGui::PopStyleVar(3);

	// Create DockSpace on the invisible window
	ImGui::DockSpace(ImGui::GetID("Dockspace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

	// End DockSpace Window
	ImGui::End();
}

void ModuleEditor::Toggle_GL_DepthTesting(bool depthTesting)
{
	// Allows objects to be rendered based on their depth in the scene, 
	// ensuring that closer objects are rendered in front of farther objects. 

	if (depthTesting) {

		glEnable(GL_DEPTH_TEST);
		LOG("Enabled Depth Testing");

	}
	else {

		glDisable(GL_DEPTH_TEST);
		LOG("Disabled Depth Testing");

	}
}

void ModuleEditor::Toggle_GL_FaceCulling(bool faceCulling)
{
	// Enables the removal of back-facing or front-facing triangles, 
	// which can improve rendering performance.

	if (faceCulling) {

		glEnable(GL_CULL_FACE);
		LOG("Enabled Face Culling");

	}
	else {

		glDisable(GL_CULL_FACE);
		LOG("Disabled Face Culling");

	}
}

void ModuleEditor::Toggle_GL_Lighting(bool lighting)
{
	// Enables OpenGL's lighting and shading capabilities.

	if (lighting) {

		glEnable(GL_LIGHTING);
		LOG("Enabled Lighting");

	}
	else {

		glDisable(GL_LIGHTING);
		LOG("Disabled Lighting");

	}
}

void ModuleEditor::Toggle_GL_ColorMaterial(bool colorMaterial)
{
	// Enables the automatic generation of material properties based on the current color.

	if (colorMaterial) {

		glEnable(GL_COLOR_MATERIAL);
		LOG("Enabled Color Material");

	}
	else {

		glDisable(GL_COLOR_MATERIAL);
		LOG("Disabled Color Material");

	}
}

void ModuleEditor::Toggle_GL_TextureMapping2D(bool textureMapping2D)
{
	// Enables texture mapping for 2D objects. This is essential for applying textures to surfaces.

	if (textureMapping2D) {

		glEnable(GL_TEXTURE_2D);
		App->renderer3D->texturingEnabled = true;
		LOG("Enabled Texture Mapping 2D");

	}
	else {

		glDisable(GL_TEXTURE_2D);
		App->renderer3D->texturingEnabled = false;
		LOG("Disabled Texture Mapping 2D");

	}
}

void ModuleEditor::Toggle_GL_TextureMapping3D(bool textureMapping3D)
{
	// Enables texture mapping for 3D objects.

	if (textureMapping3D) {

		glEnable(GL_TEXTURE_3D);
		LOG("Enabled Texture Mapping 3D");

	}
	else {

		glDisable(GL_TEXTURE_3D);
		LOG("Disabled Texture Mapping 3D");

	}
}

void ModuleEditor::Toggle_GL_Blending(bool blending)
{
	// Enables alpha blending, which is used to create transparency 
	// and translucency effects in your renderings.

	if (blending) {

		glEnable(GL_BLEND);
		LOG("Enabled Blending");

	}
	else {

		glDisable(GL_BLEND);
		LOG("Disabled Blending");

	}
}

void ModuleEditor::Toggle_GL_MSAA(bool msaa)
{
	// Multisampling Anti-Aliasing (MSAA): Provides smoother edges by 
	// sampling multiple points within a pixel.

	if (msaa) {

		glEnable(GL_MULTISAMPLE);
		LOG("Enabled Multisampling Anti-Aliasing (MSAA)");

	}
	else {

		glDisable(GL_MULTISAMPLE);
		LOG("Disabled Multisampling Anti-Aliasing (MSAA)");

	}
}

void ModuleEditor::Toggle_GL_StencilTesting(bool stencilTesting)
{
	// Allows you to perform operations based on stencil values, 
	// which can be used for various effects.

	if (stencilTesting) {

		glEnable(GL_STENCIL_TEST);
		LOG("Enabled Stencil Testing");

	}
	else {

		glDisable(GL_STENCIL_TEST);
		LOG("Disabled Stencil Testing");

	}
}

void ModuleEditor::Toggle_GL_ScissorTesting(bool scissorTesting)
{
	// Clips rendering to a specified rectangular region on the screen.

	if (scissorTesting) {

		glEnable(GL_SCISSOR_TEST);
		LOG("Enabled Scissor Testing");

	}
	else {

		glDisable(GL_SCISSOR_TEST);
		LOG("Disabled Scissor Testing");

	}
}

void ModuleEditor::Toggle_GL_AlphaTesting(bool alphaTesting)
{
	// Enables the discarding of fragments based on an alpha test value. 
	// This can be useful for certain rendering effects.

	if (alphaTesting) {

		glEnable(GL_ALPHA_TEST);
		LOG("Enabled Alpha Testing");

	}
	else {

		glDisable(GL_ALPHA_TEST);
		LOG("Disabled Alpha Testing");

	}
}

void ModuleEditor::Toggle_GL_PointSprites(bool pointSprites)
{
	// Enables rendering points as sprites, which can be textured and have other properties.

	if (pointSprites) {

		glEnable(GL_POINT_SPRITE);
		LOG("Enabled Point Sprites");

	}
	else {

		glDisable(GL_POINT_SPRITE);
		LOG("Disabled Point Sprites");

	}
}

void ModuleEditor::Toggle_GL_Fog(bool fog)
{
	// Enables fog effects in the scene.

	if (fog) {

		glEnable(GL_FOG);
		LOG("Enabled Fog");

	}
	else {

		glDisable(GL_FOG);
		LOG("Disabled Fog");

	}
}

void ModuleEditor::Toggle_GL_PointSmooth(bool pointSmooth)
{
	// Enables point size smoothing, which can make points appear smoother.

	if (pointSmooth) {

		glEnable(GL_POINT_SMOOTH);
		LOG("Enabled Point Smooth");

	}
	else {

		glDisable(GL_POINT_SMOOTH);
		LOG("Disabled Point Smooth");

	}
}

void ModuleEditor::Toggle_GL_LineSmooth(bool lineSmooth)
{
	// Enables line width smoothing for smoother lines.

	if (lineSmooth) {

		glEnable(GL_LINE_SMOOTH);
		LOG("Enabled Line Smooth");

	}
	else {

		glDisable(GL_LINE_SMOOTH);
		LOG("Disabled Line Smooth");

	}
}

void ModuleEditor::Toggle_GL_Normalization(bool normalization)
{
	// Automatically normalizes normals in fixed-function lighting calculations.

	if (normalization) {

		glEnable(GL_NORMALIZE);
		LOG("Enabled Auto Normalization");

	}
	else {

		glDisable(GL_NORMALIZE);
		LOG("Disabled Auto Normalization");

	}
}

void ModuleEditor::Toggle_GL_PolygonOffset(bool polygonOffset)
{
	// Enables the addition of an offset to the depth values of 
	// rendered polygons, useful for avoiding z-fighting.

	if (polygonOffset) {

		glEnable(GL_POLYGON_OFFSET_FILL);
		LOG("Enabled Polygon Offset");

	}
	else {

		glDisable(GL_POLYGON_OFFSET_FILL);
		LOG("Disabled Polygon Offset");

	}
}

void ModuleEditor::Toggle_GL_WireframeMode(bool wireframe)
{
	// Enable or disable wireframe mode for rendering by setting the 
	// polygon mode to either GL_FILL or GL_LINE. When you set it to GL_FILL, 
	// the objects will be rendered as solid surfaces, while setting 
	// it to GL_LINE will render them in wireframe mode.

	if (wireframe) {

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		LOG("Enabled Wireframe Mode");

	}
	else {

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		LOG("Disabled Wireframe Mode");

	}
}

bool ModuleEditor::CleanUp()
{
	OPTICK_EVENT();

	bool ret = true;

	LOG("Deleting editor...");

	delete scriptEditor;

	nodeEditor.Destroy();

	// ImGui CleanUp

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return ret;
}

void ModuleEditor::AddFPS(const float aFPS)
{
	if (FPSvec.size() < 30) {

		FPSvec.push_back(aFPS);

	}
	else {

		for (int i = 0; i < FPSvec.size(); i++) {

			if (i + 1 < FPSvec.size()) {

				float copy = FPSvec[i + 1];
				FPSvec[i] = copy;

			}

		}
		FPSvec[FPSvec.capacity() - 1] = aFPS;
	}

}

void ModuleEditor::AddDT(const float aDT)
{
	if (DTvec.size() < 30) {

		DTvec.push_back(aDT);

	}
	else {

		for (int i = 0; i < DTvec.size(); i++) {

			if (i + 1 < DTvec.size()) {

				float copy = DTvec[i + 1];
				DTvec[i] = copy;

			}

		}
		DTvec[DTvec.capacity() - 1] = aDT;
	}

}

void ModuleEditor::AddMS(const float aMS)
{
	if (MSvec.size() < 30) {

		MSvec.push_back(aMS);

	}
	else {

		for (int i = 0; i < MSvec.size(); i++) {

			if (i + 1 < MSvec.size()) {

				float copy = MSvec[i + 1];
				MSvec[i] = copy;

			}

		}
		MSvec[MSvec.capacity() - 1] = aMS;
	}

}

void ModuleEditor::RequestBrowser(const char* url)
{
	HINSTANCE result = ShellExecuteA(nullptr, "open", url, nullptr, nullptr, SW_SHOWNORMAL);
}

void ModuleEditor::ToggleFullscreen(bool fullscreen)
{
	if (fullscreen) {

		SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);

	}
	else {

		SDL_SetWindowFullscreen(App->window->window, 0);

	}
}

void ModuleEditor::ToggleResizable(bool resizable)
{
	if (resizable) {

		SDL_SetWindowResizable(App->window->window, SDL_TRUE);

	}
	else {

		SDL_SetWindowResizable(App->window->window, SDL_FALSE);

	}
}

void ModuleEditor::ToggleBorderless(bool borderless)
{
	if (borderless) {

		SDL_SetWindowBordered(App->window->window, SDL_FALSE);

	}
	else {

		SDL_SetWindowBordered(App->window->window, SDL_TRUE);

	}
}

void ModuleEditor::ToggleFullscreenDesktop(bool fullscreenDesktop)
{
	if (fullscreenDesktop) {

		SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);

	}
	else {

		SDL_SetWindowFullscreen(App->window->window, 0);

	}
}

void ModuleEditor::ToggleVSync(bool vsync)
{
	if (vsync) {

		SDL_GL_SetSwapInterval(1);

	}
	else {

		SDL_GL_SetSwapInterval(0);

	}
}

void ModuleEditor::ToggleLightMode(bool lightMode)
{
	if (lightMode) {

		ImGui::StyleColorsLight();
		LOG("Enable light mode");

	}
	else {

		ImGui::StyleColorsDark();
		LOG("Enable dark mode");

	}
}

void ModuleEditor::ToggleMeshesVertexNormals(bool showVertexNormals)
{
	for (auto it1 = App->renderer3D->models.begin(); it1 != App->renderer3D->models.end(); ++it1) {

		for (auto it2 = (*it1).meshes.begin(); it2 != (*it1).meshes.end(); ++it2) {

			if (showVertexNormals) {

				(*it2).enableVertexNormals = true;

			}
			else {

				(*it2).enableVertexNormals = false;

			}

		}

	}

}

void ModuleEditor::ToggleMeshesFaceNormals(bool showFaceNormals)
{
	for (auto it1 = App->renderer3D->models.begin(); it1 != App->renderer3D->models.end(); ++it1) {

		for (auto it2 = (*it1).meshes.begin(); it2 != (*it1).meshes.end(); ++it2) {

			if (showFaceNormals) {

				(*it2).enableFaceNormals = true;

			}
			else {

				(*it2).enableFaceNormals = false;

			}

		}

	}

}

void ModuleEditor::ShowPlatformInfo() {

	ImGui::Text("Platform:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", SDL_GetPlatform());

	ImGui::Text("SDL Version:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "2.0.4");

}

void ModuleEditor::ShowCPUInfo()
{
	ImGui::Text("CPU Cores:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), std::to_string(SDL_GetCPUCount()).c_str());

	ImGui::Text("CPU Cache:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s kb", std::to_string(SDL_GetCPUCacheLineSize()).c_str());

	ImGui::Text("Caps:");
	ImGui::SameLine();

	if (SDL_Has3DNow()) {

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "3DNow,");

	}

	ImGui::SameLine();

	if (SDL_HasAltiVec()) {

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Altivec,");

	}

	ImGui::SameLine();

	if (SDL_HasAVX()) {

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "AVX,");

	}

	ImGui::SameLine();

	if (SDL_HasAVX2()) {

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "AVX2,");

	}

	ImGui::SameLine();

	if (SDL_HasMMX()) {

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "MMX,");

	}

	ImGui::SameLine();

	if (SDL_HasRDTSC()) {

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "RDTSC,");

	}

	ImGui::SameLine();

	if (SDL_HasSSE()) {

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE,");

	}

	if (SDL_HasSSE2()) {

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE2,");

	}

	ImGui::SameLine();

	if (SDL_HasSSE3()) {

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE3,");

	}

	ImGui::SameLine();

	if (SDL_HasSSE41()) {

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE41,");

	}

	ImGui::SameLine();

	if (SDL_HasSSE42()) {

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE42");

	}

}

void ModuleEditor::ShowGPUInfo()
{
	const GLubyte* vendor = glGetString(GL_VENDOR);
	ImGui::Text("Vendor:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", vendor);

	const GLubyte* renderer = glGetString(GL_RENDERER);
	ImGui::Text("GPU:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", renderer);

	const GLubyte* version = glGetString(GL_VERSION);
	ImGui::Text("OpenGL version supported:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", version);

	const GLubyte* glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
	ImGui::Text("GLSL:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glsl);

	GLint totalMemoryKB = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMemoryKB);
	ImGui::Text("Total VRAM:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.3f mb", static_cast<float>(totalMemoryKB / 1000.0f));

	GLint currentMemoryKB = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currentMemoryKB);
	ImGui::Text("Available VRAM:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.3f mb", static_cast<float>(currentMemoryKB / 1000.0f));

	PERFORMANCE_INFORMATION perfInfo;
	GetPerformanceInfo(&perfInfo, sizeof(perfInfo));
	ImGui::Text("VRAM Usage:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.3f mb", static_cast<float>(perfInfo.CommitTotal / 1000000.0f));
}

void ModuleEditor::ShowRAMInfo()
{
	ImGui::Text("System Total RAM:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.2f gb", static_cast<float>(SDL_GetSystemRAM()) / 1000.0f);

	MEMORYSTATUSEX memStatus;
	memStatus.dwLength = sizeof(memStatus);
	GlobalMemoryStatusEx(&memStatus);

	ImGui::Text("Available RAM:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.3f mb", static_cast<float>(memStatus.ullAvailPhys / (1024.0f * 1024.0f)));

	ImGui::Text("Total RAM Usage:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.3f mb", static_cast<float>(memStatus.ullTotalPhys - memStatus.ullAvailPhys) / (1024.0f * 1024.0f));
}

void ModuleEditor::ShowDiskInfo()
{
	ULARGE_INTEGER totalFreeBytes;
	ULARGE_INTEGER totalBytes;
	ULARGE_INTEGER totalFreeBytesToCaller;

	GetDiskFreeSpaceEx(
		nullptr,                   // Use the default drive (usually C:)
		&totalFreeBytesToCaller,   // Total number of free bytes available to the caller
		&totalBytes,               // Total number of bytes on the disk
		&totalFreeBytes            // Total number of free bytes on the disk
	);

	double totalSpaceGB = static_cast<double>(totalBytes.QuadPart) / (1024 * 1024 * 1024);
	double freeSpaceGB = static_cast<double>(totalFreeBytes.QuadPart) / (1024 * 1024 * 1024);

	ImGui::Text("Total C: Disk Space:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.3f gb", static_cast<float>(totalSpaceGB));

	ImGui::Text("Available C: Disk Space:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.3f gb", static_cast<float>(freeSpaceGB));
}

void ModuleEditor::AboutModalWindowContent()
{
	ImGui::Text("Ymir Engine v0.1");

	ImGui::NewLine();

	ImGui::SeparatorText("DESCRIPTION");
	ImGui::NewLine();

	const char* engineInfo = "Ymir Engine is part of the Game Engines subject, on the third year of the\nVideogame Design and Development degree, taught at CITM UPC (Terrassa)\nand supervised by Miquel Suau.\n\nThe engine has been made from scratch in C++ using SDL and OpenGL to manage\ngraphics. The main target of the project is to be able to integrate complex\nsystems such as graphics management and game engine architecture in a simple\nand understandable way for educational purposes.";

	ImGui::Text("%s", engineInfo);

	ImGui::NewLine();

	ImGui::SeparatorText("AUTHORS");
	ImGui::NewLine();

	ImGui::Text("Francesc Teruel Rodriguez ->");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "francesctr4");
	if (ImGui::IsItemClicked()) {

		RequestBrowser("https://github.com/francesctr4");

	}

	ImGui::Text("Joel Romero Botella ->");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "Joeltecke25");
	if (ImGui::IsItemClicked()) {

		RequestBrowser("https://github.com/Joeltecke25");

	}

	ImGui::NewLine();
	ImGui::SeparatorText("THIRD PARTY LIBRARIES");
	ImGui::NewLine();

	SDL_version version;
	SDL_GetVersion(&version);

	ImGui::BulletText("SDL v%d.%d.%d -> ", version.major, version.minor, version.patch);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "https://github.com/libsdl-org/SDL");
	if (ImGui::IsItemClicked()) {

		RequestBrowser("https://github.com/libsdl-org/SDL");

	}

	ImGui::BulletText("OpenGL v%s ->", glGetString(GL_VERSION));
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "https://www.opengl.org/");
	if (ImGui::IsItemClicked()) {

		RequestBrowser("https://www.opengl.org/");

	}

	ImGui::BulletText("Glew v%s ->", glewGetString(GLEW_VERSION));
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "https://github.com/nigels-com/glew");
	if (ImGui::IsItemClicked()) {

		RequestBrowser("https://github.com/nigels-com/glew");

	}

	ImGui::BulletText("ImGui v%s ->", IMGUI_VERSION);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "https://github.com/ocornut/imgui");
	if (ImGui::IsItemClicked()) {

		RequestBrowser("https://github.com/ocornut/imgui");

	}

	ImGui::BulletText("Assimp v%d.%d.%d ->", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "https://github.com/assimp/assimp");
	if (ImGui::IsItemClicked()) {

		RequestBrowser("https://github.com/assimp/assimp");

	}

	ILint ILversion = ilGetInteger(IL_VERSION_NUM);

	uint ILmajor = (ILversion / 100) % 10;
	uint ILminor = ILversion % 100;

	ImGui::BulletText("DevIL v%d.%d ->", ILmajor, ILminor);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "https://openil.sourceforge.net/download.php");
	if (ImGui::IsItemClicked()) {

		RequestBrowser("https://openil.sourceforge.net/download.php");

	}

	ImGui::BulletText("MathGeoLib ->");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "https://github.com/juj/MathGeoLib");
	if (ImGui::IsItemClicked()) {

		RequestBrowser("https://github.com/juj/MathGeoLib");

	}

	ImGui::BulletText("Optick ->");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "https://github.com/bombomby/optick");
	if (ImGui::IsItemClicked()) {

		RequestBrowser("https://github.com/bombomby/optick");

	}

	ImGui::BulletText("mmgr ->");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "https://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml");
	if (ImGui::IsItemClicked()) {

		RequestBrowser("https://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml");

	}

	ImGui::BulletText("Parson ->");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "https://github.com/kgabis/parson");
	if (ImGui::IsItemClicked()) {

		RequestBrowser("https://github.com/kgabis/parson");

	}

	ImGui::NewLine();
	ImGui::SeparatorText("LICENSE");
	ImGui::NewLine();

	ImGui::TextWrapped("%s", licenseFileContents.c_str());

	ImGui::NewLine();
	if (ImGui::Button("Close")) {

		showAboutPopUp = false;

		ImGui::CloseCurrentPopup();

	}
}

std::string ModuleEditor::ReadFile(const std::string& filename) {

	std::ifstream file(filename);

	if (!file.is_open()) {

		return "[ERROR] Unable to open file.";

	}

	std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	file.close();

	return fileContents;
}

void ModuleEditor::RedirectLogOutput()
{
	if (ImGui::Begin("Console", &showConsole, ImGuiWindowFlags_MenuBar))
	{
		static ImGuiTextFilter filter;

		ImGui::BeginMenuBar();
		if (ImGui::Button("Clear"))
		{
			ClearVec(Log::debugStrings);
		}
		if (ImGui::Button("Show Errors"))
		{
			filter.Filters.push_back(ImGuiTextFilter::ImGuiTextRange("[ERROR", NULL));
		}
		if (ImGui::Button("Show Warnings"))
		{
			filter.Filters.push_back(ImGuiTextFilter::ImGuiTextRange("[WARNING", NULL));
		}
		if (ImGui::Button("Add Error"))
		{
			Log::debugStrings.push_back("[ERROR] debug error message");
		}
		if (ImGui::Button("Add Warning"))
		{
			Log::debugStrings.push_back("[WARNING] debug warning message");
		}

		ImGui::Dummy(ImVec2(720 / 4, 0));
		filter.Draw("Search", ImGui::GetFontSize() * 15);
		ImGui::EndMenuBar();

		//const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("##output", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		for (int n = 0; n < Log::debugStrings.size(); n++)
		{
			if (filter.PassFilter(Log::debugStrings[n].c_str()))
			{
				if (strstr(Log::debugStrings[n].c_str(), "[ERROR")) { ImGui::TextColored(ImVec4(0.9f, 0.0f, 0.0f, 1.0f), Log::debugStrings[n].c_str()); }
				else if (strstr(Log::debugStrings[n].c_str(), "[WARNING")) { ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), Log::debugStrings[n].c_str()); }
				else { ImGui::Text(Log::debugStrings[n].c_str(), n); }
			}
		}

		// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
		// Using a scrollbar or mouse-wheel will take away from the bottom edge.
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) { ImGui::SetScrollHereY(1.0f); }

		ImGui::EndChild();
	} ImGui::End();
}

void ModuleEditor::MemoryLeaksOutput()
{
	ImGui::TextWrapped("%s", memleaksFileContents.c_str());
}

void ModuleEditor::AssimpLogOutput()
{
	ImGui::TextWrapped("%s", AssimpLogFileContents.c_str());
}

void ModuleEditor::DrawHierarchy()
{
	CreateHierarchyTree(App->scene->mRootNode);
}

void ModuleEditor::CreateHierarchyTree(GameObject* node)
{
	if (node != nullptr) {

		// Set flags to open the tree nodes

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | (node->selected ? ImGuiTreeNodeFlags_Selected : 0) | (node->mChildren.size() ? 0 : ImGuiTreeNodeFlags_Leaf);

		if (!node->active) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.421f, 0.421f, 0.421f, 1.0f));

		bool isNodeOpen = ImGui::TreeNodeEx((node->name + "##" + std::to_string(node->UID)).c_str(), flags);

		if (!node->active) ImGui::PopStyleColor();

		if (node != App->scene->mRootNode && ImGui::IsItemClicked())
		{
			App->scene->SetSelected(node);
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("GameObject", node, sizeof(GameObject*));

			draggedGO = node;
			ImGui::Text("Drag to");
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		{
			hoveredGO = node;
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject")) {

				draggedGO->ReParent(hoveredGO);
			}
			ImGui::EndDragDropTarget();
		}

		if (node != App->scene->mRootNode && ImGui::BeginPopupContextItem()) {

			App->scene->SetSelected(node);

			if (ImGui::MenuItem("Delete")) {

				if (node != App->scene->mRootNode && node->selected) {

					node->DestroyGameObject();

				}
				else if (node == App->scene->mRootNode && node->selected) {

					App->scene->ClearScene();

					for (const auto& pair : App->resourceManager->GetResourcesMap()) {

						Resource* resource = pair.second;

						App->resourceManager->ReleaseResource(resource);

					}

				}

			}

			if (ImGui::MenuItem("Save Prefab")) {

				SaveAsPrefabPopUp(node);

			}

			if (ImGui::MenuItem("Create Empty Children")) {

				GameObject* empty = App->scene->CreateGameObject("Empty", node);
				empty->UID = Random::Generate();

			}

			if (ImGui::MenuItem("Move Up")) {

				if (node != App->scene->mRootNode && node->selected) {

					auto it = std::find(node->mParent->mChildren.begin(), node->mParent->mChildren.end(), node);

					if (it != node->mParent->mChildren.end())
					{
						int index = it - node->mParent->mChildren.begin();

						if (index > 0)
						{
							node->SwapChildren(node->mParent->mChildren[index - 1]);
						}
						/*else
						{
							node->SwapChildren(node->mParent->mChildren[node->mParent->mChildren.size() - 1]);
						}*/
					}

				}

			}

			if (ImGui::MenuItem("Move Down")) {

				if (node != App->scene->mRootNode && node->selected) {

					auto it = std::find(node->mParent->mChildren.begin(), node->mParent->mChildren.end(), node);

					if (it != node->mParent->mChildren.end())
					{
						int index = it - node->mParent->mChildren.begin();

						if (index < node->mParent->mChildren.size() - 1)
						{
							node->SwapChildren(node->mParent->mChildren[index + 1]);
						}
						/*else
						{
							node->SwapChildren(node->mParent->mChildren[0]);
						}*/
					}

				}

			}

			ImGui::EndPopup();
		}

		if (isNodeOpen)
		{
			// Display the children if the node is open

			if (node != nullptr) {

				if (node->mChildren.size())
				{
					for (uint i = 0; i < node->mChildren.size(); i++)
					{
						CreateHierarchyTree(node->mChildren[i]);
					}
				}

			}

			// Close the TreeNode when you're done with its children
			ImGui::TreePop();
		}

	}

}

//void ModuleEditor::DestroyHierarchyTree(GameObject* node)
//{
//	if (node == nullptr) {
//		return;
//	}
//
//	App->scene->DestroyGameObject(node);
//
//}

void ModuleEditor::DrawBakingTab()
{
	char buffer[50];

	ImGui::Text("Agent Properties");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Columns(2, NULL, FALSE);
	ImGui::Spacing();
	ImGui::Text("Radius");
	ImGui::Spacing();
	ImGui::Text("Height");
	ImGui::Spacing();
	ImGui::Text("Step Height");
	ImGui::Spacing();
	ImGui::Text("Max Slope");
	ImGui::Spacing();
	ImGui::NextColumn();

	sprintf_s(buffer, 50, "%.2f", External->pathFinding->bakedNav.radius);
	if (ImGui::InputText("##Radius", &buffer[0], sizeof(buffer)))
	{
		if (buffer[0] != '\0') {
			External->pathFinding->bakedNav.radius = strtod(buffer, NULL);
		}
	}
	sprintf_s(buffer, 50, "%.2f", External->pathFinding->bakedNav.height);
	if (ImGui::InputText("##Height", &buffer[0], sizeof(buffer)))
	{
		if (buffer[0] != '\0') {
			External->pathFinding->bakedNav.height = strtod(buffer, NULL);
		}
	}
	sprintf_s(buffer, 50, "%.2f", External->pathFinding->bakedNav.stopHeight);
	if (ImGui::InputText("##StopHeight", &buffer[0], sizeof(buffer)))
	{
		if (buffer[0] != '\0') {
			External->pathFinding->bakedNav.stopHeight = strtod(buffer, NULL);
		}
	}
	sprintf_s(buffer, 50, "%d", External->pathFinding->bakedNav.maxSlope);
	if (ImGui::InputText("##Slope", &buffer[0], sizeof(buffer)))
	{
		if (buffer[0] != '\0') {
			External->pathFinding->bakedNav.maxSlope = strtod(buffer, NULL);
		}
	}

	ImGui::Columns(1);

	ImGui::Dummy({ 0,10 });

	if (ImGui::Button("Calculate"))
	{
		External->pathFinding->BakeNavMesh();
	}

	ImGui::SameLine();
	if (ImGui::Button("Clear"))
	{
		External->pathFinding->ClearNavMeshes();
	}

	ImGui::Checkbox("Debug Draw", &External->pathFinding->debugDraw);

	/*ImGui::Text("Input Mesh");
	ImGui::Button("Drop mesh here");
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_GAMEOBJECT"))
		{
			int uid = *(int*)payload->Data;

			GameObject* droppedGO = External->moduleScene->GetGOFromUID(External->moduleScene->root, uid);
			External->pathFinding->AddGameObjectToNavMesh(droppedGO);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::Text("Current NavMesh");
	*/

#ifndef _STANDALONE
	NavMeshBuilder* navMeshBuilder = External->pathFinding->GetNavMeshBuilder();
	if (navMeshBuilder != nullptr)
	{
		navMeshBuilder->OnEditor();
	}

#endif // !STANDALONE

	/*if(ImGui::Button("Create Walkability Test"))
	{
		External->pathFinding->CreateWalkabilityTestPoint();
	}*/

	ImGui::Text("Path Type");

	if (ImGui::RadioButton("Smooth Path", External->pathFinding->pathfinder.pathType == PathType::SMOOTH))
		External->pathFinding->pathfinder.pathType = PathType::SMOOTH;

	ImGui::SameLine();
	if (ImGui::RadioButton("Straight Path", External->pathFinding->pathfinder.pathType == PathType::STRAIGHT))
		External->pathFinding->pathfinder.pathType = PathType::STRAIGHT;

}

void ModuleEditor::DrawInspector()
{
	//for (auto it = App->scene->gameObjects.begin(); it != App->scene->gameObjects.end(); ++it) {

	if (App->scene->selectedGO != nullptr) {

		if (App->scene->selectedGO->selected) {

			if (ImGui::Checkbox("##Active", &App->scene->selectedGO->active)) {

				App->scene->SetActiveRecursively(App->scene->selectedGO, App->scene->selectedGO->active);
				if (!App->scene->selectedGO->active) {
					CScript* aux = static_cast<CScript*>(App->scene->selectedGO->GetComponent(ComponentType::SCRIPT));

					if (aux) {
						aux->isStarting = true;
					}
				}

			}

			ImGui::SameLine();
			char nameBuffer[256]; // You can adjust the buffer size as needed

			// Copy the current name to the buffer
			strcpy(nameBuffer, App->scene->selectedGO->name.c_str());

			// Create an input text field in your ImGui window
			if (ImGui::InputText(" ", nameBuffer, sizeof(nameBuffer)))
			{
				// The input text has changed, update the name
				App->scene->selectedGO->name = nameBuffer;
			}

			ImGui::Spacing();

			ImGui::Checkbox("Static", &App->scene->selectedGO->isStatic);

			ImGui::Text("Tag"); ImGui::SameLine();

			ImGuiStyle& style = ImGui::GetStyle();
			float w = ImGui::CalcItemWidth() * 1.25;
			float spacing = style.ItemInnerSpacing.x;
			float button_sz = ImGui::GetFrameHeight();

			ImGui::PushItemWidth((w - spacing * 0.5f - button_sz * 0.5f) * 0.5f);

			std::vector<std::string> tags = External->scene->tags;

			for (std::vector<std::string>::iterator it = External->scene->tags.begin(); it != External->scene->tags.end(); ++it)
			{
				for (auto to = it + 1; to != External->scene->tags.end();)
				{
					if ((*it) == (*to))
					{
						to = External->scene->tags.erase(to);
					}
					else
					{
						++to;
					}
				}
			}
			if (ImGui::BeginCombo("##tags", App->scene->selectedGO->tag.c_str()))
			{
				for (int t = 0; t < tags.size(); t++)
				{
					bool is_selected = strcmp(App->scene->selectedGO->tag.c_str(), tags[t].c_str()) == 0;
					if (ImGui::Selectable(tags[t].c_str(), is_selected)) {
						App->scene->selectedGO->tag = tags[t];
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				if (ImGui::BeginMenu("Add Tag"))
				{
					static char newTag[32];
					ImGui::InputText("##Juan", newTag, IM_ARRAYSIZE(newTag));
					bool estaVacio = (std::strlen(newTag) == 0);

					if (ImGui::Button("Save Tag"))
					{
						if (!estaVacio)
						{
							char* tagToAdd = new char[IM_ARRAYSIZE(newTag)];
							strcpy(tagToAdd, newTag);
							External->scene->tags.push_back(tagToAdd);
							newTag[0] = '\0';
							delete[] tagToAdd;
						}
					}
					ImGui::EndMenu();
				}

				int tag_to_remove = -1;
				if (ImGui::BeginMenu("Remove Tag"))
				{
					for (int t = 0; t < tags.size(); t++)
					{
						if (ImGui::Selectable(tags[t].c_str(), false)) {
							tag_to_remove = t;
						}
					}
					ImGui::EndMenu();
				}

				if (tag_to_remove != -1)
					External->scene->tags.erase(External->scene->tags.begin() + tag_to_remove);

				ImGui::EndCombo();
			}
			ImGui::SameLine();
			ImGui::Text("       UID: %d", App->scene->selectedGO->UID);


			ImGui::Spacing();

			if (!App->scene->selectedGO->active) { ImGui::BeginDisabled(); }

			/*Component* transform =  App->scene->selectedGO->GetComponent(ComponentType::TRANSFORM);
			Component* mesh =  App->scene->selectedGO->GetComponent(ComponentType::MESH);
			Component* material =  App->scene->selectedGO->GetComponent(ComponentType::MATERIAL);
			Component* camera =  App->scene->selectedGO->GetComponent(ComponentType::CAMERA);
			Component* audioListener =  App->scene->selectedGO->GetComponent(ComponentType::AUDIO_LISTENER);
			Component* audioSource =  App->scene->selectedGO->GetComponent(ComponentType::AUDIO_SOURCE);
			Component* physics =  App->scene->selectedGO->GetComponent(ComponentType::PHYSICS);
			Component* animation =  App->scene->selectedGO->GetComponent(ComponentType::ANIMATION);
			Component* script =  App->scene->selectedGO->GetComponent(ComponentType::SCRIPT);

			if (transform != nullptr) transform->OnInspector(); ImGui::Spacing();
			if (mesh != nullptr) mesh->OnInspector(); ImGui::Spacing();
			if (material != nullptr) material->OnInspector(); ImGui::Spacing();
			if (camera != nullptr) camera->OnInspector(); ImGui::Spacing();
			if (audioListener != nullptr) audioListener->OnInspector(); ImGui::Spacing();
			if (audioSource != nullptr) audioSource->OnInspector(); ImGui::Spacing();
			if (physics != nullptr) physics->OnInspector(); ImGui::Spacing();
			if (animation != nullptr) animation->OnInspector(); ImGui::Spacing();
			if (script != nullptr) script->OnInspector(); ImGui::Spacing();
			if (camera != nullptr) camera->OnInspector(); ImGui::Spacing();*/

			for (auto i = 0; i < App->scene->selectedGO->mComponents.size(); i++)
			{
				App->scene->selectedGO->mComponents[i]->OnInspector();
				ImGui::Spacing();
			}

			//if (camera != nullptr) camera->OnInspector(); ImGui::Spacing();
			//if (audioListener != nullptr) audioListener->OnInspector(); ImGui::Spacing();
			//if (audioSource != nullptr) audioSource->OnInspector(); ImGui::Spacing();

			float buttonWidth = 120.0f;  // Adjust the width as needed
			float windowWidth = ImGui::GetWindowWidth();
			float xPos = (windowWidth - buttonWidth) * 0.5f;

			// Set the cursor position to center the button within the menu
			ImGui::SetCursorPosX(xPos);

			static bool click = false;

			if (ImGui::Button("Add Component", ImVec2(110, 30)))
			{
				ImGui::OpenPopup("AddComponents");
				click = false;
			}

			if (!click)
			{
				if (ImGui::BeginPopup("AddComponents"))
				{
					ImGui::SeparatorText("Components");

					// Skip transform
					// --- Add component Mesh ---

					/*if (mesh == nullptr)
					{
						if (ImGui::BeginMenu("Mesh"))
						{
							App->editor->PrimitivesMenu();
							ImGui::EndMenu();
						}
					}*/

					// --- Add component Material ---
					if ((CMaterial*)App->scene->selectedGO->GetComponent(ComponentType::CAMERA) == nullptr)
					{
						if (ImGui::MenuItem("Material"))
						{
							App->scene->selectedGO->AddComponent(ComponentType::MATERIAL);
						}
					}


					//// --- Add component Camera ---

					if ((CCamera*)App->scene->selectedGO->GetComponent(ComponentType::CAMERA) == nullptr)
					{
						if (ImGui::MenuItem("Camera"))
						{
							App->scene->selectedGO->AddComponent(ComponentType::CAMERA);
						}
					}

					//// --- Add component Physics ---

					if ((CCollider*)App->scene->selectedGO->GetComponent(ComponentType::PHYSICS) == nullptr)
					{
						if (ImGui::MenuItem("Physics"))
						{
							App->scene->selectedGO->AddComponent(ComponentType::PHYSICS);
						}
					}

					if ((CAudioSource*)App->scene->selectedGO->GetComponent(ComponentType::AUDIO_SOURCE) == nullptr)
					{
						if (ImGui::MenuItem("Audio_Source"))
						{
							App->scene->selectedGO->AddComponent(ComponentType::AUDIO_SOURCE);
						}
					}

					if ((CAudioListener*)App->scene->selectedGO->GetComponent(ComponentType::AUDIO_LISTENER) == nullptr)
					{
						if (ImGui::MenuItem("Audio_Listener"))
						{
							App->scene->selectedGO->AddComponent(ComponentType::AUDIO_LISTENER);
						}
					}

					if (ImGui::BeginMenu("Script"))
					{
						if (ImGui::MenuItem("Add New Script")) {

							//Todo: Add NewScript
							showNewScriptPopUp = true;

						}

						static ImGuiTextFilter scriptFilter;

						scriptFilter.Draw("Search", ImGui::GetFontSize() * 15);

						ImGui::Separator();

						for (const auto& entry : std::filesystem::directory_iterator("Assets/Scripts")) {

							std::string entryName = entry.path().filename().string();

							if (!entry.is_directory() && scriptFilter.PassFilter(entryName.c_str())) {

								if (ImGui::BeginChild("##ScriptsAddComponent", ImVec2(0, 300), true))
								{
									if (ImGui::MenuItem(entryName.c_str()))
									{
										script_name = entryName.c_str();
										App->scene->selectedGO->AddComponent(ComponentType::SCRIPT);

										click = true;
									}
								}

								ImGui::EndChild();

							}
						}

						ImGui::EndMenu();
					}

					if ((CNavMeshAgent*)App->scene->selectedGO->GetComponent(ComponentType::NAVMESHAGENT) == nullptr)
					{
						if (ImGui::MenuItem("NavMesh_Agent"))
						{
							App->scene->selectedGO->AddComponent(ComponentType::NAVMESHAGENT);
						}
					}

					//delete physics;
					ImGui::EndPopup();
				}



			}

			if (!App->scene->selectedGO->active) { ImGui::EndDisabled(); }
		}
	}
}

void ModuleEditor::DrawGizmo(const ImVec2& sceneWindowPos, const ImVec2& sceneContentRegionMax, const float& sceneFrameHeightOffset)
{
	// Begin the ImGuizmo frame.
	ImGuizmo::BeginFrame();

	// Check if the current game object is selected.
	if (App->scene->selectedGO != nullptr) {

		ImGuizmo::SetDrawlist();
		// Set the rectangle for ImGuizmo in the editor window.
		ImGuizmo::SetRect(sceneWindowPos.x, sceneWindowPos.y + sceneFrameHeightOffset, sceneContentRegionMax.x, sceneContentRegionMax.y);

		if (App->camera->hoveringEditor)
		{
			// Check for key presses to set the gizmo operation and mode.
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {

				gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
			}

			if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {

				gizmoOperation = ImGuizmo::OPERATION::ROTATE;
			}

			if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {

				gizmoOperation = ImGuizmo::OPERATION::SCALE;

			}

			if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) {

				gizmoMode = ImGuizmo::MODE::WORLD;

			}

			if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {

				gizmoMode = ImGuizmo::MODE::LOCAL;

			}

		}

		ImGuizmo::MODE modeApplied;

		// Hardcoded local mode to prevent Scale from Reseting the Rotation.
		if (gizmoOperation == ImGuizmo::OPERATION::SCALE) {

			modeApplied = ImGuizmo::MODE::LOCAL;
		}
		else {

			modeApplied = gizmoMode;

		}

		// Get the view and projection matrices from the editor camera.
		float4x4 viewMatrix = App->camera->editorCamera->GetViewMatrix();
		float4x4 projectionMatrix = App->camera->editorCamera->GetProjectionMatrix();

		// Get the transform component of the current game object.
		CTransform* ctransform = (CTransform*)App->scene->selectedGO->GetComponent(ComponentType::TRANSFORM);

		modelMatrix = ctransform->mGlobalMatrix.Transposed();

		//// Copy the model matrix to a float array for ImGuizmo.
		//float modelPtr[16];
		//memcpy(modelPtr, modelMatrix.ptr(), 16 * sizeof(float));

		//Snap
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		{
			snapValue = 1.0f; // Snap to 1.0m for translation/scale

			if (gizmoOperation == ImGuizmo::OPERATION::ROTATE)
			{
				// Snap to 45 degrees for rotation
				snapValue = 45.0f;
			}
		}

		else
		{
			snapValue = 0.0f;
		}

		float snapValues[3] = { snapValue, snapValue, snapValue };

		// Use ImGuizmo to manipulate the object in the scene.
		ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), gizmoOperation, modeApplied, modelMatrix.ptr(), nullptr, snapValues);

		// Check if the gizmo is being used.
		if (ImGuizmo::IsUsing())
		{
			float4x4 matrix = modelMatrix.Transposed();
			App->scene->selectedGO->mTransform->UpdateTransformGuizmo(matrix);
		}

		// Check if the reset button is pressed, and reset the model matrix.
		if (ctransform->resetPressed) {

			modelMatrix = float4x4::identity;
			ctransform->resetPressed = false;
		}
	}

}

void ModuleEditor::DeleteFileAndRefs(const char* filePath)
{
	std::string path = filePath;

	switch (App->resourceManager->CheckExtensionType(filePath))
	{
	case ResourceType::UNKNOWN:
		break;
	case ResourceType::TEXTURE:

		if (PhysfsEncapsule::FileExists((path + ".meta").c_str()))
		{
			std::unique_ptr<JsonFile> tmpMetaFile = JsonFile::GetJSON(path + ".meta");

			if (tmpMetaFile)
			{
				PhysfsEncapsule::DeleteFS(tmpMetaFile->GetString("Library Path").c_str());
			}

			PhysfsEncapsule::DeleteFS((path + ".meta").c_str());
		}

		break;
	case ResourceType::MESH:

		if (PhysfsEncapsule::FileExists((path + ".meta").c_str()))
		{
			std::unique_ptr<JsonFile> tmpMetaFile = JsonFile::GetJSON(path + ".meta");

			if (tmpMetaFile)
			{
				std::unique_ptr<int[]> ids = tmpMetaFile->GetIntArray("Meshes Embedded UID");

				for (int i = 0; i < tmpMetaFile->GetInt("Meshes num"); i++)
				{
					PhysfsEncapsule::DeleteFS((".\/Library\/Meshes\/" + std::to_string(ids[i]) + ".ymesh").c_str());
				}

				PhysfsEncapsule::DeleteFS(tmpMetaFile->GetString("Library Path").c_str());
			}

			PhysfsEncapsule::DeleteFS((path + ".meta").c_str());
		}

		break;
	case ResourceType::MODEL:
		break;
	case ResourceType::SCENE:
		break;
	case ResourceType::META:
	{
		// TODO: Sara -> borrar library file from meta
		//App->parson->DeleteLibDirs(path.c_str());
	}

	break;
	case ResourceType::SHADER:
		break;
	case ResourceType::MATERIAL:
		break;
	case ResourceType::ALL_TYPES:
		break;
	default:
		break;
	}

	PhysfsEncapsule::DeleteFS(filePath);
}

void ModuleEditor::DrawFileExplorer(const std::string& rootFolder) {

	// Process Directories First

	for (const auto& entry : std::filesystem::directory_iterator(rootFolder)) {

		if (entry.is_directory()) {

			std::string entryName = entry.path().filename().string();

			if (entryName != "." && entryName != "..") {

				if (ImGui::TreeNodeEx(entryName.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)) {

					DrawFileExplorer(entry.path().string());

					ImGui::TreePop();

				}

			}

		}

	}

	// Process Files Afterwards

	for (const auto& entry : std::filesystem::directory_iterator(rootFolder)) {

		if (!entry.is_directory()) {

			std::string entryName = entry.path().filename().string();

			if (entryName != "." && entryName != "..") {

				ImGui::Selectable(entryName.c_str());

			}

		}

	}

}

void ModuleEditor::CreateNewFolder()
{
	if (createFolder)
	{
		ImGui::OpenPopup("Create new folder");
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Create new folder", &createFolder, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static std::string folderName = "NewFolder";
		ImGui::InputText("File Name", &folderName);

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			PhysfsEncapsule::CreateFolder(currentDir, folderName);
			createFolder = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			createFolder = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void ModuleEditor::DrawAssetsWindow(const std::string& assetsFolder)
{
	// Start Menu Bar

	ImGui::BeginMenuBar();

	if (ImGui::Button(("<- " + currentDir).c_str()))
	{
		std::size_t found = currentDir.find_last_of("/");
		currentDir = currentDir.substr(0, found);

	}

	if (currentDir != "Assets") {

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("tex"))
			{
				std::string assetsFilePathDrop = (const char*)payload->Data;
				if (assetsFilePathDrop.find(".png") != std::string::npos) {

					assetsFilePathDrop.erase(assetsFilePathDrop.find(".png") + 4);

					LOG("File path: %s", assetsFilePathDrop.c_str());

				}

				MoveAsset(assetsFilePathDrop);

			}

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("asset"))
			{
				std::string assetsFilePathDrop = (const char*)payload->Data;

				if (assetsFilePathDrop.find(".fbx") != std::string::npos) {

					assetsFilePathDrop.erase(assetsFilePathDrop.find(".fbx") + 4);

					LOG("File path: %s", assetsFilePathDrop.c_str());

				}

				else if (assetsFilePathDrop.find(".yscene") != std::string::npos) {

					assetsFilePathDrop.erase(assetsFilePathDrop.find(".yscene") + 7);

					LOG("File path: %s", assetsFilePathDrop.c_str());

				}

				else if (assetsFilePathDrop.find(".yfab") != std::string::npos) {

					assetsFilePathDrop.erase(assetsFilePathDrop.find(".yfab") + 5);

					LOG("File path: %s", assetsFilePathDrop.c_str());

				}

				else if (assetsFilePathDrop.find(".ttf") != std::string::npos) {

					assetsFilePathDrop.erase(assetsFilePathDrop.find(".ttf") + 4);

					LOG("File path: %s", assetsFilePathDrop.c_str());

				}

				else if (assetsFilePathDrop.find(".otf") != std::string::npos) {

					assetsFilePathDrop.erase(assetsFilePathDrop.find(".otf") + 4);

					LOG("File path: %s", assetsFilePathDrop.c_str());

				}


				MoveAsset(assetsFilePathDrop);
			}

			ImGui::EndDragDropTarget();

		}

	}

	if (ImGui::Button("Create Folder"))
	{
		createFolder = true;
	}

	ImGui::Dummy(ImVec2(10, 0));

	static ImGuiTextFilter filter;
	filter.Draw("Search", ImGui::GetFontSize() * 15);

	ImGui::Dummy(ImVec2(10, 0));

	if (ImGui::Button("Regenerate Library"))
	{
		DeleteAssetConfirmationPopup("Library");
	}

	ImGui::EndMenuBar();

	// End Menu Bar

	CreateNewFolder();

	if (ImGui::BeginTable("DirectoryTable", 8))
	{
		int columnCount = 0;

		// Process directories with filtering

		for (const auto& entry : std::filesystem::directory_iterator(assetsFolder)) {

			std::string entryName = entry.path().filename().string();

			if (entry.is_directory() && (entryName != "." && entryName != "..") && filter.PassFilter(entryName.c_str())) {

				ImGui::TableNextColumn();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.3f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.14f, 0.14f, 1.0f)); // Default button color

				// Display folder icon and name

				if (ImGui::ImageButton(entryName.c_str(), reinterpret_cast<void*>(static_cast<intptr_t>(folderIcon.ID)), ImVec2(64, 64)), true) {

					// ---Click event---

					if (ImGui::IsItemClicked()) {

						selectedDir = entry.path().string();

					}
					else {

						selectedDir = currentDir;

					}

					if (ImGui::IsMouseDoubleClicked(0)) {

						currentDir = selectedDir;

					}

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("tex"))
						{
							std::string assetsFilePathDrop = (const char*)payload->Data;
							if (assetsFilePathDrop.find(".png") != std::string::npos) {

								assetsFilePathDrop.erase(assetsFilePathDrop.find(".png") + 4);

								LOG("File path: %s", assetsFilePathDrop.c_str());

							}

							MoveAsset(assetsFilePathDrop);

						}

						else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("asset"))
						{
							std::string assetsFilePathDrop = (const char*)payload->Data;

							if (assetsFilePathDrop.find(".fbx") != std::string::npos) {

								assetsFilePathDrop.erase(assetsFilePathDrop.find(".fbx") + 4);

								LOG("File path: %s", assetsFilePathDrop.c_str());

							}

							else if (assetsFilePathDrop.find(".yscene") != std::string::npos) {

								assetsFilePathDrop.erase(assetsFilePathDrop.find(".yscene") + 7);

								LOG("File path: %s", assetsFilePathDrop.c_str());

							}

							else if (assetsFilePathDrop.find(".yfab") != std::string::npos) {

								assetsFilePathDrop.erase(assetsFilePathDrop.find(".yfab") + 5);

								LOG("File path: %s", assetsFilePathDrop.c_str());

							}

							else if (assetsFilePathDrop.find(".ttf") != std::string::npos) {

								assetsFilePathDrop.erase(assetsFilePathDrop.find(".ttf") + 4);

								LOG("File path: %s", assetsFilePathDrop.c_str());

							}

							else if (assetsFilePathDrop.find(".otf") != std::string::npos) {

								assetsFilePathDrop.erase(assetsFilePathDrop.find(".otf") + 4);

								LOG("File path: %s", assetsFilePathDrop.c_str());

							}

							MoveAsset(assetsFilePathDrop);

						}

						ImGui::EndDragDropTarget();

					}

					// ---RMB Click event---

					if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
					{
						ImGui::MenuItem(entryName.c_str(), NULL, false, false);
						ImGui::Separator();

						if (ImGui::MenuItem("Delete Folder"))
						{
							DeleteAssetConfirmationPopup(entry.path().string().c_str());
						}

						ImGui::EndPopup();
					}
					else
					{
						rmbMenu = false;
					}

				}

				ImGui::Text("%s", entryName.c_str());
				ImGui::PopStyleColor(2);

				++columnCount;

			}

		}

		// Process files with filtering

		for (const auto& entry : std::filesystem::directory_iterator(assetsFolder)) {

			std::string entryName = entry.path().filename().string();

			if (!entry.is_directory() && (entryName != "." && entryName != ".." && (shouldIgnoreMeta ? entryName.find(".meta") == std::string::npos : true)) && filter.PassFilter(entryName.c_str())) {

				ImGui::TableNextColumn();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Default text color for files
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.14f, 0.14f, 1.0f)); // Default button color for files

				// Process different file types

				switch (App->resourceManager->CheckExtensionType(entryName.c_str()))
				{
				case ResourceType::UNKNOWN:
				{
					ImGui::ImageButton(entryName.c_str(), reinterpret_cast<void*>(static_cast<intptr_t>(fileIcon.ID)), ImVec2(64, 64));
				}
				break;
				case ResourceType::TEXTURE:
				{
					ImGui::ImageButton(entryName.c_str(), reinterpret_cast<void*>(static_cast<intptr_t>(imageIcon.ID)), ImVec2(64, 64));

					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("tex", entry.path().string().data(), entry.path().string().length());

						ImGui::Text("Import Texture: %s", entry.path().string().c_str());

						ImGui::EndDragDropSource();
					}
				}
				break;
				case ResourceType::MESH:
				{
					ImGui::ImageButton(entryName.c_str(), reinterpret_cast<void*>(static_cast<intptr_t>(modelIcon.ID)), ImVec2(64, 64));

					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("asset", entry.path().string().data(), entry.path().string().length());

						ImGui::Text("Import Model: %s", entry.path().string().c_str());

						ImGui::EndDragDropSource();
					}

				}
				break;
				case ResourceType::SCENE:

					ImGui::ImageButton(entryName.c_str(), reinterpret_cast<void*>(static_cast<intptr_t>(sceneIcon.ID)), ImVec2(64, 64));

					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("asset", entry.path().string().data(), entry.path().string().length());

						ImGui::Text("Load Scene: %s", entry.path().string().c_str());

						ImGui::EndDragDropSource();
					}

					break;
				case ResourceType::PREFAB:

					ImGui::ImageButton(entryName.c_str(), reinterpret_cast<void*>(static_cast<intptr_t>(prefabIcon.ID)), ImVec2(64, 64));

					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("asset", entry.path().string().data(), entry.path().string().length());

						ImGui::Text("Load Prefab: %s", entry.path().string().c_str());

						ImGui::EndDragDropSource();
					}

					break;
				case ResourceType::SHADER:
				{
					ImGui::ImageButton(entryName.c_str(), reinterpret_cast<void*>(static_cast<intptr_t>(shaderIcon.ID)), ImVec2(64, 64));

					if (ImGui::IsItemClicked()) {

						shaderEditor.LoadShaderTXT(entry.path().string());

					}
				}
				break;
				case ResourceType::META:
				{
					ImGui::ImageButton(entryName.c_str(), reinterpret_cast<void*>(static_cast<intptr_t>(fileIcon.ID)), ImVec2(64, 64));
				}
				break;
				case ResourceType::ANIMATION:
				{
					ImGui::ImageButton(entryName.c_str(), reinterpret_cast<void*>(static_cast<intptr_t>(animIcon.ID)), ImVec2(64, 64));

					if ((entryName.find(".yanim") != std::string::npos)) {

						if (ImGui::BeginDragDropSource())
						{
							ImGui::SetDragDropPayload("yanim", entry.path().string().data(), entry.path().string().length());

							ImGui::Text("Import Animation: %s", entry.path().string().c_str());

							ImGui::EndDragDropSource();
						}

					}

				}
				break;
				case ResourceType::FONT:
				{
					ImGui::ImageButton(entryName.c_str(), reinterpret_cast<void*>(static_cast<intptr_t>(fontIcon.ID)), ImVec2(64, 64));

					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("font", entry.path().string().data(), entry.path().string().length());

						ImGui::Text("Load Font: %s", entry.path().string().c_str());

						ImGui::EndDragDropSource();
					}

				}
				break;
				case ResourceType::MATERIAL:
				{

				}
				break;
				case ResourceType::ALL_TYPES:
					break;
				default:
					break;
				}

				// ---RMB Click event---

				if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
				{
					selectedFile = entryName;
					currentFile = selectedFile;

					ImGui::MenuItem(selectedFile.c_str(), NULL, false, false);
					ImGui::Separator();

					if (App->resourceManager->CheckExtensionType(selectedFile.c_str()) != ResourceType::META && App->resourceManager->CheckExtensionType(selectedFile.c_str()) != ResourceType::SCENE && App->resourceManager->CheckExtensionType(selectedFile.c_str()) != ResourceType::PREFAB)
					{
						if (ImGui::MenuItem("Import to Scene"))
						{
							App->resourceManager->ImportFile(entry.path().string());
						}
					}

					if (App->resourceManager->CheckExtensionType(selectedFile.c_str()) == ResourceType::SCENE)
					{
						if (ImGui::MenuItem("Load Scene"))
						{
							PhysfsEncapsule::SplitFilePath(entryName.c_str(), nullptr, &App->scene->currentSceneFile, nullptr);
							App->scene->LoadScene(currentDir, App->scene->currentSceneFile);
						}
					}

					if (App->resourceManager->CheckExtensionType(selectedFile.c_str()) == ResourceType::PREFAB)
					{
						if (ImGui::MenuItem("Load Prefab"))
						{
							std::string fileName;
							PhysfsEncapsule::SplitFilePath(entryName.c_str(), nullptr, &fileName, nullptr);
							App->scene->LoadPrefab(currentDir, fileName);
						}
					}

					if (PhysfsEncapsule::FileExists((entry.path().string() + ".meta")))
					{
						if (ImGui::MenuItem("Delete Metadata"))
						{
							DeleteAssetConfirmationPopup((entry.path().string() + ".meta").c_str());
						}
					}

					if (ImGui::MenuItem("Delete File"))
					{
						DeleteAssetConfirmationPopup(entry.path().string().c_str());
					}

					ImGui::EndPopup();
				}

				ImGui::Text(entryName.c_str());

				ImGui::PopStyleColor(2);

				++columnCount;

			}

		}

		ImGui::EndTable();

	}

}

void ModuleEditor::MoveAsset(const std::string& assetsFilePathDrop)
{
	// Extract the file name from the path
	std::filesystem::path assetFilePath(assetsFilePathDrop);
	std::string fileName = assetFilePath.filename().string();

	// Calculate parent directory from current directory
	std::filesystem::path parentDirPath(currentDir);
	parentDirPath = parentDirPath.parent_path();
	std::string parentDir = parentDirPath.string();

	// Construct the new path by appending the file name to the parent directory
	std::filesystem::path newFilePath = std::filesystem::path(parentDir) / fileName;

	// Perform the move operation
	std::error_code ec;
	std::filesystem::rename(assetFilePath, newFilePath, ec);

	if (!ec) {
		LOG("Moved asset '%s' to directory '%s'", fileName.c_str(), currentDir.c_str());
	}
	else {
		LOG("[ERROR] moving asset: %s", ec.message().c_str());
	}
}

static bool showDeleteAssetPopup = false;
static std::string assetToDelete;

void ModuleEditor::DeleteAssetConfirmationPopup(const char* filePath) {

	showDeleteAssetPopup = true;
	assetToDelete = filePath;

}

void ModuleEditor::RenderDeleteAssetConfirmationPopup() {

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (showDeleteAssetPopup) {

		if (assetToDelete == "Library") {

			ImGui::OpenPopup("Delete Library Confirmation");

		}
		else {

			ImGui::OpenPopup("Delete Asset Confirmation");

		}

		// Display the confirmation popup
		if (ImGui::BeginPopupModal((assetToDelete == "Library" ? "Delete Library Confirmation" : "Delete Asset Confirmation"), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

			if (assetToDelete == "Library") {

				ImGui::Text("Are you sure you want to regenerate library?");

			}
			else {

				ImGui::Text("Are you sure you want to delete this asset?");
				ImGui::TextWrapped("%s", assetToDelete.c_str());

			}

			ImGui::Separator();

			if (ImGui::Button("Confirm", ImVec2(120, 0))) {

				// Call your delete function
				DeleteFileAndRefs(assetToDelete.c_str());

				// Close the popup

				ImGui::CloseCurrentPopup();
				showDeleteAssetPopup = false;

			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0))) {

				// Close the popup

				ImGui::CloseCurrentPopup();
				showDeleteAssetPopup = false;

			}

			ImGui::EndPopup();
		}

	}

}

void ModuleEditor::DrawLibraryWindow(const std::string& libraryFolder) {

	if (PhysfsEncapsule::FolderExists(libraryFolder)) {

		// Process Directories First

		for (const auto& entry : std::filesystem::directory_iterator(libraryFolder)) {

			if (entry.is_directory()) {

				std::string entryName = entry.path().filename().string();

				if (entryName != "." && entryName != "..") {

					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.3f, 1.0f));

					if (ImGui::TreeNodeEx(entryName.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)) {

						DrawLibraryWindow(entry.path().string());

						ImGui::TreePop();

					}

					ImGui::PopStyleColor();

				}

			}

		}

		// Process Files Afterwards

		for (const auto& entry : std::filesystem::directory_iterator(libraryFolder)) {

			if (!entry.is_directory()) {

				std::string entryName = entry.path().filename().string();

				if (entryName != "." && entryName != "..") {

					if ((entryName.find(".yscene") != std::string::npos) ||
						(entryName.find(".ymodel") != std::string::npos) ||
						(entryName.find(".json") != std::string::npos)) {

						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.6f, 0.6f, 1.0f));

						if (ImGui::Selectable(entryName.c_str())) {

							selectedFilePath = entry.path().string();
							showModal = true;  // Set the flag to open the modal

						}

						ImGui::PopStyleColor();

					}
					else {

						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

						ImGui::Selectable(entryName.c_str());

						if ((entryName.find(".dds") != std::string::npos)) {

							if (ImGui::BeginDragDropSource())
							{
								ImGui::SetDragDropPayload("dds", entry.path().string().data(), entry.path().string().length());

								ImGui::Text("Import Texture: %s", entry.path().string().c_str());

								ImGui::EndDragDropSource();
							}

						}

						if ((entryName.find(".ymesh") != std::string::npos)) {

							if (ImGui::BeginDragDropSource())
							{
								ImGui::SetDragDropPayload("ymesh", entry.path().string().data(), entry.path().string().length());

								ImGui::Text("Import Mesh: %s", entry.path().string().c_str());

								ImGui::EndDragDropSource();
							}

						}

						if ((entryName.find(".yanim") != std::string::npos)) {

							if (ImGui::BeginDragDropSource())
							{
								ImGui::SetDragDropPayload("yanim", entry.path().string().data(), entry.path().string().length());

								ImGui::Text("Import Animation: %s", entry.path().string().c_str());

								ImGui::EndDragDropSource();
							}

						}

						ImGui::PopStyleColor();

					}

				}

			}

		}

	}

}

// Function to handle Mouse Picking
void ModuleEditor::MousePickingManagement(const ImVec2& mousePosition, const ImVec2& sceneWindowPos, const ImVec2& sceneWindowSize, const float& sceneFrameHeightOffset) {

	ImVec2 normalizedPoint = NormalizePoint(sceneWindowPos.x, sceneWindowPos.y + (sceneFrameHeightOffset * 2), sceneWindowSize.x, sceneWindowSize.y - (sceneFrameHeightOffset * 2), mousePosition);

	// The point values should be within the range of [-1,1] for the frustum.UnProjectLineSegment() function.

	normalizedPoint.x = (normalizedPoint.x - 0.5f) / 0.5f;
	normalizedPoint.y = -((normalizedPoint.y - 0.5f) / 0.5f);

	if ((normalizedPoint.x >= -1 && normalizedPoint.x <= 1) && (normalizedPoint.y >= -1 && normalizedPoint.y <= 1))
	{
		App->camera->CreateMousePickingRay(normalizedPoint.x, normalizedPoint.y);
	}

}

// Support function to normalize the given coordinates based on the specified ImGui Window Bounding Box.
ImVec2 ModuleEditor::NormalizePoint(const float& x, const float& y, const float& w, const float& h, const ImVec2& originalPoint)
{
	ImVec2 normalizedPoint;

	normalizedPoint.x = (originalPoint.x - x) / ((x + w) - x);
	normalizedPoint.y = (originalPoint.y - y) / ((y + h) - y);

	return normalizedPoint;
}