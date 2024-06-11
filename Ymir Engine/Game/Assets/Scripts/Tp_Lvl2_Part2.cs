using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Tp_Lvl2_Part2 : YmirComponent
{
    // Loading scene
    private GameObject loadSceneImg;
    private bool loadScene = false;

    private float finishTimer = 2f;
    public void Start()
    {
        loadSceneImg = InternalCalls.GetGameObjectByName("Load Scene Lvl2_2");

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
                InternalCalls.LoadScene("Assets/LVL2_LAB_PART2_FINAL/LVL2_LAB_PART2_COLLIDERS.yscene");
                loadScene = false;

            return;
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
            if (loadSceneImg != null)
            {
                
                loadSceneImg.SetActive(true);
            }

            loadScene = true; 
            Globals.GetPlayerScript().SavePlayer();
        }
    }    
}