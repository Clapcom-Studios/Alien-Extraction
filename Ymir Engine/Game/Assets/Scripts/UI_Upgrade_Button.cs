using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class UI_Upgrade_Button : YmirComponent
{
    public Upgrade upgrade;

    public string name = "";
    public string description = "";
    public int cost;
    public bool isUnlocked;
    public string stationName = "";

    public UI_Upgrade_Station currentStation;
    private GameObject _parent;
    private GameObject audioSource;
    private bool _setFocused = false;

    public void Start()
    {
        GameObject goText = InternalCalls.GetChildrenByName(gameObject, "Text");
        description = UI.GetUIText(goText);
        audioSource = InternalCalls.GetGameObjectByName("UI Audio");

        upgrade = new Upgrade(name, description, cost, isUnlocked);

        _setFocused = false;

        if (!upgrade.isUnlocked && cost != 1)
        {
            UI.SetUIState(gameObject, (int)UI_STATE.DISABLED);
        }

        GameObject go = InternalCalls.GetGameObjectByName("Upgrade Station");

        if (go != null)
        {
            currentStation = go.GetComponent<UI_Upgrade_Station>();
        }

        if (cost == 2 || cost == 4)
        {
            _parent = InternalCalls.GetGameObjectByName(stationName + " End");
        }
    }

    public void Update()
    {
        if (_setFocused)
        {
            UI.SetUIState(gameObject, (int)UI_STATE.FOCUSED);
            _setFocused = false;
        }
        return;
    }

    public void OnClickButton()
    {
        if (!upgrade.isUnlocked && currentStation.currentScore >= upgrade.cost)
        {
            Audio.PlayAudio(audioSource, "UI_WeaponUpgrade");

            switch (cost)
            {
                case 1:
                    {
                        GameObject go2 = InternalCalls.GetChildrenByName(InternalCalls.GetGameObjectByName(stationName), "Upgrade 2");
                        UI.SetUIState(go2, (int)UI_STATE.NORMAL);
                        currentStation.currentScore -= upgrade.cost;
                        upgrade.isUnlocked = true;
                    }
                    break;
                case 2:
                    {
                        GameObject go3 = InternalCalls.GetChildrenByName(_parent, "Upgrade 3");
                        GameObject go4 = InternalCalls.GetChildrenByName(_parent, "Upgrade 4");

                        UI.SetUIState(go3, (int)UI_STATE.NORMAL);
                        UI.SetUIState(go4, (int)UI_STATE.NORMAL);

                        currentStation.currentScore -= upgrade.cost;
                        upgrade.isUnlocked = true;
                    }
                    break;
                case 4:
                    {
                        if (gameObject.Name == "Upgrade 3")
                        {
                            GameObject go4 = InternalCalls.GetChildrenByName(_parent, "Upgrade 4");
                            UI.SetUIState(go4, (int)UI_STATE.DISABLED);
                        }
                        else
                        {
                            GameObject go3 = InternalCalls.GetChildrenByName(_parent, "Upgrade 4");
                            UI.SetUIState(go3, (int)UI_STATE.DISABLED);
                        }

                        currentStation.currentScore -= upgrade.cost;
                        upgrade.isUnlocked = true;
                    }
                    break;
            }
            currentStation.UpdateCoins();
        }

        _setFocused = true;
    }

    public void OnHoverButton()
    {
        if (currentStation != null)
        {
            UI.TextEdit(currentStation.description, description);
            UI.TextEdit(currentStation.cost, upgrade.cost.ToString());
        }
    }
}