using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Enemy_Test : YmirComponent
{
	public void Start()
	{
 
	}

	public void Update()
	{

	}

    public void OnCollisionStay(GameObject other)
    {
        if (other.Name == "Tail")
        {
            gameObject.SetImpulse(gameObject.transform.GetForward() * -1 * 60);
            //InternalCalls.Destroy(gameObject);
            //Le hace da�o
        }
    }
}