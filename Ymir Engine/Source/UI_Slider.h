#pragma once
#ifndef __UI_Slider_H__
#define __UI_Slider_H__
#pragma once

#include "Globals.h"
#include "G_UI.h"
#include "C_UI.h"

#include "UI_Image.h"

enum class SLIDER_DIRECTION
{
	LEFT_TO_RIGHT,
	RIGHT_TO_LEFT,
	BOTTOM_TO_UP,
	UP_TO_DOWN,

	NONE
};

union uValue
{
	int iValue;
	float fValue;
};

class UI_Slider : public C_UI
{
public:
	// x = 0, y = 0, w = 300, y = 50
	UI_Slider(GameObject* g, bool floats = false, float min = 0, float max = 10, float value = 0, float x = 0, float y = 0, G_UI* fill = nullptr, G_UI* handle = nullptr, float w = 300, float h = 50);
	~UI_Slider();

	update_status Update(float dt);

	void OnInspector();
	void Draw(bool game) override;

	void SetReference() override;
	void OnReferenceDestroyed(void* ptr = nullptr);

	//
	void OnNormal();
	void OnFocused();
	void OnPressed();
	void OnSelected();
	void OnRelease();

	//
	void SetValue(float val);

	//
	void SliderManagement(float dt);
	void ValueCalculationsFromHandles(float val, float max);

public:
	bool isInteractable;

	G_UI* fillImage;
	G_UI* handleImage;

	uValue minValue, maxValue;
	bool useFloat;

	uValue value;

	//color
	Color focusedColor;
	Color pressedColor;
	Color selectedColor;
	Color disabledColor;

	SLIDER_DIRECTION direction;

private:
	bool usingBar;
	bool usingHandle;
};
#endif // __UI_Slider_H__