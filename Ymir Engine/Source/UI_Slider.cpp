#include "UI_Slider.h"
#include "GameObject.h"
#include "G_UI.h"

#include "UI_Canvas.h"
#include "ModuleEditor.h"

#include "External/mmgr/mmgr.h"

UI_Slider::UI_Slider(GameObject* g, bool floats, float min, float max, float value, float x, float y, G_UI* fill, G_UI* handle, float w, float h) : C_UI(UI_TYPE::SLIDER, ComponentType::UI, g, "Slider", x, y, w, h)
{
	isInteractable = true;

	focusedColor = { 0, 1, 0, 1 };
	pressedColor = { 1, 0, 1, 1 };
	selectedColor = { 0, 0, 1, 1 };
	disabledColor = { 1, 1, 1, 1 };

	fillImage = fill;

	if (fillImage != nullptr)
	{
		fillImage->vReferences.push_back(this);
	}

	handleImage = handle;
	if (handleImage != nullptr)
	{
		handleImage->vReferences.push_back(this);
	}

	useFloat = floats;
	if (useFloat)
	{
		minValue.fValue = 0;
		maxValue.fValue = 10;
	}
	else
	{
		minValue.iValue = 0;
		maxValue.iValue = 10;
	}

	direction = SLIDER_DIRECTION::LEFT_TO_RIGHT;
	usingBar = false;
	usingHandle = false;

	// Dragging
	dragLimits.x = x;
	dragLimits.y = y;
	dragLimits.z = w;
	dragLimits.w = h;

	SetValue(value);

	// TODO: esto tiene que ser true por defecto pero ahora mismo causa problemas
	tabNav_ = false;
}

UI_Slider::~UI_Slider()
{
}

update_status UI_Slider::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	if (External->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
	{
		usingBar = false;
		usingHandle = false;
	}

	if (fillImage != nullptr && usingBar || handleImage != nullptr && usingHandle)
	{
		SliderManagement(dt);
	}

	return ret;
}

void UI_Slider::OnInspector()
{
	bool exists = true;
	ImGui::Checkbox(("##UI Slider Checkbox" + std::to_string(mOwner->UID)).c_str(), &active);		ImGui::SameLine();

	if (ImGui::CollapsingHeader(("Slider##UI Collapsing Header" + std::to_string(mOwner->UID)).c_str(), &exists, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!active) { ImGui::BeginDisabled(); }

		ImGui::Dummy(ImVec2(0, 10));

		if (ImGui::Checkbox("Interactable", &isInteractable))
		{
			if (!isInteractable)
			{
				//static_cast<UI_Image*>(handleImage->GetComponentUI(UI_TYPE::IMAGE))->color = disabledColor;
			}
		} ImGui::SameLine();

		ImGui::Dummy(ImVec2(0, 10));

		ImGui::SeparatorText("Colors");
		ImGui::ColorEdit4("Normal color", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Focused color", (float*)&focusedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Pressed color", (float*)&pressedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Selected color", (float*)&selectedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Disabled color", (float*)&disabledColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

		ImGui::Dummy(ImVec2(0, 10));

		ImGui::SeparatorText("Game Objects references");
		ImGui::Dummy(ImVec2(0, 10));

		//
		ImGui::Text("Fill Image: ");	ImGui::SameLine();
		fillImage = static_cast<G_UI*>(ImGui_GameObjectReference(fillImage));

		// Handle Image reference
		ImGui::Text("Handle Image: ");	ImGui::SameLine();
		handleImage = static_cast<G_UI*>(ImGui_GameObjectReference(handleImage));

		ImGui::Dummy(ImVec2(0, 10));

		ImGui::Separator();

		ImGui::Dummy(ImVec2(0, 10));

		const char* directions[]{ "Left to Right", "Right to Left", "Bottom to Up", "Up to Bottom" };
		if (ImGui::Combo("Direction", (int*)&direction, directions, IM_ARRAYSIZE(directions)))
		{
			if (useFloat)
			{
				SetValue(value.fValue);
			}
			else
			{
				SetValue(value.iValue);
			}
		}

		ImGui::Dummy(ImVec2(0, 10));

		if (ImGui::Checkbox("Use Floats", &useFloat))
		{
			if (useFloat)
			{
				minValue.fValue = (float)minValue.iValue;
				maxValue.fValue = (float)maxValue.iValue;

				value.fValue = (float)value.iValue;
			}
			else
			{
				minValue.iValue = (int)minValue.fValue;
				maxValue.iValue = (int)maxValue.fValue;

				value.iValue = (int)value.fValue;
			}
		}

		if (useFloat)
		{
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3);
			ImGui::DragFloat("Min Value", &minValue.fValue);
			ImGui::DragFloat("Max Value", &maxValue.fValue);
			ImGui::PopItemWidth();

			if (minValue.fValue == maxValue.fValue)
			{
				ImGui::TextColored(ImVec4(1, 0, 1, 1), "[!]");
				ImGui::SameLine();
				ImGui::Text("Min value and Max value can't be the same");
			}

			if (minValue.fValue > maxValue.fValue)
			{
				ImGui::TextColored(ImVec4(1, 0, 1, 1), "[!]");
				ImGui::SameLine();
				ImGui::Text("Min value can't be bigger than Max value");
			}

			if (ImGui::SliderFloat("Value", &value.fValue, minValue.fValue, maxValue.fValue, "%.2f"))
			{
				SetValue(value.fValue);
			}
		}
		else
		{
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3);
			ImGui::DragInt("Min Value", &minValue.iValue);
			ImGui::DragInt("Max Value", &maxValue.iValue);
			ImGui::PopItemWidth();

			if (minValue.iValue == maxValue.iValue)
			{
				ImGui::TextColored(ImVec4(1, 0, 1, 1), "[!]");
				ImGui::SameLine();
				ImGui::Text("Min value and Max value can't be the same");
			}

			if (minValue.iValue > maxValue.iValue)
			{
				ImGui::TextColored(ImVec4(1, 0, 1, 1), "[!]");
				ImGui::SameLine();
				ImGui::Text("Min value can't be bigger than Max value");
			}

			if (ImGui::SliderInt("Value", &value.iValue, minValue.iValue, maxValue.iValue))
			{
				SetValue(value.iValue);
			}
		}

		if (!active) { ImGui::EndDisabled(); }
	}
	ImGui::SameLine();

	if (!exists) { mOwner->RemoveComponent(this); }
}

void UI_Slider::Draw(bool game)
{
}

void UI_Slider::SetReference()
{
	std::map<std::string, int>::iterator it = vTempReferences.begin();

	while (it != vTempReferences.end())
	{
		if (it->first == "Fill image")
		{
			fillImage = (G_UI*)External->scene->mRootNode->FindChild(it->second);

			if (fillImage != nullptr)
			{
				fillImage->vReferences.push_back(this);
			}
		}
		else if (it->first == "Handle image")
		{
			handleImage = (G_UI*)External->scene->mRootNode->FindChild(it->second);

			if (handleImage != nullptr)
			{
				handleImage->vReferences.push_back(this);
			}
		}

		++it;
	}

	SetValue((useFloat ? value.fValue : value.iValue));
}

void UI_Slider::OnReferenceDestroyed(void* ptr)
{
	if (fillImage == ptr)
	{
		fillImage = nullptr;
	}
	else if (handleImage == ptr)
	{
		handleImage = nullptr;
	}
}

void UI_Slider::OnNormal()
{
	if (isInteractable)
	{
		//image->color = color;
	}
	else
	{
		//image->color = disabledColor;
	}
}

void UI_Slider::OnFocused()
{
	//image->color = focusedColor;
}

void UI_Slider::OnPressed()
{
	//image->color = pressedColor;

	if (fillImage != nullptr && fillImage->active)
	{
		usingBar = true;
	}

	if (handleImage != nullptr && handleImage->active)
	{
		usingHandle = true;
	}
}

void UI_Slider::OnSelected()
{
	// TODO: Sara --> test this
	if ((External->input->GetGamepadLeftJoystickPositionValueX() != 0) || External->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		if (useFloat)
		{
			ValueCalculationsFromHandles(value.fValue, dragLimits.x + dragLimits.z);
		}
		else
		{
			ValueCalculationsFromHandles(value.iValue, dragLimits.x + dragLimits.z);
		}
	}
	else if ((External->input->GetGamepadLeftJoystickPositionValueY() != 0) || External->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		if (useFloat)
		{
			ValueCalculationsFromHandles(value.fValue, dragLimits.y + dragLimits.w);
		}
		else
		{
			ValueCalculationsFromHandles(value.iValue, dragLimits.y + dragLimits.w);
		}
	}
}

void UI_Slider::OnRelease()
{
	if (fillImage != nullptr)
	{
		usingBar = false;
	}

	if (handleImage != nullptr)
	{
		usingHandle = false;
	}
}

void UI_Slider::SetValue(float val)
{
	if (useFloat)
	{
		if (val < minValue.fValue)
		{
			value.fValue = minValue.fValue;
			val = minValue.fValue;
		}
		else if (val > maxValue.fValue)
		{
			value.fValue = maxValue.fValue;
			val = maxValue.fValue;
		}
		else
		{
			value.fValue = val;
		}

		// Fill bar
		if (fillImage != nullptr)
		{
			UI_Image& img = *(UI_Image*)fillImage->GetComponentUI(UI_TYPE::IMAGE);

			if (&img != nullptr)
			{
				switch (direction)
				{
				case SLIDER_DIRECTION::LEFT_TO_RIGHT:
				{
					img.height = dragLimits.w;
					img.width = ((dragLimits.x + dragLimits.z) * val / maxValue.fValue);

					img.boundsGame->vertices[0].position = float3(img.posX, img.posY + (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[1].position = float3((img.width * img.scaleBounds.x), img.posY + (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[2].position = float3(img.posX, img.posY, 0);
					img.boundsGame->vertices[3].position = float3((img.width * img.scaleBounds.x), img.posY, 0);
				}
				break;
				case SLIDER_DIRECTION::RIGHT_TO_LEFT:
				{
					// TODO: Sara
					img.height = dragLimits.w;
					img.width = ((dragLimits.x + dragLimits.z) * val / maxValue.fValue);

					img.boundsGame->vertices[0].position = float3(img.posX, img.posY + (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[1].position = float3((img.width * img.scaleBounds.x), img.posY + (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[2].position = float3(img.posX, img.posY, 0);
					img.boundsGame->vertices[3].position = float3((img.width * img.scaleBounds.x), img.posY, 0);
				}
				break;
				case SLIDER_DIRECTION::BOTTOM_TO_UP:
				{
					img.width = dragLimits.z;
					img.height = dragLimits.w;

					img.posY = dragLimits.y + img.height + (1 - ((value.fValue - minValue.fValue) / (maxValue.fValue - minValue.fValue)) * dragLimits.w);

					img.boundsGame->vertices[0].position = float3(img.posX, img.dragLimits.y + (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[1].position = float3(img.posX + (img.width * img.scaleBounds.x), img.dragLimits.y + (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[2].position = float3(img.posX, img.posY, 0);
					img.boundsGame->vertices[3].position = float3(img.posX + (img.width * img.scaleBounds.x), img.posY, 0);
				}
				break;
				case SLIDER_DIRECTION::UP_TO_DOWN:
				{
					img.posY = dragLimits.y;
					img.width = dragLimits.z;
					img.height = ((dragLimits.y + dragLimits.w) * val / maxValue.fValue);

					img.boundsGame->vertices[0].position = float3(img.posX, (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[1].position = float3(img.posX + (img.width * img.scaleBounds.x), (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[2].position = float3(img.posX, img.posY, 0);
					img.boundsGame->vertices[3].position = float3(img.posX + (img.width * img.scaleBounds.x), img.posY, 0);
				}
				break;
				case SLIDER_DIRECTION::NONE:
					break;
				default:
					break;
				}

				img.boundsGame->RegenerateVBO();
			}
		}

		// Handle
		if (handleImage != nullptr)
		{
			UI_Image& img = *(UI_Image*)handleImage->GetComponentUI(UI_TYPE::IMAGE);

			if (direction == SLIDER_DIRECTION::LEFT_TO_RIGHT || direction == SLIDER_DIRECTION::RIGHT_TO_LEFT)
			{
				img.posX = dragLimits.x + ((val - minValue.iValue) / (maxValue.iValue - minValue.iValue)) * dragLimits.z - (img.width * img.scaleBounds.x) / 2;

				img.boundsGame->vertices[0].position = float3(img.posX, img.posY + (img.height * img.scaleBounds.y), 0);
				img.boundsGame->vertices[1].position = float3(img.posX + (img.width * img.scaleBounds.x), img.posY + (img.height * img.scaleBounds.y), 0);
				img.boundsGame->vertices[2].position = float3(img.posX, img.posY, 0);
				img.boundsGame->vertices[3].position = float3(img.posX + (img.width * img.scaleBounds.x), img.posY, 0);
			}
			else
			{
				img.posY = dragLimits.y + ((val - minValue.iValue) / (maxValue.iValue - minValue.iValue)) * dragLimits.w - (img.height * img.scaleBounds.y) / 2;

				img.boundsGame->vertices[0].position = float3(img.posX, img.posY + (img.height * img.scaleBounds.y), 0);
				img.boundsGame->vertices[1].position = float3(img.posX + (img.width * img.scaleBounds.x), img.posY + (img.height * img.scaleBounds.y), 0);
				img.boundsGame->vertices[2].position = float3(img.posX, img.posY, 0);
				img.boundsGame->vertices[3].position = float3(img.posX + (img.width * img.scaleBounds.x), img.posY, 0);
			}

			img.boundsGame->RegenerateVBO();
		}
	}
	else
	{
		if (val < minValue.iValue)
		{
			value.iValue = minValue.iValue;
			val = minValue.iValue;
		}
		else if (val > maxValue.iValue)
		{
			value.iValue = maxValue.iValue;
			val = maxValue.iValue;
		}
		else
		{
			value.iValue = val;

		}

		// Fill bar
		if (fillImage != nullptr)
		{
			UI_Image& img = *(UI_Image*)fillImage->GetComponentUI(UI_TYPE::IMAGE);

			if (&img != nullptr)
			{
				switch (direction)
				{
				case SLIDER_DIRECTION::LEFT_TO_RIGHT:
				{
					img.height = dragLimits.w;
					img.width = ((/*dragLimits.x + */dragLimits.z) * val / maxValue.iValue);

					img.boundsGame->vertices[0].position = float3(img.posX, img.posY + (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[1].position = float3((img.width * img.scaleBounds.x), img.posY + (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[2].position = float3(img.posX, img.posY, 0);
					img.boundsGame->vertices[3].position = float3((img.width * img.scaleBounds.x), img.posY, 0);
				}
				break;
				case SLIDER_DIRECTION::RIGHT_TO_LEFT:
				{
					// TODO: Sara
					img.height = dragLimits.w;
					img.width = ((/*dragLimits.x + */dragLimits.z) * val / maxValue.iValue);

					img.boundsGame->vertices[0].position = float3(img.posX, img.posY + (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[1].position = float3((img.width * img.scaleBounds.x), img.posY + (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[2].position = float3(img.posX, img.posY, 0);
					img.boundsGame->vertices[3].position = float3((img.width * img.scaleBounds.x), img.posY, 0);
				}
				break;
				case SLIDER_DIRECTION::BOTTOM_TO_UP:
				{
					img.width = dragLimits.z;
					img.height = ((/*dragLimits.y + */dragLimits.w) * val / maxValue.iValue);

					// TODO: Sara (13 --> "ARREGLO" FEO)
					img.posY = 13 + dragLimits.y + dragLimits.w + (1 - ((val - minValue.iValue) / (maxValue.iValue - minValue.iValue)) * dragLimits.w) - 1;

					img.boundsGame->vertices[0].position = float3(img.posX, dragLimits.y + (dragLimits.w * img.scaleBounds.y), 0);
					img.boundsGame->vertices[1].position = float3(img.posX + (img.width * img.scaleBounds.x), dragLimits.y + (dragLimits.w * img.scaleBounds.y), 0);
					img.boundsGame->vertices[2].position = float3(img.posX, img.posY, 0);
					img.boundsGame->vertices[3].position = float3(img.posX + (img.width * img.scaleBounds.x), img.posY, 0);
				}
				break;
				case SLIDER_DIRECTION::UP_TO_DOWN:
				{
					img.posY = dragLimits.y;
					img.width = dragLimits.z;
					img.height = ((/*dragLimits.y + */dragLimits.w) * val / maxValue.iValue);

					img.boundsGame->vertices[0].position = float3(img.posX, (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[1].position = float3(img.posX + (img.width * img.scaleBounds.x), (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[2].position = float3(img.posX, img.posY, 0);
					img.boundsGame->vertices[3].position = float3(img.posX + (img.width * img.scaleBounds.x), img.posY, 0);
				}
				break;
				case SLIDER_DIRECTION::NONE:
					break;
				default:
					break;
				}

				img.boundsGame->RegenerateVBO();
			}
		}

		// Handle
		if (handleImage != nullptr)
		{
			UI_Image& img = *(UI_Image*)handleImage->GetComponentUI(UI_TYPE::IMAGE);

			if (&img != nullptr)
			{
				if (direction == SLIDER_DIRECTION::LEFT_TO_RIGHT || direction == SLIDER_DIRECTION::RIGHT_TO_LEFT)
				{
					//img.posX = ((dragLimits.x + dragLimits.z) * val / maxValue.iValue) - (img.width * img.scaleBounds.x) / 2;
					img.posX = dragLimits.x + ((val - minValue.iValue) / (maxValue.iValue - minValue.iValue)) * dragLimits.z - (img.width * img.scaleBounds.x) / 2;

					img.boundsGame->vertices[0].position = float3(img.posX, img.posY + (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[1].position = float3(img.posX + (img.width * img.scaleBounds.x), img.posY + (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[2].position = float3(img.posX, img.posY, 0);
					img.boundsGame->vertices[3].position = float3(img.posX + (img.width * img.scaleBounds.x), img.posY, 0);
				}
				else
				{
					//img.posY = ((dragLimits.y + dragLimits.w) * val / maxValue.iValue) - (img.height * img.scaleBounds.y) / 2;
					img.posY = dragLimits.y + ((val - minValue.iValue) / (maxValue.iValue - minValue.iValue)) * dragLimits.w - (img.height * img.scaleBounds.y) / 2;

					img.boundsGame->vertices[0].position = float3(img.posX, img.posY + (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[1].position = float3(img.posX + (img.width * img.scaleBounds.x), img.posY + (img.height * img.scaleBounds.y), 0);
					img.boundsGame->vertices[2].position = float3(img.posX, img.posY, 0);
					img.boundsGame->vertices[3].position = float3(img.posX + (img.width * img.scaleBounds.x), img.posY, 0);
				}

				img.boundsGame->RegenerateVBO();
			}
		}
	}
}

void UI_Slider::SliderManagement(float dt)
{
	//if (img.state == UI_STATE::PRESSED)
	{
		int movementX = External->input->GetMouseXMotion() * dt * 50;
		int movementY = -External->input->GetMouseYMotion() * dt * 50;

		// Get the Mouse Position using ImGui.
		ImVec2 mousePosition = ImGui::GetMousePos();

		// Get the position of the ImGui window.
		ImVec2 sceneWindowPos = External->editor->gameViewPos;

		ImVec2 normalizedPoint;
		normalizedPoint.x = mousePosition.x - sceneWindowPos.x;
		normalizedPoint.y = mousePosition.y - sceneWindowPos.y;

		// Calculate new position of the slider handle
		float newX = normalizedPoint.x + movementX;
		float newY = normalizedPoint.y + movementY;

		// Calculate new position of the slider handle
		//float newX = External->input->GetMouseX() + movementX;
		//float newY = External->input->GetMouseY() + movementY;

		LOG("%f - %f", newX, newY);

		// Check if the new position is within dragLimits
		if (newX >= dragLimits.x && newX <= dragLimits.x + dragLimits.z &&
			newY >= dragLimits.y && newY <= dragLimits.y + dragLimits.w)
		{
			if (direction == SLIDER_DIRECTION::LEFT_TO_RIGHT || direction == SLIDER_DIRECTION::RIGHT_TO_LEFT)
			{
				ValueCalculationsFromHandles(newX, dragLimits.x + dragLimits.z);
			}
			else
			{
				ValueCalculationsFromHandles(newY, dragLimits.y + dragLimits.w);
			}
		}
	}
}

void UI_Slider::ValueCalculationsFromHandles(float val, float max)
{
	if (useFloat)
	{
		float aux = (val * maxValue.fValue) / max;

		SetValue(aux);
	}
	else
	{
		float aux = (val * maxValue.iValue) / max;

		SetValue(aux);
	}
}
