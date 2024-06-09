using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class UI_Upgrade_Button : YmirComponent
{
    public Upgrade upgrade;

    public string name = "";
    public string description = "";
    public int cost;
    public bool isUnlocked;
    public string stationName = "";

    public UI_Upgrade_Station currentStation;
    private GameObject _parent;
    private GameObject audioSource;
    private bool _setFocused = false;

    public void Start()
    {
        GameObject goText = InternalCalls.GetChildrenByName(gameObject, "Text");
        description = UI.GetUIText(goText);
        audioSource = InternalCalls.GetGameObjectByName("UI Audio");

        upgrade = new Upgrade(name, description, cost, isUnlocked);

        if (stationName.Contains("Sub"))
        {
            upgrade.type = WEAPON_TYPE.SMG;
        }
        else if (stationName.Contains("Shotgun"))
        {
            upgrade.type = WEAPON_TYPE.SHOTGUN;
        }
        else
        {
            upgrade.type = WEAPON_TYPE.PLASMA;
        }

        _setFocused = false;


        GameObject go = InternalCalls.GetGameObjectByName("Upgrade Station");

        if (go != null)
        {
            currentStation = go.GetComponent<UI_Upgrade_Station>();
        }

        if (cost == 2)
        {
            _parent = InternalCalls.GetGameObjectByName(stationName + " End");
        }

        LoadWeaponUpgrade();
        ManageStart();
    }

    public void Update()
    {
        if (_setFocused)
        {
            UI.SetUIState(gameObject, (int)UI_STATE.FOCUSED);
            _setFocused = false;
        }
        return;
    }

    public void OnClickButton()
    {
        if (!upgrade.isUnlocked && currentStation._player.numCores >= upgrade.cost)
        {
            Audio.PlayAudio(audioSource, "UI_WeaponUpgrade");

            switch (cost)
            {
                case 1:
                    {
                        GameObject go2 = InternalCalls.GetChildrenByName(InternalCalls.GetGameObjectByName(stationName), "Upgrade 2");
                        UI.SetUIState(go2, (int)UI_STATE.NORMAL);

                        upgrade.upgradeType = UPGRADE.LVL_1;

                        upgrade.isUnlocked = true;

                        // Switch unlocked texture to current button
                        switch (upgrade.type)
                        {
                            case WEAPON_TYPE.SMG:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl0-1/SmgLvl0Hover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl0-1/SmgLvl0.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl0-1/SmgLvl0Pressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl0-1/SmgLvl0.png", (int)(UI_STATE.SELECTED));
                                break;
                            case WEAPON_TYPE.SHOTGUN:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl0-1/ShotgunLvl0Hover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl0-1/ShotgunLvl0.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl0-1/ShotgunLvl0Pressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl0-1/ShotgunLvl0.png", (int)(UI_STATE.SELECTED));
                                break;
                            case WEAPON_TYPE.PLASMA:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0Hover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0Pressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0.png", (int)(UI_STATE.SELECTED));
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                case 2:
                    {
                        GameObject go3 = InternalCalls.CS_GetChild(InternalCalls.GetGameObjectByName(stationName + " 3"), 0);
                        Debug.Log(go3.Name);
                        GameObject go4 = InternalCalls.CS_GetChild(InternalCalls.GetGameObjectByName(stationName + " 4"), 0);
                        Debug.Log(go4.Name);
                        UI.SetUIState(go3, (int)UI_STATE.NORMAL);
                        UI.SetUIState(go4, (int)UI_STATE.NORMAL);

                        upgrade.upgradeType = UPGRADE.LVL_2;

                        upgrade.isUnlocked = true;

                        // Switch unlocked texture to current button
                        switch (upgrade.type)
                        {
                            case WEAPON_TYPE.SMG:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl2/SmgLvl2Hover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl2/SmgLvl2.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl2/SmgLvl2Pressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl2/SmgLvl2.png", (int)(UI_STATE.SELECTED));
                                break;
                            case WEAPON_TYPE.SHOTGUN:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl2/ShotgunLvl2Hover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl2/ShotgunLvl2.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl2/ShotgunLvl2Pressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl2/ShotgunLvl2.png", (int)(UI_STATE.SELECTED));
                                break;
                            case WEAPON_TYPE.PLASMA:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0Hover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0Pressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0.png", (int)(UI_STATE.SELECTED));
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                case 4:
                    {
                        if (UI.CompareStringToName(gameObject, "Upgrade 3"))
                        {
                            Debug.Log(gameObject.Name);
                            Debug.Log(stationName + " 3");

                            GameObject go4 = InternalCalls.CS_GetChild(InternalCalls.GetGameObjectByName(stationName + " 4"), 0);
                            UI.SetUIState(go4, (int)UI_STATE.DISABLED);

                            upgrade.upgradeType = UPGRADE.LVL_3_ALPHA;

                            // Switch unlocked texture to current button
                            switch (upgrade.type)
                            {
                                case WEAPON_TYPE.SMG:
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3B/SmgLvl3BHover.png", (int)(UI_STATE.NORMAL));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3B/SmgLvl3B.png", (int)(UI_STATE.FOCUSED));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3B/SmgLvl3BPressed.png", (int)(UI_STATE.PRESSED));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3B/SmgLvl3B.png", (int)(UI_STATE.SELECTED));
                                    break;
                                case WEAPON_TYPE.SHOTGUN:
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3B/ShotgunLvl3BHover.png", (int)(UI_STATE.NORMAL));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3B/ShotgunLvl3B.png", (int)(UI_STATE.FOCUSED));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3B/ShotgunLvl3BPressed.png", (int)(UI_STATE.PRESSED));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3B/ShotgunLvl3B.png", (int)(UI_STATE.SELECTED));
                                    break;
                                case WEAPON_TYPE.PLASMA:
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3B/LaserLvl3BHover.png", (int)(UI_STATE.NORMAL));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3B/LaserLvl3B.png", (int)(UI_STATE.FOCUSED));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3B/LaserLvl3BPressed.png", (int)(UI_STATE.PRESSED));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3B/LaserLvl3B.png", (int)(UI_STATE.SELECTED));
                                    break;
                                default:
                                    break;
                            }
                        }
                        else
                        {
                            GameObject go3 = InternalCalls.CS_GetChild(InternalCalls.GetGameObjectByName(stationName + " 3"), 0);
                            UI.SetUIState(go3, (int)UI_STATE.DISABLED);

                            GameObject goStation = InternalCalls.GetGameObjectByName(stationName);
                            Debug.Log(goStation.Name);
                            goStation.GetComponent<UI_Inventory_Grid>().downGrid = InternalCalls.GetGameObjectByName(stationName + " 4");

                            upgrade.upgradeType = UPGRADE.LVL_3_BETA;

                            // Switch unlocked texture to current button
                            switch (upgrade.type)
                            {
                                case WEAPON_TYPE.SMG:
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3B/SmgLvl3BHover.png", (int)(UI_STATE.NORMAL));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3B/SmgLvl3B.png", (int)(UI_STATE.FOCUSED));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3B/SmgLvl3BPressed.png", (int)(UI_STATE.PRESSED));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3B/SmgLvl3B.png", (int)(UI_STATE.SELECTED));
                                    break;
                                case WEAPON_TYPE.SHOTGUN:
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3B/ShotgunLvl3BHover.png", (int)(UI_STATE.NORMAL));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3B/ShotgunLvl3B.png", (int)(UI_STATE.FOCUSED));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3B/ShotgunLvl3BPressed.png", (int)(UI_STATE.PRESSED));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3B/ShotgunLvl3B.png", (int)(UI_STATE.SELECTED));
                                    break;
                                case WEAPON_TYPE.PLASMA:
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3B/LaserLvl3BHover.png", (int)(UI_STATE.NORMAL));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3B/LaserLvl3B.png", (int)(UI_STATE.FOCUSED));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3B/LaserLvl3BPressed.png", (int)(UI_STATE.PRESSED));
                                    UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3B/LaserLvl3B.png", (int)(UI_STATE.SELECTED));
                                    break;
                                default:
                                    break;
                            }

                        }

                        upgrade.isUnlocked = true;
                    }
                    break;
            }

            currentStation._player.UseAlienCore(upgrade.cost);
            currentStation.UpdateCoins();
            SaveWeaponUpgrade();
        }

        _setFocused = true;
    }

    public void OnHoverButton()
    {
        if (currentStation != null)
        {
            UI.TextEdit(currentStation.description, description);
            UI.TextEdit(currentStation.cost, upgrade.cost.ToString());
        }
    }

    public void SaveWeaponUpgrade()
    {
        string saveName = SaveLoad.LoadString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame);
        SaveLoad.SaveInt(Globals.saveGameDir, saveName, "Upgrade " + upgrade.type.ToString(), (int)upgrade.upgradeType);

        currentStation._player.SaveItems();
        Debug.Log("saved " + upgrade.type.ToString() + ": " + upgrade.upgradeType.ToString());
    }

    public void LoadWeaponUpgrade()
    {
        string saveName = SaveLoad.LoadString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame);
        upgrade.upgradeType = (UPGRADE)SaveLoad.LoadInt(Globals.saveGameDir, saveName, "Upgrade " + upgrade.type.ToString());
    }

    private void ManageStart()
    {
        int num = name.Contains("0") ? 0 : name.Contains("1") ? 1 : name.Contains("2") ? 2 : name.Contains("3") ? 3 : 4;

        //if (num < 4)
        //{
        //    if ((int)upgrade.upgradeType >= num)
        //    {
        //        upgrade.isUnlocked = true;
        //        UI.SetUIState(gameObject, (int)UI_STATE.NORMAL);
        //    }
        //    else if ((int)upgrade.upgradeType >= (num - 1))
        //    {
        //        UI.SetUIState(gameObject, (int)UI_STATE.NORMAL);
        //    }
        //    else
        //    {
        //        upgrade.isUnlocked = false;
        //        UI.SetUIState(gameObject, (int)UI_STATE.DISABLED);
        //    }
        //}
        //else
        //{
        //    if ((int)upgrade.upgradeType == 4)
        //    {
        //        upgrade.isUnlocked = true;
        //        UI.SetUIState(gameObject, (int)UI_STATE.NORMAL);
        //    }
        //    else if ((int)upgrade.upgradeType == 2)
        //    {
        //        UI.SetUIState(gameObject, (int)UI_STATE.NORMAL);
        //    }
        //    else
        //    {
        //        upgrade.isUnlocked = false;
        //        UI.SetUIState(gameObject, (int)UI_STATE.DISABLED);
        //    }
        //}

        switch (num)
        {
            case 0:
                {
                    upgrade.isUnlocked = true;
                    UI.SetUIState(gameObject, (int)UI_STATE.NORMAL);
                }
                break;
            case 1:
                {
                    if ((int)upgrade.upgradeType >= 1)
                    {
                        upgrade.isUnlocked = true;
                        UI.SetUIState(gameObject, (int)UI_STATE.NORMAL);

                        // Switch unlocked texture to current button
                        switch (upgrade.type)
                        {
                            case WEAPON_TYPE.SMG:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl0-1/SmgLvl0Hover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl0-1/SmgLvl0.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl0-1/SmgLvl0Pressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl0-1/SmgLvl0.png", (int)(UI_STATE.SELECTED));
                                break;
                            case WEAPON_TYPE.SHOTGUN:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl0-1/ShotgunLvl0Hover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl0-1/ShotgunLvl0.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl0-1/ShotgunLvl0Pressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl0-1/ShotgunLvl0.png", (int)(UI_STATE.SELECTED));
                                break;
                            case WEAPON_TYPE.PLASMA:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0Hover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0Pressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0.png", (int)(UI_STATE.SELECTED));
                                break;
                            default:
                                break;
                        }
                    }
                    else if ((int)upgrade.upgradeType == 0)
                    {
                        UI.SetUIState(gameObject, (int)UI_STATE.NORMAL);
                    }
                    else
                    {
                        upgrade.isUnlocked = false;
                        UI.SetUIState(gameObject, (int)UI_STATE.DISABLED);
                    }
                }
                break;
            case 2:
                {
                    if ((int)upgrade.upgradeType >= 2)
                    {
                        upgrade.isUnlocked = true;
                        UI.SetUIState(gameObject, (int)UI_STATE.NORMAL);

                        // Switch unlocked texture to current button
                        switch (upgrade.type)
                        {
                            case WEAPON_TYPE.SMG:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl2/SmgLvl2Hover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl2/SmgLvl2.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl2/SmgLvl2Pressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl2/SmgLvl2.png", (int)(UI_STATE.SELECTED));
                                break;
                            case WEAPON_TYPE.SHOTGUN:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl2/ShotgunLvl2Hover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl2/ShotgunLvl2.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl2/ShotgunLvl2Pressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl2/ShotgunLvl2.png", (int)(UI_STATE.SELECTED));
                                break;
                            case WEAPON_TYPE.PLASMA:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0Hover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0Pressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl0-2/LaserLvl0.png", (int)(UI_STATE.SELECTED));
                                break;
                            default:
                                break;
                        }
                    }
                    else
                    {
                        upgrade.isUnlocked = false;
                        UI.SetUIState(gameObject, (int)UI_STATE.DISABLED);
                    }
                }
                break;
            case 3:
                {
                    if ((int)upgrade.upgradeType == 3)
                    {
                        upgrade.isUnlocked = true;
                        UI.SetUIState(gameObject, (int)UI_STATE.NORMAL);

                        // Switch unlocked texture to current button
                        switch (upgrade.type)
                        {
                            case WEAPON_TYPE.SMG:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3A/SmgLvl3AHover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3A/SmgLvl3A.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3A/SmgLvl3APressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3A/SmgLvl3A.png", (int)(UI_STATE.SELECTED));
                                break;
                            case WEAPON_TYPE.SHOTGUN:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3A/ShotgunLvl3AHover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3A/ShotgunLvl3A.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3A/ShotgunLvl3APressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3A/ShotgunLvl3A.png", (int)(UI_STATE.SELECTED));
                                break;
                            case WEAPON_TYPE.PLASMA:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3A/LaserLvl3AHover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3A/LaserLvl3A.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3A/LaserLvl3APressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3A/LaserLvl3A.png", (int)(UI_STATE.SELECTED));
                                break;
                            default:
                                break;
                        }
                    }
                    else if ((int)upgrade.upgradeType == 2)
                    {
                        UI.SetUIState(gameObject, (int)UI_STATE.NORMAL);
                    }
                    else
                    {
                        upgrade.isUnlocked = false;
                        UI.SetUIState(gameObject, (int)UI_STATE.DISABLED);
                    }
                }
                break;
            case 4:
                {
                    if ((int)upgrade.upgradeType == 4)
                    {
                        upgrade.isUnlocked = true;
                        UI.SetUIState(gameObject, (int)UI_STATE.NORMAL);

                        GameObject goStation = InternalCalls.GetGameObjectByName(stationName);
                        Debug.Log(goStation.Name);
                        goStation.GetComponent<UI_Inventory_Grid>().downGrid = InternalCalls.GetGameObjectByName(stationName + " 4");

                        // Switch unlocked texture to current button
                        switch (upgrade.type)
                        {
                            case WEAPON_TYPE.SMG:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3B/SmgLvl3BHover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3B/SmgLvl3B.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3B/SmgLvl3BPressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/SmgLvl3B/SmgLvl3B.png", (int)(UI_STATE.SELECTED));
                                break;
                            case WEAPON_TYPE.SHOTGUN:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3B/ShotgunLvl3BHover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3B/ShotgunLvl3B.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3B/ShotgunLvl3BPressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/ShotgunLvl3B/ShotgunLvl3B.png", (int)(UI_STATE.SELECTED));
                                break;
                            case WEAPON_TYPE.PLASMA:
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3B/LaserLvl3BHover.png", (int)(UI_STATE.NORMAL));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3B/LaserLvl3B.png", (int)(UI_STATE.FOCUSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3B/LaserLvl3BPressed.png", (int)(UI_STATE.PRESSED));
                                UI.ChangeImageUI(gameObject, "Assets/UI/Upgrade Buttons/LaserLvl3B/LaserLvl3B.png", (int)(UI_STATE.SELECTED));
                                break;
                            default:
                                break;
                        }
                    }
                    else if ((int)upgrade.upgradeType == 2)
                    {
                        UI.SetUIState(gameObject, (int)UI_STATE.NORMAL);
                    }
                    else
                    {
                        upgrade.isUnlocked = false;
                        UI.SetUIState(gameObject, (int)UI_STATE.DISABLED);
                    }
                }
                break;
        }
    }
}