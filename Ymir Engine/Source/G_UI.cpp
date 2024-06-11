#include "G_UI.h"

#include "UI_Canvas.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_InputBox.h"
#include "UI_CheckBox.h"
#include "UI_Slider.h"

#include "UI_Transform.h"

#include "ImporterTexture.h"

#include "External/ImGui/imgui.h"

#include "ModuleScene.h"

#include "External/mmgr/mmgr.h"

G_UI::G_UI(UI_TYPE t, GameObject* pParent, float x, float y) : GameObject("", pParent)
{
	//RemoveComponent(mTransform); // TODO: fer amb altre transform
	canvas = nullptr;

	mParent->AddChild(this);

	AddUIComponent(t, x, y, pParent);
}

G_UI::G_UI(GameObject* pParent, float x, float y) : GameObject("", pParent)
{
	//RemoveComponent(mTransform); // TODO: fer amb altre transform
	canvas = nullptr;

	mParent->AddChild(this);
}

G_UI::~G_UI()
{
	canvas = nullptr;
}

update_status G_UI::Update(float dt)
{
	for (auto it = External->scene->gameObjects.begin(); it != External->scene->gameObjects.end(); ++it) {

		if ((*it)->UID == this->UID && (*it) != this) { // If it is repeated, regenerate

			this->UID = Random::Generate();

		}

		if ((*it)->name == this->name && (*it) != this) { // If it is repeated, regenerate

			this->name = External->scene->GetUniqueName(this->name);

		}

	}

	//if (External->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN && !ImGui::GetIO().WantTextInput && !External->input->GetInputActive())
	//{
	//	LOG("Break!");
	//}

	update_status ret = UPDATE_CONTINUE;

	if (active)
	{
		if (!mChildren.empty())
		{
			for (auto i = 0; i < mChildren.size(); i++)
			{
				mChildren[i]->Update(dt);
			}
		}

		//CTransform* transformComponent = mParent->mTransform;
		if (mTransform != nullptr)
		{
			if (mTransform->dirty_)
			{
				mTransform->UpdateTransformsChilds();
				//canvas->UpdateBoundingBoxes(); Not implemented yet

				std::vector<C_UI*> listUI;
				External->renderer3D->GetUIGOs(this, listUI);

				for (auto i = 0; i < listUI.size(); i++)
				{
					listUI[i]->dirty_ = true;
				}
			}
		}

		if (mComponents.size() > 1)
		{
			for (int i = 0; i < mComponents.size(); i++)
			{
				if (mComponents[i]->ctype == ComponentType::UI)
				{
					if (TimeManager::gameTimer.GetState() == TimerState::RUNNING)
					{
						static_cast<C_UI*>(mComponents[i])->StateLogic();
					}

					if (mComponents[i]->active)
					{
						static_cast<C_UI*>(mComponents[i])->Update(dt);
					}

					if (static_cast<C_UI*>(mComponents[i])->dirty_)
					{
						static_cast<C_UI*>(mComponents[i])->UpdateUITransform();
					}

					if (static_cast<C_UI*>(mComponents[i])->isDragging)
					{
						static_cast<C_UI*>(mComponents[i])->Drag(dt);
					}
					if (static_cast<C_UI*>(mComponents[i])->fade)
					{
						static_cast<C_UI*>(mComponents[i])->FadeUI(dt);
					}
				}
			}

			if (canvas != nullptr)
			{
				canvas->UpdateUITransform(); 
			}
		}
	}

	return ret;
}

C_UI* G_UI::GetComponentUI(UI_TYPE type)
{
	C_UI* comp = nullptr;

	for (auto i = 0; i < mComponents.size(); i++)
	{
		if (mComponents[i]->ctype == ComponentType::UI && static_cast<C_UI*>(mComponents[i])->UI_type == type)
		{
			return comp = static_cast<C_UI*>(mComponents[i]);
		}
	}

	return nullptr;
}

bool G_UI::AddUIComponent(UI_TYPE type, float x, float y, GameObject* parent)
{
	bool ret = true;

	C_UI* component;

	switch (type)
	{
	case UI_TYPE::CANVAS:
	{
		if (canvas == nullptr)
		{
			UI_Canvas* comp = new UI_Canvas(this);
			mComponents.push_back(comp);

			component = comp;
		}

		name = "Canvas";
	}
	break;
	case UI_TYPE::IMAGE:
	{
		component = AddImage("Assets/ImgDefault.png", x, y);
	}
	break;
	case UI_TYPE::TEXT:
	{
		component = AddText("This is a text", x, y);
	}
	break;
	case UI_TYPE::BUTTON:
	{
		component = AddButton("Button", x, y);
	}
	break;
	case UI_TYPE::INPUTBOX:
	{
		component = AddInputBox(x, y);
	}
	break;
	case UI_TYPE::CHECKBOX:
	{
		component = AddCheckBox(false, x, y);
	}
	break;
	case UI_TYPE::SLIDER:
	{
		component = AddSlider(false, 0, 10, 0, x, y);
	}
	case UI_TYPE::NONE:
		break;
	default:
		break;
	}

	/*component->transformUI = new UI_Transform(component);
	mComponents.push_back(component->transformUI);*/

	return ret;
}

UI_Image* G_UI::AddImage(std::string imgPath, float x, float y, float w, float h, std::string shaderPath)
{
	if (External->scene->GetCanvas() == nullptr)
	{
		External->scene->CreateGUI(UI_TYPE::CANVAS);
	}
	if (mParent == External->scene->mRootNode)
	{
		ReParent(External->scene->GetCanvas());
	}

	UI_Image* comp = new UI_Image(this, x, y, w, h, imgPath, shaderPath);
	mComponents.push_back(comp);

	name = "Image";

	comp->transformUI = new UI_Transform(comp);
	mComponents.push_back(comp->transformUI);

	canvas = static_cast<G_UI*>(mParent)->canvas;

	return comp;
}

UI_Text* G_UI::AddText(const char* text, float x, float y, float fontSize, float lineSpacing, std::string fontName, std::string fontPath)
{
	if (External->scene->GetCanvas() == nullptr)
	{	
		External->scene->CreateGUI(UI_TYPE::CANVAS);
	}
	if (mParent == External->scene->mRootNode)
	{
		ReParent(External->scene->GetCanvas());
	}

	UI_Text* comp = new UI_Text(this, x, y, text, fontSize, lineSpacing, fontName, fontPath);
	mComponents.push_back(comp);

	name = "Text";

	comp->transformUI = new UI_Transform(comp);
	mComponents.push_back(comp->transformUI);

	canvas = static_cast<G_UI*>(mParent)->canvas;

	return comp;
}

UI_Button* G_UI::AddButton(const char* text, float x, float y, std::string imgPath, float w, float h)
{
	if (External->scene->GetCanvas() == nullptr)
	{
		External->scene->CreateGUI(UI_TYPE::CANVAS);
	}
	if (mParent == External->scene->mRootNode)
	{
		ReParent(External->scene->GetCanvas());
	}

	// Unity-like
	AddUIComponent(UI_TYPE::IMAGE, x, y);
	GetComponentUI(UI_TYPE::IMAGE)->width = w;
	GetComponentUI(UI_TYPE::IMAGE)->height = h;

	UI_Button* comp = new UI_Button(this, x, y, w, h);
	mComponents.push_back(comp);

	/*comp->transformUI = new UI_Transform(comp);
	mComponents.push_back(comp->transformUI);*/

	canvas = static_cast<G_UI*>(mParent)->canvas;

	G_UI* aux = External->scene->CreateGUI(UI_TYPE::TEXT, this, 20, h / 3);
	aux->GetComponentUI(UI_TYPE::TEXT)->width = w;
	aux->GetComponentUI(UI_TYPE::TEXT)->height = h;
	static_cast<UI_Text*>(aux->GetComponentUI(UI_TYPE::TEXT))->text = text;
	aux->vReferences.push_back(comp);

	comp->image = static_cast<UI_Image*>(GetComponentUI(UI_TYPE::IMAGE));
	comp->displayText = aux;

	// States
	comp->image->SetImg("Assets/juan.png", UI_STATE::FOCUSED);
	comp->image->SetImg("Assets/pato.png", UI_STATE::PRESSED);
	comp->image->SetImg("Assets/UI/Buttons.png", UI_STATE::SELECTED);
	comp->image->SetImg("Assets/penguin.png", UI_STATE::RELEASE);
	comp->image->SetImg("Assets/Lava.png", UI_STATE::DISABLED);

	name = "Button";

	aux = nullptr;

	return comp;
}

UI_InputBox* G_UI::AddInputBox(float x, float y, std::string imgPath, float w, float h)
{
	if (External->scene->GetCanvas() == nullptr)
	{
		External->scene->CreateGUI(UI_TYPE::CANVAS);
	}
	if (mParent == External->scene->mRootNode)
	{
		ReParent(External->scene->GetCanvas());
	}

	UI_InputBox* comp = new UI_InputBox(this, x, y);
	mComponents.push_back(comp);

	comp->transformUI = new UI_Transform(comp);
	mComponents.push_back(comp->transformUI);

	canvas = static_cast<G_UI*>(mParent)->canvas;

	// Unity-like
	AddUIComponent(UI_TYPE::IMAGE, x, y);
	GetComponentUI(UI_TYPE::IMAGE)->width = w;
	GetComponentUI(UI_TYPE::IMAGE)->height = h;

	G_UI* aux = External->scene->CreateGUI(UI_TYPE::TEXT, this, 20, h / 3);
	aux->GetComponentUI(UI_TYPE::TEXT)->width = w;
	aux->GetComponentUI(UI_TYPE::TEXT)->height = h;

	comp->displayText = static_cast<UI_Text*>(aux->GetComponentUI(UI_TYPE::TEXT));
	comp->image = static_cast<UI_Image*>(GetComponentUI(UI_TYPE::IMAGE));

	comp->image->SetImg(imgPath, UI_STATE::NORMAL);

	name = "Input Box";

	aux = nullptr;

	return comp;
}

UI_Checkbox* G_UI::AddCheckBox(bool checked, float x, float y, float w, float h)
{
	if (External->scene->GetCanvas() == nullptr)
	{
		External->scene->CreateGUI(UI_TYPE::CANVAS);
	}
	if (mParent == External->scene->mRootNode)
	{
		ReParent(External->scene->GetCanvas());
	}

	canvas = static_cast<G_UI*>(mParent)->canvas;

	G_UI* aux = External->scene->CreateGUI(UI_TYPE::IMAGE, this);
	aux->name = "Background";

	// Checkmark
	G_UI* aux2 = External->scene->CreateGUI(UI_TYPE::IMAGE, this);
	aux2->name = "Checkmark";

	float widthCheck = aux->GetComponentUI(UI_TYPE::IMAGE)->width;
	float heightCheck = aux->GetComponentUI(UI_TYPE::IMAGE)->height;

	// Label
	G_UI* aux3 = External->scene->CreateGUI(UI_TYPE::TEXT, this, widthCheck * 1.2, heightCheck * 0.4);
	aux3->GetComponentUI(UI_TYPE::TEXT)->width = w;
	aux3->GetComponentUI(UI_TYPE::TEXT)->height = h;

	UI_Checkbox* comp = new UI_Checkbox(this, x, y, widthCheck, heightCheck);
	mComponents.push_back(comp);

	name = "Checkbox";

	comp->transformUI = new UI_Transform(comp);
	mComponents.push_back(comp->transformUI);

	comp->isChecked = checked;

	comp->bgImg = aux;
	comp->cmImg = aux2;
	comp->cmImg->GetComponentUI(UI_TYPE::IMAGE)->active = checked;
	comp->displayText = aux3;

	aux = nullptr;
	aux2 = nullptr;
	aux3 = nullptr;

	return comp;
}

UI_Slider* G_UI::AddSlider(bool floats, float min, float max, float value, float x, float y, float w, float h, float hw, float hh)
{
	if (External->scene->GetCanvas() == nullptr)
	{
		External->scene->CreateGUI(UI_TYPE::CANVAS);
	}
	if (mParent == External->scene->mRootNode)
	{
		ReParent(External->scene->GetCanvas());
	}

	canvas = static_cast<G_UI*>(mParent)->canvas;

	// Background
	G_UI* aux = External->scene->CreateGUI(UI_TYPE::IMAGE, this, x, y);
	aux->GetComponentUI(UI_TYPE::IMAGE)->width = w;
	aux->GetComponentUI(UI_TYPE::IMAGE)->height = h;
	aux->name = "Background";

	// Fill
	G_UI* aux1 = External->scene->CreateGUI(UI_TYPE::IMAGE, this, x, y);
	aux1->GetComponentUI(UI_TYPE::IMAGE)->width = 0;
	aux1->GetComponentUI(UI_TYPE::IMAGE)->height = h;
	aux1->name = "Fill";

	// Handle
	G_UI* aux2 = External->scene->CreateGUI(UI_TYPE::IMAGE, this, x - hw / 2, y + h / 2 - hh / 2);
	aux2->GetComponentUI(UI_TYPE::IMAGE)->width = hw;
	aux2->GetComponentUI(UI_TYPE::IMAGE)->height = hh;
	aux2->name = "Handle";

	aux2->GetComponentUI(UI_TYPE::IMAGE)->dragLimits = { x, y, w, h };


	UI_Slider* comp = new UI_Slider(this, floats, min, max, value, x, y, static_cast<G_UI*>(GetChildByUID(aux1->UID)), static_cast<G_UI*>(GetChildByUID(aux2->UID)), w, h);
	mComponents.push_back(comp);

	name = "Slider";

	comp->transformUI = new UI_Transform(comp);
	mComponents.push_back(comp->transformUI);

	aux = nullptr;
	aux1 = nullptr;
	aux2 = nullptr;

	return comp;
}