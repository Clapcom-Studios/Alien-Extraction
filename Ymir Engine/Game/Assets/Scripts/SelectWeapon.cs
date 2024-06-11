using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class SelectWeapon : YmirComponent
{
    public int weapon = 0;  // TODO: Sara --> change to enum
    private BaseTeleporter levelSelector;

    public void Start()
    {
        GameObject gameObject = InternalCalls.GetGameObjectByName("Background");
        if (gameObject != null)
        {
            levelSelector = gameObject.GetComponent<BaseTeleporter>();
        }
    }

    public void Update()
    {
        return;
    }

    public void OnClickButton()
    {
        GameObject gameObject0 = InternalCalls.GetGameObjectByName("Lvl (" + ((int)levelSelector.selectedLvl) + ")");
        GameObject gameObject1 = InternalCalls.GetGameObjectByName("Weapon (" + ((int)levelSelector.selectedWeapon + 1) + ")");

        //Debug.Log("Weapon (" + ((int)levelSelector.selectedWeapon + 1) + ")");

        LEVEL selectedLvlPrev = levelSelector.selectedLvl;
        WEAPON_TYPE selectedWeaponPrev = levelSelector.selectedWeapon;

        if (levelSelector.selectedWeapon != (WEAPON_TYPE)weapon)
        {
            levelSelector.selectedWeapon = (WEAPON_TYPE)weapon;
            Globals.GetPlayerScript().weaponType = (WEAPON_TYPE)weapon;
        }
        //else
        //{
        //    levelSelector.selectedWeapon = WEAPON_TYPE.NONE;
        //    UI.ChangeImageUI(gameObject0, "Assets/UI/Teleport Buttons/BotonUnselected.png", (int)UI_STATE.NORMAL);
        //}

        if (gameObject0 != null/* && levelSelector.selectedLvl != selectedLvlPrev*/)
        {
            UI.ChangeImageUI(gameObject0, "Assets/UI/Teleport Buttons/BotonHover.png", (int)UI_STATE.NORMAL);
        }

        if (gameObject1 != null)
        {
            switch (selectedWeaponPrev)
            {
                case WEAPON_TYPE.NONE:
                    {
                        break;
                    }
                case WEAPON_TYPE.SMG:
                    {
                        UI.ChangeImageUI(gameObject1, "Assets/UI/Teleport Buttons/SmgUnselected.png", (int)UI_STATE.NORMAL);
                        break;
                    }
                case WEAPON_TYPE.SHOTGUN:
                    {
                        UI.ChangeImageUI(gameObject1, "Assets/UI/Teleport Buttons/ShotgunUnselected.png", (int)UI_STATE.NORMAL);
                        break;
                    }
                case WEAPON_TYPE.PLASMA:
                    {
                        UI.ChangeImageUI(gameObject1, "Assets/UI/Teleport Buttons/LaserUnselected.png", (int)UI_STATE.NORMAL);
                        break;
                    }
            }
        }

        Debug.Log("Weapon: " + levelSelector.selectedWeapon.ToString());

        //UI.TextEdit(levelSelector.weaponText, levelSelector.weaponDescriptions[(int)levelSelector.selectedWeapon]);
    }

    public void OnHoverButton()
    {
        if (levelSelector != null)
        {
            switch ((WEAPON_TYPE)weapon)
            {
                case WEAPON_TYPE.NONE:
                    {
                        UI.TextEdit(levelSelector.weaponText, "");
                    }
                    break;
                case WEAPON_TYPE.SMG:
                    {
                        UI.TextEdit(levelSelector.weaponText, "The 'S.T.A.P.L.E.R.' is a fast submachine gun\ncapable of blasting multiple low damage bullets\nin less than a second, perfect for a fast cleaning.");
                    }
                    break;
                case WEAPON_TYPE.SHOTGUN:
                    {
                        UI.TextEdit(levelSelector.weaponText, "The 'Spar-X' is a shotgun that allows\nyou to eliminate multiple enemies in\none shot, perfect for close-distance combat.");
                    }
                    break;
                case WEAPON_TYPE.PLASMA:
                    {
                        UI.TextEdit(levelSelector.weaponText, "The 'EM-MK2' is a plasma weapon capable of firing\na constant beam of lethal energy, ideal for\nsuppressing enemies in medium and long-range\ncombat.");
                    }
                    break;

            }
        }
    }
}