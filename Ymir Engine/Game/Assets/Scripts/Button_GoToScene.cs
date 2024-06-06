using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Button_GoToScene : YmirComponent
{
    public string sceneName = "BASE_FINAL/LVL_BASE_COLLIDERS";

    // Loading scene
    private GameObject loadSceneLvl1;
    private GameObject loadSceneLvl2;
    private GameObject loadSceneLvl3;
    private bool loadScene = false;

    public float time = 10;
    public bool saveGame = true;

    public void Start()
    {
        loadSceneLvl1 = InternalCalls.GetGameObjectByName("Loading Scene Canvas");

        if (loadSceneLvl1 != null)
        {
            loadSceneLvl1.SetActive(false);
        }

        time = 10;
        loadScene = false;
    }

    public void Update()
    {
        time -= Time.deltaTime;

        if (loadScene)
        {
            if (sceneName == "LVL1_FINAL/LVL1_FINAL_COLLIDERS")
            {
                loadSceneLvl1.SetActive(true);

                if (time <= 0)
                {
                    InternalCalls.LoadScene("Assets/" + sceneName + ".yscene");
                    loadScene = false;
                }
                return;
            }
            if (sceneName == "LVL2_LAB_PART1_FINAL/LVL2_LAB_PART1_COLLIDERS")
            {
                loadSceneLvl2.SetActive(true);

                if (time <= 0)
                {
                    InternalCalls.LoadScene("Assets/" + sceneName + ".yscene");
                    loadScene = false;
                }
                return;
            }
            if (sceneName == "LVL3_BlockOut/LVL3_PART1_COLLIDERS")
            {
                loadSceneLvl3.SetActive(true);

                if (time <= 0)
                {
                    InternalCalls.LoadScene("Assets/" + sceneName + ".yscene");
                    loadScene = false;
                }
                return;
            }
        }
    }

    public void OnClickButton()
    {
        Debug.Log("Go to scene " + sceneName + ".yscene");
        Audio.PauseAllAudios();
        if (sceneName == "LVL1_FINAL/LVL1_FINAL_COLLIDERS")
        {
            if (loadSceneLvl1 != null)
            {
                loadSceneLvl1.SetActive(true);
                loadScene = true;
            }
        }
        if (sceneName == "LVL2_LAB_PART1_FINAL/LVL2_LAB_PART1_COLLIDERS")
        {
            if (loadSceneLvl2 != null)
            {
                loadSceneLvl2.SetActive(true);
                loadScene = true;
           
            }
        }
        if (sceneName == "LVL2_LAB_PART1_FINAL/LVL2_LAB_PART1_COLLIDERS")
        {
            if (loadSceneLvl2 != null)
            {
                loadSceneLvl2.SetActive(true);
                loadScene = true;
            }
        } if (sceneName == "LVL3_BlockOut/LVL3_PART1_COLLIDERS")
        {
            if (loadSceneLvl3 != null)
            {
                loadSceneLvl3.SetActive(true);
                loadScene = true;
            }
        }

        if (saveGame)
        {
            Globals.GetPlayerScript().SavePlayer();
        }
    }
}