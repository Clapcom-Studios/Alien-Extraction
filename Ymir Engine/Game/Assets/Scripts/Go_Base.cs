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

    public float time = 5;
    public bool saveGame = true;

    public void Start()
    {
        loadSceneImg = InternalCalls.GetGameObjectByName("Loading Scene Base");

        if (loadSceneImg != null)
        {
            loadSceneImg.SetActive(false);
        }

        time = 5;
        loadScene = false;
    }

    public void Update()
    {
        time -= Time.deltaTime;

        if (loadScene)
        {
            loadSceneImg.SetActive(true);

            if (time <= 0)
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