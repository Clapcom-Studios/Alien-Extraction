using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class OpenMenu_OnCollision : YmirComponent
{
    public string goName = "";
    private Player player = null;

    public void Start()
    {
        GameObject gameObject = InternalCalls.GetGameObjectByName("Player");

        if (gameObject != null)
        {
            player = gameObject.GetComponent<Player>();
        }
    }

    public void Update()
    {
        return;
    }

    public void OnCollisionEnter(GameObject other)
    {
        if (other.Tag == "Player" || other.Name == "Player")
        {
            //player.currentMenu = goName;
            player.ToggleMenu(true, goName);
        }
    }
}