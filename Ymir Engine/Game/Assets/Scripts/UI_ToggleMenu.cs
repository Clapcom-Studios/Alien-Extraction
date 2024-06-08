using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class UI_ToggleMenu : YmirComponent
{
    public string menu = "";
    public bool active = true;

    private Player player;

    public void Start()
    {
        player = Globals.GetPlayerScript();
    }

    public void Update()
    {
        active = true;
        return;
    }

    public void OnClickButton()
    {
        active = !active;

        if (player != null)
        {
            //player.currentMenu = menu;
            player.ToggleMenu(active, menu);
        }
    }
}