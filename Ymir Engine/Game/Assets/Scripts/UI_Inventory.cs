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
    public GameObject focusedGO, goDescription, goText;
    public bool _droppable = true, _disable = false;
    private bool _show;
    public Player player = null;
    public Health health = null;

    public void Start()
    {
        focusedGO = UI.GetFocused();
        _selectedGO = UI.GetSelected();

        goDescription = InternalCalls.GetChildrenByName(gameObject, "Item Description Image"); // TODO: ARREGLAR-HO, FER SIGUI PARE TEXT
        goText = InternalCalls.GetChildrenByName(gameObject, "Item Description Text");
        goDescription.SetActive(false);// TODO: when menu opened
        goText.SetActive(false);
       
        _disable = false;
        _show = false;

        GetPlayerScript();
        GetHealthScript();

        if (_droppable)
        {
            _textHP = InternalCalls.GetGameObjectByName("Text HP");
            _textArmor = InternalCalls.GetGameObjectByName("Text Armor");
            _textSpeed = InternalCalls.GetGameObjectByName("Text Speed");
            _textReload = InternalCalls.GetGameObjectByName("Text Reload");
            _textDamage = InternalCalls.GetGameObjectByName("Text Damage");
            _textRate = InternalCalls.GetGameObjectByName("Text Rate");
            _textResin = InternalCalls.GetGameObjectByName("Text Resin");
        }

        //UpdateTextStats();

        //UI.SetFirstFocused(gameObject); // TODO: MissingMethodException WHY?
    }

    public void Update()
    {
        if (player == null)
        {
            GetPlayerScript();
        }

        if (player != null && player.setHover)
        {
            Debug.Log("set first");
            goDescription.SetActive(false);// TODO: when menu opened
            goText.SetActive(false);

            player.setHover = false;
        }

        focusedGO = UI.GetFocused();// call this when menu starts or when changed, not efficient rn

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
                focusedGO.GetComponent<UI_Item_Button>().UpdateInfo();
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

            if (focusedGO.GetComponent<UI_Item_Button>() != null && _droppable)
            {
                Debug.Log(" " + focusedGO.GetComponent<UI_Item_Button>().item.itemType.ToString());
                Debug.Log(" " + focusedGO.GetComponent<UI_Item_Button>().item.currentSlot.ToString());

                if (((focusedGO.GetComponent<UI_Item_Button>().item.itemType != ITEM_SLOT.NONE ||
                                focusedGO.GetComponent<UI_Item_Button>().item.itemType != ITEM_SLOT.SAVE) &&
                                focusedGO.GetComponent<UI_Item_Button>().item.currentSlot == ITEM_SLOT.NONE) &&
                                Input.GetGamepadButton(GamePadButton.LEFTSHOULDER) == KeyState.KEY_DOWN)
                {
                    Debug.Log("333333");
                    focusedGO.GetComponent<UI_Item_Button>().item.currentSlot = ITEM_SLOT.NONE;
                    Debug.Log("444444444");
                    focusedGO.GetComponent<UI_Item_Button>().item.itemType = ITEM_SLOT.NONE;
                    Debug.Log("66666666666");

                    // Add real art and other stuff

                    GameObject imageItem = InternalCalls.GetChildrenByName(focusedGO.parent, "Image Item");

                    UI.ChangeImageUI(imageItem, "Assets/UI/Inventory Buttons/New Buttons/Unselected.png", (int)UI_STATE.NORMAL);

                    focusedGO.GetComponent<UI_Item_Button>().descriptionText = "Empty";
                    focusedGO.GetComponent<UI_Item_Button>().UpdateInfo();
                }
            }

            //Debug.Log(_focusedGO.GetComponent<UI_Item_Button>().item.itemType.ToString());
            //Debug.Log(_focusedGO.GetComponent<UI_Item_Button>().item.currentSlot.ToString());
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
                (focusedGO.GetComponent<UI_Item_Button>().item.currentSlot == ITEM_SLOT.SAVE && _selectedGO.GetComponent<UI_Item_Button>().item.currentSlot == ITEM_SLOT.NONE)||
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
    }

    public void ShowText(bool isActive)
    {
        goDescription.SetActive(isActive);
        goText.SetActive(isActive);// TODO: ARREGLAR - HO, FER SIGUI PARE TEXT
    }
    private void GetPlayerScript()
    {
        GameObject gameObject = InternalCalls.GetGameObjectByName("Player");

        if (gameObject != null)
        {
            player = gameObject.GetComponent<Player>();
        }
    }   
    
    private void GetHealthScript()
    {
        GameObject gameObject = InternalCalls.GetGameObjectByName("Player");

        if (gameObject != null)
        {
            health = gameObject.GetComponent<Health>();
        }
    }

    public void UpdateTextStats()
    {
        if (player != null)
        {
            UI.TextEdit(_textSpeed, player.movementSpeed.ToString());
            UI.TextEdit(_textRate, player.fireRate.ToString());
            UI.TextEdit(_textReload, player.reloadDuration.ToString());
            UI.TextEdit(_textDamage, player.damageMultiplier.ToString());
            UI.TextEdit(_textResin, player.resin.ToString());
        }

        if (health != null)
        {
            UI.TextEdit(_textHP, health.currentHealth.ToString());
            UI.TextEdit(_textArmor, health.armor.ToString());
        }
    }

}

