using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Item_Stash : YmirComponent
{
    public string goName = "Stash Canvas";
    private Player player = null;

    public GameObject popup;
    bool show_menu = true;

    public void Start()
    {
        GameObject gameObject_ = InternalCalls.GetGameObjectByName("Player");

        if (gameObject_ != null)
        {
            player = gameObject_.GetComponent<Player>();
        }

        goName = "Stash Canvas";
        popup = InternalCalls.CS_GetChild(gameObject, 1);
        popup.SetActive(false);
    }

    public void Update()
    {
        popup.SetAsBillboard();
        return;
    }

    public void OnCollisionEnter(GameObject other)
    {
        if (other.Tag == "Player")
        {
            popup.SetActive(true);
            show_menu = true;
        }

    }
    public void OnCollisionStay(GameObject other)
    {
        if (show_menu)
        {
            if (other.Tag == "Player" && Input.IsGamepadButtonAPressedCS() || other.Name == "Player" && Input.IsGamepadButtonAPressedCS())
            {
                popup.SetActive(false);
                //player.currentMenu = goName;
                player.ToggleMenu(true, goName);
                show_menu = false;
            }

        }

        if (other.Tag == "Player" && Input.IsGamepadButtonBPressedCS() || other.Name == "Player" && Input.IsGamepadButtonBPressedCS())
        {
            //TODO: Añadir timer al darle a la B
            popup.SetActive(true);
            show_menu = true;
        }
    }
    public void OnCollisionExit(GameObject other)
    {
        if (other.Tag == "Player")
        {
            popup.SetActive(false);
            show_menu = true;
        }
    }
}