using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class UI_Item_Button : YmirComponent
{
    public Item item;
    public ITEM_SLOT currentSlot;
    public ITEM_SLOT itemType;

    public string enumItem = "";
    public string enumSlot = "";
    public string descriptionText = "";

    private GameObject _inventoryReference;

    public void Start()
    {
        GameObject goDescription = InternalCalls.GetChildrenByName(gameObject, "Description");
        GameObject goText = InternalCalls.GetChildrenByName(goDescription, "Text");
        descriptionText = UI.GetUIText(goText);

        _inventoryReference = InternalCalls.GetGameObjectByName("Inventory Menu");

        itemType = SetType(enumItem);
        currentSlot = SetType(enumSlot);

        item = new Item(currentSlot, itemType);
        //Debug.Log(item.currentSlot.ToString());
        //Debug.Log(item.itemType.ToString());
    }

    public void Update()
    {
        //if (_inventoryReference.GetComponent<UI_Inventory>()._focusedGO != null)
        //{
        //    if (InternalCalls.CompareGameObjectsByUID(_inventoryReference.GetComponent<UI_Inventory>()._focusedGO, gameObject))
        //    {
        //        UI.TextEdit(_inventoryReference.GetComponent<UI_Inventory>().goDescription, descriptionText);
        //    }
        //}
        return;
    }

    public void UpdateInfo()
    {
        if (_inventoryReference.GetComponent<UI_Inventory>().goDescription != null)
        {
            UI.TextEdit(_inventoryReference.GetComponent<UI_Inventory>().goText, descriptionText);
        }
    }

    private ITEM_SLOT SetType(string type)
    {
        ITEM_SLOT elementChanged = ITEM_SLOT.NONE;

        switch (type)
        {
            case "ARMOR":
                elementChanged = ITEM_SLOT.ARMOR;
                break;
            case "CHIP1":
                elementChanged = ITEM_SLOT.CHIP1;
                break;
            case "CHIP2":
                elementChanged = ITEM_SLOT.CHIP2;
                break;
            case "CONSUMABLE1":
                elementChanged = ITEM_SLOT.CONSUMABLE1;
                break;
            case "CONSUMABLE2":
                elementChanged = ITEM_SLOT.CONSUMABLE2;
                break;
            case "SAVE":
                elementChanged = ITEM_SLOT.SAVE;
                break;
            case "NONE":
                elementChanged = ITEM_SLOT.NONE;
                break;
            default:
                break;
        }

        return elementChanged;
    }

}