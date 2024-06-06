using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using YmirEngine;

public class Button_Navigation : YmirComponent
{
    public string sceneName = "BASE_FINAL/LVL_BASE_COLLIDERS";

    public bool isNewGame = true;
    public bool isContinueGame = false;

    // Loading scene
    private GameObject loadSceneImg;
    private bool loadScene = false;
    public bool showLoadScene = true;

    public float time = 2;

    public void Start()
    {
        loadSceneImg = InternalCalls.GetGameObjectByName("Loading Scene Canvas");

        if (loadSceneImg != null)
        {
            loadSceneImg.SetActive(false);
        }

        time = 2;
        loadScene = false;

        if (!SaveLoad.GameFileExists(Globals.saveGameDir, Globals.saveGamesInfoFile))
        {
            SaveLoad.CreateSaveGameFile(Globals.saveGameDir, Globals.saveGamesInfoFile);
            SaveLoad.SaveString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame, "Player_0");
        }
    }

    public void Update()
    {
        if (loadScene)
        {
            time -= Time.deltaTime;
            loadSceneImg.SetActive(true);

            if (time <= 0)
            {
                if (isNewGame)
                {
                    InternalCalls.LoadScene("Assets/" + sceneName + ".yscene");
                }

                else if (isContinueGame)
                {
                    string saveName = SaveLoad.LoadString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame);

                    int currentLvl = SaveLoad.LoadInt(Globals.saveGameDir, saveName, "Current Lvl");

                    switch (currentLvl)
                    {
                        case 0:
                            {
                                InternalCalls.LoadScene("Assets/BASE_FINAL/LVL_BASE_COLLIDERS.yscene");
                                break;
                            }
                        case 1:
                            {
                                InternalCalls.LoadScene("Assets/LVL1_FINAL/LVL1_FINAL_COLLIDERS.yscene");
                                break;
                            }
                        case 2:
                            {
                                InternalCalls.LoadScene("Assets/LVL2_LAB_PART1_FINAL/LVL2_LAB_PART1_COLLIDERS.yscene");
                                break;
                            }
                        case 3:
                            {
                                InternalCalls.LoadScene("Assets/LVL2_LAB_PART2_FINAL/LVL2_LAB_PART2_COLLIDERS.yscene");
                                break;
                            }
                        case 4:
                            {
                                InternalCalls.LoadScene("Assets/LVL3_BlockOut/LVL3_PART1_COLLIDERS.yscene");
                                break;
                            }
                        case 5:
                            {
                                InternalCalls.LoadScene("Assets/LVL3_BlockOut/LVL3_BOSS_COLLDIERS.yscene");
                                break;
                            }
                        default:
                            break;
                    }

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

        if (isNewGame)
        {
            SaveNewEmptyGame();
        }
        else
        {

        }

        if (showLoadScene)
        {
            if (loadSceneImg != null)
            {
                loadSceneImg.SetActive(true);
                loadScene = true;
            }
        }
        else
        {
            InternalCalls.LoadScene("Assets/" + sceneName + ".yscene");
        }
    }

    public void SaveNewEmptyGame()
    {
        int i = 0;

        while (SaveLoad.GameFileExists(Globals.saveGameDir, "Player_" + i.ToString()))
        {
            i++;
        }

        string fileName = "Player_" + i.ToString();

        SaveLoad.SaveString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame, fileName);

        // Player
        SaveLoad.CreateSaveGameFile(Globals.saveGameDir, fileName);

        // Lvls
        SaveLoad.SaveInt(Globals.saveGameDir, fileName, "Current Lvl", 0);
        SaveLoad.SaveInt(Globals.saveGameDir, fileName, "Last unlocked Lvl", 1);

        // Weapons
        SaveLoad.SaveInt(Globals.saveGameDir, fileName, "Current weapon", 0);
        SaveLoad.SaveInt(Globals.saveGameDir, fileName, "Upgrade " + WEAPON_TYPE.SMG.ToString(), (int)UPGRADE.LVL_0);
        SaveLoad.SaveInt(Globals.saveGameDir, fileName, "Upgrade " + WEAPON_TYPE.SHOTGUN.ToString(), (int)UPGRADE.LVL_0);
        SaveLoad.SaveInt(Globals.saveGameDir, fileName, "Upgrade " + WEAPON_TYPE.PLASMA.ToString(), (int)UPGRADE.LVL_0);

        // Dialogue
        SaveLoad.SaveBool(Globals.saveGameDir, fileName, "Caius intro dialogue", false);
        SaveLoad.SaveBool(Globals.saveGameDir, fileName, "Interacted Holo Screen", false);
        SaveLoad.SaveBool(Globals.saveGameDir, fileName, "Interacted Android Head", false);
        SaveLoad.SaveBool(Globals.saveGameDir, fileName, "Interacted Corpse", false);
        SaveLoad.SaveBool(Globals.saveGameDir, fileName, "Lvl 1 Completed", false);
        SaveLoad.SaveBool(Globals.saveGameDir, fileName, "Lvl 2 Completed", false);
        SaveLoad.SaveBool(Globals.saveGameDir, fileName, "Has dead", false);
        SaveLoad.SaveBool(Globals.saveGameDir, fileName, "First Incursion", false);
        SaveLoad.SaveBool(Globals.saveGameDir, fileName, "Boss Figth", false);

        // Stats
        SaveLoad.SaveFloat(Globals.saveGameDir, fileName, "Health", 1200);

        // Resin vessels
        SaveLoad.SaveInt(Globals.saveGameDir, fileName, "Current potties", 2);
        SaveLoad.SaveInt(Globals.saveGameDir, fileName, "Max potties", 2);
        SaveLoad.SaveFloat(Globals.saveGameDir, fileName, "Potties healing", 400);

        SaveLoad.SaveInt(Globals.saveGameDir, fileName, "Items num", 0);
        SaveLoad.SaveString(Globals.saveGameDir, fileName, "Save Item", "NONE");
    }
}