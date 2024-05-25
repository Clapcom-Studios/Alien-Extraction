using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Threading;
using YmirEngine;

public class UI_Crafting_Grid : YmirComponent
{
    public int rows;
    public int cols;
    private bool _canTab;

    private GameObject leftGrid;
    private GameObject rightGrid;
    public GameObject downGrid; // Public required for Upgrade station
    private GameObject upGrid;
    public string leftGridName = " ";
    public string rightGridName = " ";
    public string downGridName = " ";
    public string upGridName = " ";
    public bool navigateX = false;
    public bool navigateY = false;
    public bool bounceX = false;
    public bool bounceY = false;

    private float _time;
    private float _timer;

    private GameObject audioSource;
    public void Start()
	{
        audioSource = InternalCalls.GetGameObjectByName("UI Audio");
        leftGrid = InternalCalls.GetGameObjectByName(leftGridName);
        rightGrid = InternalCalls.GetGameObjectByName(rightGridName);
        downGrid = InternalCalls.GetGameObjectByName(downGridName);
        upGrid = InternalCalls.GetGameObjectByName(upGridName);
        _timer = 0.0f;
        _time = 0.1f;
    }

	public void Update()
	{
        if (!_canTab)
        {
            if (_time > _timer)
            {
                _timer += Time.deltaTime;
            }

            else
            {
                _timer = 0.0f;
                _canTab = true;
                UI.SetCanNav(true);
            }
        }

        if ((Input.GetLeftAxisX() > 0 || Input.GetGamepadButton(GamePadButton.DPAD_RIGHT) == KeyState.KEY_DOWN || Input.GetKey(YmirKeyCode.D) == KeyState.KEY_DOWN) && _canTab)
        {
            if (audioSource != null)
            {
                Audio.PlayAudio(audioSource, "UI_MoveHover");
            }

            _canTab = false;
            UI.NavigateCraftingVertical(gameObject, rows, cols, true, navigateY, downGrid, upGrid, bounceY);
        }

        else if ((Input.GetLeftAxisX() < 0 || Input.GetGamepadButton(GamePadButton.DPAD_LEFT) == KeyState.KEY_DOWN || Input.GetKey(YmirKeyCode.A) == KeyState.KEY_DOWN) && _canTab)
        {
            if (audioSource != null)
            {
                Audio.PlayAudio(audioSource, "UI_MoveHover");
            }

            UI.NavigateCraftingVertical(gameObject, rows, cols, false, navigateY, downGrid, upGrid, bounceY);
            _canTab = false;
        }

        else if ((Input.GetLeftAxisY() > 0 || Input.GetGamepadButton(GamePadButton.DPAD_DOWN) == KeyState.KEY_DOWN || Input.GetKey(YmirKeyCode.S) == KeyState.KEY_DOWN) && _canTab)
        {
            if (audioSource != null)
            {
                Audio.PlayAudio(audioSource, "UI_MoveHover");
            }

            _canTab = false;
            UI.NavigateCraftingHorizontal(gameObject, rows, cols, true, navigateX, leftGrid, rightGrid, bounceX);
        }

        else if ((Input.GetLeftAxisY() < 0 || Input.GetGamepadButton(GamePadButton.DPAD_UP) == KeyState.KEY_DOWN || Input.GetKey(YmirKeyCode.W) == KeyState.KEY_DOWN) && _canTab)
        {
            if (audioSource != null)
            {
                Audio.PlayAudio(audioSource, "UI_MoveHover");
            }

            _canTab = false;
            UI.NavigateCraftingHorizontal(gameObject, rows, cols, false, navigateX, leftGrid, rightGrid, bounceX);
        }

        return;
    }
}
