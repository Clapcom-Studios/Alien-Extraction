using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Go_Base : YmirComponent
{
    private string sceneName = "BASE_FINAL/LVL_BASE_COLLIDERS";

    // Loading scene
    private GameObject loadSceneImg;
    private bool loadScene = false;


    private float finishTimer = 2f;
    public bool saveGame = true;

    public void Start()
    {
        loadSceneImg = InternalCalls.GetGameObjectByName("Loading Scene Base");

        if (loadSceneImg != null)
        {
            loadSceneImg.SetActive(false);
        }

        finishTimer = 2;
        loadScene = false;
    }

    public void Update()
    {
        finishTimer -= Time.deltaTime;

        if (loadScene)
        {
            loadSceneImg.SetActive(true);

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
            loadScene = true;
        }

        if (saveGame)
        {
            Globals.GetPlayerScript().SavePlayer();
        }
    }
}