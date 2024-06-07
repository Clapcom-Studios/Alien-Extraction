using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Electric_sound : YmirComponent
{
    private bool active;
    private bool soundPlayed;
    private Vector3 originalPosition;

    public void Start()
    {
        //Debug.Log("HelloWorld");
        active = true;
        soundPlayed = false;
        originalPosition = gameObject.transform.localPosition;
    }

    public void Update()
    {
        if (gameObject.transform.localPosition.y > originalPosition.y + 1 || gameObject.transform.localPosition.y < originalPosition.y - 1)
        {
            active = true;
            soundPlayed = false;
          //  Debug.Log("1111111111111111111111111111111111111111111111111111111111111111");
          //  Debug.Log("" + originalPosition);
           // Debug.Log("" + gameObject.transform.globalPosition);
          //  Debug.Log("" + gameObject.transform.localPosition);
        }
        else
        {
           // Debug.Log("222222222222222222222222222222222222222222222222222222222222222222222222222");
           // Debug.Log("" + originalPosition);
           // Debug.Log("" + gameObject.transform.globalPosition);
           // Debug.Log("" + gameObject.transform.localPosition);
            active = false;
        }
    }

    public void OnCollisionStay(GameObject other)
    {
        if (other.Tag == "Player" && !active && !soundPlayed)
        {
            //Debug.Log("ssooooooooooooooooniiiiiiiiiiidoooooooooooooooooooooooo");
            Audio.PlayAudio(gameObject, "LV2_ElectricT");
            soundPlayed = true;
        }
    }
}