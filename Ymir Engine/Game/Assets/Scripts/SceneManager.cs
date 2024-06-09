using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class SceneManager : YmirComponent
{
    // Loading scene
    private GameObject loadScenebase;
    private GameObject loadScenelvl1;
    private GameObject loadScenelvl2_1;
    private GameObject loadScenelvl2_2;
    private GameObject loadScenelvl3_1;
    private GameObject loadScenelvlBoss;
    private bool loadScene = false;

    private string sceneName = "Assets/BASE_FINAL/LVL_BASE_COLLIDERS.yscene";
    private float finishTimer = 2f;
    public void Start()
    {
        //----------Base-----------//
        loadScenebase = InternalCalls.GetGameObjectByName("Loading Scene base");
        if (loadScenebase != null)
        {
            loadScenebase.SetActive(false);
        }

        //----------LVL1-----------//
        loadScenelvl1 = InternalCalls.GetGameObjectByName("Loading Scene 1");
        if (loadScenelvl1 != null)
        {
            loadScenelvl1.SetActive(false);
        }

        //----------lvl2_1-----------//
        loadScenelvl2_1 = InternalCalls.GetGameObjectByName("Loading Scene 2_1");
        if (loadScenelvl2_1 != null)
        {
            loadScenelvl2_1.SetActive(false);
        }

        //----------lvl2_1-----------//
        loadScenelvl2_2 = InternalCalls.GetGameObjectByName("Loading Scene 2_2");
        if (loadScenelvl2_2 != null)
        {
            loadScenelvl2_2.SetActive(false);
        }

        //----------lvl3_1-----------//
        loadScenelvl3_1 = InternalCalls.GetGameObjectByName("Loading Scene 3_1");
        if (loadScenelvl3_1 != null)
        {
            loadScenelvl3_1.SetActive(false);
        } 
        
        //----------lvlBoss-----------//
        loadScenelvlBoss = InternalCalls.GetGameObjectByName("Loading Scene Boss");
        if (loadScenelvlBoss != null)
        {
            loadScenelvlBoss.SetActive(false);
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
            if (loadScenebase != null)
            {
                loadScenebase.SetActive(true);
            }

            loadScene = true;

            sceneName = "Assets/BASE_FINAL/LVL_BASE_COLLIDERS.yscene";
            return;
        }

        if (Input.GetKey(YmirKeyCode.KP_2) == KeyState.KEY_DOWN)
        {
            finishTimer = 2;
            Audio.StopAllAudios();
            if (loadScenelvl1 != null)
            {
                loadScenelvl1.SetActive(true);
            }

            loadScene = true;

            sceneName ="Assets/LVL1_FINAL/LVL1_FINAL_COLLIDERS.yscene";
            return;
        }

        if (Input.GetKey(YmirKeyCode.KP_3) == KeyState.KEY_DOWN)
        {
            finishTimer = 2;
            Audio.StopAllAudios();
            if (loadScenelvl2_1 != null)
            {
                loadScenelvl2_1.SetActive(true);
            }

            loadScene = true;

            sceneName = "Assets/LVL2_LAB_PART1_FINAL/LVL2_LAB_PART1_COLLIDERS.yscene";
            return;
        }

        if (Input.GetKey(YmirKeyCode.KP_4) == KeyState.KEY_DOWN)
        {
            finishTimer = 2;
            Audio.StopAllAudios();
            if (loadScenelvl2_2 != null)
            {
                loadScenelvl2_2.SetActive(true);
            }

            loadScene = true;

            sceneName = "Assets/LVL2_LAB_PART2_FINAL/LVL2_LAB_PART2_COLLIDERS.yscene";
            return;
        }

        if (Input.GetKey(YmirKeyCode.KP_5) == KeyState.KEY_DOWN)
        {
            finishTimer = 2;
            Audio.StopAllAudios();
            if (loadScenelvl3_1 != null)
            {
                loadScenelvl3_1.SetActive(true);
            }

            loadScene = true;

            sceneName = "Assets/LVL3_BlockOut/LVL3_PART1_COLLIDERS.yscene";
            return;
        }

        if (Input.GetKey(YmirKeyCode.KP_6) == KeyState.KEY_DOWN)
        {
            Audio.StopAllAudios();
            if (loadScenelvlBoss != null)
            {
                loadScenelvlBoss.SetActive(true);
            }

            loadScene = true;

            sceneName = "Assets/LVL3_BlockOut/LVL3_BOSS_COLLDIERS.yscene";
            return;
        }

        return;
    }
}