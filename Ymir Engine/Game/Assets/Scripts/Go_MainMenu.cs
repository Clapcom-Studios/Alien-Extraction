using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Go_MainMenu : YmirComponent
{
    private string sceneName = "UI/Scenes/StartScene";

    // Loading scene
    private GameObject loadSceneImg;
    private bool loadScene = false;

    public float finishTimer = 2f;
    public bool saveGame = true;

    public void Start()
    {
        loadSceneImg = InternalCalls.GetGameObjectByName("Loading Scene MainMenu");

        if (loadSceneImg != null)
        {
            loadSceneImg.SetActive(false);
        }

        finishTimer = 2;
        loadScene = false;
    }

    public void Update()
    {
        if (loadScene)
        {
            finishTimer -= Time.deltaTime;

            if (finishTimer <= 0)
            {
                InternalCalls.LoadScene("Assets/" + sceneName + ".yscene");
                loadScene = false;
            }
            return;
        }
    }

    public void OnClickButton()
    {
        Audio.PauseAllAudios();

        if (loadSceneImg != null)
        {
            loadSceneImg.SetActive(true);
        }

        loadScene = true;

        if (saveGame)
        {
            Globals.GetPlayerScript().SavePlayer();
        }
    }
}