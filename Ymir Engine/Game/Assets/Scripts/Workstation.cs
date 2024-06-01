using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Workstation : YmirComponent
{
    public string goName = "Upgrade Station";
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

        goName = "Upgrade Station";
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
                player.currentMenu = goName;
                player.ToggleMenu(true);
                show_menu = false;
            }
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