#include "UI_Image.h"
#include "GameObject.h"
#include "UI_Transform.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleResourceManager.h"
#include "ImporterTexture.h"

//#include "External/ImGui/imgui_custom.h"

#include "External/mmgr/mmgr.h"

UI_Image::UI_Image(GameObject* g, float x, float y, float w, float h, std::string imgPath, std::string shaderPath) : C_UI(UI_TYPE::IMAGE, ComponentType::UI, g, "Image", x, y, w, h)
{
	mat = new CMaterial(g);
	mat->shaderPath = shaderPath;
	mat->shader.LoadShader(mat->shaderPath);

	SetImg(imgPath, UI_STATE::NORMAL);

	selectedTexture = mapTextures.find(state)->second;

	ssRows = 1;
	ssColumns = 1;

	ssCoordsY = 0;
	ssCoordsX = 0;

	SetSpriteSize();

	tabNav_ = false;
}

UI_Image::~UI_Image()
{
	/*if (mapTextures.size() > 0)
	{
		for (std::map<UI_STATE, ResourceTexture*>::iterator it = mapTextures.begin(); it != mapTextures.end(); ++it)
		{
			External->resourceManager->UnloadResource(it->second->UID);
		}
		mapTextures.clear();
	}*/

	RELEASE(mat);

	//External->resourceManager->UnloadResource(selectedTexture->UID);

	selectedTexture = nullptr;
	mapTextures.clear();
}

void UI_Image::OnInspector()
{
	// Vectors of shader paths and names
	std::vector<const char*> listShaderPaths;
	std::vector<const char*> listShaderNames;

	// Manage loaded shaders
	for (auto& it = Shader::loadedShaders.begin(); it != Shader::loadedShaders.end(); ++it) {

		listShaderPaths.push_back(it->first.c_str());

		std::string shaderFileName = std::filesystem::path(it->first).stem().string();
		listShaderNames.push_back(strdup(shaderFileName.c_str())); // strdup to allocate new memory

	}

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

	ImGui::Checkbox(("##" + mOwner->name + std::to_string(ctype)).c_str(), &active);
	ImGui::SameLine();

	bool exists = true;

	if (ImGui::CollapsingHeader("Image", &exists, flags))
	{
		ImGui::Indent();

		ImGui::Spacing();

		if (!active) { ImGui::BeginDisabled(); }

		// ------------------------------------ IMAGE ------------------------------------

		ImGui::SeparatorText("Image");

		ImGui::Spacing();
		ImGui::Text("Image Width: %d", width);
		ImGui::Text("Image Height: %d", height);

		ImGui::Spacing();

		// Change Texture
		if (ImGui::BeginCombo("##Texture", selectedTexture->GetAssetsFilePath().c_str()))
		{
			for (auto& textures : mat->rTextures)
			{
				const bool is_selected = (textures == selectedTexture);
				if (ImGui::Selectable((textures->GetAssetsFilePath() + "##" + std::to_string(textures->GetUID())).c_str(), is_selected))
				{
					selectedTexture = textures;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SeparatorText("States");
		SetStateImg("Normal", UI_STATE::NORMAL);
		if (mapTextures.find(UI_STATE::NORMAL) != mapTextures.end())
		{
			ImGui::SameLine();
			ImGui::Text(mapTextures[UI_STATE::NORMAL]->GetAssetsFilePath().c_str());
		}

		SetStateImg("Focused", UI_STATE::FOCUSED);
		if (mapTextures.find(UI_STATE::FOCUSED) != mapTextures.end())
		{
			ImGui::SameLine();
			ImGui::Text(mapTextures[UI_STATE::FOCUSED]->GetAssetsFilePath().c_str());
		}

		SetStateImg("Pressed", UI_STATE::PRESSED);
		if (mapTextures.find(UI_STATE::PRESSED) != mapTextures.end())
		{
			ImGui::SameLine();
			ImGui::Text(mapTextures[UI_STATE::PRESSED]->GetAssetsFilePath().c_str());
		}

		SetStateImg("Selected", UI_STATE::SELECTED);
		if (mapTextures.find(UI_STATE::SELECTED) != mapTextures.end())
		{
			ImGui::SameLine();
			ImGui::Text(mapTextures[UI_STATE::SELECTED]->GetAssetsFilePath().c_str());
		}

		SetStateImg("Disabled", UI_STATE::DISABLED);
		if (mapTextures.find(UI_STATE::DISABLED) != mapTextures.end())
		{
			ImGui::SameLine();
			ImGui::Text(mapTextures[UI_STATE::DISABLED]->GetAssetsFilePath().c_str());
		}

		ImGui::SeparatorText("Parameters");
		if (ImGui::Button("Set Native Size", ImVec2(110, 30)))
		{
			SetNativeSize();
		}

		// Current frame
		ImGui::SeparatorText("Current frame");
		ImGui::SetNextItemWidth(70.0f);
		if (ImGui::DragInt("X", &ssCoordsX, 0.05f, 1, 0, "%d", ImGuiSliderFlags_AlwaysClamp))
		{
			SetSpriteSize();
		} ImGui::SameLine();

		ImGui::SetNextItemWidth(70.0f);
		if (ImGui::DragInt("Y", &ssCoordsY, 0.05f, 1, 0, "%d", ImGuiSliderFlags_AlwaysClamp))
		{
			SetSpriteSize();
		}

		// Rows and columns
		ImGui::SetNextItemWidth(70.0f);
		if (ImGui::DragInt("Rows", &ssRows, 0.05f, 1, 0, "%d", ImGuiSliderFlags_AlwaysClamp))
		{
			SetSpriteSize();
		} ImGui::SameLine();

		ImGui::SetNextItemWidth(70.0f);
		if (ImGui::DragInt("Columns", &ssColumns, 0.05f, 1, 0, "%d", ImGuiSliderFlags_AlwaysClamp))
		{
			SetSpriteSize();
		}

		ImGui::Spacing();

		// ------------------------------------ SHADER ------------------------------------

		ImGui::SeparatorText("SHADER");

		ImGui::Spacing();

		ImGui::Text("Shader: ");
		ImGui::SameLine();

		if (mOwner->selected) {

			// Find the index of the current shader path in listShaderPaths
			auto it = std::find(listShaderPaths.begin(), listShaderPaths.end(), mat->shaderPath);

			if (it != listShaderPaths.end()) {

				mat->selectedShader = static_cast<int>(std::distance(listShaderPaths.begin(), it));

			}

		}

		// Choose between the list of shaders
		if (ImGui::Combo("##ChooseShader", &mat->selectedShader, listShaderNames.data(), listShaderNames.size())) {

			mat->shaderDirtyFlag = true;

		}

		if (mat->shaderDirtyFlag) {

			// When shader changes, update the shader path and recompile
			mat->shaderPath = listShaderPaths[mat->selectedShader];
			mat->shader.LoadShader(mat->shaderPath);

			// Reset the dirty flag after handling the change
			mat->shaderDirtyFlag = false;

		}

		ImGui::Spacing();

		// Shader Uniforms Management

		if (mOwner->selected) {

			if (mat->shader.uniforms.size() == 0) {

				ImGui::Text("No editable uniforms.");

			}
			else {

				ImGui::Text("Uniforms:");

			}

			ImGui::Spacing();

			ImGui::Indent();

			// In case the shader has editable uniforms:
			for (auto kt = mat->shader.uniforms.begin(); kt != mat->shader.uniforms.end(); ++kt) {

				std::string label = "##" + kt->name;

				ImGui::Text("%s", kt->name.c_str());
				ImGui::SameLine();

				// Change display according to uniform type
				switch (kt->type)
				{
				case UniformType::boolean:

					ImGui::Checkbox(label.c_str(), (bool*)kt->value.get());

					mat->shader.SetUniformValue(kt->name, (bool*)kt->value.get());

					break;

				case UniformType::i1:

					ImGui::DragInt(label.c_str(), (int*)kt->value.get(), 0.1f);

					mat->shader.SetUniformValue(kt->name, (int*)kt->value.get());

					break;

				case UniformType::i2:

					ImGui::DragInt2(label.c_str(), (int*)kt->value.get(), 0.1f);

					mat->shader.SetUniformValue(kt->name, (int*)kt->value.get());

					break;

				case UniformType::i3:

					ImGui::DragInt3(label.c_str(), (int*)kt->value.get(), 0.1f);

					mat->shader.SetUniformValue(kt->name, (int*)kt->value.get());

					break;

				case UniformType::i4:

					ImGui::DragInt4(label.c_str(), (int*)kt->value.get(), 0.1f);

					mat->shader.SetUniformValue(kt->name, (int*)kt->value.get());

					break;

				case UniformType::f1:

					ImGui::DragFloat(label.c_str(), (float*)kt->value.get(), 0.1f);

					mat->shader.SetUniformValue(kt->name, (float*)kt->value.get());

					break;

				case UniformType::f2:

					ImGui::DragFloat2(label.c_str(), (float*)kt->value.get(), 0.1f);

					mat->shader.SetUniformValue(kt->name, (float*)kt->value.get());

					break;

				case UniformType::f3:

					ImGui::DragFloat3(label.c_str(), (float*)kt->value.get(), 0.1f);

					mat->shader.SetUniformValue(kt->name, (float*)kt->value.get());

					break;

				case UniformType::f4:

					ImGui::DragFloat4(label.c_str(), (float*)kt->value.get(), 0.1f);

					mat->shader.SetUniformValue(kt->name, (float*)kt->value.get());

					color.r = ((float*)kt->value.get())[0];
					color.g = ((float*)kt->value.get())[1];
					color.b = ((float*)kt->value.get())[2];
					color.a = ((float*)kt->value.get())[3];

					break;

				}

			}

			ImGui::Unindent();

		}

		ImGui::Spacing();

		// ------------------------------------ TEXTURES ------------------------------------

		ImGui::SeparatorText("TEXTURES");

		ImGui::Spacing();

		// Display texture maps of the gameobject material

		ImVec2 textureMapSize(20, 20);

		ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(mat->diffuse_ID)), textureMapSize);
		mat->DdsDragDropTarget();
		ImGui::SameLine();
		ImGui::Text("Diffuse");
		ImGui::SameLine();
		ImGui::Text("(%s)", mat->diffuse_path.c_str());

		ImGui::Spacing();

		ImGui::ColorButton("Specular", ImVec4(0, 0, 0, 0), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder, textureMapSize);
		mat->DdsDragDropTarget();
		ImGui::SameLine();
		ImGui::Text("Specular");

		ImGui::Spacing();

		ImGui::ColorButton("Normal", ImVec4(0, 0, 0, 0), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder, textureMapSize);
		mat->DdsDragDropTarget();
		ImGui::SameLine();
		ImGui::Text("Normal");

		ImGui::Spacing();

		ImGui::ColorButton("Height", ImVec4(0, 0, 0, 0), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder, textureMapSize);
		mat->DdsDragDropTarget();
		ImGui::SameLine();
		ImGui::Text("Height");

		ImGui::Spacing();

		ImGui::ColorButton("Ambient", ImVec4(0, 0, 0, 0), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder, textureMapSize);
		mat->DdsDragDropTarget();
		ImGui::SameLine();
		ImGui::Text("Ambient");

		ImGui::Spacing();

		ImGui::ColorButton("Emissive", ImVec4(0, 0, 0, 0), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder, textureMapSize);
		mat->DdsDragDropTarget();
		ImGui::SameLine();
		ImGui::Text("Emissive");

		ImGui::Spacing();

		// Utility buttons

		//if (ImGui::Button("Apply Checker Texture")) {

		//	External->renderer3D->ApplyCheckerTexture();

		//}

		ImGui::Spacing();

		if (ImGui::Button("Clear Actual Texture")) {

			External->renderer3D->ClearActualTexture();

		}

		if (!active) { ImGui::EndDisabled(); }

		ImGui::Spacing();

		ImGui::Unindent();
	}

	if (!exists) { mOwner->RemoveComponent(this); }
}

void UI_Image::Draw(bool game)
{
	UI_Bounds* boundsDrawn = nullptr;

	selectedTexture->BindTexture(true, 0);

	mat->shader.UseShader(true);
	mat->shader.SetShaderUniforms(&transformUI->mMatrixUI, mOwner->selected);

	if (game)
	{
		boundsDrawn = boundsGame;
	}

	else
	{
		boundsDrawn = boundsEditor;

		/*glPushMatrix();
		glMultMatrixf(mOwner->mTransform->mGlobalMatrix.Transposed().ptr());*/
	}

	// Render
	glBindVertexArray(boundsDrawn->VAO);

	glDrawElements(GL_TRIANGLES, boundsDrawn->indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	if (!game)
	{
		//glPopMatrix();
	}

	boundsDrawn = nullptr;

	mat->shader.UseShader(false);
	selectedTexture->BindTexture(false, 0);

}

update_status UI_Image::Update(float dt)
{
	return update_status();
}

void UI_Image::OnNormal()
{
	color = color;
	selectedTexture = mapTextures.find(state)->second;
}

void UI_Image::OnFocused()
{
	color = color;
	selectedTexture = mapTextures.find(state)->second;
}

void UI_Image::OnPressed()
{
	color = color;
	selectedTexture = mapTextures.find(state)->second;
}

void UI_Image::OnSelected()
{
	color = color;
	selectedTexture = mapTextures.find(state)->second;
}

void UI_Image::OnRelease()
{
	color = color;
	selectedTexture = mapTextures.find(state)->second;
}

void UI_Image::OnDisabled()
{
	color = color;
	selectedTexture = mapTextures.find(state)->second;
}

void UI_Image::SetImg(std::string imgPath, UI_STATE state)
{
	std::string metaFilePath = imgPath + ".meta"; // Assuming the meta file exists.

	std::unique_ptr<JsonFile> metaFile = JsonFile::GetJSON(metaFilePath);

	if (metaFile == nullptr) {

		// Chuekada para crear el meta en library :((((

		ResourceTexture* rTexMeta = new ResourceTexture();
		ImporterTexture::Import(imgPath, rTexMeta);
		RELEASE(rTexMeta);

		// Get meta

		metaFile = JsonFile::GetJSON(imgPath + ".meta");

		std::string libraryPath = metaFile->GetString("Library Path");
		
		uint UID = metaFile->GetInt("UID");
		TextureType type = ResourceTexture::GetTextureTypeFromName(metaFile->GetString("TextureType"));

		ResourceTexture* rTexTemp = (ResourceTexture*)External->resourceManager->CreateResourceFromLibrary(imgPath, ResourceType::TEXTURE, UID, type);
		rTexTemp->SetAssetsFilePath(metaFile->GetString("Assets Path"));

		auto itr = mapTextures.find(state);
		if (itr != mapTextures.end())
		{
			External->resourceManager->UnloadResource(itr->second->GetUID());
			mat->rTextures.erase(std::find(mat->rTextures.begin(), mat->rTextures.end(), itr->second));
			mat->rTextures.shrink_to_fit();

			mapTextures.erase(state);
		}

		mat->diffuse_path = imgPath;
		mat->rTextures.push_back(rTexTemp);

		mapTextures.insert({ state, rTexTemp });

	}
	else {

		std::string libraryPath = metaFile->GetString("Library Path");
		

		uint UID = metaFile->GetInt("UID");
		TextureType type = ResourceTexture::GetTextureTypeFromName(metaFile->GetString("TextureType"));

		auto itr = External->resourceManager->resources.find(UID);

		if (itr == External->resourceManager->resources.end())
		{
			// We are maintaining to Assets for now

			//rTexTemp = static_cast<ResourceTexture*>
				//(CreateResourceFromLibrary(libraryPath.c_str(), ResourceType::TEXTURE, UID, type));
			ResourceTexture* rTexTemp = static_cast<ResourceTexture*>
				(External->resourceManager->CreateResourceFromLibrary(imgPath, ResourceType::TEXTURE, UID, type));
			rTexTemp->SetAssetsFilePath(metaFile->GetString("Assets Path"));

			auto itr = mapTextures.find(state);
			if (itr != mapTextures.end())
			{
				External->resourceManager->UnloadResource(itr->second->GetUID());
				mat->rTextures.erase(std::find(mat->rTextures.begin(), mat->rTextures.end(), itr->second));
				mat->rTextures.shrink_to_fit();

				mapTextures.erase(state);
			}

			mat->diffuse_path = imgPath;
			mat->rTextures.push_back(rTexTemp);

			mapTextures.insert({ state, rTexTemp });

		}
		else
		{
			ResourceTexture* rTexTemp = static_cast<ResourceTexture*>(itr->second);
			rTexTemp->SetAssetsFilePath(metaFile->GetString("Assets Path"));
			rTexTemp->type = type;
			itr->second->IncreaseReferenceCount();

			auto itr = mapTextures.find(state);
			if (itr != mapTextures.end())
			{
				External->resourceManager->UnloadResource(itr->second->GetUID());
				mat->rTextures.erase(std::find(mat->rTextures.begin(), mat->rTextures.end(), itr->second));
				mat->rTextures.shrink_to_fit();

				mapTextures.erase(state);
			}

			mat->diffuse_path = imgPath;
			mat->rTextures.push_back(rTexTemp);

			mapTextures.insert({ state, rTexTemp });
		}

	}

}

void UI_Image::SetStateImg(const char* label, UI_STATE s)
{
	ImGui::Button(label, ImVec2(70, 30));

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("tex"))
		{
			std::string path = (const char*)payload->Data;

			// Fix ImGui problems with big sized strings. Modify if enine supports other type of imgs.
			path.erase(path.find(".png") + 4);

			SetImg(path, s);
		}

		ImGui::EndDragDropTarget();
	}

	if (state == s)
	{
		selectedTexture = mapTextures.find(state)->second;
	}
}

void UI_Image::SetNativeSize()
{
	width = selectedTexture->GetWidth();
	height = selectedTexture->GetHeight();

	dirty_ = true;
}

void UI_Image::SetSpriteSize()
{
	if (selectedTexture != nullptr)
	{
		// Calculate the size of each sprite cell in the sprite sheet
		float spriteWidth = selectedTexture->GetWidth() / ssColumns;
		float spriteHeight = selectedTexture->GetHeight() / ssRows;

		// Calculate the texture coordinates of the sprite within the sprite sheet
		float minX = ssCoordsX * spriteHeight / selectedTexture->GetHeight();
		float minY = ssCoordsY * spriteWidth / selectedTexture->GetWidth();
		float maxX = (ssCoordsX + 1) * spriteHeight / selectedTexture->GetHeight();
		float maxY = (ssCoordsY + 1) * spriteWidth / selectedTexture->GetWidth();

		// Set the texture coordinates for each vertex of the quad
		boundsGame->vertices[0].textureCoordinates = float2(minY, maxX); // Bottom-left
		boundsGame->vertices[1].textureCoordinates = float2(maxY, maxX); // Bottom-right
		boundsGame->vertices[2].textureCoordinates = float2(minY, minX); // Top-left
		boundsGame->vertices[3].textureCoordinates = float2(maxY, minX); // Top-right
	}
}
