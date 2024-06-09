using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using YmirEngine;

public class QueenXenomorphClawAttack : YmirComponent
{
    public GameObject thisReference = null;

    private float movementSpeed;

    private float damage = 600f;

    private GameObject player;

    private GameObject boss;
    private QueenXenomorphBaseScript bossScript;

    private Health healthScript;

    private bool destroyed;

    private float destroyTimer;

    public void Start()
    {
        movementSpeed = 5000f;
        player = InternalCalls.GetGameObjectByName("Player");
        healthScript = player.GetComponent<Health>();
        boss = InternalCalls.GetGameObjectByName("Boss");
        bossScript = boss.GetComponent<QueenXenomorphBaseScript>();
        gameObject.SetImpulse(gameObject.transform.GetForward() * movementSpeed * Time.deltaTime);
        destroyed = false;
        destroyTimer = 0f;
    }

    public void Update()
    {
        destroyTimer += Time.deltaTime;

        if (destroyed || destroyTimer >= 0.3f)
        {
            InternalCalls.Destroy(gameObject);
        }

    }

    public void OnCollisionStay(GameObject other)
    {
        if (other.Name == "Player" && destroyed == false && player.GetComponent<Player>().vulnerable)
        {
            Debug.Log("[ERROR] CLAW");
            healthScript.TakeDmg(bossScript.clawDMG);
            destroyed = true;
        }
    }
}