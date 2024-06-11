using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.ConstrainedExecution;
using System.Runtime.InteropServices;

using YmirEngine;

public class UI_Inventory : YmirComponent
{
    private GameObject _selectedGO, _textHP, _textArmor, _textSpeed, _textReload, _textDamage, _textRate, _textResin;
    public GameObject focusedGO, goDescription, goText, goName;

    private bool _show;

    public Player player = null;
    public Health health = null;

    public void Start()
    {
        focusedGO = UI.GetFocused();
        _selectedGO = UI.GetSelected();

        goDescription = InternalCalls.GetChildrenByName(gameObject, "Item Description Image"); // TODO: ARREGLAR-HO, FER SIGUI PARE TEXT
        goText = InternalCalls.GetChildrenByName(gameObject, "Item Description Text");
        goName = InternalCalls.GetChildrenByName(gameObject, "Item Description Name");

        goDescription.SetActive(false);// TODO: when menu opened
        goText.SetActive(false);
        goName.SetActive(false);

        _show = false;

        player = Globals.GetPlayerScript();

        health = Globals.GetPlayerHealthScript();

        switch (player.weaponType)
        {
            case WEAPON_TYPE.NONE:
                {
                    UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon"),
                        "Assets\\/UI\\/Fondo.png", (int)UI_STATE.NORMAL);
                }
                break;
            case WEAPON_TYPE.SMG:
                {
                    switch (player.upgradeType)
                    {
                        case UPGRADE.LVL_0:

                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon"),
                                "Assets\\UI\\HUD Buttons\\Icons\\SmgHUD.png", (int)UI_STATE.NORMAL);
                            break;
                        case UPGRADE.LVL_1:

                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon"),
                                "Assets\\UI\\HUD Buttons\\Icons\\SmgHUD.png", (int)UI_STATE.NORMAL);
                            break;
                        case UPGRADE.LVL_2:

                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon"),
                                "Assets\\UI\\HUD Buttons\\Icons\\SmgLvl2.png", (int)UI_STATE.NORMAL);
                            break;
                        case UPGRADE.LVL_3_ALPHA:

                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon"),
                                "Assets\\UI\\HUD Buttons\\Icons\\SmgLvl31.png", (int)UI_STATE.NORMAL);
                            break;
                        case UPGRADE.LVL_3_BETA:

                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon"),
                                "Assets\\UI\\HUD Buttons\\Icons\\SmgLvl3B.png", (int)UI_STATE.NORMAL);
                            break;
                        default:
                            break;
                    }
                }
                break;

            case WEAPON_TYPE.SHOTGUN:
                {
                    switch (player.upgradeType)
                    {
                        case UPGRADE.LVL_0:

                            player.currentWeapon = player.w_Shotgun_0.GetComponent<Shotgun>();
                            player.w_Shotgun_0.SetActive(true);
                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon Image"),
                                "Assets\\UI\\HUD Buttons\\Icons\\ShotgunHUD.png", (int)UI_STATE.NORMAL);
                            break;
                        case UPGRADE.LVL_1:

                            player.currentWeapon = player.w_Shotgun_1.GetComponent<Shotgun>();
                            player.w_Shotgun_1.SetActive(true);
                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon Image"),
                                "Assets\\UI\\HUD Buttons\\Icons\\ShotgunHUD.png", (int)UI_STATE.NORMAL);
                            break;
                        case UPGRADE.LVL_2:

                            player.currentWeapon = player.w_Shotgun_2.GetComponent<Shotgun>();
                            player.w_Shotgun_2.SetActive(true);
                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon Image"),
                                "Assets\\UI\\HUD Buttons\\Icons\\Shotgunlvl2.png", (int)UI_STATE.NORMAL);
                            break;
                        case UPGRADE.LVL_3_ALPHA:

                            player.currentWeapon = player.w_Shotgun_3a.GetComponent<Shotgun>();
                            player.w_Shotgun_3a.SetActive(true);
                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon Image"),
                                "Assets\\UI\\HUD Buttons\\Icons\\ShotgunLvl31.png", (int)UI_STATE.NORMAL);
                            break;
                        case UPGRADE.LVL_3_BETA:

                            player.currentWeapon = player.w_Shotgun_3b.GetComponent<Shotgun>();
                            player.w_Shotgun_3b.SetActive(true);
                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon Image"),
                                "Assets\\UI\\HUD Buttons\\Icons\\ShotgunLvl3B.png", (int)UI_STATE.NORMAL);
                            break;
                        default:
                            break;
                    }
                }
                break;

            case WEAPON_TYPE.PLASMA:
                {
                    switch (player.upgradeType)
                    {
                        case UPGRADE.LVL_0:

                            player.currentWeapon = player.w_Plasma_0.GetComponent<Plasma>();
                            player.w_Plasma_0.SetActive(true);
                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon Image"),
                                "Assets\\UI\\HUD Buttons\\Icons\\LaserHUD.png", (int)UI_STATE.NORMAL);
                            break;
                        case UPGRADE.LVL_1:

                            player.currentWeapon = player.w_Plasma_1.GetComponent<Plasma>();
                            player.w_Plasma_1.SetActive(true);
                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon Image"),
                                "Assets\\UI\\HUD Buttons\\Icons\\LaserHUD.png", (int)UI_STATE.NORMAL);
                            break;
                        case UPGRADE.LVL_2:

                            player.currentWeapon = player.w_Plasma_2.GetComponent<Plasma>();
                            player.w_Plasma_2.SetActive(true);
                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon Image"),
                                "Assets\\UI\\HUD Buttons\\Icons\\LaserHUD.png", (int)UI_STATE.NORMAL);
                            break;
                        case UPGRADE.LVL_3_ALPHA:

                            player.currentWeapon = player.w_Plasma_3a.GetComponent<Plasma>();
                            player.w_Plasma_3a.SetActive(true);
                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon Image"),
                                "Assets\\UI\\HUD Buttons\\Icons\\LaserLvl31.png", (int)UI_STATE.NORMAL);
                            break;
                        case UPGRADE.LVL_3_BETA:

                            player.currentWeapon = player.w_Plasma_3b.GetComponent<Plasma>();
                            player.w_Plasma_3b.SetActive(true);
                            UI.ChangeImageUI(InternalCalls.GetGameObjectByName("Weapon Image"),
                                "Assets\\UI\\HUD Buttons\\Icons\\LaserLvl3B.png", (int)UI_STATE.NORMAL);
                            break;
                        default:
                            break;
                    }
                }
                break;
        }

        _textHP = InternalCalls.GetGameObjectByName("Text HP");
        _textArmor = InternalCalls.GetGameObjectByName("Text Armor");
        _textSpeed = InternalCalls.GetGameObjectByName("Text Speed");
        _textReload = InternalCalls.GetGameObjectByName("Text Reload");
        _textDamage = InternalCalls.GetGameObjectByName("Text Damage");
        _textRate = InternalCalls.GetGameObjectByName("Text Rate");
        _textResin = InternalCalls.GetGameObjectByName("Text Resin");

        ResetMenuSlots();

        UpdateTextStats();

        SetSlots();
    }

    public void Update()
    {
        if (Input.GetKey(YmirKeyCode.T) == KeyState.KEY_DOWN)
        {
            LogInventoryItems();
        }

        focusedGO = UI.GetFocused();// call this when menu starts or when changed, not efficient rn

        UI_Item_Button cs_UI_Item_Button = focusedGO.GetComponent<UI_Item_Button>();

        if (focusedGO != null)
        {
            if (Input.GetGamepadButton(GamePadButton.RIGHTSHOULDER) == KeyState.KEY_REPEAT)
            {
                if (!_show)
                {
                    _show = true;
                    ShowText(_show);
                }

                UpdateTextPos();
                cs_UI_Item_Button.UpdateInfo();
            }

            else if (Input.GetGamepadButton(GamePadButton.RIGHTSHOULDER) == KeyState.KEY_UP)
            {
                if (_show)
                {
                    _show = false;
                    ShowText(_show);
                }
            }

            if (Input.GetGamepadButton(GamePadButton.A) == KeyState.KEY_DOWN)
            {
                SwitchItems();
            }

            if (Input.GetGamepadButton(GamePadButton.X) == KeyState.KEY_DOWN)
            {
                UI.SetFirstFocused(gameObject);
            }

            if (cs_UI_Item_Button != null)
            {
                // Si se quita peta xd
                cs_UI_Item_Button.item.itemType.ToString();
                //cs_UI_Item_Button.item.currentSlot.ToString();
                //

                if (((cs_UI_Item_Button.item.itemType != ITEM_SLOT.NONE ||
                    cs_UI_Item_Button.item.itemType != ITEM_SLOT.SAVE) &&
                    cs_UI_Item_Button.item.currentSlot == ITEM_SLOT.NONE) &&
                    Input.GetGamepadButton(GamePadButton.LEFTSHOULDER) == KeyState.KEY_DOWN)
                {
                    //Delete item from player list
                    player.itemsList.Remove(cs_UI_Item_Button.item);

                    // Change current item to an empty one
                    cs_UI_Item_Button.item.itemType = ITEM_SLOT.NONE;
                    cs_UI_Item_Button.itemType = ITEM_SLOT.NONE;
                    cs_UI_Item_Button.SetInspectorType(ITEM_SLOT.NONE);
                    cs_UI_Item_Button.item.itemRarity = ITEM_RARITY.NONE;
                    cs_UI_Item_Button.itemRarity = ITEM_RARITY.NONE;
                    cs_UI_Item_Button.item = cs_UI_Item_Button.CreateItemBase();
                    cs_UI_Item_Button.UpdateInfo();
                }
            }

            //Debug.Log(_cs_UI_Item_Button.item.itemType.ToString());
            //Debug.Log(_cs_UI_Item_Button.item.currentSlot.ToString());
        }

        //if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN)
        //{
        //    Deactivate();
        //}

        return;
    }

    private void SwitchItems()
    {
        _selectedGO = UI.GetSelected();

        if (_selectedGO != null)
        {
            //Debug.Log(_selectedGO.GetComponent<UI_Item_Button>().item.itemType.ToString());
            //Debug.Log(_selectedGO.GetComponent<UI_Item_Button>().item.currentSlot.ToString());

            if ((_selectedGO.GetComponent<UI_Item_Button>().item.itemType == focusedGO.GetComponent<UI_Item_Button>().item.currentSlot &&
                _selectedGO.GetComponent<UI_Item_Button>().item.itemType != ITEM_SLOT.NONE) ||
                (focusedGO.GetComponent<UI_Item_Button>().item.currentSlot == ITEM_SLOT.NONE && focusedGO.GetComponent<UI_Item_Button>().item.itemType == ITEM_SLOT.NONE) ||
                (focusedGO.GetComponent<UI_Item_Button>().item.currentSlot == ITEM_SLOT.SAVE && _selectedGO.GetComponent<UI_Item_Button>().item.currentSlot == ITEM_SLOT.NONE) ||
                (focusedGO.GetComponent<UI_Item_Button>().item.currentSlot == ITEM_SLOT.NONE && _selectedGO.GetComponent<UI_Item_Button>().item.currentSlot == ITEM_SLOT.NONE))
            {
                UI.SwitchPosition(_selectedGO.parent, focusedGO.parent);

                //_show = false;
                //_focusedGO.GetComponent<UI_Item_Button>().ShowInfo(_show);
                //_selectedGO.GetComponent<UI_Item_Button>().ShowInfo(_show);

                ITEM_SLOT aux = _selectedGO.GetComponent<UI_Item_Button>().item.currentSlot;
                _selectedGO.GetComponent<UI_Item_Button>().item.currentSlot = focusedGO.GetComponent<UI_Item_Button>().item.currentSlot;
                focusedGO.GetComponent<UI_Item_Button>().item.currentSlot = aux;

                focusedGO.GetComponent<UI_Item_Button>().updateStats = true;
                _selectedGO.GetComponent<UI_Item_Button>().updateStats = true;
            }

            else
            {
                // maybe error sound?
            }

            UI.SetUIState(_selectedGO, (int)UI_STATE.NORMAL);
            //UI.SetUIState(focusedGO, (int)UI_STATE.NORMAL);
        }
    }

    private void SwitchMenu()
    {
        // Can't do it with names
        GameObject inventoryGO = InternalCalls.GetGameObjectByName("Inventory");

        if (InternalCalls.CompareGameObjectsByUID(InternalCalls.CS_GetParent(focusedGO), inventoryGO))
        {
            GameObject gridGO = InternalCalls.GetGameObjectByName("Grid Armor");
            UI.SetUIState(InternalCalls.CS_GetChild(gridGO, 0), (int)UI_STATE.FOCUSED);
        }

        else
        {
            UI.SetUIState(InternalCalls.CS_GetChild(inventoryGO, 0), (int)UI_STATE.FOCUSED);
        }
    }

    void UpdateTextPos() // Place the descrition game object on the selected GO
    {
        UI.SetUIPosWithOther(goDescription, focusedGO.parent);// TODO: ARREGLAR - HO, FER SIGUI PARE TEXT
        UI.SetUIPosWithOther(goText, focusedGO.parent);
        UI.SetUIPosWithOther(goName, focusedGO.parent);
    }

    public void ShowText(bool isActive) // Show description of item when pressing R1
    {
        goDescription.SetActive(isActive);
        goText.SetActive(isActive);// TODO: ARREGLAR - HO, FER SIGUI PARE TEXT
        goName.SetActive(isActive);
    }

    public void UpdateTextStats() // Print player info on the screen
    {
        if (health != null)
        {
            UI.TextEdit(_textHP, health.currentHealth.ToString());
            UI.TextEdit(_textArmor, health.armor.ToString());
        }

        if (player != null)
        {
            UI.TextEdit(_textSpeed, player.movementSpeed.ToString());

            if (Globals.GetPlayerScript().currentWeapon != null)
            {
                UI.TextEdit(_textRate, player.currentWeapon.fireRate.ToString());
                UI.TextEdit(_textReload, player.currentWeapon.reloadTime.ToString() + "%");
            }
            else
            {
                UI.TextEdit(_textRate, "0");
                UI.TextEdit(_textReload, "0" + "%");
            }

            UI.TextEdit(_textDamage, player.damageMultiplier.ToString() + "%");
            UI.TextEdit(_textResin, player.currentResinVessels.ToString());
        }
    }

    private void SetSlots() // Place the items from player to inventory
    {
        bool isInventory = false;

        for (int i = 0; i < player.itemsList.Count; i++)
        {
            //player.itemsList[i].LogStats();

            if (!player.itemsList[i].inInventory)
            {
                isInventory = true;

                GameObject character = InternalCalls.CS_GetChild(gameObject, 1);
                GameObject inventory = InternalCalls.CS_GetChild(gameObject, 2);

                for (int c = 0; c < InternalCalls.CS_GetChildrenSize(character); c++)
                {
                    GameObject button = InternalCalls.CS_GetChild(InternalCalls.CS_GetChild(InternalCalls.CS_GetChild(character, c), 0), 2);  // (Grid (Slot (Button)))                                                                                                                                             //Debug.Log("button name " + button.Name);

                    if (button != null)
                    {
                        if (button.GetComponent<UI_Item_Button>().SetItem(player.itemsList[i]))
                        {
                            isInventory = false;
                            player.itemsList[i].inInventory = true;
                            Debug.Log("button name " + button.Name);
                            break;
                        }
                    }
                }

                if (isInventory)
                {
                    for (int inv = 0; inv < InternalCalls.CS_GetChildrenSize(inventory); inv++)
                    {
                        GameObject button = InternalCalls.CS_GetChild(InternalCalls.CS_GetChild(inventory, inv), 2);  // (Slot (Button)))                                                                                                              //Debug.Log("button name " + button.Name);

                        if (button != null)
                        {
                            if (button.GetComponent<UI_Item_Button>().SetItem(player.itemsList[i]))
                            {
                                //player.itemsList[i].inInventory = true;
                                Debug.Log("button name " + button.Name);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    private void ResetMenuSlots()
    {
        // Reset Slots to null to update
        GameObject inv = InternalCalls.CS_GetChild(gameObject, 2);

        for (int c = 0; c < InternalCalls.CS_GetChildrenSize(inv); c++)
        {
            GameObject button = InternalCalls.CS_GetChild(InternalCalls.CS_GetChild(inv, c), 2);  // (Grid (Slot (Button)))

            if (button != null)
            {
                if (button.GetComponent<UI_Item_Button>().item != null)
                {
                    button.GetComponent<UI_Item_Button>().ResetSlot();
                    button.GetComponent<UI_Item_Button>().item = button.GetComponent<UI_Item_Button>().CreateItemBase();
                }
            }
        }
    }

    public void LogInventoryItems()
    {
        GameObject inventory = InternalCalls.CS_GetChild(gameObject, 2);

        Debug.Log("Inventory");

        for (int inv = 0; inv < InternalCalls.CS_GetChildrenSize(inventory); inv++)
        {
            GameObject button = InternalCalls.CS_GetChild(InternalCalls.CS_GetChild(inventory, inv), 2);  // (Slot (Button)))                                                                                                              //Debug.Log("button name " + button.Name);

            if (button != null)
            {
                button.GetComponent<UI_Item_Button>().item.LogStats();
                //Debug.Log("Inventory item " + inv.ToString() + " name " + button.GetComponent<UI_Item_Button>().item.name);
            }
        }

        for (int i = 0; i < player.itemsList.Count; i++)
        {
            Debug.Log("Player list item: " + player.itemsList[i].name);
        }
    }
}