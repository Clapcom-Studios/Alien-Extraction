using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class SelectLvl : YmirComponent
{
    public int lvl = 1;  // TODO: Sara --> change to enum
    private BaseTeleporter levelSelector;

    public GameObject locked;
    public bool isLocked = false;

    public void Start()
    {
        GameObject gameObject = InternalCalls.GetGameObjectByName("Background");
        if (gameObject != null)
        {
            levelSelector = gameObject.GetComponent<BaseTeleporter>();
        }

        locked = InternalCalls.GetChildrenByName(this.gameObject, "Lock");

        int lastLvl = Globals.GetPlayerScript().lastUnlockedLvl;

        switch (lvl)
        {
            case 1:
                {
                    isLocked = true;
                    break;
                }
            case 2:
                {
                    if (lvl <= lastLvl)
                    {
                        isLocked = false;
                    }
                    else
                    {
                        isLocked = true;
                    }
                    break;
                }
            case 3:
                {
                    if (lvl < lastLvl)
                    {
                        isLocked = false;
                    }
                    else
                    {
                        isLocked = true;
                    }
                    break;
                }
            default:
                break;
        }

        locked.SetActive(isLocked);
    }

    public void Update()
    {
        locked.SetActive(isLocked);
        return;
    }

    public void OnClickButton()
    {
        if (!isLocked)
        {
            GameObject gameObject0 = InternalCalls.GetGameObjectByName("Lvl (" + ((int)levelSelector.selectedLvl) + ")");
            GameObject gameObject1 = InternalCalls.GetGameObjectByName("Weapon (" + ((int)levelSelector.selectedWeapon + 1) + ")");

            //Debug.Log("Lvl (" + ((int)levelSelector.selectedLvl + 1) + ")");

            LEVEL selectedLvlPrev = levelSelector.selectedLvl;
            WEAPON_TYPE selectedWeaponPrev = levelSelector.selectedWeapon;

            if (levelSelector.selectedLvl != (LEVEL)lvl)
            {
                levelSelector.selectedLvl = (LEVEL)lvl;
            }
            //else
            //{
            //    levelSelector.selectedLvl = LEVEL.NONE;
            //    UI.ChangeImageUI(gameObject0, "Assets/UI/Teleport Buttons/BotonUnselected.png", (int)UI_STATE.NORMAL);
            //    //UI.SetUIState(gameObject, (int)UI_STATE.NORMAL);
            //}

            if (gameObject0 != null)
            {
                UI.ChangeImageUI(gameObject0, "Assets/UI/Teleport Buttons/BotonUnselected.png", (int)UI_STATE.NORMAL);
            }

            if (gameObject1 != null/* && levelSelector.selectedWeapon != selectedWeaponPrev*/)
            {
                switch (selectedWeaponPrev)
                {
                    case WEAPON_TYPE.NONE:
                        {
                            break;
                        }
                    case WEAPON_TYPE.SMG:
                        {
                            UI.ChangeImageUI(gameObject1, "Assets/UI/Teleport Buttons/SmgHover.png", (int)UI_STATE.NORMAL);
                            break;
                        }
                    case WEAPON_TYPE.SHOTGUN:
                        {
                            UI.ChangeImageUI(gameObject1, "Assets/UI/Teleport Buttons/ShotgunHover.png", (int)UI_STATE.NORMAL);
                            break;
                        }
                    case WEAPON_TYPE.PLASMA:
                        {
                            UI.ChangeImageUI(gameObject1, "Assets/UI/Teleport Buttons/LaserHover.png", (int)UI_STATE.NORMAL);
                            break;
                        }
                }
            }

            Debug.Log("Lvl: " + levelSelector.selectedLvl.ToString());
        }
    }

    public void OnHoverButton()
    {
        if (levelSelector != null)
        {
            switch ((LEVEL)lvl)
            {
                case LEVEL.NONE:
                    {
                        UI.TextEdit(levelSelector.lvlText, "");
                    }
                    break;
                case LEVEL.WAREHOUSE:
                    {
                        UI.TextEdit(levelSelector.lvlText, "An unused warehouse under the name of\nWeyland-Yutani corp. It doesn't look like a\ndangerous place, it smells a bit burnt.");
                    }
                    break;
                case LEVEL.LAB:
                    {
                        UI.TextEdit(levelSelector.lvlText, "There are no records of this place, although it\nlooks like a research area. A laboratory?\nUnderground?");
                    }
                    break;
                case LEVEL.HATCHERY:
                    {
                        UI.TextEdit(levelSelector.lvlText, "If this was once a laboratory, it is definitely\nno longer one. All of it is covered in something\nblack and slimy.");
                    }
                    break;

            }
        }
    }
}