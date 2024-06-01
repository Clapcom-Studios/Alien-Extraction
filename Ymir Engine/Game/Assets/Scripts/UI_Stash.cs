using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class UI_Stash : YmirComponent
{
    private GameObject _selectedGO;
    public GameObject focusedGO, goDescription, goText, goName;

    private bool _show;

    public Player player = null;

    public List<Item> stashItemsList;

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
        stashItemsList = new List<Item>();

        ResetMenuSlots();
        LoadStashItems();
        SetSlots();
    }

    public void Update()
    {
        if (Input.GetKey(YmirKeyCode.R) == KeyState.KEY_DOWN)
        {
            LogStashItems();
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

            if (Input.GetGamepadButton(GamePadButton.LEFTSHOULDER) == KeyState.KEY_DOWN)
            {
                //UI.SetFirstFocused(gameObject);
                StashInventory();
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
                ManageItemsLists();

                UI.SwitchPosition(_selectedGO.parent, focusedGO.parent);

                //_show = false;
                //_focusedGO.GetComponent<UI_Item_Button>().ShowInfo(_show);
                //_selectedGO.GetComponent<UI_Item_Button>().ShowInfo(_show);

                ITEM_SLOT aux = _selectedGO.GetComponent<UI_Item_Button>().item.currentSlot;
                _selectedGO.GetComponent<UI_Item_Button>().item.currentSlot = focusedGO.GetComponent<UI_Item_Button>().item.currentSlot;
                focusedGO.GetComponent<UI_Item_Button>().item.currentSlot = aux;

                focusedGO.GetComponent<UI_Item_Button>().updateStats = true;
                _selectedGO.GetComponent<UI_Item_Button>().updateStats = true;

                SaveStashItems();
                //LogStashItems();
            }

            else
            {
                // maybe error sound?
            }

            UI.SetUIState(_selectedGO, (int)UI_STATE.NORMAL);
            UI.SetUIState(focusedGO, (int)UI_STATE.NORMAL);
        }
    }

    private void ManageItemsLists()
    {
        if (_selectedGO.parent.parent.Name != focusedGO.parent.parent.Name)
        {
            if (_selectedGO.parent.parent.Name == "Inventory Grid")
            {
                // Remove from player and add to stash
                if (_selectedGO.GetComponent<UI_Item_Button>().item.itemType != ITEM_SLOT.NONE)
                {
                    player.itemsList.Remove(_selectedGO.GetComponent<UI_Item_Button>().item);

                    if (_selectedGO.GetComponent<UI_Item_Button>().item.itemType != ITEM_SLOT.NONE)
                    {
                        stashItemsList.Add(_selectedGO.GetComponent<UI_Item_Button>().item);
                    }
                }
                // If empty swapped with existing item, remove from stash and add to player
                else
                {
                    if (_selectedGO.GetComponent<UI_Item_Button>().item.itemType != ITEM_SLOT.NONE)
                    {
                        _selectedGO.GetComponent<UI_Item_Button>().item.inInventory = false;
                        player.itemsList.Add(_selectedGO.GetComponent<UI_Item_Button>().item);
                    }

                    stashItemsList.Remove(_selectedGO.GetComponent<UI_Item_Button>().item);
                }

                // 
                if (focusedGO.GetComponent<UI_Item_Button>().item.itemType == ITEM_SLOT.NONE)
                {
                    player.itemsList.Remove(focusedGO.GetComponent<UI_Item_Button>().item);

                    if (focusedGO.GetComponent<UI_Item_Button>().item.itemType != ITEM_SLOT.NONE)
                    {
                        stashItemsList.Add(focusedGO.GetComponent<UI_Item_Button>().item);
                    }
                }
                // If swapped with existing item, remove from stash and add to player
                else
                {
                    if (_selectedGO.GetComponent<UI_Item_Button>().item.itemType != ITEM_SLOT.NONE)
                    {
                        focusedGO.GetComponent<UI_Item_Button>().item.inInventory = false;
                        player.itemsList.Add(focusedGO.GetComponent<UI_Item_Button>().item);
                    }

                    stashItemsList.Remove(focusedGO.GetComponent<UI_Item_Button>().item);
                }
            }
            else if (_selectedGO.parent.parent.Name == "Stash Grid")
            {
                // Remove from stash and add to player
                if (_selectedGO.GetComponent<UI_Item_Button>().item.itemType != ITEM_SLOT.NONE)
                {
                    if (_selectedGO.GetComponent<UI_Item_Button>().item.itemType != ITEM_SLOT.NONE)
                    {
                        _selectedGO.GetComponent<UI_Item_Button>().item.inInventory = false;
                        player.itemsList.Add(_selectedGO.GetComponent<UI_Item_Button>().item);
                    }

                    stashItemsList.Remove(_selectedGO.GetComponent<UI_Item_Button>().item);
                }
                // If empty swapped with existing item, remove from player and add to stash
                else
                {
                    player.itemsList.Remove(_selectedGO.GetComponent<UI_Item_Button>().item);

                    if (_selectedGO.GetComponent<UI_Item_Button>().item.itemType != ITEM_SLOT.NONE)
                    {
                        stashItemsList.Add(_selectedGO.GetComponent<UI_Item_Button>().item);
                    }
                }

                // 
                if (focusedGO.GetComponent<UI_Item_Button>().item.itemType == ITEM_SLOT.NONE)
                {
                    if (focusedGO.GetComponent<UI_Item_Button>().item.itemType != ITEM_SLOT.NONE)
                    {
                        focusedGO.GetComponent<UI_Item_Button>().item.inInventory = false;
                        player.itemsList.Add(focusedGO.GetComponent<UI_Item_Button>().item);
                    }

                    stashItemsList.Remove(focusedGO.GetComponent<UI_Item_Button>().item);
                }
                // If swapped with existing item, remove from player and add to stash
                else
                {
                    player.itemsList.Remove(focusedGO.GetComponent<UI_Item_Button>().item);

                    if (focusedGO.GetComponent<UI_Item_Button>().item.itemType != ITEM_SLOT.NONE)
                    {
                        stashItemsList.Add(focusedGO.GetComponent<UI_Item_Button>().item);
                    }
                }
            }

            Debug.Log("player: " + player.itemsList.Count.ToString());
            //for (int i = 0; i < player.itemsList.Count; i++)
            //{
            //    Debug.Log(player.itemsList[i].name);
            //}

            Debug.Log("stashItemsList: " + stashItemsList.Count.ToString());
            //for (int i = 0; i < stashItemsList.Count; i++)
            //{
            //    Debug.Log(stashItemsList[i].name);
            //}
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

    void UpdateTextPos()
    {
        UI.SetUIPosWithOther(goDescription, focusedGO.parent);// TODO: ARREGLAR - HO, FER SIGUI PARE TEXT
        UI.SetUIPosWithOther(goText, focusedGO.parent);
        UI.SetUIPosWithOther(goName, focusedGO.parent);
    }

    public void ShowText(bool isActive)
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

        for (int i = 0; i < stashItemsList.Count; i++)
        {
            GameObject stash = InternalCalls.CS_GetChild(gameObject, 1);

            for (int c = 0; c < InternalCalls.CS_GetChildrenSize(stash); c++)
            {
                GameObject button = InternalCalls.CS_GetChild(InternalCalls.CS_GetChild(stash, c), 2);  // (Grid (Slot (Button)))

                if (gameObject != null)
                {
                    if (button.GetComponent<UI_Item_Button>().SetItem(stashItemsList[i]))
                    {
                        break;
                    }
                }
            }
        }

        SaveStashItems();
    }

    private void ResetMenuSlots()
    {
        // Reset Slots to null to update
        GameObject inv = InternalCalls.CS_GetChild(gameObject, 1);

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

        // Reset Slots to null to update
        inv = InternalCalls.CS_GetChild(gameObject, 2);

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
    }

    public void SaveStashItems()
    {
        player.saveName = SaveLoad.LoadString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame);

        SaveLoad.SaveInt(Globals.saveGameDir, player.saveName, "Stash Items num", stashItemsList.Count);

        for (int i = 0; i < stashItemsList.Count; i++)
        {
            SaveLoad.SaveString(Globals.saveGameDir, player.saveName, "Stash Item " + i.ToString(), stashItemsList[i].dictionaryName);
            //SaveLoad.SaveBool(Globals.saveGameDir, player.saveName, "Stash Item in " + i.ToString(), stashItemsList[i].inStash);
        }
    }

    public void LoadStashItems()
    {
        player.saveName = SaveLoad.LoadString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame);

        Debug.Log("saveName " + player.saveName);

        for (int i = 0; i < SaveLoad.LoadInt(Globals.saveGameDir, player.saveName, "Stash Items num"); i++)
        {
            string name = SaveLoad.LoadString(Globals.saveGameDir, player.saveName, "Stash Item " + i.ToString());

            Item _item = Globals.SearchItemInDictionary(name);
            //_item.inStash = SaveLoad.LoadBool(Globals.saveGameDir, player.saveName, "Stash Item in " + i.ToString());
            stashItemsList.Add(_item);
        }

        Debug.Log("Stash loaded");
    }

    public void LogStashItems()
    {
        // Reset Slots to null to update
        GameObject inv = InternalCalls.CS_GetChild(gameObject, 1);

        Debug.Log("Stash");

        for (int c = 0; c < InternalCalls.CS_GetChildrenSize(inv); c++)
        {
            GameObject button = InternalCalls.CS_GetChild(InternalCalls.CS_GetChild(inv, c), 2);  // (Grid (Slot (Button)))

            if (gameObject != null)
            {
                Debug.Log("Stash item " + inv.ToString() + " name " + button.GetComponent<UI_Item_Button>().item.name);
            }
        }

        // Reset Slots to null to update
        inv = InternalCalls.CS_GetChild(gameObject, 2);

        for (int c = 0; c < InternalCalls.CS_GetChildrenSize(inv); c++)
        {
            GameObject button = InternalCalls.CS_GetChild(InternalCalls.CS_GetChild(inv, c), 2);  // (Grid (Slot (Button)))

            if (gameObject != null)
            {
                Debug.Log("Inv item " + inv.ToString() + "name " + button.GetComponent<UI_Item_Button>().item.name);
            }
        }

        for (int i = 0; i < stashItemsList.Count; i++)
        {
            Debug.Log("Stash list item: " + stashItemsList[i].name);
        }
    }

    private void StashInventory()
    {
        for (int i = 0; i < player.itemsList.Count; i++)
        {
            stashItemsList.Add(player.itemsList[i]);   
        }

        for (int i = 0; i < stashItemsList.Count; i++)
        {
            player.itemsList.Remove(stashItemsList[i]);
        }

        SaveStashItems();
        player.SaveItems();
        ResetMenuSlots();
        SetSlots();
    }
}