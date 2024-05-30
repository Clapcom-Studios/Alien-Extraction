using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class PickUpPopUp : YmirComponent
{
	public float popUpTime = 4;
	private float _popupTimer = 0;

	public void Start()
	{
		EndPopUp();
	}

	public void Update()
	{
		if (gameObject.IsActive())
		{
			_popupTimer += Time.deltaTime;
			if (_popupTimer > popUpTime)
			{
				EndPopUp();
			}
		}
	}

	public void ResetTimer() 
	{ 
		_popupTimer = 0; 
	}

	void EndPopUp()
	{
		ResetTimer();
        gameObject.SetActive(false);
    }
}