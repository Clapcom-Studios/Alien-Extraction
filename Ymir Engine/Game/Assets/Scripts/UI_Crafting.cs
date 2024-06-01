using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using YmirEngine;

public class UI_Crafting : YmirComponent
{
    private GameObject _selectedGO;
    public GameObject focusedGO, goDescription, goText, goName;

    private bool _show;

    public Player player = null;
    public Health health = null;

    private bool _startCheck = false;

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

        // TODO: Very bad postupdate, fix
        _startCheck = false;

        player = Globals.GetPlayerScript();

        ResetMenuSlots();
        SetSlots();
    }

    public void Update()
    {
        focusedGO = UI.GetFocused();// call this when menu starts or when changed, not efficient rn

        if (_startCheck)
        {
            _startCheck = false;
            focusedGO.parent.parent.GetComponent<UI_Crafting_Recipe>().Check();
        }

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

            if (Input.GetGamepadButton(GamePadButton.A) == KeyState.KEY_DOWN || Input.GetKey(YmirKeyCode.RETURN) == KeyState.KEY_DOWN)
            {
                SwitchItems();
            }

            if (Input.GetGamepadButton(GamePadButton.X) == KeyState.KEY_DOWN)
            {
                UI.SetFirstFocused(gameObject);
            }

            //Debug.Log(focusedGO.GetComponent<UI_Item_Button>().item.itemType.ToString());
            //Debug.Log(focusedGO.GetComponent<UI_Item_Button>().item.currentSlot.ToString());
        }

        return;
    }

    private void SwitchItems()
    {
        _selectedGO = UI.GetSelected();

        if (_selectedGO != null)
        {
            //Debug.Log(_selectedGO.GetComponent<UI_Item_Button>().item.itemType.ToString());
            //Debug.Log(_selectedGO.GetComponent<UI_Item_Button>().item.currentSlot.ToString());

            if ((UI.CompareStringToName(focusedGO.parent, _selectedGO.GetComponent<UI_Item_Button>().item.name)) ||
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

                // Check item craft
                if (_selectedGO.GetComponent<UI_Item_Button>().item.currentSlot == ITEM_SLOT.MATERIAL)
                {
                    _startCheck = true;
                }

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

    void UpdateTextPos() // Place the description game object on the selected GO
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

    private void SetSlots()
    {
        bool isInventory = false;

        for (int i = 0; i < player.itemsList.Count; i++)
        {
            GameObject inventory = InternalCalls.CS_GetChild(gameObject, 2);
            GameObject save = InternalCalls.CS_GetChild(gameObject, 3);

            for (int inv = 0; inv < InternalCalls.CS_GetChildrenSize(inventory); inv++)
            {
                GameObject button = InternalCalls.CS_GetChild(InternalCalls.CS_GetChild(inventory, inv), 2);  // (Slot (Button)))
                isInventory = true;

                if (gameObject != null)
                {
                    if (button.GetComponent<UI_Item_Button>().SetItem(player.itemsList[i]))
                    {
                        isInventory = false;
                        break;
                    }
                }
            }

            if (isInventory)
            {
                for (int s = 0; s < InternalCalls.CS_GetChildrenSize(save); s++)
                {
                    GameObject button = InternalCalls.CS_GetChild(InternalCalls.CS_GetChild(save, s), 2);  // (Slot (Button)))

                    if (gameObject != null)
                    {
                        if (button.GetComponent<UI_Item_Button>().SetItem(player.itemsList[i]))
                        {
                            break;
                        }
                    }
                }
            }
        }
    }

    private void ResetMenuSlots()
    {
        // Reset inventory Slots to null to update
        GameObject inv = InternalCalls.CS_GetChild(gameObject, 2);

        for (int c = 0; c < InternalCalls.CS_GetChildrenSize(inv); c++)
        {
            GameObject button = InternalCalls.CS_GetChild(InternalCalls.CS_GetChild(inv, c), 2);  // (Grid (Slot (Button)))

            if (gameObject != null)
            {
                if (button.GetComponent<UI_Item_Button>().item != null)
                {
                    button.GetComponent<UI_Item_Button>().ResetSlot();
                    button.GetComponent<UI_Item_Button>().item = button.GetComponent<UI_Item_Button>().CreateItemBase();
                }
            }
        }

        // Reset crafting Slots to null to update
        GameObject craft = InternalCalls.CS_GetChild(gameObject, 1);

        for (int c = 0; c < InternalCalls.CS_GetChildrenSize(craft); c++)
        {
            GameObject recipe = InternalCalls.CS_GetChild(craft, c);  // Get crafting recipe, buttons with item are inside it

            for (int j = 0; j < InternalCalls.CS_GetChildrenSize(recipe); j++)
            {
                GameObject button = InternalCalls.CS_GetChild(InternalCalls.CS_GetChild(recipe, j), 2);  // (Grid (Slot (Button)))

                if (gameObject != null)
                {
                    if (button.GetComponent<UI_Item_Button>().item != null)
                    {
                        button.GetComponent<UI_Item_Button>().ResetSlot();
                        button.GetComponent<UI_Item_Button>().item = button.GetComponent<UI_Item_Button>().CreateItemBase();
                    }
                }
            }
        }
    }
}