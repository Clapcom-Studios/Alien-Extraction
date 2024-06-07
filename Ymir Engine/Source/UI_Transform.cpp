#include "UI_Canvas.h"

#include "UI_Transform.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"

#include "External/mmgr/mmgr.h"

UI_Transform::UI_Transform(C_UI* owner) : Component(owner->mOwner, ComponentType::UI_TRAMSFORM)
{
	componentReference = owner;

	auxPosX = componentReference->posX;
	auxPosY = componentReference->posY;
	anchorX = 0;
	anchorY = 0;

	mMatrixUI = float4x4::identity;

	anchorType = UI_ANCHOR::TOP_LEFT;
}

UI_Transform::~UI_Transform()
{
	//RELEASE(boundsEditorReference);
	//RELEASE(boundsGameReference);
}


void UI_Transform::OnInspector()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::CollapsingHeader("UI Transform", flags))
	{
		ImGui::Indent();

		auxPosX = componentReference->posX;
		auxPosY = componentReference->posY;

		if (ImGui::DragFloat("POSITION X", &componentReference->posX, 1.0f))
		{
			UpdateUITransformChilds();
			componentReference->dirty_ = true;
		}

		if (ImGui::DragFloat("POSITION Y", &componentReference->posY, 1.0f))
		{
			UpdateUITransformChilds();
			componentReference->dirty_ = true;
		}

		if (ImGui::DragFloat("WIDTH", &componentReference->width, 1.0f))
		{
			UpdateUITransformChilds();
			componentReference->dirty_ = true;
		}

		if (ImGui::DragFloat("HEIGHT", &componentReference->height, 1.0f))
		{
			UpdateUITransformChilds();
			componentReference->dirty_ = true;
		}

		if (ImGui::DragFloat3("SCALE", &componentReference->scaleBounds[0], 0.1f))
		{
			UpdateUITransformChilds();
			componentReference->dirty_ = true;
		}

		const char* anchor[]{ "TOP LEFT", "TOP", "TOP RIGHT", "LEFT", "CENTER", "RIGHT", "BOTTOM LEFT", "BOTTOM", "BOTTOM RIGHT" };
		ImGui::Combo("Direction", (int*)&anchorType, anchor, IM_ARRAYSIZE(anchor));

		ImGui::Unindent();
	}

}

void UI_Transform::UpdateUITransformChilds()
{
	std::vector<C_UI*> listUI;
	External->renderer3D->GetUIGOs(mOwner, listUI);

	for (auto i = 1; i < listUI.size(); i++)
	{
		listUI[i]->posX = listUI[i]->posX + componentReference->posX - auxPosX;
		listUI[i]->posY = listUI[i]->posY + componentReference->posY - auxPosY;
		listUI[i]->scaleBounds = componentReference->scaleBounds;
		listUI[i]->dirty_ = true;
	}
}

void UI_Transform::RecalculateAnchor()
{
	float sceneX = External->editor->gameViewSize.x - componentReference->posX;
	float sceneY = External->editor->gameViewSize.y - componentReference->posY;

	switch (anchorType)
	{
	case UI_ANCHOR::TOP_LEFT:
		anchorX = 0;
		anchorY = 0;
		break;
	case UI_ANCHOR::TOP:
		anchorY = componentReference->posY;
		break;
	case UI_ANCHOR::TOP_RIGHT:
		anchorX = sceneX;
		anchorY = componentReference->posY;
		break;
	case UI_ANCHOR::LEFT:
		anchorX = componentReference->posX;
		break;
	case UI_ANCHOR::RIGHT:
		anchorX = sceneX;
		break;
	case UI_ANCHOR::BOTTOM_LEFT:
		anchorX = componentReference->posX;
		anchorY = sceneY;
		break;
	case UI_ANCHOR::BOTTOM:
		anchorY = sceneY;
		break;
	case UI_ANCHOR::BOTTOM_RIGHT:
		anchorX = sceneX;
		anchorY = sceneY;
		break;
	default:
		break;
	}

	//float2 final_pos = { componentReference->posX / External->editor->gameViewSize.x, componentReference->posY / External->editor->gameViewSize.y };
}

update_status UI_Transform::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	return ret;
}



