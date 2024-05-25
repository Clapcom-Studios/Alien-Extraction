#pragma once
#include "Globals.h"
#include "Application.h"
#include "GameObject.h"

#include "G_UI.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_InputBox.h"
#include "UI_CheckBox.h"
#include "UI_Slider.h"
#include "UI_Transform.h"

#include "Component.h"

//
int GetUIState(MonoObject* object)
{
	G_UI* go = (G_UI*)External->moduleMono->GameObject_From_CSGO(object);
	return (int)((C_UI*)(go->GetComponent(ComponentType::UI)))->state;
}

void SetUIState(MonoObject* object, int uiState)
{
	G_UI* go = (G_UI*)External->moduleMono->GameObject_From_CSGO(object);
	std::vector<Component*> vec = go->GetAllComponentsByType(ComponentType::UI);

	for (auto it = vec.begin(); it != vec.end(); ++it)
	{
		//if (((C_UI*)(*it))->state != UI_STATE::DISABLED)
		//{
		((C_UI*)(*it))->SetState((UI_STATE)uiState);

		// TODO: if menus do not work delete this, is for inventory
		if (((C_UI*)(*it))->tabNav_ && (UI_STATE)uiState == UI_STATE::NORMAL)
		{
			if (External->scene->selectedGO != nullptr && External->scene->selectedGO->UID == go->UID)
			{
				std::vector<Component*> listComponents = External->scene->focusedUIGO->GetAllComponentsByType(ComponentType::UI);
				for (auto it = listComponents.begin(); it != listComponents.end(); ++it)
				{
					if (((C_UI*)(*it))->tabNav_)
					{
						((C_UI*)(*it))->SetState((UI_STATE)uiState);
					}
				}

				External->scene->selectedGO = nullptr;
				External->scene->SetSelected();
			}
		}

		if (((C_UI*)(*it))->tabNav_ && (UI_STATE)uiState == UI_STATE::FOCUSED)
		{
			int offset = 0;
			std::vector<C_UI*> listOffset;
			for (int i = 0; i < External->scene->vCanvas.size(); ++i)
			{
				External->scene->GetUINavigate(External->scene->vCanvas[i], listOffset);
			}

			for (auto i = 0; i < listOffset.size(); i++)
			{
				if (listOffset[i]->GetUID() != (int)(C_UI*)(*it)->GetUID())
				{
					offset++;
				}

				else
				{
					break;
				}
			}

			External->scene->onHoverUI = offset;

			if (External->scene->focusedUIGO != nullptr)
			{
				std::vector<Component*> listComponents = External->scene->focusedUIGO->GetAllComponentsByType(ComponentType::UI);
				for (auto it = listComponents.begin(); it != listComponents.end(); ++it)
				{
					if (((C_UI*)(*it))->tabNav_)
					{
						((C_UI*)(*it))->SetState(UI_STATE::NORMAL);
					}
				}
			}

			External->scene->focusedUIGO = ((C_UI*)(*it))->mOwner;
			External->scene->SetSelected(((C_UI*)(*it))->mOwner);

			std::vector<Component*> listComponentsNew = External->scene->focusedUIGO->GetAllComponentsByType(ComponentType::UI);

			for (auto it = listComponentsNew.begin(); it != listComponentsNew.end(); ++it)
			{
				if (((C_UI*)(*it))->tabNav_)
				{
					((C_UI*)(*it))->SetState(UI_STATE::FOCUSED);
				}
			}
		}
		//}
	}
}

bool GetCanNav()
{
	return External->scene->canNav;
}

void SetCanNav(bool set)
{
	External->scene->canNav = set;
}

MonoObject* CreateImageUI(MonoObject* pParent, MonoString* newImage, int x, int y)
{
	GameObject* ui_gameObject = External->moduleMono->GameObject_From_CSGO(pParent);
	std::string _newImage = mono_string_to_utf8(newImage);

	G_UI* tempGameObject = new G_UI(External->scene->mRootNode, 0, 0);

	tempGameObject->AddImage(_newImage, x, y, 100, 100);

	External->scene->PostUpdateCreateGameObject_UI((GameObject*)tempGameObject);

	return External->moduleMono->GoToCSGO(tempGameObject);
}

void ChangeImageUI(MonoObject* go, MonoString* newImage, int state)
{
	GameObject* go_image_to_change = External->moduleMono->GameObject_From_CSGO(go);
	std::string _newImage = mono_string_to_utf8(newImage);

	if (go_image_to_change != nullptr)
	{
		UI_Image* image_to_change = static_cast<UI_Image*>(static_cast<G_UI*>(go_image_to_change)->GetComponentUI(UI_TYPE::IMAGE));
		image_to_change->SetImg(_newImage, (UI_STATE)state);
	}
}

// Image Animations
int GetImageRows(MonoObject* object)
{
	G_UI* go = (G_UI*)External->moduleMono->GameObject_From_CSGO(object);
	return static_cast<UI_Image*>(go->GetComponentUI(UI_TYPE::IMAGE))->ssRows;
}

int GetImageColumns(MonoObject* object)
{
	G_UI* go = (G_UI*)External->moduleMono->GameObject_From_CSGO(object);
	return static_cast<UI_Image*>(go->GetComponentUI(UI_TYPE::IMAGE))->ssColumns;
}

int GetImageCurrentFrameX(MonoObject* object)
{
	G_UI* go = (G_UI*)External->moduleMono->GameObject_From_CSGO(object);
	return static_cast<UI_Image*>(go->GetComponentUI(UI_TYPE::IMAGE))->ssCoordsX;
}

int GetImageCurrentFrameY(MonoObject* object)
{
	G_UI* go = (G_UI*)External->moduleMono->GameObject_From_CSGO(object);
	return static_cast<UI_Image*>(go->GetComponentUI(UI_TYPE::IMAGE))->ssCoordsY;
}

void SetImageCurrentFrame(MonoObject* object, int x, int y)
{
	G_UI* go = (G_UI*)External->moduleMono->GameObject_From_CSGO(object);
	static_cast<UI_Image*>(go->GetComponentUI(UI_TYPE::IMAGE))->ssCoordsX = x;
	static_cast<UI_Image*>(go->GetComponentUI(UI_TYPE::IMAGE))->ssCoordsY = y;

	static_cast<UI_Image*>(go->GetComponentUI(UI_TYPE::IMAGE))->SetSpriteSize();
}

//
void TextEdit(MonoObject* object, MonoString* text)
{
	G_UI* go = (G_UI*)External->moduleMono->GameObject_From_CSGO(object);
	static_cast<UI_Text*>(go->GetComponentUI(UI_TYPE::TEXT))->SetText(mono_string_to_utf8(text));
}

void SliderEdit(MonoObject* object, double value)
{
	G_UI* go = (G_UI*)External->moduleMono->GameObject_From_CSGO(object);
	static_cast<UI_Slider*>(go->GetComponentUI(UI_TYPE::SLIDER))->SetValue(value);
}

void SliderSetRange(MonoObject* object, double min, double max)
{
	G_UI* go = (G_UI*)External->moduleMono->GameObject_From_CSGO(object);

	if (static_cast<UI_Slider*>(go->GetComponentUI(UI_TYPE::SLIDER))->useFloat)
	{
		static_cast<UI_Slider*>(go->GetComponentUI(UI_TYPE::SLIDER))->minValue.fValue = min;
		static_cast<UI_Slider*>(go->GetComponentUI(UI_TYPE::SLIDER))->maxValue.fValue = max;
	}
	else
	{
		static_cast<UI_Slider*>(go->GetComponentUI(UI_TYPE::SLIDER))->minValue.iValue = min;
		static_cast<UI_Slider*>(go->GetComponentUI(UI_TYPE::SLIDER))->maxValue.iValue = max;
	}
}

void SliderSetMin(MonoObject* object, double value)
{
	G_UI* go = (G_UI*)External->moduleMono->GameObject_From_CSGO(object);

	if (static_cast<UI_Slider*>(go->GetComponentUI(UI_TYPE::SLIDER))->useFloat)
	{
		static_cast<UI_Slider*>(go->GetComponentUI(UI_TYPE::SLIDER))->minValue.fValue = value;
	}
	else
	{
		static_cast<UI_Slider*>(go->GetComponentUI(UI_TYPE::SLIDER))->minValue.iValue = value;
	}
}

void SliderSetMax(MonoObject* object, double value)
{
	G_UI* go = (G_UI*)External->moduleMono->GameObject_From_CSGO(object);

	if (static_cast<UI_Slider*>(go->GetComponentUI(UI_TYPE::SLIDER))->useFloat)
	{
		static_cast<UI_Slider*>(go->GetComponentUI(UI_TYPE::SLIDER))->maxValue.fValue = value;
	}
	else
	{
		static_cast<UI_Slider*>(go->GetComponentUI(UI_TYPE::SLIDER))->maxValue.iValue = value;
	}
}

// Inventory

MonoObject* GetSelected()
{
	if (External->scene->selectedUIGO != nullptr)
	{
		return External->moduleMono->GoToCSGO(External->scene->selectedUIGO);
	}

	return nullptr;
}

MonoObject* GetFocused()
{
	if (External->scene->focusedUIGO != nullptr)
	{
		return External->moduleMono->GoToCSGO(External->scene->focusedUIGO);
	}

	return nullptr;
}

void SwitchPosition(MonoObject* selectedObject, MonoObject* targetObject)
{
	GameObject* selectedgo = External->moduleMono->GameObject_From_CSGO(selectedObject);
	CTransform* selectedTransform = selectedgo->mTransform;

	GameObject* targetgo = (G_UI*)External->moduleMono->GameObject_From_CSGO(targetObject);
	CTransform* targetTransform = targetgo->mTransform;

	float3 auxPos = float3(targetTransform->translation);

	targetTransform->SetPosition(selectedTransform->translation);
	selectedTransform->SetPosition(auxPos);

	for (int i = 0; i < External->scene->selectedUIGO->mComponents.size(); i++)
	{
		if (External->scene->selectedUIGO->mComponents[i]->ctype == ComponentType::UI)
		{
			if (static_cast<C_UI*>(External->scene->selectedUIGO->mComponents[i])->state == UI_STATE::SELECTED)
			{
				static_cast<C_UI*>(External->scene->selectedUIGO->mComponents[i])->state = UI_STATE::FOCUSED;
			}
		}
	}

	External->scene->swapList.insert(std::pair<GameObject*, GameObject*>(selectedgo, targetgo));

	// Switch name required for crafting, doesn't affect rest of scripts
	std::string aux = selectedgo->name;
	selectedgo->name = targetgo->name;
	targetgo->name = aux;
	//selectedgo->SwapChildren(targetgo);

	External->scene->focusedUIGO = External->scene->selectedUIGO;
	External->scene->selectedUIGO = nullptr;

}

void NavigateGridHorizontal(MonoObject* go, int rows, int columns, bool isRight, bool navigateGrids, MonoObject* gridLeft, MonoObject* gridRight, bool bounce, int childNavigate = 0, bool isEmpty = false)
{
	if (External->scene->canNav)
	{
		// Get UI elements to navigate
		std::vector<C_UI*> listUI;
		GameObject* gameObject = External->moduleMono->GameObject_From_CSGO(go);
		External->scene->GetUINavigate(gameObject, listUI); bool isInGO = false;
		int offset = 0;

		std::vector<C_UI*> listOffset;
		for (int i = 0; i < External->scene->vCanvas.size(); ++i)
		{
			External->scene->GetUINavigate(External->scene->vCanvas[i], listOffset);
		}


		if (isEmpty)
		{
			std::vector<C_UI*> listCompare;
			External->scene->GetUINavigate(gameObject->mChildren[0], listCompare);

			for (auto i = 0; i < listOffset.size(); i++)
			{
				if (listOffset[i]->mOwner->UID != listCompare[0]->mOwner->UID)
				{
					offset++;
				}

				else
				{
					break;
				}
			}
		}

		else
		{

			for (auto i = 0; i < listOffset.size(); i++)
			{
				if (listOffset[i]->mOwner->UID != gameObject->mChildren[0]->UID)
				{
					offset++;
				}

				else
				{
					break;
				}
			}
		}

		if (External->scene->focusedUIGO == nullptr)
		{
			External->scene->SetSelected(listUI[0]->mOwner);
			External->scene->focusedUIGO = listUI[0]->mOwner;
			External->scene->onHoverUI = offset;
		}

		else
		{
			for (auto i = 0; i < listUI.size(); i++)
			{
				if (External->scene->focusedUIGO != nullptr)
				{
					if (listUI[i]->mOwner->UID == External->scene->focusedUIGO->UID)
					{
						isInGO = true;
						break;
					}
				}
			}
		}

		if (isInGO)
		{
			if (isRight)
			{
				if (External->scene->onHoverUI + rows >= listUI.size() + offset)
				{
					if (navigateGrids)
					{
						bool isBlocked = false;

						GameObject* gridGo = External->moduleMono->GameObject_From_CSGO(gridRight);

						if (gridGo != nullptr)
						{
							std::vector<C_UI*> listUIGrid;
							External->scene->GetUINavigate(gridGo, listUIGrid);

							if (listUIGrid.size() != 0)
							{
								std::vector<Component*> listComponents = listUIGrid[childNavigate]->mOwner->GetAllComponentsByType(ComponentType::UI);

								for (auto it = listComponents.begin(); it != listComponents.end(); ++it)
								{
									if (((C_UI*)(*it))->tabNav_)
									{
										if (((C_UI*)(*it))->state == UI_STATE::DISABLED)
										{
											isBlocked = true;
											break;
										}
									}
								}

								if (!isBlocked)
								{
									if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
									{
										listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
									}

									SetUIState(External->moduleMono->GoToCSGO(listUIGrid[childNavigate]->mOwner), (int)UI_STATE::FOCUSED);

									External->scene->canNav = false;
								}
							}
						}

						else if (bounce)
						{
							if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
							{
								listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
							}

							External->scene->SetSelected(listUI[External->scene->onHoverUI - offset - (rows * (columns - 1))]->mOwner);

							External->scene->focusedUIGO = listUI[External->scene->onHoverUI - offset - (rows * (columns - 1))]->mOwner;

							if (listUI[External->scene->onHoverUI - offset - (rows * (columns - 1))]->state != UI_STATE::SELECTED)
							{
								listUI[External->scene->onHoverUI - offset - (rows * (columns - 1))]->SetState(UI_STATE::FOCUSED);
							}

							External->scene->onHoverUI -= (rows * (columns - 1));
						}
					}

					else
					{
						if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
						{
							listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
						}

						// Same as below, should make a function
						External->scene->SetSelected(listUI[External->scene->onHoverUI - offset - (rows * (columns - 1))]->mOwner);

						External->scene->focusedUIGO = listUI[External->scene->onHoverUI - offset - (rows * (columns - 1))]->mOwner;

						if (listUI[External->scene->onHoverUI - offset - (rows * (columns - 1))]->state != UI_STATE::SELECTED)
						{
							listUI[External->scene->onHoverUI - offset - (rows * (columns - 1))]->SetState(UI_STATE::FOCUSED);
						}

						External->scene->onHoverUI -= (rows * (columns - 1));
					}
				}

				else
				{
					External->scene->SetSelected(listUI[External->scene->onHoverUI - offset + rows]->mOwner);

					External->scene->focusedUIGO = listUI[External->scene->onHoverUI - offset + rows]->mOwner;

					if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
					{
						listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
					}

					if (listUI[External->scene->onHoverUI - offset + rows]->state != UI_STATE::SELECTED)
					{
						listUI[External->scene->onHoverUI - offset + rows]->SetState(UI_STATE::FOCUSED);
					}

					External->scene->onHoverUI += rows;
				}
			}

			else
			{
				if (External->scene->onHoverUI - rows < offset)
				{
					if (navigateGrids)
					{
						bool isBlocked = false;

						GameObject* gridGo = External->moduleMono->GameObject_From_CSGO(gridLeft);

						if (gridGo != nullptr)
						{
							std::vector<C_UI*> listUIGrid;
							External->scene->GetUINavigate(gridGo, listUIGrid);

							if (listUIGrid.size() != 0)
							{
								std::vector<Component*> listComponents = listUIGrid[childNavigate]->mOwner->GetAllComponentsByType(ComponentType::UI);

								for (auto it = listComponents.begin(); it != listComponents.end(); ++it)
								{
									if (((C_UI*)(*it))->tabNav_)
									{
										if (((C_UI*)(*it))->state == UI_STATE::DISABLED)
										{
											isBlocked = true;
											break;
										}
									}
								}

								if (!isBlocked)
								{
									if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
									{
										listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
									}

									SetUIState(External->moduleMono->GoToCSGO(listUIGrid[childNavigate]->mOwner), (int)UI_STATE::FOCUSED);

									External->scene->canNav = false;
								}
							}
						}

						else if (bounce)
						{
							if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
							{
								listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
							}

							External->scene->SetSelected(listUI[External->scene->onHoverUI - offset + (rows * (columns - 1))]->mOwner);
							External->scene->focusedUIGO = listUI[External->scene->onHoverUI - offset + (rows * (columns - 1))]->mOwner;

							if (listUI[External->scene->onHoverUI - offset + (rows * (columns - 1))]->state != UI_STATE::SELECTED)
							{
								listUI[External->scene->onHoverUI - offset + (rows * (columns - 1))]->SetState(UI_STATE::FOCUSED);
							}

							External->scene->onHoverUI += (rows * (columns - 1));
						}
					}

					else
					{
						if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
						{
							listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
						}

						External->scene->SetSelected(listUI[External->scene->onHoverUI - offset + (rows * (columns - 1))]->mOwner);
						External->scene->focusedUIGO = listUI[External->scene->onHoverUI - offset + (rows * (columns - 1))]->mOwner;

						if (listUI[External->scene->onHoverUI - offset + (rows * (columns - 1))]->state != UI_STATE::SELECTED)
						{
							listUI[External->scene->onHoverUI - offset + (rows * (columns - 1))]->SetState(UI_STATE::FOCUSED);
						}

						External->scene->onHoverUI += (rows * (columns - 1));
					}
				}

				else
				{
					External->scene->SetSelected(listUI[External->scene->onHoverUI - offset - rows]->mOwner);
					External->scene->focusedUIGO = listUI[External->scene->onHoverUI - offset - rows]->mOwner;

					if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
					{
						listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
					}

					if (listUI[External->scene->onHoverUI - offset - rows]->state != UI_STATE::SELECTED)
					{
						listUI[External->scene->onHoverUI - offset - rows]->SetState(UI_STATE::FOCUSED);
					}

					External->scene->onHoverUI -= rows;
				}

			}
		}
	}
}

void NavigateGridVertical(MonoObject* go, int rows, int columns, bool isDown, bool navigateGrids, MonoObject* gridDown, MonoObject* gridUp, bool bounce, int childNavigate = 0, bool isEmpty = false)
{
	if (External->scene->canNav)
	{
		// Get UI elements to navigate
		std::vector<C_UI*> listUI;
		GameObject* gameObject = External->moduleMono->GameObject_From_CSGO(go);
		External->scene->GetUINavigate(gameObject, listUI); bool isInGO = false;
		int offset = 0;

		std::vector<C_UI*> listOffset;
		for (int i = 0; i < External->scene->vCanvas.size(); ++i)
		{
			External->scene->GetUINavigate(External->scene->vCanvas[i], listOffset);
		}

		if (isEmpty)
		{
			std::vector<C_UI*> listCompare;
			External->scene->GetUINavigate(gameObject->mChildren[0], listCompare);

			for (auto i = 0; i < listOffset.size(); i++)
			{
				if (listOffset[i]->mOwner->UID != listCompare[0]->mOwner->UID)
				{
					offset++;
				}

				else
				{
					break;
				}
			}
		}

		else
		{

			for (auto i = 0; i < listOffset.size(); i++)
			{
				if (listOffset[i]->mOwner->UID != gameObject->mChildren[0]->UID)
				{
					offset++;
				}

				else
				{
					break;
				}
			}
		}

		if (External->scene->focusedUIGO == nullptr)
		{
			External->scene->focusedUIGO = listUI[0]->mOwner;
			External->scene->SetSelected(listUI[0]->mOwner);
			External->scene->onHoverUI = offset;
		}

		else
		{
			for (auto i = 0; i < listUI.size(); i++)
			{
				if (External->scene->focusedUIGO != nullptr)
				{
					if (listUI[i]->mOwner->UID == External->scene->focusedUIGO->UID)
					{
						isInGO = true;
						break;
					}
				}
			}
		}

		if (isInGO)
		{
			if (isDown)
			{
				if ((External->scene->onHoverUI - offset + 1) % rows == 0 || External->scene->onHoverUI - offset == listUI.size() - 1)
				{
					if (navigateGrids)
					{
						bool isBlocked = false;

						GameObject* gridGo = External->moduleMono->GameObject_From_CSGO(gridDown);

						if (gridGo != nullptr)
						{
							std::vector<C_UI*> listUIGrid;
							External->scene->GetUINavigate(gridGo, listUIGrid);

							if (listUIGrid.size() != 0)
							{
								std::vector<Component*> listComponents = listUIGrid[childNavigate]->mOwner->GetAllComponentsByType(ComponentType::UI);

								for (auto it = listComponents.begin(); it != listComponents.end(); ++it)
								{
									if (((C_UI*)(*it))->tabNav_)
									{
										if (((C_UI*)(*it))->state == UI_STATE::DISABLED)
										{
											isBlocked = true;
											break;
										}
									}
								}

								if (!isBlocked)
								{
									if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
									{
										listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
									}

									SetUIState(External->moduleMono->GoToCSGO(listUIGrid[childNavigate]->mOwner), (int)UI_STATE::FOCUSED);

									External->scene->canNav = false;
								}
							}
						}

						else if (bounce)
						{
							if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
							{
								listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
							}

							External->scene->SetSelected(listUI[External->scene->onHoverUI - offset - rows + 1]->mOwner);

							External->scene->focusedUIGO = listUI[External->scene->onHoverUI - offset - rows + 1]->mOwner;

							if (listUI[External->scene->onHoverUI - offset - rows + 1]->state != UI_STATE::SELECTED)
							{
								listUI[External->scene->onHoverUI - offset - rows + 1]->SetState(UI_STATE::FOCUSED);
							}

							External->scene->onHoverUI -= (rows - 1);
						}
					}

					else
					{
						if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
						{
							listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
						}

						// Same as below, should make a function
						External->scene->SetSelected(listUI[External->scene->onHoverUI - offset - rows + 1]->mOwner);

						External->scene->focusedUIGO = listUI[External->scene->onHoverUI - offset - rows + 1]->mOwner;

						if (listUI[External->scene->onHoverUI - offset - rows + 1]->state != UI_STATE::SELECTED)
						{
							listUI[External->scene->onHoverUI - offset - rows + 1]->SetState(UI_STATE::FOCUSED);
						}

						External->scene->onHoverUI -= (rows - 1);
					}
				}

				else
				{
					External->scene->SetSelected(listUI[External->scene->onHoverUI - offset + 1]->mOwner);

					External->scene->focusedUIGO = listUI[External->scene->onHoverUI - offset + 1]->mOwner;

					if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
					{
						listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
					}

					if (listUI[External->scene->onHoverUI - offset + 1]->state != UI_STATE::SELECTED)
					{
						listUI[External->scene->onHoverUI - offset + 1]->SetState(UI_STATE::FOCUSED);
					}

					External->scene->onHoverUI += 1;
				}
			}

			else
			{
				if ((External->scene->onHoverUI - offset) % rows == 0 || External->scene->onHoverUI - offset == 0)
				{
					if (navigateGrids)
					{
						bool isBlocked = false;

						GameObject* gridGo = External->moduleMono->GameObject_From_CSGO(gridUp);

						if (gridGo != nullptr)
						{
							std::vector<C_UI*> listUIGrid;
							External->scene->GetUINavigate(gridGo, listUIGrid);

							if (listUIGrid.size() != 0)
							{
								std::vector<Component*> listComponents = listUIGrid[childNavigate]->mOwner->GetAllComponentsByType(ComponentType::UI);

								for (auto it = listComponents.begin(); it != listComponents.end(); ++it)
								{
									if (((C_UI*)(*it))->tabNav_)
									{
										if (((C_UI*)(*it))->state == UI_STATE::DISABLED)
										{
											isBlocked = true;
											break;
										}
									}
								}

								if (!isBlocked)
								{
									if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
									{
										listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
									}

									SetUIState(External->moduleMono->GoToCSGO(listUIGrid[childNavigate]->mOwner), (int)UI_STATE::FOCUSED);

									External->scene->canNav = false;
								}
							}
						}

						else if (bounce)
						{
							if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
							{
								listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
							}

							External->scene->SetSelected(listUI[External->scene->onHoverUI - offset + rows - 1]->mOwner);
							External->scene->focusedUIGO = listUI[External->scene->onHoverUI - offset + rows - 1]->mOwner;

							if (listUI[External->scene->onHoverUI - offset + rows - 1]->state != UI_STATE::SELECTED)
							{
								listUI[External->scene->onHoverUI - offset + rows - 1]->SetState(UI_STATE::FOCUSED);
							}

							External->scene->onHoverUI += (rows - 1);
						}
					}

					else
					{
						if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
						{
							listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
						}

						External->scene->SetSelected(listUI[External->scene->onHoverUI - offset + rows - 1]->mOwner);
						External->scene->focusedUIGO = listUI[External->scene->onHoverUI - offset + rows - 1]->mOwner;

						if (listUI[External->scene->onHoverUI - offset + rows - 1]->state != UI_STATE::SELECTED)
						{
							listUI[External->scene->onHoverUI - offset + rows - 1]->SetState(UI_STATE::FOCUSED);
						}

						External->scene->onHoverUI += (rows - 1);
					}
				}

				else
				{
					External->scene->SetSelected(listUI[External->scene->onHoverUI - offset - 1]->mOwner);
					External->scene->focusedUIGO = listUI[External->scene->onHoverUI - offset - 1]->mOwner;

					if (listUI[External->scene->onHoverUI - offset]->state != UI_STATE::SELECTED)
					{
						listUI[External->scene->onHoverUI - offset]->SetState(UI_STATE::NORMAL);
					}

					if (listUI[External->scene->onHoverUI - offset - 1]->state != UI_STATE::SELECTED)
					{
						listUI[External->scene->onHoverUI - offset - 1]->SetState(UI_STATE::FOCUSED);
					}

					External->scene->onHoverUI -= 1;
				}
			}
		}
	}
}

void NavigateCraftingHorizontal(MonoObject* go, int rows, int columns, bool isRight, bool navigateGrids, MonoObject* gridLeft, MonoObject* gridRight, bool bounce)
{
	if (External->scene->canNav)
	{
		// Get UI elements to navigate
		std::vector<C_UI*> listUI;
		GameObject* gameObject = External->moduleMono->GameObject_From_CSGO(go);
		External->scene->GetUINavigate(gameObject, listUI); bool isInGO = false;
		int offset = 0;

		for (auto i = 0; i < listUI.size(); i++)
		{
			if (listUI[i]->mOwner->UID == External->scene->focusedUIGO->UID)
			{
				offset = i;
				break;
			}
		}

		if (External->scene->focusedUIGO == nullptr)
		{
			External->scene->focusedUIGO = listUI[0]->mOwner;
			External->scene->SetSelected(listUI[0]->mOwner);
			External->scene->onHoverUI = offset;
		}

		else
		{
			for (auto i = 0; i < listUI.size(); i++)
			{
				if (External->scene->focusedUIGO != nullptr)
				{
					if (listUI[i]->mOwner->UID == External->scene->focusedUIGO->UID)
					{
						isInGO = true;
						break;
					}
				}
			}
		}

		if (isInGO)
		{
			if (isRight)
			{
				if (offset >= listUI.size())
				{
					if (navigateGrids)
					{
						bool isBlocked = false;

						GameObject* gridGo = External->moduleMono->GameObject_From_CSGO(gridRight);

						if (gridGo != nullptr)
						{
							std::vector<C_UI*> listUIGrid;
							External->scene->GetUINavigate(gridGo, listUIGrid);

							if (listUIGrid.size() != 0)
							{
								std::vector<Component*> listComponents = listUIGrid[0]->mOwner->GetAllComponentsByType(ComponentType::UI);

								for (auto it = listComponents.begin(); it != listComponents.end(); ++it)
								{
									if (((C_UI*)(*it))->tabNav_)
									{
										if (((C_UI*)(*it))->state == UI_STATE::DISABLED)
										{
											isBlocked = true;
											break;
										}
									}
								}

								if (!isBlocked)
								{
									if (listUI[offset]->state != UI_STATE::SELECTED)
									{
										listUI[offset]->SetState(UI_STATE::NORMAL);
									}

									SetUIState(External->moduleMono->GoToCSGO(listUIGrid[0]->mOwner), (int)UI_STATE::FOCUSED);

									External->scene->canNav = false;
								}
							}
						}

						else if (bounce)
						{
							if (listUI[offset]->state != UI_STATE::SELECTED)
							{
								listUI[offset]->SetState(UI_STATE::NORMAL);
							}

							External->scene->SetSelected(listUI[offset - (rows * (columns - 1))]->mOwner);

							External->scene->focusedUIGO = listUI[offset - (rows * (columns - 1))]->mOwner;

							if (listUI[offset - (rows * (columns - 1))]->state != UI_STATE::SELECTED)
							{
								listUI[offset - (rows * (columns - 1))]->SetState(UI_STATE::FOCUSED);
							}

							External->scene->onHoverUI -= (rows * (columns - 1));
						}
					}

					else
					{
						if (listUI[offset]->state != UI_STATE::SELECTED)
						{
							listUI[offset]->SetState(UI_STATE::NORMAL);
						}

						// Same as below, should make a function
						External->scene->SetSelected(listUI[offset - (rows * (columns - 1))]->mOwner);

						External->scene->focusedUIGO = listUI[offset - (rows * (columns - 1))]->mOwner;

						if (listUI[offset - (rows * (columns - 1))]->state != UI_STATE::SELECTED)
						{
							listUI[offset - (rows * (columns - 1))]->SetState(UI_STATE::FOCUSED);
						}

						External->scene->onHoverUI -= (rows * (columns - 1));
					}
				}

				else
				{
					External->scene->SetSelected(listUI[offset + rows]->mOwner);

					External->scene->focusedUIGO = listUI[offset + rows]->mOwner;

					if (listUI[offset]->state != UI_STATE::SELECTED)
					{
						listUI[offset]->SetState(UI_STATE::NORMAL);
					}

					if (listUI[offset + rows]->state != UI_STATE::SELECTED)
					{
						listUI[offset + rows]->SetState(UI_STATE::FOCUSED);
					}

					External->scene->onHoverUI += rows;
				}
			}

			else
			{
				if (rows < offset)
				{
					if (navigateGrids)
					{
						bool isBlocked = false;

						GameObject* gridGo = External->moduleMono->GameObject_From_CSGO(gridLeft);

						if (gridGo != nullptr)
						{
							std::vector<C_UI*> listUIGrid;
							External->scene->GetUINavigate(gridGo, listUIGrid);

							if (listUIGrid.size() != 0)
							{
								std::vector<Component*> listComponents = listUIGrid[0]->mOwner->GetAllComponentsByType(ComponentType::UI);

								for (auto it = listComponents.begin(); it != listComponents.end(); ++it)
								{
									if (((C_UI*)(*it))->tabNav_)
									{
										if (((C_UI*)(*it))->state == UI_STATE::DISABLED)
										{
											isBlocked = true;
											break;
										}
									}
								}

								if (!isBlocked)
								{
									if (listUI[offset]->state != UI_STATE::SELECTED)
									{
										listUI[offset]->SetState(UI_STATE::NORMAL);
									}

									SetUIState(External->moduleMono->GoToCSGO(listUIGrid[0]->mOwner), (int)UI_STATE::FOCUSED);

									External->scene->canNav = false;
								}
							}
						}

						else if (bounce)
						{
							if (listUI[offset]->state != UI_STATE::SELECTED)
							{
								listUI[offset]->SetState(UI_STATE::NORMAL);
							}

							External->scene->SetSelected(listUI[offset + (rows * (columns - 1))]->mOwner);
							External->scene->focusedUIGO = listUI[offset + (rows * (columns - 1))]->mOwner;

							if (listUI[offset + (rows * (columns - 1))]->state != UI_STATE::SELECTED)
							{
								listUI[offset + (rows * (columns - 1))]->SetState(UI_STATE::FOCUSED);
							}

							External->scene->onHoverUI += (rows * (columns - 1));
						}
					}

					else
					{
						if (listUI[offset]->state != UI_STATE::SELECTED)
						{
							listUI[offset]->SetState(UI_STATE::NORMAL);
						}

						External->scene->SetSelected(listUI[offset + (rows * (columns - 1))]->mOwner);
						External->scene->focusedUIGO = listUI[offset + (rows * (columns - 1))]->mOwner;

						if (listUI[offset + (rows * (columns - 1))]->state != UI_STATE::SELECTED)
						{
							listUI[offset + (rows * (columns - 1))]->SetState(UI_STATE::FOCUSED);
						}

						External->scene->onHoverUI += (rows * (columns - 1));
					}
				}

				else
				{
					External->scene->SetSelected(listUI[offset - rows]->mOwner);
					External->scene->focusedUIGO = listUI[offset - rows]->mOwner;

					if (listUI[offset]->state != UI_STATE::SELECTED)
					{
						listUI[offset]->SetState(UI_STATE::NORMAL);
					}

					if (listUI[offset - rows]->state != UI_STATE::SELECTED)
					{
						listUI[offset - rows]->SetState(UI_STATE::FOCUSED);
					}

					External->scene->onHoverUI -= rows;
				}

			}
		}
	}	
}

void NavigateCraftingVertical(MonoObject* go, int rows, int columns, bool isDown, bool navigateGrids, MonoObject* gridDown, MonoObject* gridUp, bool bounce)
{
	if (External->scene->canNav)
	{
		// Get UI elements to navigate
		std::vector<C_UI*> listUI;
		GameObject* gameObject = External->moduleMono->GameObject_From_CSGO(go);
		External->scene->GetUINavigate(gameObject, listUI); bool isInGO = false;
		int offset = 0;

		for (auto i = 0; i < listUI.size(); i++)
		{
			if (listUI[i]->mOwner->UID == External->scene->focusedUIGO->UID)
			{
				offset = i;
				break;
			}
		}
	

		if (External->scene->focusedUIGO == nullptr)
		{
			External->scene->focusedUIGO = listUI[0]->mOwner;
			External->scene->SetSelected(listUI[0]->mOwner);
			External->scene->onHoverUI = offset;
		}

		else
		{
			for (auto i = 0; i < listUI.size(); i++)
			{
				if (External->scene->focusedUIGO != nullptr)
				{
					if (listUI[i]->mOwner->UID == External->scene->focusedUIGO->UID)
					{
						isInGO = true;
						break;
					}
				}
			}
		}

		if (isInGO)
		{
			if (isDown)
			{
				if ((offset + 1) % rows == 0 || offset == listUI.size() - 1)
				{
					if (navigateGrids)
					{
						bool isBlocked = false;

						GameObject* gridGo = External->moduleMono->GameObject_From_CSGO(gridDown);

						if (gridGo != nullptr)
						{
							std::vector<C_UI*> listUIGrid;
							External->scene->GetUINavigate(gridGo, listUIGrid);

							if (listUIGrid.size() != 0)
							{
								std::vector<Component*> listComponents = listUIGrid[0]->mOwner->GetAllComponentsByType(ComponentType::UI);

								for (auto it = listComponents.begin(); it != listComponents.end(); ++it)
								{
									if (((C_UI*)(*it))->tabNav_)
									{
										if (((C_UI*)(*it))->state == UI_STATE::DISABLED)
										{
											isBlocked = true;
											break;
										}
									}
								}

								if (!isBlocked)
								{
									if (listUI[offset]->state != UI_STATE::SELECTED)
									{
										listUI[offset]->SetState(UI_STATE::NORMAL);
									}

									SetUIState(External->moduleMono->GoToCSGO(listUIGrid[0]->mOwner), (int)UI_STATE::FOCUSED);

									External->scene->canNav = false;
								}
							}
						}

						else if (bounce)
						{
							if (listUI[offset]->state != UI_STATE::SELECTED)
							{
								listUI[offset]->SetState(UI_STATE::NORMAL);
							}

							External->scene->SetSelected(listUI[offset - rows + 1]->mOwner);

							External->scene->focusedUIGO = listUI[offset - rows + 1]->mOwner;

							if (listUI[offset - rows + 1]->state != UI_STATE::SELECTED)
							{
								listUI[offset - rows + 1]->SetState(UI_STATE::FOCUSED);
							}

							External->scene->onHoverUI -= (rows - 1);
						}
					}

					else
					{
						if (listUI[offset]->state != UI_STATE::SELECTED)
						{
							listUI[offset]->SetState(UI_STATE::NORMAL);
						}

						// Same as below, should make a function
						External->scene->SetSelected(listUI[offset - rows + 1]->mOwner);

						External->scene->focusedUIGO = listUI[offset - rows + 1]->mOwner;

						if (listUI[offset - rows + 1]->state != UI_STATE::SELECTED)
						{
							listUI[offset - rows + 1]->SetState(UI_STATE::FOCUSED);
						}

						External->scene->onHoverUI -= (rows - 1);
					}
				}

				else
				{
					External->scene->SetSelected(listUI[offset + 1]->mOwner);

					External->scene->focusedUIGO = listUI[offset + 1]->mOwner;

					if (listUI[offset]->state != UI_STATE::SELECTED)
					{
						listUI[offset]->SetState(UI_STATE::NORMAL);
					}

					if (listUI[offset + 1]->state != UI_STATE::SELECTED)
					{
						listUI[offset + 1]->SetState(UI_STATE::FOCUSED);
					}

					External->scene->onHoverUI += 1;
				}
			}

			else
			{
				if ((offset) % rows == 0 || offset == 0)
				{
					if (navigateGrids)
					{
						bool isBlocked = false;

						GameObject* gridGo = External->moduleMono->GameObject_From_CSGO(gridUp);

						if (gridGo != nullptr)
						{
							std::vector<C_UI*> listUIGrid;
							External->scene->GetUINavigate(gridGo, listUIGrid);

							if (listUIGrid.size() != 0)
							{
								std::vector<Component*> listComponents = listUIGrid[0]->mOwner->GetAllComponentsByType(ComponentType::UI);

								for (auto it = listComponents.begin(); it != listComponents.end(); ++it)
								{
									if (((C_UI*)(*it))->tabNav_)
									{
										if (((C_UI*)(*it))->state == UI_STATE::DISABLED)
										{
											isBlocked = true;
											break;
										}
									}
								}

								if (!isBlocked)
								{
									if (listUI[offset]->state != UI_STATE::SELECTED)
									{
										listUI[offset]->SetState(UI_STATE::NORMAL);
									}

									SetUIState(External->moduleMono->GoToCSGO(listUIGrid[0]->mOwner), (int)UI_STATE::FOCUSED);

									External->scene->canNav = false;
								}
							}
						}

						else if (bounce)
						{
							if (listUI[offset]->state != UI_STATE::SELECTED)
							{
								listUI[offset]->SetState(UI_STATE::NORMAL);
							}

							External->scene->SetSelected(listUI[offset + rows - 1]->mOwner);
							External->scene->focusedUIGO = listUI[offset + rows - 1]->mOwner;

							if (listUI[offset + rows - 1]->state != UI_STATE::SELECTED)
							{
								listUI[offset + rows - 1]->SetState(UI_STATE::FOCUSED);
							}

							External->scene->onHoverUI += (rows - 1);
						}
					}

					else
					{
						if (listUI[offset]->state != UI_STATE::SELECTED)
						{
							listUI[offset]->SetState(UI_STATE::NORMAL);
						}

						External->scene->SetSelected(listUI[offset + rows - 1]->mOwner);
						External->scene->focusedUIGO = listUI[offset + rows - 1]->mOwner;

						if (listUI[offset + rows - 1]->state != UI_STATE::SELECTED)
						{
							listUI[offset + rows - 1]->SetState(UI_STATE::FOCUSED);
						}

						External->scene->onHoverUI += (rows - 1);
					}
				}

				else
				{
					External->scene->SetSelected(listUI[offset - 1]->mOwner);
					External->scene->focusedUIGO = listUI[offset - 1]->mOwner;

					if (listUI[offset]->state != UI_STATE::SELECTED)
					{
						listUI[offset]->SetState(UI_STATE::NORMAL);
					}

					if (listUI[offset - 1]->state != UI_STATE::SELECTED)
					{
						listUI[offset - 1]->SetState(UI_STATE::FOCUSED);
					}

					External->scene->onHoverUI -= 1;
				}
			}
		}

	}
	
}
//
void SetActiveAllUI(MonoObject* go, bool isActive)
{
	GameObject* gameObject = External->moduleMono->GameObject_From_CSGO(go);

	// Get UI elements to navigate
	std::vector<C_UI*> listUI;
	External->scene->GetUINavigate(gameObject, listUI);

	for (auto i = 0; i < listUI.size(); i++)
	{
		listUI[i]->mOwner->mChildren[0]->active = isActive;
	}
}

void SetFirstFocused(MonoObject* go)
{
	// Get UI elements to navigate
	std::vector<C_UI*> listUI;
	GameObject* gameObject = External->moduleMono->GameObject_From_CSGO(go);
	External->scene->GetUINavigate(gameObject, listUI); bool isInGO = false;
	//int offset = 0;

	//std::vector<C_UI*> listOffset;
	//for (int i = 0; i < External->scene->vCanvas.size(); ++i)
	//{
	//	External->scene->GetUINavigate(External->scene->vCanvas[i], listOffset);
	//}

	//for (auto i = 0; i < listOffset.size(); i++)
	//{
	//	if (listOffset[i]->mOwner->UID != listUI[0]->mOwner->UID)
	//	{
	//		offset++;
	//	}

	//	else
	//	{
	//		break;
	//	}
	//}

	SetUIState(External->moduleMono->GoToCSGO(listUI[0]->mOwner), (int)UI_STATE::FOCUSED);

	if (External->scene->selectedUIGO != nullptr)
	{
		for (int i = 0; i < External->scene->selectedUIGO->mComponents.size(); i++)
		{
			if (External->scene->selectedUIGO->mComponents[i]->ctype == ComponentType::UI)
			{
				if (static_cast<C_UI*>(External->scene->selectedUIGO->mComponents[i])->state == UI_STATE::SELECTED)
				{
					static_cast<C_UI*>(External->scene->selectedUIGO->mComponents[i])->state = UI_STATE::NORMAL;
				}
			}
		}

		External->scene->selectedUIGO = nullptr;
	}

	//External->scene->focusedUIGO = listUI[0]->mOwner;
	//External->scene->SetSelected(listUI[0]->mOwner);
	//External->scene->onHoverUI = offset;
}

MonoString* GetUIText(MonoObject* go)
{
	G_UI* gameObject = (G_UI*)External->moduleMono->GameObject_From_CSGO(go);
	UI_Text* textUI = static_cast<UI_Text*>(gameObject->GetComponentUI(UI_TYPE::TEXT));

	return mono_string_new(External->moduleMono->domain, textUI->text.c_str());
}

void SetUIPosWithOther(MonoObject* goSource, MonoObject* goDestination)
{
	GameObject* selectedgo = External->moduleMono->GameObject_From_CSGO(goSource);
	CTransform* selectedTransform = selectedgo->mTransform;

	GameObject* targetgo = External->moduleMono->GameObject_From_CSGO(goDestination);
	CTransform* targetTransform = targetgo->mTransform;

	selectedTransform->SetPosition(float3(targetTransform->translation.x, targetTransform->translation.y, 0));

	//selectedTransform->UpdateUITransformChilds();
	//selectedTransform->componentReference->dirty_ = true;

}

bool CheckUI(MonoObject* goTarget, MonoObject* goOrigin)
{
	G_UI* gameObjectTarget = (G_UI*)External->moduleMono->GameObject_From_CSGO(goTarget);
	G_UI* gameObjectOrigin = (G_UI*)External->moduleMono->GameObject_From_CSGO(goOrigin);

	std::vector<C_UI*> listUI;
	External->scene->GetUINavigate(gameObjectOrigin, listUI);

	for (auto i = 0; i < listUI.size(); i++)
	{
		if (listUI[i]->mOwner->UID == gameObjectTarget->UID)
		{
			return true;
		}
	}

	return false;
}

bool CompareStringToName(MonoObject* go, MonoString* name)
{
	GameObject* gameObject = External->moduleMono->GameObject_From_CSGO(go);
	std::string nameCompare = mono_string_to_utf8(name);

	if (gameObject->name.find(nameCompare) != std::string::npos) 
	{
		return true;
	}

	return false;
}