using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class TriggerTutorialUI : YmirComponent
{
    
    public GameObject tutorialUI;
    public bool isHealingTuto = false;
    bool show;

    private bool _healingTutorialDone = false;

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

            //Para fines del healing tutorial
            if (isHealingTuto && !_healingTutorialDone)
            {
                _healingTutorialDone = true;
                other.GetComponent<Health>().TakeDmg(400);
            }
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