using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using YmirEngine;

public class Loot_Chest : YmirComponent
{
    public int spawnRange;

    private float time = 0f;
    private float animationTime = 1;

    private bool isOpened = false;
    private bool isOpening = false;

    private float velocity = 30f;

    private Vector3 pos = Vector3.zero;
    Random random = new Random();

    private GameObject popup;

    public string itemPath = "";

    public void Start()
	{
        pos = gameObject.transform.localPosition;
        spawnRange = 15;
        popup = InternalCalls.GetChildrenByName(gameObject, "Pop-Up");

        //Saefty Check - Si no hay ningun item predefinido poner un item generico: acidvesicle_common
        if (itemPath == "")
        {
            itemPath = "Assets/Prefabs/Items/Common Items/acidvesicle_common";
        }

	}

	public void Update()
	{
        popup.SetAsBillboard();
        if (!isOpened)
        {
            if (popup.IsActive())
            {
                popup.SetActive(false);
            }

            if (time > 0)
            {
                InternalCalls.CS_GetChild(gameObject, 1).transform.localRotation = Quaternion.Euler(180f, velocity * (animationTime - time), 0f);// GetGameObjectByName("PROP_Base_Chest_Lid").transform.localRotation = Quaternion.Euler(180f, velocity * time, 0f);
                time -= Time.deltaTime;

                if (time <= 0 && !isOpened)
                {
                    isOpened = true;
                    SpawnPrefab(itemPath);
                }
            }
        }
        else
        {
            popup.SetActive(false);
        }
    }

    public void OnCollisionStay(GameObject other)
    {
        if (other.Tag == "Player" && !isOpened)
        {
            popup.SetActive(true);
        }

        if (other.Tag == "Player" && (Input.IsGamepadButtonAPressedCS() || Input.GetKey(YmirKeyCode.SPACE) == KeyState.KEY_DOWN) && !isOpened && !isOpening)
        {
            if (itemPath != null)
            {
                time = animationTime;
                isOpening = true;
            }
        }
    }

    private void SpawnPrefab(string path)
    {

        //Spawn items in a range random position offset
        float randPosX = random.Next(-spawnRange, spawnRange + 1);
        float randPosZ = random.Next(-spawnRange, spawnRange + 1);
        Debug.Log("[WARNING] PickUp offset: " + randPosX + ", " + randPosZ);

        pos.x += randPosX;
        pos.z += randPosZ;

        InternalCalls.CreateGOFromPrefab(SplitPath(path).Item1, SplitPath(path).Item2, pos);

        //Clear the pos value
        pos = gameObject.transform.localPosition;

    }

    static (string, string) SplitPath(string path)
    {
        int ultimoSeparador = path.LastIndexOf('/');
        string directorioBase = path.Substring(0, ultimoSeparador);
        string nombreArchivo = path.Substring(ultimoSeparador + 1);

        return (directorioBase, nombreArchivo);
    }
}


