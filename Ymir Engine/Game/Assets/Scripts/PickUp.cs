using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class PickUp : YmirComponent
{
    private bool picked = false;
    private Player player = null;
    private GameObject _itemPickedPopUp = new GameObject();
    private GameObject _inventoryFullPopUp = new GameObject();

    public void Start()
    {
        picked = false;
        _itemPickedPopUp = InternalCalls.GetGameObjectByName("ItemPicked");
        _inventoryFullPopUp = InternalCalls.GetGameObjectByName("InventoryFull");
    }

    public void Update()
    {

    }

    public void OnCollisionEnter(GameObject other)
    {
        if (other.Tag == "Player" && !picked)
        {
            player = other.GetComponent<Player>();

            if (gameObject.Tag == "Resin")
            {
                if (player.currentResinVessels < player.maxResinVessels)
                {
                    gameObject.SetActive(false);
                    Debug.Log("Pick up resin");
                    player.currentResinVessels++;

                    player.UpdateResin();

                    picked = true;
                    _itemPickedPopUp?.SetActive(true);
                    _itemPickedPopUp?.GetComponent<PickUpPopUp>().ResetTimer();

                    InternalCalls.Destroy(gameObject);
                }
            }
            else
            {
                int nonEquipped = 0;

                for (int i = 0; i < player.itemsList.Count; i++)
                {
                    if (!player.itemsList[i].isEquipped)
                    {
                        nonEquipped++;
                    }
                }

                // 15 --> inventory full
                if (nonEquipped < 14)
                {
                    Audio.PlayEmbedAudio(gameObject);

                    gameObject.SetActive(false);
                    Debug.Log("Pick up " + gameObject.Name);
                    player.itemsList.Add(Globals.SearchItemInDictionary(gameObject.Name));

                    if (gameObject.Name == "core_mythic")
                    {
                        player.numCores++;
                    }

                    picked = true;
                    _itemPickedPopUp?.SetActive(true);
                    _itemPickedPopUp?.GetComponent<PickUpPopUp>().ResetTimer();

                    InternalCalls.Destroy(gameObject);
                }
                else
                {
                    // TODO: Feedback inventory full
                    _inventoryFullPopUp?.SetActive(true);
                    _inventoryFullPopUp?.GetComponent<PickUpPopUp>().ResetTimer();
                }
            }
        }
    }
}