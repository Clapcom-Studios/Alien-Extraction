using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Tp_To_Lvl1 : YmirComponent
{
    // Loading scene
    private GameObject loadSceneLvl1;
    private bool loadScene = false;

    public void Start()
    {
        loadSceneLvl1 = InternalCalls.GetGameObjectByName("Loading Scene Lvl1");

        if (loadSceneLvl1 != null)
        {
            loadSceneLvl1.SetActive(false);
        }

        loadScene = false;
    }

    public void Update()
    {
        if (loadScene)
        {
            InternalCalls.LoadScene("Assets/LVL1_FINAL/LVL1_FINAL_COLLIDERS.yscene");
            loadScene = false;

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
            if (loadSceneLvl1 != null)
            {
                loadSceneLvl1.SetActive(true);
            }

            loadScene = true;
            Globals.GetPlayerScript().SavePlayer();
        }
    }
}