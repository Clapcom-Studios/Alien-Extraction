using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Tp_To_Lvl2_Part1 : YmirComponent
{
    private GameObject loadSceneLvL2_1;
    private bool loadScene = false;

    private float finishTimer = 2f;

    public void Start()
    {
        loadSceneLvL2_1 = InternalCalls.GetGameObjectByName("Loading Scene Lvl2");

        if (loadSceneLvL2_1 != null)
        {
            loadSceneLvL2_1.SetActive(false);
        }

        loadScene = false;
    }
    public void Update()
    {
        if (loadScene)
        {
            if (finishTimer >= 0)
            {
                Debug.Log("asdfjasdf");
                finishTimer -= Time.deltaTime;
            }

            if (finishTimer <= 0)
            {
                InternalCalls.LoadScene("Assets/LVL2_LAB_PART1_FINAL/LVL2_LAB_PART1_COLLIDERS.yscene");
                loadScene = false;
            }
        }

        return;
    }
    public void OnCollisionEnter(GameObject other)
    {
        //TODO: Mostrat UI de que puede interactuar si pulsa el boton asignado
        if (other.Tag == "Player")
        {
            Audio.StopAllAudios();
            if (loadSceneLvL2_1 != null)
            {
                finishTimer = 2f;
                loadSceneLvL2_1.SetActive(true);
            }

            loadScene = true;
            Globals.GetPlayerScript().SavePlayer();
        }
    }
}