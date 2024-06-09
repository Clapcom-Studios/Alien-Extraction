using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class VoiceOver_intro : YmirComponent
{
	private GameObject audio_source;
    public int actual_frame = 2;
	public int max_frame;
    private float timer = 5.5f;
    private float finishTimer = 5.5f;
    public bool audio_played;
    public void Start()
	{
		audio_source = InternalCalls.GetGameObjectByName("Audio_Source");
        audio_played = false;
    }

	public void Update()
	{
        if (finishTimer >= 0)
        {
            finishTimer -= Time.deltaTime;
        }
        if(!audio_played)
        {
            Audio.PlayAudio(audio_source, "Dialogue_" + actual_frame.ToString()); 
            audio_played = true;
            Debug.Log("Dialogue_" + actual_frame.ToString());
        }
        
        if (finishTimer <= 0)
        {
            if (Input.GetKey(YmirKeyCode.A) == KeyState.KEY_DOWN || Input.GetGamepadButton(GamePadButton.A) == KeyState.KEY_DOWN)
            {
                finishTimer= timer;
                audio_played = false;
                actual_frame++;
                return;
            }
        }
	}
}