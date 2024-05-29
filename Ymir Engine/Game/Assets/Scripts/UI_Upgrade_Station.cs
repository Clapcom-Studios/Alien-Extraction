using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using YmirEngine;

public class UI_Upgrade_Station : YmirComponent
{
    private GameObject _focusedGO;

    public GameObject description, cost, coins;
    public Player _player = null;

    public void Start()
    {
        _focusedGO = UI.GetFocused();
        description = InternalCalls.GetChildrenByName(gameObject, "Description");
        cost = InternalCalls.GetChildrenByName(gameObject, "Cost");        
        coins = InternalCalls.GetChildrenByName(gameObject, "Coins");

        _player = Globals.GetPlayerScript();
        UI.TextEdit(coins, _player.numCores.ToString());
    }

    public void Update()
    {
        return;
    }

    public void UpdateCoins()
    {
        UI.TextEdit(coins, _player.numCores.ToString());
    }
}