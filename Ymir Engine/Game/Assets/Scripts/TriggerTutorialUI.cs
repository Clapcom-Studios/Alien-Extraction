using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class TriggerTutorialUI : YmirComponent
{
    
    public GameObject tutorialUI;
    bool show;

    public void Start()
    {
        tutorialUI.SetActive(false);
    }

    public void OnCollisionStay(GameObject other)
    {
        if (other.Tag == "Player")
        {
            show = true;
            tutorialUI.SetActive(true);
        }
    }

    public void Update()
    {
        if (!show)
        {
            tutorialUI.SetActive(false);
        }

        show = false;
    }
}