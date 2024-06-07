using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Tp_To_Lvl3 : YmirComponent
{
    // Loading scene
    private GameObject loadSceneImg;
    private bool loadScene = false;
    private float finishTimer = 2f;

    public void Start()
    {
        loadSceneImg = InternalCalls.GetGameObjectByName("Loading Scene Lvl3_2");

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
            if (finishTimer >= 0)
            {
                finishTimer -= Time.deltaTime;
            }
            if (finishTimer <= 0)
            {
                InternalCalls.LoadScene("Assets/LVL3_BlockOut/LVL3_BOSS_COLLDIERS.yscene");
                loadScene = false;
            }

            return;
        }

        return;
    }

    public void OnCollisionEnter(GameObject other)
    {
        //TODO: Mostrat UI de que puede interactuar si pulsa el boton asignado
        if (other.Tag == "Player")
        {
            Audio.StopAllAudios();
            if (loadSceneImg != null)
            {
                finishTimer = 2f;
                loadSceneImg.SetActive(true);
            }

            loadScene = true;
            Globals.GetPlayerScript().SavePlayer();
        }
    }
}