using System.Collections.Generic;
using System.Linq;
using YmirEngine;

public enum LEVEL
{
    BASE,
    WAREHOUSE,
    LAB,
    HATCHERY,

    NONE
}

public class BaseTeleporter : YmirComponent
{
    public LEVEL selectedLvl = LEVEL.NONE;
    public WEAPON_TYPE selectedWeapon = WEAPON_TYPE.NONE;

    public GameObject canvas, button, lvlText, weaponText;
    private Player csPlayer = null;

    private bool _setNormal = false;

    private GameObject _grid;

    public void Start()
    {
        button = InternalCalls.GetGameObjectByName("Go to raid");
        lvlText = InternalCalls.GetGameObjectByName("Lvl description");
        weaponText = InternalCalls.GetGameObjectByName("Weapon description");

        csPlayer = Globals.GetPlayerScript();

        canvas = InternalCalls.GetGameObjectByName("Level Selection Canvas");

        selectedLvl = LEVEL.NONE;
        selectedWeapon = WEAPON_TYPE.NONE;

        _grid = InternalCalls.GetGameObjectByName("Grid");
    }

    public void Update()
    {
        if (Input.GetGamepadButton(GamePadButton.B) == KeyState.KEY_DOWN)
        {
            csPlayer.PlayerStopState(false);
            canvas.SetActive(false);
        }

        if (!_setNormal && selectedLvl != LEVEL.NONE && selectedWeapon != WEAPON_TYPE.NONE)
        {
            Debug.Log("Lvl: " + selectedLvl.ToString() + " Weapon: " + selectedWeapon.ToString());

            UI.SetUIState(button, (int)UI_STATE.NORMAL);
            //UI.SetFirstFocused(button.parent);

            _grid.GetComponent<UI_Inventory_Grid>().navigateY = true;

            _setNormal = true;

            csPlayer.weaponType = selectedWeapon;

            Debug.Log("scene: " + button.GetComponent<Button_GoToScene>().sceneName);
        }
        else if ((UI_STATE)UI.GetUIState(button) != UI_STATE.DISABLED &&
            (selectedLvl == LEVEL.NONE || selectedWeapon == WEAPON_TYPE.NONE))
        {
            Debug.Log("Lvl: " + selectedLvl.ToString() + " Weapon: " + selectedWeapon.ToString());

            UI.SetUIState(button, (int)UI_STATE.DISABLED);
            _grid.GetComponent<UI_Inventory_Grid>().navigateY = false;

            _setNormal = false;
        }

        switch (selectedLvl)
        {
            case LEVEL.WAREHOUSE:
                {
                    button.GetComponent<Button_GoToScene>().sceneName = "LVL1_FINAL/LVL1_FINAL_COLLIDERS";
                }
                break;

            case LEVEL.LAB:
                {
                    button.GetComponent<Button_GoToScene>().sceneName = "LVL2_LAB_PART1_FINAL/LVL2_LAB_PART1_COLLIDERS";
                }
                break;

            case LEVEL.HATCHERY:
                {
                    button.GetComponent<Button_GoToScene>().sceneName = "LVL3_BlockOut/LVL3_PART1_COLLIDERS";
                }
                break;
        }

        return;
    }
}