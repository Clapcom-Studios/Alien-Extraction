using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Key : YmirComponent
{
	public GameObject door1;
    public GameObject door2;
    public GameObject door3;
    private GameObject _pickupPopUp = new GameObject();

    private List<GameObject> doorList = new List<GameObject>();

	public void Start()
	{
        doorList.Add(door1);
        doorList.Add(door2);
        doorList.Add(door3);

        _pickupPopUp = InternalCalls.GetGameObjectByName("ItemPicked");
	}

	public void Update()
	{
		
	}

    public void OnCollisionStay(GameObject other)
    {
        if (other.Tag == "Player")
        {
            Audio.PlayEmbedAudio(gameObject);
            _pickupPopUp?.SetActive(true);

            foreach (GameObject go in doorList)
            {
                if (go != null)
                {
                    InternalCalls.Destroy(go);
                }
            }

            InternalCalls.Destroy(gameObject);
        }
    }
}