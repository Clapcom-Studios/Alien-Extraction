using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using YmirEngine;

public class UI_AnimationByFrames : YmirComponent
{
    public GameObject img = null;
    public string imgPath = "Assets\\Cutscenes\\";
    public string imgName = "";

    public int currentFrame = 0;
    public int maxFrames = 0;

    public float delay = 0;

    public bool isLoop = false;
    public bool hasFinished = false;

    private float timer = 2f;

    public void Start()
    {
        img = InternalCalls.GetGameObjectByName("AnimationImg");
        currentFrame = 0;

        hasFinished = false;
        timer = 0;
    }

    public void Update()
    {
        if (img != null && !hasFinished)
        {
            timer += Time.deltaTime;

            if (delay <= timer)
            {
                timer = 0;

                currentFrame++;
                UI.ChangeImageUI(img, imgPath + imgName + "(" + currentFrame.ToString() + ")" + ".png", (int)UI_STATE.NORMAL);

                if (currentFrame == maxFrames)
                {
                    hasFinished = true;

                    if (isLoop)
                    {
                        Reset();
                    }
                }
            }
        }

        return;
    }
    public void Reset()
    {
        hasFinished = false;
        currentFrame = 0;
    }
}