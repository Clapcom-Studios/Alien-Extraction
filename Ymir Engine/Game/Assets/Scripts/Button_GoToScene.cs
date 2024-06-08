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
    private GameObject loadScene_images;
    private GameObject loadScenelvl1;
    private GameObject loadScenelvl2;
    private GameObject loadScenelvl3;

    private bool loadScene = false;

    public float time = 10;
    public bool saveGame = true;

    public void Start()
    {
        loadScene_images = InternalCalls.GetGameObjectByName("Loading Scene Canvas");
        loadScenelvl1 = InternalCalls.CS_GetChild(loadScene_images, 0);
        loadScenelvl2 = InternalCalls.CS_GetChild(loadScene_images, 1);
        loadScenelvl3= InternalCalls.CS_GetChild(loadScene_images, 2);


        if (loadScenelvl1 != null)
        {
            loadScenelvl1.SetActive(false);
        }

        if (loadScenelvl2 != null)
        {
            loadScenelvl2.SetActive(false);
        }
        if (loadScenelvl3 != null)
        {
            loadScenelvl2.SetActive(false);
        }

        time = 10;
        loadScene = false;
    }

    public void Update()
    {
        time -= Time.deltaTime;

        if (loadScene)
        { 
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
        Debug.Log("Go to scene " + sceneName + ".yscene");
        Audio.PauseAllAudios();

        //if (loadScenelvl1 != null)
        //{
        //    loadScenelvl1.SetActive(true);
        //    loadScene = true;
        //}
        if (sceneName == "LVL1_FINAL/LVL1_FINAL_COLLIDERS")
        {
            if (loadScenelvl1 != null)
            {
                time = 2;
                loadScenelvl1.SetActive(true);
                loadScene = true;
            }
        }
        if (sceneName == "LVL2_LAB_PART1_FINAL/LVL2_LAB_PART1_COLLIDERS") 
        { 
            if (loadScenelvl2 != null)
            {
                time = 2;
                loadScenelvl2.SetActive(true);
                loadScene = true;
            }
        }
        if (sceneName == "LVL3_BlockOut/LVL3_PART1_COLLIDERS")
        {
            if (loadScenelvl3 != null)
            {
                time = 2;
                loadScenelvl3.SetActive(true);
                loadScene = true;
            }
        }



        if (saveGame)
        {
            Globals.GetPlayerScript().SavePlayer();
        }
    }
}