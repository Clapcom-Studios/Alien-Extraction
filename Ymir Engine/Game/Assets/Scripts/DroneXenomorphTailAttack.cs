using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using YmirEngine;

public class DroneXenomorphTailAttack : YmirComponent
{
    public GameObject thisReference = null;

    private float movementSpeed;

    private float damage = 250f;

    private GameObject player;

    private Health healthScript;

    private bool destroyed;

    private float destroyTimer;

    private float mass;

    public void Start()
    {
        movementSpeed = 5000f;
        player = InternalCalls.GetGameObjectByName("Player");
        healthScript = player.GetComponent<Health>();
        damage = gameObject.GetMass();
        mass = gameObject.GetMass();
        gameObject.SetMass(1.0f);
        gameObject.SetImpulse(gameObject.transform.GetForward() * movementSpeed * mass * Time.deltaTime);
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
            healthScript.TakeDmg(damage);
            destroyed = true;
        }
    }
}