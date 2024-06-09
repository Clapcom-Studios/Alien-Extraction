using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class QueenXenomorphPuddle : YmirComponent
{
    public GameObject thisReference = null;

    private float damage = 20f;

    private GameObject player;

    private GameObject boss;
    private QueenXenomorphBaseScript bossScript;


    private Health healthScript;

    private float destroyTimer;

    private float puddleTimer;

    //private float puddleDamageTimer;

    public void Start()
    {
        player = InternalCalls.GetGameObjectByName("Player");
        healthScript = player.GetComponent<Health>();
        boss = InternalCalls.GetGameObjectByName("Boss");
        bossScript = boss.GetComponent<QueenXenomorphBaseScript>();
        destroyTimer = 0f;
        puddleTimer = 0f;
    }

    public void Update()
    {
        destroyTimer += Time.deltaTime;

        puddleTimer += Time.deltaTime;

        if (destroyTimer >= 50f)
        {
            InternalCalls.Destroy(gameObject);
        }

    }

    public void OnCollisionStay(GameObject other)
    {
        if (other.Name == "Player" && puddleTimer >= 0.5f && player.GetComponent<Player>().vulnerable)
        {
            Debug.Log("[ERROR] ACID PUDDLE");
            healthScript.TakeDmg(bossScript.acidPudleDMG);
            puddleTimer = 0f;
        }
    }
}