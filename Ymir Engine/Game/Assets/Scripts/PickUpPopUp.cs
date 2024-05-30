using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class PickUpPopUp : YmirComponent
{
	public float popUpTime = 2;
	private float _popupTimer = 0;

	public void Start()
	{
		EndPopUp();
	}

	public void Update()
	{
		_popupTimer += Time.deltaTime;
		if (_popupTimer > popUpTime) EndPopUp();
	}

	void EndPopUp()
	{
		_popupTimer = 0;
        gameObject.SetActive(false);
    }
}