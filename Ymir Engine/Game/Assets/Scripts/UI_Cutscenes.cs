using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class UI_Cutscenes : YmirComponent
{
    public GameObject img = null;
    public GameObject button = null;
    public string imgPath = "Assets\\Cutscenes\\";
    public string imgName = "";

    public int currentFrame = 0;
    public int maxFrames = 0;

    public bool hasFinished = false;
    public bool introScene = false;
    public bool winScene1 = false;
    public bool winScene2 = false;

    private float timer = 5.5f;
    private float finishTimer = 5.5f;

    // Loading scene
    private GameObject loadSceneImg;
    private bool loadScene = false;

    public void Start()
    {
        button = InternalCalls.GetGameObjectByName("Button_A");
        img = InternalCalls.GetGameObjectByName("CutsceneImg");
        currentFrame = 0;

        hasFinished = false;

        loadSceneImg = InternalCalls.GetGameObjectByName("Load Scene Img");

        if (loadSceneImg != null)
        {
            loadSceneImg.SetActive(false);
        }

        loadScene = false;
    }

    public void Update()
    {
        if (loadScene)
        {
            if (introScene)
            {
                button.SetActive(false);
           
                InternalCalls.LoadScene("Assets/BASE_FINAL/LVL_BASE_COLLIDERS");
            }
            if (winScene1)
            {
                InternalCalls.LoadScene("Assets/UI/Scenes/StartScene");
            }
            loadScene = false;

            return;
        }

        if (img != null && !hasFinished && button != null)
        {
            if (finishTimer >= 0)
            {
                finishTimer -= Time.deltaTime;
                button.SetActive(false);

            }
            if (finishTimer <= 0)
            {
                button.SetActive(true);

                if (Input.GetKey(YmirKeyCode.A) == KeyState.KEY_DOWN || Input.GetGamepadButton(GamePadButton.A) == KeyState.KEY_DOWN)
                {
                    currentFrame++;
                    finishTimer = timer;
                    UI.ChangeImageUI(img, imgPath + imgName + "(" + currentFrame.ToString() + ")" + ".png", (int)UI_STATE.NORMAL);

                    if (currentFrame == maxFrames)
                    {
                        hasFinished = true;

                        if (introScene)
                        {
                            if (loadSceneImg != null)
                            {
                                loadSceneImg.SetActive(true);
                            }

                            loadScene = true;
                        }
                        if (winScene1)
                        {
                            if (loadSceneImg != null)
                            {
                                loadSceneImg.SetActive(true);
                            }

                            loadScene = true;

                            if (winScene2)
                            {
                                if (loadSceneImg != null)
                                {
                                    loadSceneImg.SetActive(true);
                                }

                                loadScene = true;
                            }
                        }
                    }
                }
            }

            return;
        }
    }
}