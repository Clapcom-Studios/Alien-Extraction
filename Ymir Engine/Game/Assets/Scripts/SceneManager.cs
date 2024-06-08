using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class SceneManager : YmirComponent
{
    // Loading scene
    private GameObject loadSceneImg;
    private bool loadScene = false;

    private string sceneName = "Assets/BASE_FINAL/LVL_BASE_COLLIDERS.yscene";
    private float finishTimer = 2f;
    public void Start()
    {
        loadSceneImg = InternalCalls.GetGameObjectByName("Loading Scene Canvas");

        if (loadSceneImg != null)
        {
            loadSceneImg.SetActive(false);
        }

        loadScene = false;
    }

    public void Update()
    {
        if (finishTimer >= 0)
        {
            finishTimer -= Time.deltaTime;
        }
        if (loadScene)
        {
            if (finishTimer <= 0)
            {
                Globals.GetPlayerScript().SavePlayer();

                InternalCalls.LoadScene(sceneName);
                loadScene = false;
            }

            return;
        }

        if (Input.GetKey(YmirKeyCode.KP_1) == KeyState.KEY_DOWN)
        {
            finishTimer = 2;
            Audio.StopAllAudios();
            if (loadSceneImg != null)
            {
                loadSceneImg.SetActive(true);
            }

            loadScene = true;

            sceneName = "Assets/BASE_FINAL/LVL_BASE_COLLIDERS.yscene";
            return;
        }

        if (Input.GetKey(YmirKeyCode.KP_2) == KeyState.KEY_DOWN)
        {
            finishTimer = 2;
            Audio.StopAllAudios();
            if (loadSceneImg != null)
            {
                loadSceneImg.SetActive(true);
            }

            loadScene = true;

            sceneName ="Assets/LVL1_FINAL/LVL1_FINAL_COLLIDERS.yscene";
            return;
        }

        if (Input.GetKey(YmirKeyCode.KP_3) == KeyState.KEY_DOWN)
        {
            finishTimer = 2;
            Audio.StopAllAudios();
            if (loadSceneImg != null)
            {
                loadSceneImg.SetActive(true);
            }

            loadScene = true;

            sceneName = "Assets/LVL2_LAB_PART1_FINAL/LVL2_LAB_PART1_COLLIDERS.yscene";
            return;
        }

        if (Input.GetKey(YmirKeyCode.KP_4) == KeyState.KEY_DOWN)
        {
            finishTimer = 2;
            Audio.StopAllAudios();
            if (loadSceneImg != null)
            {
                loadSceneImg.SetActive(true);
            }

            loadScene = true;

            sceneName = "Assets/LVL2_LAB_PART2_FINAL/LVL2_LAB_PART2_COLLIDERS.yscene";
            return;
        }

        if (Input.GetKey(YmirKeyCode.KP_5) == KeyState.KEY_DOWN)
        {
            finishTimer = 2;
            Audio.StopAllAudios();
            if (loadSceneImg != null)
            {
                loadSceneImg.SetActive(true);
            }

            loadScene = true;

            sceneName = "Assets/LVL3_BlockOut/LVL3_PART1_COLLIDERS.yscene";
            return;
        }

        if (Input.GetKey(YmirKeyCode.KP_6) == KeyState.KEY_DOWN)
        {
            Audio.StopAllAudios();
            if (loadSceneImg != null)
            {
                loadSceneImg.SetActive(true);
            }

            loadScene = true;

            sceneName = "Assets/LVL3_BlockOut/LVL3_BOSS_COLLDIERS.yscene";
            return;
        }

        return;
    }
}